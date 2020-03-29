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
	int port;
	QString ip;
	int timeInterval;
	int get_port();
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
