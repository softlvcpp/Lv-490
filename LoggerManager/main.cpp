#include "LoggerManager.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	LoggerManager w;
	w.show();
	return a.exec();
}
