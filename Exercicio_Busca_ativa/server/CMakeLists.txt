cmake_minimum_required(VERSION 3.10)
project(DevicesManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

include_directories(model view controller ${SQLITE3_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    view/interface.cpp
    controller/deviceController.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
    Qt5::Widgets 
    Qt5::Network 
    ${SQLITE3_LIBRARIES}
)