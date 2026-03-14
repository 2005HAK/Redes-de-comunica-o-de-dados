#include "interface.hpp"

Interface::Interface(DeviceController& controller, CaptureController& captureController, QWidget *parent) : QMainWindow(parent), controller(&controller), captureController(&captureController){
	this->setupUI();

	this->resize(400, 300);
	this->setMinimumSize(400, 300);

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

	connect(registerButton, &QPushButton::clicked, this, &Interface::handleRegister);

	// ------------ Devices list Tab ------------

	QWidget* listTab = new QWidget();
	QVBoxLayout* listLayout = new QVBoxLayout(listTab);
	deviceList = new QListWidget();
	listLayout->addWidget(new QLabel("Registered devices:"));
	listLayout->addWidget(deviceList);

	connect(deviceList, &QListWidget::itemDoubleClicked, this, &Interface::handleDeviceSelection);

	// ------------ Device control Tab ------------
	
	controlTab = new QWidget();
	QVBoxLayout* controlLayout = new QVBoxLayout(controlTab);
	
	detailsLabel = new QLabel("Select a device to see details and controls.");
	detailsLabel->setTextFormat(Qt::RichText);
	
	QHBoxLayout* intervalLayout = new QHBoxLayout();
	intervalInput = new QSpinBox();
	intervalInput->setRange(0, 1440);
	intervalInput->setSuffix(" min");
	setIntervalButton = new QPushButton("Set Capture Interval");
	
	intervalLayout->addWidget(intervalInput);
	intervalLayout->addWidget(setIntervalButton);
	
	captureButton = new QPushButton("Instant Capture");
	toggleStatusButton = new QPushButton("Toggle Active/Inactive");
	deleteButton = new QPushButton("Delete Device");
	deleteButton->setStyleSheet("background-color: #ff0000; color: #ffffff;");
	
	controlLayout->addWidget(detailsLabel);
	controlLayout->addLayout(intervalLayout);
	controlLayout->addWidget(captureButton);
	controlLayout->addWidget(toggleStatusButton);
	controlLayout->addStretch();
	controlLayout->addWidget(deleteButton);
	
	connect(setIntervalButton, &QPushButton::clicked, this, &Interface::handleSetInterval);
	connect(captureButton, &QPushButton::clicked, this, &Interface::handleInstantCapture);
	connect(toggleStatusButton, &QPushButton::clicked, this, &Interface::handleToggleStatus);
	connect(deleteButton, &QPushButton::clicked, this, &Interface::handleDeleteDevice);

	// ------------ Add tabs to main widget ------------

	tabWidget->addTab(registerTab, "Register");
	tabWidget->addTab(listTab, "Devices");
	tabWidget->addTab(controlTab, "Control");

	tabWidget->setTabEnabled(2, false);

	setCentralWidget(tabWidget);
}

void Interface::refreshDeviceList() {
	deviceList->clear();

    std::vector<Device> devices = controller->getDevices();

    for (const auto& device : devices) {
        QString itemText = QString("%1 (%2) - %3")
							.arg(QString::fromStdString(device.getName()))
							.arg(QString::fromStdString(device.getIp()))
							.arg(device.isActive() ? "Ativo" : "Inativo");

        deviceList->addItem(itemText);
    }
}

// ---------- Slots implementation ----------

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

void Interface::handleDeviceSelection(QListWidgetItem *item) {
	currentDeviceIndex = deviceList->row(item);

	std::vector<Device> devices = controller->getDevices();
	Device selectedDevice = devices[currentDeviceIndex];

	QString details = QString("<b>Name:</b> %1<br><b>IP:</b> %2<br><b>MAC:</b> %3<br><b>Status:</b> %4<br><b>Capture Interval:</b> %5 min")
						.arg(QString::fromStdString(selectedDevice.getName()))
						.arg(QString::fromStdString(selectedDevice.getIp()))
						.arg(QString::fromStdString(selectedDevice.getMac()))
						.arg(selectedDevice.isActive() ? "<font color='green'>Ativo</font>" : "<font color='red'>Inativo</font>")
						.arg(selectedDevice.getInterval());

	detailsLabel->setText(details);

	tabWidget->setTabEnabled(2, true);
	tabWidget->setCurrentIndex(2);
}

void Interface::handleSetInterval() {
	if(currentDeviceIndex < 0) return;

	std::vector<Device> devices = controller->getDevices();

	controller->updateDeviceInterval(devices[currentDeviceIndex].getName(), devices[currentDeviceIndex].isActive(), intervalInput->value());

	this->handleDeviceSelection(deviceList->currentItem());
}

void Interface::handleInstantCapture() {
	if(currentDeviceIndex < 0) return;

	std::vector<Device> devices = controller->getDevices();

	captureController->captureNow(devices[currentDeviceIndex]);
}

void Interface::handleToggleStatus() {
	if(currentDeviceIndex < 0) return;

	std::vector<Device> devices = controller->getDevices();

	controller->updateDeviceStatus(devices[currentDeviceIndex].getName(), devices[currentDeviceIndex].getInterval(), !devices[currentDeviceIndex].isActive());

	this->handleDeviceSelection(deviceList->currentItem());
}

void Interface::handleDeleteDevice() {
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Confirm Deletion", "Are you sure you want to delete this device?", QMessageBox::Yes | QMessageBox::No);

	if(reply == QMessageBox::Yes) {
		std::vector<Device> devices = controller->getDevices();
		if(currentDeviceIndex >= 0 && currentDeviceIndex < devices.size()) {
			controller->removeDevice(devices[currentDeviceIndex].getName());
			this->refreshDeviceList();
			detailsLabel->setText("Select a device to see details and controls.");
			tabWidget->setTabEnabled(2, false);
			tabWidget->setCurrentIndex(1);
			currentDeviceIndex = -1;
		}
	}
}