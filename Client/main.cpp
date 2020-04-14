#include "Client.h"
#include <QtWidgets/QApplication>
#include <QApplication>
#include <iostream>
#include <QStorageInfo>

#include <filesystem>

#include "ClientSysInfo.h"
#include<qdebug.h>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Client w;
	w.show();
	return a.exec();
}