#ifndef AGENT_HPP
#define AGENT_HPP

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QBuffer>
#include <QDataStream>
#include <iostream>

const QByteArray CLIENT_REQUEST = "CMD_CAPTURE";

class Agent : public QObject {
	Q_OBJECT
	private:
		QTcpServer* tcpServer;
		void sendCapture(QTcpSocket* socket);

	private slots:
		void onNewConnection();
		void onReadyRead();

	public:
		Agent(QObject* parent = nullptr);
		~Agent();
};

#endif // AGENT_HPP