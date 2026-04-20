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

		/**
		 * @brief Callback to process incoming MQTT messages.
		 * 
		 * This static method is called whenever a message is received on a subscribed topic. It processes the message, extracts the topic and payload, 
		 * and then calls the `processIncomingData` method to handle the data accordingly.
		 * 
		 * @param mosq Pointer to the mosquitto instance that received the message.
		 * @param userdata Pointer to user data, which is expected to be a pointer to the `CaptureController` instance.
		 * @param message Pointer to the `mosquitto_message` structure containing the topic and payload of the received message.
		 */
		static void on_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);

		/**
		 * @brief Callback to handle MQTT connection events.
		 * 
		 * This static method is called when the MQTT client successfully connects to the broker. It subscribes to the relevant topic to receive data 
		 * from devices.
		 * 
		 * @param mosq Pointer to the mosquitto instance that established the connection.
		 * @param userdata Pointer to user data, which is expected to be a pointer to the `CaptureController` instance.
		 * @param result An integer indicating the result of the connection attempt. A value of 0 indicates a successful connection, while non-zero 
		 * values indicate various connection errors.
		 */
		static void on_connect_callback(struct mosquitto *mosq, void *userdata, int result);

	private slots:
		/**
		 * @brief Process incoming data from MQTT messages.
		 * 
		 * This slot is called when an incoming MQTT message is received and processed. It handles the data accordingly.
		 * 
		 * @param topic The topic of the received message.
		 * @param payload The payload of the received message.
		 */
		void processIncomingData(const QString& topic, const QByteArray& payload);

	public:
		/**
		 * @brief Constructor for the CaptureController class.
		 * 
		 * This constructor initializes the CaptureController, sets up the MQTT client, and connects to the broker. It also loads the initial device schedules.
		 * 
		 * @param deviceController A reference to the DeviceController instance that manages the devices and their capture schedules.
		 * @param parent An optional parent QObject for the CaptureController.
		 */
		CaptureController(DeviceController& deviceController, QObject *parent = nullptr);

		/**
		 * @brief Destructor for the CaptureController class.
		 */
		~CaptureController();

		/**
		 * @brief Capture data from a specified device immediately.
		 * 
		 * This method sends a command to the specified device to capture data immediately. It constructs the appropriate MQTT topic based on the device's 
		 * IP address and publishes a command message to that topic.
		 * 
		 * @param device The device from which to capture data immediately.
		 */
		void captureNow(const Device& device);

		/**
		 * @brief Update the capture schedules for all devices.
		 * 
		 * This method updates the capture schedules for all devices managed by the DeviceController. It retrieves the list of devices, checks their active 
		 * status and capture intervals, and sets up QTimers to trigger captures at the appropriate times.
		 */
		void updateSchedules();
};

#endif // CAPTURECONTROLLER_HPP