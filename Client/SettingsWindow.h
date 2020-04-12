#pragma once
#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<QString>
namespace Ui {
	class Dialog;
}

class Dialog : public QDialog
{
	Q_OBJECT

public:
	short m_port;
	QString m_ip;
	int m_timeInterval;
	short get_port();
	QString get_IP();
	int get_TimeInterval();
	explicit Dialog(QWidget *parent = nullptr);
	~Dialog();

private:
	Ui::Dialog *ui;
private slots:
	void ButtonClicked();
};

#endif // DIALOG_H
