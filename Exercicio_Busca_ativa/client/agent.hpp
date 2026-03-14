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
#include <QCamera>
#include <QCameraImageCapture>
#include <iostream>

const QByteArray CLIENT_REQUEST = "CMD_CAPTURE";

class Agent : public QObject {
	Q_OBJECT
	private:
		QTcpServer* tcpServer;
		QCamera* camera;
		QCameraImageCapture* imageCapture;

		QTcpSocket* currentClientSocket;
		QByteArray pendingScreenData;

		void sendCapture(QTcpSocket* socket);

	private slots:
		void onNewConnection();
		void onReadyRead();
		void onWebcamImageCaptured(int id, const QImage &preview);
		void onCameraError(int id, int error, const QString &errorString);

	public:
		Agent(QObject* parent = nullptr);
		~Agent();
};

#endif // AGENT_HPP