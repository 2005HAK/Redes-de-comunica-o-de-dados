#ifndef AGENT_HPP
#define AGENT_HPP

#include <QObject>
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QBuffer>
#include <QDataStream>
#include <QCamera>
#include <QCameraImageCapture>
#include <QNetworkInterface>
#include <QHostAddress>
#include <iostream>
#include <mosquitto.h>

const QByteArray CLIENT_REQUEST = "CMD_CAPTURE";

class Agent : public QObject {
	Q_OBJECT
	private:
		QCamera* camera;
		QCameraImageCapture* imageCapture;

		struct mosquitto* mosq;

		QByteArray pendingScreenData;

		QString myIP;

		static void on_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);

		static void on_connect_callback(struct mosquitto *mosq, void *userdata, int result);

	private slots:
		void onWebcamImageCaptured(int id, const QImage &preview);
		void onCameraError(int id, QCameraImageCapture::Error error, const QString &errorString);
		void onCameraReadyForCapture(bool ready);

	public:
		Agent(QObject* parent = nullptr);
		~Agent();

	public slots:
		void triggerCapture();
};

#endif // AGENT_HPP