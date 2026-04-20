cmake_minimum_required(VERSION 3.10)
project(DeviceAgent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network Multimedia REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(MOSQUITTO REQUIRED libmosquitto)

set(SOURCES
	main.cpp
	agent.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${MOSQUITTO_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} 
	Qt5::Widgets 
	Qt5::Network 
	Qt5::Multimedia
	${MOSQUITTO_LIBRARIES}
)