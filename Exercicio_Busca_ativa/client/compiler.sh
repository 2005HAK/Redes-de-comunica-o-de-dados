#!/bin/bash

sudo apt install  qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qtcreator libqt5widgets5 build-essential

mkdir -p build
rm -rf build/*
cd build
cmake ..
make

QT_QPA_PLATFORM=xcb ./DeviceAgent