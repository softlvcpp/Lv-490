#include "Client.h"
#include<qdebug.h>
#include<qtimer.h>
#include <QTextEdit>
#include <QScrollBar>
#include"ClientSysInfo.h"
#include<qgraphicsview.h>
#include<qlayout.h>
#include"DefineLogger.h"
#include "../Utility/XML_Parser/XML_Parser.h"
#include<thread>
Client::Client(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.tableWidget->hide();
	setWindowIcon(QIcon("main_icon.png"));
	settings.setModal(true);
	//settings.exec();
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
		//Qt::WindowMinimizeButtonHint |// кнопка сворачивания
		//Qt::WindowMaximizeButtonHint |// кнопка разворачивания
		Qt::WindowCloseButtonHint |//кнопка красный крестик
		Qt::WindowSystemMenuHint /*|
		Qt::WindowContextHelpButtonHint*/);// кнопка-вопросик тут должна появиться, но не появилась

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);


	QGraphicsView view;
	view.setFrameStyle(QFrame::NoFrame);

	tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
	//connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime())); // In thread TODO next demo
	connect(tmr, SIGNAL(timeout()), this, SLOT(runUpdateTime())); // In thread TODO next demo
	tmr->start();

	connect(ui.comboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(indexComboChanged(int)));

	//QActionEvent::action
	//QMenu::actionEvent()
	connect(ui.actionChange_settings, SIGNAL(triggered()),
		this, SLOT(open_settings()));

	indexComboChanged(0);
	qDebug() << "hard disk";


	QString text = "OS  " + QString(client_info2.CalculateOS().c_str()) + '\n' + "CPU vendor " + QString(client_info2.CalculateCPUVendor().c_str()) + '\n';


}
void Client::closeEvent(QCloseEvent* event) {
	qDebug() << "CloseEvent:";
	if (m_th->joinable())
	{
		m_th->join();
	}
	delete m_th;

};

void Client::runUpdateTime()
{
	const unsigned int TimeMeasurement = 1000; // 1000- seconds, 1 = milliseconds, 60000 - minuts...;

	tmr->setInterval(settings.get_TimeInterval() * TimeMeasurement); // Задаем интервал таймера
	if (m_th != nullptr)
	{
		if (m_th->joinable())
		{
			m_th->join();
		}
		delete m_th;
	}
	m_th = new std::thread(&Client::updateTime, this);
}

void Client::updateTime()
{
	//const unsigned int time_measurement = 1000; // 1000- seconds, 1 = milliseconds, 60000 - minuts...;
	//tmr->setInterval(settings.get_TimeInterval() * time_measurement); // Задаем интервал таймера
	QTcpClientSocket m_socket;
	if (m_socket.Init(settings.get_IP().toStdString(), settings.get_port()))
	{
		L_TRACE << "Client socket inited.";
		qDebug()<< "Client socket inited.";

	}
	else
	{
		L_TRACE << "Client socket doesn`t inited.";
		qDebug() << "Client socket doesn`t inited.";
	}

	if (m_socket.Connect()) //connect to host
	{

		L_TRACE << "Client connected to server.";
		qDebug() << "Client connected to server.";
	}
	else
	{
		qDebug() << "Client doesn`t connect to server.";
		qDebug() << m_socket.LastError().c_str();
		L_TRACE << "Client doesn`t connect to server.";
		L_TRACE << m_socket.LastError().c_str();
		
		return;
	}

	client_info2.Update();
	string send_XML_string;
	parser.WriteSystemInformation(send_XML_string, client_info2.get_client_info());// parse in XML string
	L_TRACE << "XML string: ";
	L_TRACE << send_XML_string.c_str();
	qDebug() << send_XML_string.c_str();
	if (m_socket.Send(send_XML_string)) //send information
	{
		qDebug() << "Client sent information.";
		L_TRACE << "Client sent information.";
	}
	else
	{
		qDebug() << "Client doesn`t send information.";
		qDebug() << m_socket.LastError().c_str();
		L_TRACE << m_socket.LastError().c_str();
		L_TRACE << "Client doesn`t send information.";
		
		return;
	}if (m_socket.Disconnect()) //disconnect to host, cloce socket
	{
		qDebug() << "Client diconnect to server.";
		L_TRACE << "Client diconnect to server.";
	}
	else
	{

		L_TRACE << "Client doesn`t diconnect to server.";
		L_TRACE << m_socket.LastError().c_str();
		qDebug() << "Client doesn`t diconnect to server.";
		qDebug() << m_socket.LastError().c_str();
		return;
	}
}
//void Client::actionEvent( QAction actionChange_settings) {
//
//}
//QAction actionChange_settings() {
//	qDebug() << "1111"  ;
//}

