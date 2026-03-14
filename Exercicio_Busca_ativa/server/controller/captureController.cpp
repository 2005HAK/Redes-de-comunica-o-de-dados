#include "captureController.hpp"

CaptureController::CaptureController(DeviceController& deviceController) : deviceController(&deviceController){
	this->startScheduledCaptures();
}

void CaptureController::captureNow(const std::string& deviceName) {
	std::vector<Device> devices = deviceController->getDevices();
	for(const auto& device : devices) {
		if(device.getName() == deviceName) {
			
		}
	}
}

void CaptureController::startScheduledCaptures() {
	std::vector<Device> devices = deviceController->getDevices();
	for(const auto& device : devices) {
		if(device.isActive() && device.getInterval() > 0) {
			captureThreads.emplace_back([this, device]() {
				while(device->isActive()) {
					std::this_thread::sleep_for(std::chrono::minutes(device.getInterval()));
					this->captureNow(device.getName());
				}
			});
		}
	}
}