#ifndef CAPTURECONTROLLER_HPP
#define CAPTURECONTROLLER_HPP

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QMap>
#include <QString>
#include "deviceController.hpp"

const QByteArray CLIENT_REQUEST = "CMD_CAPTURE\n";

class CaptureController {
	Q_OBJECT

	private:
		DeviceController* deviceController;

		QMap<QString, QTimer*> captureTimers;

		QMap<QTcpSocket*, QByteArray> socketBuffers;
		
	private slots:
		void onReadyRead();
	
		void onSocketError(QAbstractSocket::SocketError socketError);
		
	public:
		CaptureController(DeviceController& deviceController, QObject *parent = nullptr);
		~CaptureController();

		void captureNow(const Device& device);

		void updateSchedules();

		void startScheduledCaptures();
	
};

#endif // CAPTURECONTROLLER_HPP