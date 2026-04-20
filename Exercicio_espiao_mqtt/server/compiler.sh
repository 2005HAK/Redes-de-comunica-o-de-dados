#!/bin/bash
sudo apt install qtbase5-dev qt5-qmake build-essential qtmultimedia5-dev libqt5multimedia5-plugins libmosquitto-dev mosquitto mosquitto-clients -y

echo "Cleaning build directory..."
mkdir -p build
cd build
rm -rf *

echo "Generating Make files..."
cmake ..

echo "Compiling..."
make

if [ $? -eq 0 ]; then
	echo "========================================="
	echo "Compilation completed successfully. Updating the system..."

	cd ..

	systemctl --user stop device-agent.service 2>/dev/null
	killall -9 DeviceAgent 2>/dev/null
	sleep 1

	mkdir -p ~/.local/bin/CaptureScreenClient/
	cp build/DeviceAgent ~/.local/bin/CaptureScreenClient/DeviceAgent

	mkdir -p ~/.config/systemd/user/
	if [ -f "device-agent.service" ]; then
		cp device-agent.service ~/.config/systemd/user/
	fi

	systemctl --user daemon-reload
	systemctl --user enable device-agent.service
	systemctl --user start device-agent.service
	
	echo "Success! Service restarted with the new version of the MQTT Agent."
else
	echo "========================================="
	echo "FATAL ERROR: Compilation failed. The old binary was kept."
	cd ..
fi