#include <QApplication>
#include "view/interface.hpp"
#include "controller/deviceController.hpp"
#include "controller/captureController.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DeviceController controller;
    CaptureController captureController(controller);
    Interface window(controller, captureController);
    window.show();

    return app.exec();
}