#include "LoggerManager.h"
#include <qfiledialog.h>
#include <QMessageBox>
#include <QTableWidgetItem >
#include <QStandardItemModel>
#include <qobject.h>
#include <Qt>

#include <iterator>
#include <algorithm>
#include <fstream>
#include <string>

#define DEBUG "DEBUG"
#define PROD "PROD"
enum HeadIndex
{
	THREAD_ID,
	DATA_TIME,
	LOG_LEVEL,
	LOG_PLACE,
	MESSAGE,
	COLUMN_COUNT
};

LoggerManager::LoggerManager(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon("C:/Users/bogda/Downloads/multimedia.png"));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(ActionOpenClicked()));
	connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(ActionCloseClicked()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(ActionExitClicked()));
	connect(ui.searchCancelButton, SIGNAL(clicked()), this, SLOT(SearchCancelButtonClicked()));
	connect(ui.searchLine, SIGNAL(textChanged(const QString)), this, SLOT(SearchText()));
	connect(ui.tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), ui.tableView, SLOT(sortByColumn(int)));
}
void LoggerManager::ActionOpenClicked() {

	QString file_path = "C:/SoftServe/Lv-490/Client/clientlog.txt";
	//QString file_path = QFileDialog::getOpenFileName(this, "Open File", "", "Text file (*.txt)");

	if (file_path.isEmpty())
	{
		QMessageBox::information(this, tr("Error "), "File path is empty!");
	}
	else
	{
		ShowTable(file_path);
	}
}

void LoggerManager::ActionCloseClicked()
{
	m_table.clear();
	this->UpdateTable(m_table);
}

void LoggerManager::ActionExitClicked()
{
	this->close();
}

void LoggerManager::SearchCancelButtonClicked()
{
	ui.searchLine->setText("");
	this->UpdateTable(m_table);
}

void LoggerManager::SearchText()
{
	std::list< LoggerLine> search_table;
	search_table.resize(m_table.size());
	const std::string text = ui.searchLine->text().toStdString();

	auto end_search_table = std::copy_if(m_table.begin(), m_table.end(), search_table.begin() // copy LoggerLine where find text
		, [&text](const LoggerLine& logger_line)
		{ return logger_line.IsIn(text); });

	search_table.resize(std::distance(search_table.begin(), end_search_table));
	this->UpdateTable(search_table);
}



void LoggerManager::UpdateTable(std::list< LoggerLine> table)
{
	if (ui.tableView->model())
	{
		ui.tableView->model()->destroyed();
	}

	QStandardItemModel* model = new QStandardItemModel();
	model->setColumnCount(COLUMN_COUNT);
	model->setRowCount(table.size());
	const char* column_thread_id = "Thread id";
	const char* column_date_time = "DateTime";
	const char* column_log_level = "Level";
	const char* column_log_place = "Function";
	const char* column_message = "Message";
		model->setHorizontalHeaderLabels(QStringList() 
			<< column_thread_id 
			<< column_date_time 
			<< column_log_level 
			<< column_log_place
			<< column_message);

	QStandardItem* item;

	int row_index = 0;
	for (auto& i : table)
	{
		QColor background = Qt::black;
		if (i.get_log_level() == DEBUG)
		{
			background = Qt::darkBlue;
		}
		else if (i.get_log_level() == PROD)
		{
			background = Qt::darkGreen;
		}

		item = new QStandardItem();
		item->setData(std::stoi(i.get_thread_id()), QVariant::Int);
		item->setForeground(background);
		model->setItem(row_index, THREAD_ID, item);

		item = new QStandardItem(i.get_data_time().c_str());
		item->setForeground(background);
		model->setItem(row_index, 1, item);

		item = new QStandardItem(i.get_log_level().c_str());
		item->setForeground(background);
		model->setItem(row_index, LOG_LEVEL, item);

		item = new QStandardItem(i.get_log_place().c_str());
		item->setForeground(background);
		model->setItem(row_index, LOG_PLACE, item);

		item = new QStandardItem(i.get_message().c_str());
		item->setForeground(background);
		model->setItem(row_index, MESSAGE, item);

		row_index++;
	}
	ui.tableView->setModel(model);
}


void LoggerManager::ShowTable(const QString& file_path)
{
	std::ifstream file(file_path.toStdString().c_str());
	std::string line;
	while (!file.eof())
	{
		getline(file, line);
		;
		if (!line.empty())
		{
			m_table.emplace_front(LoggerLine(line));
		}
	}
	this->UpdateTable(m_table);
}
