#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_LoggerManager.h"
#include "LoggerLine.h"

#include <list>

class LoggerManager : public QMainWindow
{
	Q_OBJECT
		std::list<LoggerLine> m_table;
public:
	LoggerManager(QWidget* parent = Q_NULLPTR);
private slots:
	void ActionOpenClicked();
	void ActionCloseClicked();
	void ActionExitClicked();
	void SearchCancelButtonClicked();
	void SearchText();

private:
	void UpdateTable(std::list<LoggerLine> table);
	void ShowTable(const QString& file_path);

	Ui::LoggerManagerClass ui;

};
