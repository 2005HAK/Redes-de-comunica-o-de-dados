#ifndef DEVICE_HPP
#define DEViCE_HPP

#include <string>

class Device {
	private:
		std::string name;
		std::string ip;
		std::string mac;

		bool active;

		uint16_t captureInterval;

	public:
		Device(std::string name, std::string ip, std::string mac) : name(name), ip(ip), mac(mac), active(false), captureInterval(0){}

		std::string getName() const { return this->name; }
		std::string getIp() const { return this->ip; }
		std::string getMac() const { return this->mac; }
		uint16_t getInterval() const { return this->captureInterval; }

		bool isActive() const { return this->active; }

		void setInterval(uint16_t min) { this->captureInterval = min; }

		bool setActive(bool active) { this->active = active; }
};

#endif // DEVICE_HPP