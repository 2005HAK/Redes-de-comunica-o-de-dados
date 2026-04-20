#include "deviceController.hpp"

DeviceController::DeviceController() {
	this->loadDevicesBD();
}

int DeviceController::addDevice(const std::string& name, const std::string& ip, const std::string& mac) {
	if(name.empty() || ip.empty()) {
		std::cerr << "Error: All fields must be filled." << std::endl;
		return 0;
	}

	if(name.length() < 4){
		std::cerr << "Error: The name must have minimum 4 characters";
		return 2;
	}

	std::regex ipRegex(R"(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)");
	if(!std::regex_match(ip, ipRegex)) {
		std::cerr << "Error: Invalid IP address format." << std::endl;
		return 3;
	}

	for(const auto& device : devices) {
		if(device.getIp() == ip || device.getName() == name || (device.getMac() == mac ? !device.getMac().empty() : false)) {
			std::cerr << "Error: A device with this IP, name, or MAC already exists." << std::endl;
			return 0;
		}
	}

	sqlite3* db;
	if (sqlite3_open(DB_FILE.c_str(), &db) != SQLITE_OK) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return 0;
	}

	std::string insertSQL = "INSERT INTO devices (name, ip, mac, active, captureInterval) VALUES ('" + name + "', '" + ip + "', '" + mac + "', 0, 0);";

	if (sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		std::cerr << "SQL error on insert: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return 0;
	}
	sqlite3_close(db);

	devices.emplace_back(name, ip, mac);
	return 1;
}

void DeviceController::removeDevice(const std::string& name) {
	if(name.empty()) return;

	sqlite3* db;
	if (sqlite3_open(DB_FILE.c_str(), &db) != SQLITE_OK) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string deleteSQL = "DELETE FROM devices WHERE name = '" + name + "';";

	if (sqlite3_exec(db, deleteSQL.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		std::cerr << "SQL error on delete: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}
	sqlite3_close(db);

	for(auto it = devices.begin(); it != devices.end(); ++it) {
		if(it->getName() == name) {
			devices.erase(it);
			break; 
		}
	}
}

void DeviceController::updateDeviceInterval(const std::string& name, bool active, int interval) {
	if(name.empty()) return;

	if(interval <= 0 && active == true) this->updateDeviceStatus(name, interval, false);

	sqlite3* db;
	if (sqlite3_open(DB_FILE.c_str(), &db) != SQLITE_OK) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string updateSQL = "UPDATE devices SET captureInterval = " + std::to_string(interval) + 
							" WHERE name = '" + name + "';";

	if (sqlite3_exec(db, updateSQL.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		std::cerr << "SQL error on update: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}
	sqlite3_close(db);

	for(auto& device : devices) {
		if(device.getName() == name) {
			device.setInterval(interval);
			break;
		}
	}
}

void DeviceController::updateDeviceStatus(const std::string& name, int interval, bool active) {
	if(name.empty()) return;

	if(interval <= 0 && active == true) active = false;
	
	sqlite3* db;
	if (sqlite3_open(DB_FILE.c_str(), &db) != SQLITE_OK) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string updateSQL = "UPDATE devices SET active = " + std::to_string(active) + 
							" WHERE name = '" + name + "';";

	if (sqlite3_exec(db, updateSQL.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		std::cerr << "SQL error on update: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}
	sqlite3_close(db);

	for(auto& device : devices) {
		if(device.getName() == name) {
			device.setActive(active);
			break;
		}
	}
	
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

	devices.clear();

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