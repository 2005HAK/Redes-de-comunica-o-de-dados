#include "deviceController.hpp"

int DeviceController::addDevice(const std::string& name, const std::string& ip, const std::string& mac) {
	if(name.empty() || ip.empty() || mac.empty()) {
		std::cerr << "Error: All fields must be filled." << std::endl;
		return 0;
	}


	// validar o formato do ip e do mac


	for(const auto& device : devices) {
		if(device.getIp() == ip) {
			std::cerr << "Error: A device with this IP already exists." << std::endl;
			return 0;
		}

		if(device.getMac() == mac) {
			std::cerr << "Error: A device with this MAC already exists." << std::endl;
			return 0;
		}

		if(device.getName() == name) {
			std::cerr << "Error: A device with this name already exists." << std::endl;
			return 0;
		}
	}

	devices.emplace_back(name, ip, mac);

	return 1;
}

void DeviceController::removeDevice(const std::string& name) {
	if(name.empty()) {
		std::cerr << "Error: Name field must be filled." << std::endl;
		return;
	}

	for(auto it = devices.begin(); it != devices.end(); ++it) {
		if(it->getName() == name) {
			devices.erase(it);
			return;
		}
	}

	std::cerr << "Error: No device found with the name '" << name << "'." << std::endl;
}

void DeviceController::loadDevicesBD() {
	sqlite3* db;
	int rc = sqlite3_open(DB_FILE.c_str(), &db);
	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	const char* createTableSQL = "CREATE TABLE IF NOT EXISTS devices ("
		"name TEXT PRIMARY KEY, "
		"ip TEXT, "
		"mac TEXT, "
		"active INTEGER, "
		"captureInterval INTEGER);";

	rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	const char* selectSQL = "SELECT name, ip, mac, active, captureInterval FROM devices;";
	rc = sqlite3_exec(db, selectSQL, loadCallback, this, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
	}

	sqlite3_close(db);
}

int DeviceController::loadCallback(void* data, int argc, char** argv, char** azColName) {
	DeviceController* controller = static_cast<DeviceController*>(data);
	std::string name = argv[0] ? argv[0] : "";
	std::string ip = argv[1] ? argv[1] : "";
	std::string mac = argv[2] ? argv[2] : "";
	bool active = argv[3] ? std::stoi(argv[3]) : false;
	uint16_t interval = argv[4] ? std::stoi(argv[4]) : 0;

	Device device(name, ip, mac);
	device.setInterval(interval);
	device.setActive(active);

	controller->devices.push_back(device);
	return 0;
}

void DeviceController::saveDevicesBD() {
	sqlite3* db;
	int rc = sqlite3_open(DB_FILE.c_str(), &db);
	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	const char* deleteSQL = "DELETE FROM devices;";
	rc = sqlite3_exec(db, deleteSQL, nullptr, nullptr, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	for (const auto& device : devices) {
		std::string insertSQL = "INSERT INTO devices (name, ip, mac, active, captureInterval) VALUES ('" +
			device.getName() + "', '" + device.getIp() + "', '" + device.getMac() + "', " +
			std::to_string(device.isActive()) + ", " + std::to_string(device.getInterval()) + ");";

		rc = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, nullptr);
		if (rc != SQLITE_OK) {
			std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
		}
	}

	sqlite3_close(db);
}