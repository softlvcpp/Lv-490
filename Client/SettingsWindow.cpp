#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

Dialog::Dialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dialog)
{
	ui->setupUi(this);
	setWindowIcon(QIcon("settings_icon2.png"));
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint|Qt::CustomizeWindowHint | Qt::WindowTitleHint |
		Qt::WindowCloseButtonHint |	Qt::WindowSystemMenuHint);// different flags for windows
	//setWindowFlags( Qt::MSWindowsFixedSizeDialogHint);
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonClicked()));
	ButtonClicked();
	//Settings
	//port=ui->lineEdit_7->text().toInt();
}
short Dialog::get_port() {
	return m_port;
}

QString Dialog::get_IP() {
	return  m_ip;
}
int Dialog::get_TimeInterval() {
	return  m_timeInterval;
}

void Dialog::ButtonClicked() {
	this->m_port = ui->lineEdit->text().toShort();
	this->m_ip= ui->lineEdit_7->text();
	this->m_timeInterval = ui->lineEdit_3->text().toInt();
	this->close();
}
Dialog::~Dialog()
{
	delete ui;
}