void Client::update_processes() {
	qDebug() << "_______________update_processes______________";
	ui.textEdit->hide();
	ui.tableWidget->show();
	client_info2.CalculateProcesses();
	map<int, string> tmp = client_info2.get_Processes();

	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->setRowCount(tmp.size());
	ui.tableWidget->horizontalScrollBar()->setDisabled(true);
	ui.tableWidget->setHorizontalHeaderLabels(QStringList() << "ID" << "Name");

	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//u will dont have permision to edit items
	int i = 0;
	for (map<int, string>::iterator itr = tmp.begin(); itr != tmp.end(); itr++) {
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(itr->first)));
		ui.tableWidget->setItem(i, 1, new QTableWidgetItem(QString(itr->second.c_str())));
		i++;
	}
}


void Client::open_settings() {

	settings.setModal(true);
	settings.exec();
}
void Client::indexComboChanged(int index)
{
	//ui.textEdit->show();
	client_info2.Update();
	ui.tableWidget->hide();
	ui.textEdit->show();
	int num = ui.comboBox->currentIndex();
	//timer->stop();
	if (num == 0) {
		client_info2.CalculateProcesses();
		QString str = "OS: " + QString(client_info2.CalculateOS().c_str()) + '\n';

		str += "CPU vendor: " + QString(client_info2.get_CPUVendor().c_str()) + "\n";
		str += "CPU core number: " + QString::number(client_info2.get_CPUNumbers()) + "\n";
		str += "CPU speed: " + QString::number(client_info2.get_CPUSpeed()) + "MHz\n";
		str += "Total RAM: " + QString::number(client_info2.get_TotalRAM()) + "\n";
		//+"CPU vendor " + QString(client_info2.CalculateCPU_vendor().c_str()) + '\n';
		//QString str;
		std::vector<int> free_size = client_info2.get_HardDisk_Free();
		std::vector<int> total_size = client_info2.get_HardDisk_TotalSize();
		std::vector<int> used_size = client_info2.get_HardDisk_Used();
		std::vector<std::string> volume_vector = client_info2.CalculatevectorLogicDick();//= {"C:\\","D:\\","E:\\" };
		for (int i = 0; i < volume_vector.size(); i++) {
			str += "Hard Disk (" + QString(volume_vector[i].c_str()) + ") " + QString(client_info2.CalculateHardDisk_MediaType(volume_vector[i]).c_str()) + "\n";
			str += " capacity: " + QString::number(client_info2.CalculateCapacity(volume_vector[i])) + QString("GB \n");
			str += " used: " + QString::number(used_size[i]) + QString("GB \n");
			str += " free: " + QString::number(free_size[i]) + QString("GB \n");
		}

		str += "MAC address: " + QString(client_info2.get_MacAddress().c_str()) + "\n";
		str += "IP address: " + QString(client_info2.CalculateIPAddress()) + "\n";
		ui.textEdit->setText(str);


	}
	if (num == 1) {
		QString str;

		std::vector<int> free_size = client_info2.get_HardDisk_Free();
		std::vector<int> total_size = client_info2.get_HardDisk_TotalSize();
		std::vector<int> used_size = client_info2.get_HardDisk_Used();
		std::vector<std::string> volume_vector = client_info2.CalculatevectorLogicDick();//= {"C:\\","D:\\","E:\\" };
		for (int i = 0; i < volume_vector.size(); i++) {
			str += "Hard Disk (" + QString(volume_vector[i].c_str()) + ") " + QString(client_info2.CalculateHardDisk_MediaType(volume_vector[i]).c_str()) + "\n";
			str += " capacity: " + QString::number(client_info2.CalculateCapacity(volume_vector[i])) + QString("GB \n");
			str += " used: " + QString::number(used_size[i]) + QString("GB \n");
			str += " free: " + QString::number(free_size[i]) + QString("GB \n");
		}

		ui.textEdit->setText(str);
		qDebug() << "all";
	}
	if (num == 2) {
		QString str;
		str += "CPU vendor: " + QString(client_info2.get_CPUVendor().c_str()) + "\n";
		str += "CPU core number: " + QString::number(client_info2.get_CPUNumbers()) + "\n";
		str += "CPU speed: " + QString::number(client_info2.get_CPUSpeed()) + "MHz\n";
		ui.textEdit->setText(str);
		qDebug() << "proc";

	}
	if (num == 3) {

		QString str;
		str += "Total RAM: " + QString::number(client_info2.get_TotalRAM()) + "MB\n";
		ui.textEdit->setText(str);
		qDebug() << "ram";



	}
	if (num == 4) {
		QString str;
		str += "MAC address: " + QString(client_info2.get_MacAddress().c_str()) + "\n";
		str += "IP address: " + QString(client_info2.CalculateIPAddress()) + "\n";
		ui.textEdit->setText(str);
	}
	if (num == 5) {
		update_processes();

		//timer = new QTimer(this);
		//connect(timer, SIGNAL(timeout()), this, SLOT(update_processes())); 
		//timer->start();//10 sec -> TODO -> to another const file



	}
	ui.textEdit->ensureCursorVisible();
	// Do something here on ComboBox index change
}