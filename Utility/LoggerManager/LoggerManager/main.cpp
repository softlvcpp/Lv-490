#include "LoggerManager.h"
#include <QtWidgets/QApplication>
#include <string>
#include <qdebug.h>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	LoggerManager w;
	w.show();
	return a.exec();

}
