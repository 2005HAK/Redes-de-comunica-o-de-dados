#include "agent.hpp"

Agent::Agent(QObject* parent) : QObject(parent){
	camera = new QCamera(this);
	imageCapture = new QCameraImageCapture(camera);

	connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &Agent::onWebcamImageCaptured);
	connect(imageCapture, QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error), this, &Agent::onCameraError);
	connect(imageCapture, &QCameraImageCapture::readyForCaptureChanged, this, &Agent::onCameraReadyForCapture);

	myIP = "127.0.0.1";
	for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
		if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::LocalHost) {
			myIP = address.toString();
			break;
		}
	}
	
	mosquitto_lib_init();

	QString clientId = QString("DeviceAgent_%1").arg(myIP);
	mosq = mosquitto_new(clientId.toStdString().c_str(), true, this);

	mosquitto_message_callback_set(mosq, on_message_callback);
	mosquitto_connect_callback_set(mosq, on_connect_callback);

	mosquitto_loop_start(mosq);

	int rc = mosquitto_connect_async(mosq, brokerAddress.toStdString().c_str(), brokerPort, 60);
	if(rc != MOSQ_ERR_SUCCESS) std::cerr << "Failed to connect to MQTT broker: " << mosquitto_strerror(rc) << std::endl;
}

Agent::~Agent(){
	mosquitto_loop_stop(mosq, true);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}

void Agent::on_connect_callback(struct mosquitto *mosq, void *userdata, int result) {
	Agent* self = static_cast<Agent*>(userdata);

	if(result == 0){
		std::cout << "Broker confirmed connection! Subscribing to command topic..." << std::endl;

		QString commandTopic = QString("device/%1/command").arg(self->myIP);
		mosquitto_subscribe(mosq, NULL, commandTopic.toStdString().c_str(), 0);
	} else std::cerr << "Error connecting to Broker. Rejection code: " << result << std::endl;
}

void Agent::on_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
	Agent* self = static_cast<Agent*>(userdata);

	std::cout << "Received MQTT message on topic: " << message->topic << std::endl;
	
	QByteArray request(static_cast<char*>(message->payload), message->payloadlen);

	if(request.contains(CLIENT_REQUEST)){
		std::cout << "Capture command received via MQTT. Processing..." << std::endl;
		QMetaObject::invokeMethod(self, "triggerCapture", Qt::QueuedConnection);
	}
}

void Agent::triggerCapture(){
	QScreen* screen = QGuiApplication::primaryScreen();
	if(!screen) return;

	QPixmap originalPixmap = screen->grabWindow(0);

	if (originalPixmap.isNull()) {
		std::cerr << "Warning: OS blocked the capture. Generating black screen." << std::endl;

		originalPixmap = QPixmap(800, 600);
		originalPixmap.fill(Qt::black);
	}

	QBuffer screenBuffer(&pendingScreenData);
	pendingScreenData.clear();
	screenBuffer.open(QIODevice::WriteOnly);
	originalPixmap.save(&screenBuffer, "JPG");

	camera->setCaptureMode(QCamera::CaptureStillImage);
	camera->start();
}

void Agent::onCameraReadyForCapture(bool ready) { if(ready) imageCapture->capture(); }

void Agent::onWebcamImageCaptured(int id, const QImage& preview){
	Q_UNUSED(id);
	camera->stop();

	QByteArray webCamData;
	QBuffer webCamBuffer(&webCamData);
	webCamBuffer.open(QIODevice::WriteOnly);
	preview.save(&webCamBuffer, "JPG");

	QByteArray payload;
	QDataStream stream(&payload, QIODevice::WriteOnly);
	quint32 screenSize = pendingScreenData.size();
	quint32 webCamSize = webCamData.size();
	stream << screenSize << webCamSize;

	payload.append(pendingScreenData);
	payload.append(webCamData);

	QString dataTopic = QString("device/%1/data").arg(myIP);

	int rc = mosquitto_publish(mosq, NULL, dataTopic.toStdString().c_str(), payload.size(), payload.constData(), 0, false);

	if(rc == MOSQ_ERR_SUCCESS) std::cout << "Successfully sent " << payload.size() << " bytes via MQTT" << std::endl;
	else std::cerr << "Failed to publish data via MQTT" << std::endl;
}

void Agent::onCameraError(int id, QCameraImageCapture::Error error, const QString &errorString) {
	std::cerr << "Camera error: " << errorString.toStdString() << std::endl;

	QImage fallback(640, 480, QImage::Format_RGB32);
	fallback.fill(Qt::black);
	onWebcamImageCaptured(id, fallback);
}