#include <QApplication>
#include "view/interface.hpp"
#include "controller/deviceController.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DeviceController controller;
    Interface window(controller);
    window.show();

    return app.exec();
}