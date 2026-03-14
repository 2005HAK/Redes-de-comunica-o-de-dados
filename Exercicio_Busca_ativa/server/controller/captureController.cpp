#include "captureController.hpp"

CaptureController::CaptureController(DeviceController& deviceController, QObject *parent) : QObject(parent), deviceController(&deviceController){
	this->updateSchedules();
}

CaptureController::~CaptureController() {
	qDeleteAll(captureTimers);
	captureTimers.clear();
}

void CaptureController::captureNow(const Device& device) {
	std::cout << "Capturing data from device: " << device.getName() << " (" << device.getIp() << ")" << std::endl;

	QTcpSocket* socket = new QTcpSocket(this);

	connect(socket, &QTcpSocket::readyRead, this, &CaptureController::onReadyRead);
	connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &CaptureController::onSocketError);

	socket->connectToHost(QString::fromStdString(device.getIp()), 12345);

	connect(socket, &QTcpSocket::connected, [socket]() {
		socket->write(CLIENT_REQUEST);
	});
}

void CaptureController::updateSchedules(){
	std::vector<Devices> devices = deviceController->getDevices();

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

void CaptureController::onReadyRead(){
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	if(!socket) return;

	socketBuffers[socket].append(socket->readAll());

	QbyteArray& buffer = socketBuffers[socket];

	if(buffer.size() < 8) return;

	QDataStream stream(buffer);

	quint32_t screenSize = 0;
	quint32_t webCamSize = 0;

	stream >> screenSize >> webCamSize;

	quint32_t totalExpectedSize = 8 + screenSize + webCamSize;

	if(buffer.size() >= totalExpectedSize) {
		std::cout << "Complete data received from " << socket->peerAddress().toString().toStdString() << std::endl;

		QByteArray screenData = buffer.mid(8, screenSize);
		QByteArray webCamData = (webCamSize > 0) ? buffer.mid(8 + screenSize, webCamSize) : QByteArray();

		QString ipFolder = socket->peerAddress().toString();
		QDir().mkpath("capturas/" + ipFolder);

		QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");

		QFile screenFile("capturas/" + ipFolder + "/screen_" + timestamp + ".png");
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

		socketBuffers.remove(socket);
		socket->disconnectFromHost();
		socket->deleteLater();
	}
}

void CaptureController::onSocketError(QAbstractSocket::SocketError socketError) {
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	if(socket) {
		std::cerr << "Socket error: " << socket->errorString().toStdString() << std::endl;
		socketBuffers.remove(socket);
		socket->deleteLater();
	}
}