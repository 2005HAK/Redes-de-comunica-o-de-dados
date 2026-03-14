#include "interface.hpp"

Interface::Interface(DeviceController& controller, QWidget *parent) : QMainWindow(parent), controller(&controller){
	this->setupUI();
	this->refreshDeviceList();
}

Interface::~Interface(){}

void Interface::setupUI(){
	tabWidget  = new QTabWidget(this);

	// ------------ Register Tab ------------

	QWidget* registerTab = new QWidget();
	QVBoxLayout* registerLayout = new QVBoxLayout(registerTab);

	nameInput = new QLineEdit();
	nameInput->setPlaceholderText("Device Name");

	ipInput = new QLineEdit();
	ipInput->setPlaceholderText("Device IP (e.g., 192.168.1.1)");

	macInput = new QLineEdit();
	macInput->setPlaceholderText("Device MAC (e.g., 00:1A:2B:3C:4D:5E)");

	registerButton = new QPushButton("Register Device");

	registerLayout->addWidget(new QLabel("Register a new device:"));
	registerLayout->addWidget(nameInput);
	registerLayout->addWidget(ipInput);
	registerLayout->addWidget(macInput);
	registerLayout->addWidget(registerButton);

	// ------------ Devices list Tab ------------

	QWidget* listTab = new QWidget();
	QVBoxLayout* listLayout = new QVBoxLayout(listTab);
	deviceList = new QListWidget();
	listLayout->addWidget(new QLabel("Registered devices:"));
	listLayout->addWidget(deviceList);

	tabWidget->addTab(registerTab, "Register");
	tabWidget->addTab(listTab, "Devices");

	setCentralWidget(tabWidget);

	connect(registerButton, &QPushButton::clicked, this, &Interface::handleRegister);
}

void Interface::refreshDeviceList() {
	
}

void Interface::handleRegister() {
	QString name = nameInput->text();
	QString ip = ipInput->text();
	QString mac = macInput->text();

	if(controller->addDevice(name.toStdString(), ip.toStdString(), mac.toStdString())) {
		this->refreshDeviceList();
		nameInput->clear();
		ipInput->clear();
		macInput->clear();
	}
}