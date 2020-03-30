#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ClientWindow.h"
#include"SettingsWindow.h"
#include"ClientSysInfo.h"
#include<qmenubar.h>
#include"ClientSocket.h"
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
	ClientSocket socket;
	CXMLParser::XMLParser parser;
public slots:
	void open_settings();
	void indexComboChanged(int index);
	void updateTime(); //slot for updating function(connection to server)
};
