#include "agent.hpp"

Agent::Agent(QObject* parent) : QObject(parent){
	tcpServer = new QTcpServer(this);

	connect(tcpServer, &QTcpServer::newConnection, this, &Agent::onNewConnection);

	if(tcpServer->listen(QHostAddress::Any, 55555)) std::cout << "Agent active and listening on port 12345" << std::endl; 
	else std::cerr << "Failled to initializes the agent: " << tcpServer->errorString().toStdString() << std::endl; 
}

Agent::~Agent(){
	tcpServer->close();
}

void Agent::onNewConnection(){
	QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
	
	connect(clientSocket, &QTcpSocket::readyRead, this, &Agent::onReadyRead);
	connect(clientSocket, &QTcpSocket::disconnected, this, &QObject::deleteLater);
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

	QByteArray webCamData;

	QByteArray header;
	QDataStream stream(&header, QIODevice::WriteOnly);
	quint32 screenSize = screenData.size();
	quint32 webCamSize = webCamData.size();
	stream << screenSize << webCamSize;

	socket->write(header);
	socket->write(screenData);
	socket->write(webCamData);

	socket->flush();

	std::cout << "Sucessfully on send " << screenSize << " bytes" << std::endl;
}