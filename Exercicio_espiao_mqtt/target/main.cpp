#include <QApplication>
#include "agent.hpp"

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	Agent agent;

	return app.exec();
}