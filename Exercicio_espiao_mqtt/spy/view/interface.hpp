#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QMessageBox>
#include "../controller/deviceController.hpp"
#include "../controller/captureController.hpp"

class Interface : public QMainWindow {
	Q_OBJECT

	private:
		DeviceController* controller;
		CaptureController* captureController;

		QTabWidget* tabWidget;

		// Register Tab
		QLineEdit* nameInput;
		QLineEdit* ipInput;
		QLineEdit* macInput;

		QPushButton* registerButton;

		// Devices list Tab
		QListWidget* deviceList;

		// Device control
		QWidget* controlTab;
		QLabel* detailsLabel;
		QSpinBox* intervalInput;
		QPushButton* setIntervalButton;
		QPushButton* captureButton;
		QPushButton* toggleStatusButton;
		QPushButton* deleteButton;

		int currentDeviceIndex = -1;

		void setupUI();
		void refreshDeviceList();

	private slots:
		void handleRegister();
		void handleDeviceSelection(QListWidgetItem *item);
		void handleDeleteDevice();
		void handleInstantCapture();
		void handleToggleStatus();
		void handleSetInterval();

	public:
		Interface(DeviceController& controller, CaptureController& captureController, QWidget *parent = nullptr);

		~Interface();
};

#endif // INTERFACE_HPP