#include "agent.hpp"

Agent::Agent(QObject* parent) : QObject(parent){
	tcpServer = new QTcpServer(this);

	camera = new QCamera(this);
	imageCapture = new QCameraImageCapture(camera);


	connect(imageCapture, &QImageCapture::imageCaptured, this, &Agent::onWebcamImageCaptured);
	connect(imageCapture, QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error), this, &Agent::onCameraError);

	connect(tcpServer, &QTcpServer::newConnection, this, &Agent::onNewConnection);

	if(tcpServer->listen(QHostAddress::Any, 55555)) std::cout << "Agent active and listening on port 55555" << std::endl; 
	else std::cerr << "Failled to initializes the agent: " << tcpServer->errorString().toStdString() << std::endl; 
}

Agent::~Agent(){
	tcpServer->close();
}

void Agent::onNewConnection(){
	QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
	
	connect(clientSocket, &QTcpSocket::readyRead, this, &Agent::onReadyRead);
	connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QObject::deleteLater);
}

void Agent::onReadyRead(){
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
	if(!socket) return;

	QByteArray request = socket->readAll();

	if(request.contains(CLIENT_REQUEST)){
		std::cout << "Capture command recieved. Processing..." << std::endl;
		this->sendCapture(socket);
	}
}

void Agent::sendCapture(QTcpSocket* socket){
	this->currentClientSocket = socket;

	QScreen* screen = QGuiApplication::primaryScreen();
	if(!screen) return;

	QPixmap originalPixmap = screen->grabWindow(0);

	if (originalPixmap.isNull()) {
		std::cerr << "Aviso: Captura bloqueada pelo SO. A gerar ecrã preto." << std::endl;

		originalPixmap = QPixmap(800, 600);
		originalPixmap.fill(Qt::black);
	}

	QByteArray screenData;
	QBuffer screenBuffer(&screenData);
	screenBuffer.open(QIODevice::WriteOnly);
	originalPixmap.save(&screenBuffer, "JPG");

	camera->setCaptureMode(QCamera::CaptureStillImage);
	camera->start();
	imageCapture->capture();
}

void Agent::onWebcamImageCaptured(int id, const QImage& preview){
	Q_UNUSED(id);
	camera->stop();

	if(!currentClientSocket || currentClientSocket->state() != QAbstractSocket::ConnectedState) return;

	QByteArray webCamData;
	QBuffer webCamBuffer(&webCamData);
	webCamBuffer.open(QIODevice::WriteOnly);
	preview.save(&webCamBuffer, "JPG");

	QByteArray header;
	QDataStream stream(&header, QIODevice::WriteOnly);
	quint32 screenSize = screenData.size();
	quint32 webCamSize = webCamData.size();
	stream << screenSize << webCamSize;

	socket->write(header);
	socket->write(screenData);
	socket->write(webCamData);

	socket->flush();

	std::cout << "Sucessfully on send " << (screenSize + webCamSize) << " bytes" << std::endl;
}

void Agent::onCameraError(int id, QCameraImageCapture::Error error, const QString &errorString) {
	std::cerr << "Camera error: " << errorString.toStdString() << std::endl;
	
	QImage fallback(640, 480, QImage::Format_RGB32);
	fallback.fill(Qt::black);
	onWebcamImageCaptured(id, fallback);
}