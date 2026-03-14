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
#include "../controller/deviceController.hpp"

class Interface : public QMainWindow {
	private:
		DeviceController* controller;

		QTabWidget* tabWidget;

		QLineEdit* nameInput;
		QLineEdit* ipInput;
		QLineEdit* macInput;

		QListWidget* deviceList;

		QPushButton* registerButton;
		QPushButton* captureButton;
		QPushButton* toggleStatusButton;
		QSpinBox* intervalInput;

		void setupUI();
		void refreshDeviceList();

		void handleRegister();
		void handleDeviceSelection(QListWidgetItem *item);
		void handleInstantCapture();
		void handleToggleStatus();

	public:
		Interface(DeviceController& controller, QWidget *parent = nullptr);

		~Interface();
};

#endif // INTERFACE_HPP