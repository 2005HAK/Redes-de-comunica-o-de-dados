#!/bin/bash

sudo apt install  qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qtcreator libqt5widgets5 qtmultimedia5-dev libqt5multimedia5-plugins build-essential

mkdir -p build
rm -rf build/*
cd build
cmake ..
make

mkdir -p ~/.local/bin/CaptureScreenClient
cp DeviceAgent ~/.local/bin/CaptureScreenClient/
cd ..
mkdir -p ~/.config/systemd/user/
cp capturescreenclient.service ~/.config/systemd/user/

systemctl --user daemon-reload
systemctl --user enable capturescreenclient.service