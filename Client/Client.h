#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ClientWindow.h"
#include"SettingsWindow.h"
#include"ClientSysInfo.h"
#include<qmenubar.h>
#include"QTcpClientSocket.h"
class Client : public QMainWindow
{
	Q_OBJECT

public:
	Client(QWidget *parent = Q_NULLPTR);
	ClientSysInfo client_info2;
private:
	Ui::Client ui;
	Dialog settings;
	QTimer *tmr;//timer for timecycle connection to server
	QTimer* timer;//for processes
	CXMLParser::XMLParser parser;
	std::thread* m_th;
public slots:
	void closeEvent(QCloseEvent* event);//performed by pressing the red cross button
	void runUpdateTime();// runs updateTime() in a separate thread
	void open_settings();
	void indexComboChanged(int index);
	void updateTime(); //slot for updating function(connection to server)
	void update_processes();//update list of processes
};
