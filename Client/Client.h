#pragma once
#include <QtWidgets/QMainWindow>
#include<qmenubar.h>
#include "ui_ClientWindow.h"
#include"SettingsWindow.h"
#include"ClientSysInfo.h"
#include"QTcpClientSocket.h"

//some constants 
const int PROCESS_UPDATE_DURATION = 5000;
const int PROCESS_COLUMN_COUNT = 2;
const unsigned int TIME_MEASUREMENT= 1000; // 1000- seconds, 1 = milliseconds, 60000 - minuts...;


class Client : public QMainWindow
{
	Q_OBJECT

public:
	Client(QWidget *parent = Q_NULLPTR);
	ClientSysInfo client_info;
private:
	Ui::Client ui;
	Dialog settings;
	QTimer *tmr;//timer for timecycle connection to server
	QTimer* timer;//for processes
	
	std::thread* m_th=nullptr;
public slots:
	void closeEvent(QCloseEvent* event);//performed by pressing the red cross button
	void SendToServerThread();// runs updateTime() in a separate thread
	void UpdateProccesThread();//runs update_processes in a thread
	void open_settings();
	void indexComboChanged(int index);
	bool SendToServer(); //slot for updating function(connection to server)
	void UpdateProcesses();//update list of processes
};
