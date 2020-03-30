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

Client::Client(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	setWindowIcon(QIcon("main_icon.png"));
	settings.setModal(true);
	//settings.exec();
	setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint |
		//Qt::WindowMinimizeButtonHint |// кнопка сворачивания
		//Qt::WindowMaximizeButtonHint |// кнопка разворачивания
		Qt::WindowCloseButtonHint |//кнопка красный крестик
		Qt::WindowSystemMenuHint /*|
		Qt::WindowContextHelpButtonHint*/);// кнопка-вопросик тут должна появиться, но не появилась

//	QLayout::setSizeConstraint(3);
	//QLayout *d = layout();
	//d.set;
	//d->setSizeConstraint(QLayout::SetMaximumSize);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	//setWindowFlags(Qt::CustomizeWindowHint /*| Qt::WindowTitleHint*/ /*| Qt::WindowMinimizeButtonHint*/);
//	ui.label_2->setText("234234");
	//ClientSysInfo client_info2;
	
	QGraphicsView view;
	view.setFrameStyle(QFrame::NoFrame);
	//QTimer *tmr;
	//ui.setupUi(this);
	tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
	connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime())); // Подключаем сигнал таймера к нашему слоту
	tmr->start();

	connect(ui.comboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(indexComboChanged(int)));
	//QActionEvent::action
	//QMenu::actionEvent()
	connect(ui.actionChange_settings, SIGNAL(triggered()),
		this, SLOT(open_settings()));
	/*connect(ui.comboBox, QOverload<int>::of(&QComboBox::activated),
		[=](int index) {ui.textEdit->setText("proba"); });*/
	//qDebug() << "Logical drives ";
	//QString str;
	//str += "OS  " + QString(client_info2.CalculateOS_name().c_str())+"\n";
	//std::vector<int> free_size = client_info2.get_HardDisk_Free();
	//std::vector<int> total_size = client_info2.get_HardDisk_TotalSize();
	//std::vector<int> used_size = client_info2.get_HardDisk_Used();
	//std::vector<std::string> volume_vector = client_info2.Calculatevector_logic_dick();//= {"C:\\","D:\\","E:\\" };
	//for (int i = 0; i < volume_vector.size(); i++) {
	//	str += "Hard Disk (" + QString(volume_vector[i].c_str()) + ") \n";Calculate
	//	str += " capacity:" + QString::number(client_info2.Calculatecapacity(volume_vector[i])) + QString("GB \n");
	//	str += " used:" + QString::number(used_size[i]) + QString("GB \n");
	//	str += " free:" + QString::number(free_size[i]) + QString("GB \n");
	//}
	//ui.textEdit->setText(str);

	indexComboChanged(0);
	qDebug() << "hard disk";

	//ClientSysInfo client_info2;
	QString text = "OS  " + QString(client_info2.CalculateOS_name().c_str()) + '\n' + "CPU vendor " + QString(client_info2.CalculateCPU_vendor().c_str()) + '\n';
//	ui.label_2->setText(text);

}
//void Client::smth_triggered(QAction::set) {
//
//}
void Client::updateTime()
{

	tmr->setInterval(settings.get_TimeInterval() * 1000); // Задаем интервал таймера
	//qDebug() << "_____SENDING TO SERVER_____";
	//qDebug() << settings.get_port();
	//qDebug() << settings.get_IP();
	//qDebug() << settings.get_TimeInterval()<<" s";
	//tmr->setInterval(settings.get_TimeInterval() * 1000); // Задаем интервал таймера
	client_info2.Update();
	if (socket.connect(settings.get_IP(), settings.get_port())) {

		L_TRACE << "Client connected to server.";
		qDebug() << "Client connected to server.";
	}
	else
	{
		qDebug() << "Client doesn`t connect to server.";
		qDebug() << socket.lastError().c_str();
		L_TRACE << "Client doesn`t connect to server.";
		L_TRACE << socket.lastError().c_str();
		return;
	}

	//string send_XML_string = "XML string must be here";
	string send_XML_string;
	parser.WriteSystemInformation(send_XML_string, client_info2.get_client_info());
	L_TRACE << "XML string: ";
	L_TRACE << send_XML_string.c_str();
	//qDebug() << send_XML_string.c_str();
	if (socket.send(send_XML_string))
	{
		qDebug() << "Client sent information.";
		L_TRACE << "Client sent information.";
	}
	else
	{
		qDebug() << "Client doesn`t send information.";
		qDebug() << socket.lastError().c_str();
		L_TRACE << socket.lastError().c_str();
		L_TRACE << "Client doesn`t send information.";

		return;
	}if (socket.disconnect())
	{
		qDebug() << "Client diconnect to server.";
		L_TRACE << "Client diconnect to server.";
	}
	else
	{

		L_TRACE << "Client doesn`t diconnect to server.";
		L_TRACE << socket.lastError().c_str();
		qDebug() << "Client doesn`t diconnect to server.";
		qDebug() << socket.lastError().c_str();
		return;
	}

}
//void Client::actionEvent( QAction actionChange_settings) {
//
//}
//QAction actionChange_settings() {
//	qDebug() << "1111"  ;
//}

void Client::open_settings() {

	settings.setModal(true);
	settings.exec();
}
void Client::indexComboChanged(int index)
{
	client_info2.Update();
	int num= ui.comboBox->currentIndex();
	if (num == 0) {

		QString str = "OS: " + QString(client_info2.CalculateOS_name().c_str()) + '\n';

		str += "CPU vendor: " + QString(client_info2.get_CPUVendor().c_str()) + "\n";
		str += "CPU core number: " + QString::number(client_info2.get_CPUNumbers()) + "\n";
		str += "CPU speed: " + QString::number(client_info2.get_CPUSpeed()) + "MHz\n";
		str += "Total RAM: " + QString::number(client_info2.get_TotalRAM()) + "\n";
		//+"CPU vendor " + QString(client_info2.CalculateCPU_vendor().c_str()) + '\n';
		//QString str;
		std::vector<int> free_size = client_info2.get_HardDisk_Free();
		std::vector<int> total_size = client_info2.get_HardDisk_TotalSize();
		std::vector<int> used_size = client_info2.get_HardDisk_Used();
		std::vector<std::string> volume_vector = client_info2.Calculatevector_logic_dick();//= {"C:\\","D:\\","E:\\" };
		for (int i = 0; i < volume_vector.size(); i++) {
			str += "Hard Disk (" + QString(volume_vector[i].c_str()) + ") " + QString(client_info2.CalculateHardDisk_MediaType(volume_vector[i]).c_str())+ "\n";
				str += " capacity: " + QString::number(client_info2.Calculatecapacity(volume_vector[i]))+QString("GB \n");
				str += " used: " + QString::number(used_size[i]) + QString("GB \n");
				str += " free: " + QString::number(free_size[i])+ QString("GB \n");
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
		std::vector<std::string> volume_vector = client_info2.Calculatevector_logic_dick();//= {"C:\\","D:\\","E:\\" };
		for (int i = 0; i < volume_vector.size(); i++) {
			str += "Hard Disk (" + QString(volume_vector[i].c_str()) + ") " + QString(client_info2.CalculateHardDisk_MediaType(volume_vector[i]).c_str()) + "\n";
			str += " capacity: " + QString::number(client_info2.Calculatecapacity(volume_vector[i])) + QString("GB \n");
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
	ui.textEdit->ensureCursorVisible();
	// Do something here on ComboBox index change
}