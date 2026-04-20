#ifndef DEVICE_HPP
#define DEViCE_HPP

#include <string>
#include <cstdint>
#include <iostream>

class Device {
	private:
		std::string name;
		std::string ip;
		std::string mac;

		bool active;

		int captureInterval;

	public:
		Device(std::string name, std::string ip, std::string mac) : name(name), ip(ip), mac(mac), active(false), captureInterval(0){}

		std::string getName() const { return this->name; }
		std::string getIp() const { return this->ip; }
		std::string getMac() const { return this->mac; }
		int getInterval() const { return this->captureInterval; }

		bool isActive() const { return this->active; }

		void setInterval(int min) { this->captureInterval = min; }

		void setActive(bool active) { this->active = active; }
};

#endif // DEVICE_HPP