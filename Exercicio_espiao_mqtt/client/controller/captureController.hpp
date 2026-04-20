#ifndef CAPTURECONTROLLER_HPP
#define CAPTURECONTROLLER_HPP

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QString>
#include <QDataStream>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <mosquitto.h>
#include "deviceController.hpp"

const QByteArray CLIENT_REQUEST = "CMD_CAPTURE\n";

class CaptureController : public QObject {
	Q_OBJECT

	private:
		DeviceController* deviceController;

		QMap<QString, QTimer*> captureTimers;

		struct mosquitto* mosq;

		static void on_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);

	private slots:
		void processIncomingData(const QString& topic, const QByteArray& payload);

	public:
		CaptureController(DeviceController& deviceController, QObject *parent = nullptr);
		~CaptureController();

		void captureNow(const Device& device);

		void updateSchedules();
};

#endif // CAPTURECONTROLLER_HPP