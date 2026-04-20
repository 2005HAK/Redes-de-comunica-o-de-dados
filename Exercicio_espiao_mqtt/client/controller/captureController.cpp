#include "captureController.hpp"

CaptureController::CaptureController(DeviceController& deviceController, QObject *parent) : QObject(parent), deviceController(&deviceController){
	mosquitto_lib_init();
	mosq = mosquitto_new("DeviceController_Master", true, this);
	mosquitto_message_callback_set(mosq, on_message_callback);

	int rc = mosquitto_connect(mosq, "localhost", 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS) {
		std::cerr << "Falha ao conectar a Central no broker MQTT: " << mosquitto_strerror(rc) << std::endl;
	} else {
		std::cout << "Central conectada ao Broker MQTT." << std::endl;
		mosquitto_subscribe(mosq, NULL, "device/+/data", 0);
	}

	mosquitto_loop_start(mosq);
	this->updateSchedules();
}

CaptureController::~CaptureController() {
	qDeleteAll(captureTimers);
	captureTimers.clear();

	mosquitto_loop_stop(mosq, true);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}

void CaptureController::captureNow(const Device& device) {
	std::cout << "Capturing data from device: " << device.getName() << " (" << device.getIp() << ")" << std::endl;

	QString topic = QString("device/%1/command").arg(QString::fromStdString(device.getIp()));
	
	int rc = mosquitto_publish(mosq, NULL, topic.toStdString().c_str(), CLIENT_REQUEST.size(), CLIENT_REQUEST.constData(), 0, false);
	
	if(rc != MOSQ_ERR_SUCCESS) std::cerr << "Falha ao enviar comando via MQTT para " << device.getIp() << std::endl;
}

void CaptureController::updateSchedules(){
	std::vector<Device> devices = deviceController->getDevices();

	qDeleteAll(captureTimers);
	captureTimers.clear();

	for(const auto& device : devices) {
		if(device.isActive() && device.getInterval() > 0) {
			QTimer* timer = new QTimer(this);

			int intervalMs = device.getInterval() * 60 * 1000;

			connect(timer, &QTimer::timeout, [this, device]() {
				this->captureNow(device);
			});

			timer->start(intervalMs);
			captureTimers.insert(QString::fromStdString(device.getName()), timer);
		}
	}
}

void CaptureController::on_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
	CaptureController* self = static_cast<CaptureController*>(userdata);
	
	QString topic(message->topic);
	QByteArray payload(static_cast<char*>(message->payload), message->payloadlen);

	QMetaObject::invokeMethod(self, "processIncomingData", Qt::QueuedConnection, Q_ARG(QString, topic), Q_ARG(QByteArray, payload));
}

void CaptureController::processIncomingData(const QString& topic, const QByteArray& buffer) {
	QStringList parts = topic.split('/');
	if(parts.size() < 3) return;
	
	QString ipFolder = parts[1]; 

	if(buffer.size() < 8) return;

	QDataStream stream(buffer);
	quint32 screenSize = 0;
	quint32 webCamSize = 0;

	stream >> screenSize >> webCamSize;
	quint32 totalExpectedSize = 8 + screenSize + webCamSize;

	if(buffer.size() >= totalExpectedSize) {
		std::cout << "Complete data packet received from " << ipFolder.toStdString() << " (" << buffer.size() << " bytes)" << std::endl;

		QByteArray screenData = buffer.mid(8, screenSize);
		QByteArray webCamData = (webCamSize > 0) ? buffer.mid(8 + screenSize, webCamSize) : QByteArray();

		QDir().mkpath("capturas/" + ipFolder);
		QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");

		QFile screenFile("capturas/" + ipFolder + "/screen_" + timestamp + ".jpg"); 
		if(screenFile.open(QIODevice::WriteOnly)) {
			screenFile.write(screenData);
			screenFile.close();
		}

		if(webCamSize > 0) {
			QFile webCamFile("capturas/" + ipFolder + "/webcam_" + timestamp + ".jpg");
			if(webCamFile.open(QIODevice::WriteOnly)) {
				webCamFile.write(webCamData);
				webCamFile.close();
			}
		}
	} else std::cerr << "Aviso: Pacote corrompido ou incompleto recebido de " << ipFolder.toStdString() << std::endl;
}