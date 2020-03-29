#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

Dialog::Dialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dialog)
{
	
	ui->setupUi(this);
	setWindowIcon(QIcon("settings_icon2.png"));
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint | Qt::WindowTitleHint |
		//Qt::WindowMinimizeButtonHint |// кнопка сворачивания
		//Qt::WindowMaximizeButtonHint |// кнопка разворачивания
		Qt::WindowCloseButtonHint |//кнопка красный крестик
		Qt::WindowSystemMenuHint/*|
		Qt::WindowContextHelpButtonHint*/);// кнопка-вопросик тут должна появиться, но не появилась
	//setWindowFlags( Qt::MSWindowsFixedSizeDialogHint);
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonClicked()));
	ButtonClicked();
	//Settings
	//port=ui->lineEdit_7->text().toInt();
}
int Dialog::get_port() {
	return port;
}

QString Dialog::get_IP() {
	return ip;
}
int Dialog::get_TimeInterval() {
	return timeInterval;
}

void Dialog::ButtonClicked() {
	this->port = ui->lineEdit->text().toInt();
	this->ip= ui->lineEdit_7->text();
	this->timeInterval = ui->lineEdit_3->text().toInt();
	this->close();
}
Dialog::~Dialog()
{
	delete ui;
}
