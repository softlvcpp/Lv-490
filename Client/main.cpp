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
	//ClientSysInfo client_info;

	//QString text = "OS  " + QString(client_info.get_OS().c_str()) + '\n' + "CPU vendor " + QString(client_info.get_CPUVendor().c_str()) + '\n';
	//qDebug() << "OS  " << QString(client_info.get_OS().c_str());
	//qDebug() << "CPU vendor "<<QString(client_info.get_CPUVendor().c_str());
	//qDebug() << "CPU Numbers "<<client_info.get_CPUNumbers(); 
	//qDebug() << "CPU speed " << client_info.get_CPUSpeed();
	//qDebug() << "RAM " << client_info.get_TotalRAM();
	//qDebug() << "Logical drives ";
	//std::vector<std::string> volume_vector = client_info.get_HardDisk_type_list();//= {"C:\\","D:\\","E:\\" };
	//std::vector<std::string> volume_MediaType = client_info.get_HardDisk_MediaType();
	//for (int i = 0; i < volume_vector.size(); i++) {
	//	qDebug() << QString(volume_vector[i].c_str()) << " " << QString(volume_MediaType[i].c_str()) << " capacity:";
	//}
	//qDebug() << "MAC adresses:"<<client_info.get_MacAddress().c_str();
	//qDebug() << "IP adresses:"<<client_info.get_IPAddress().c_str();

	return a.exec();
}