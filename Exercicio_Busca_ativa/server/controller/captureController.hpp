#ifndef CAPTURECONTROLLER_HPP
#define CAPTURECONTROLLER_HPP

#include "deviceController.hpp"

class CaptureController {
	private:
		DeviceController* deviceController;

		std::vector<std::thread> captureThreads;

	public:
		CaptureController(DeviceController& deviceController);

		void captureNow(const std::string& deviceName);

		void startScheduledCaptures();
};

#endif // CAPTURECONTROLLER_HPP