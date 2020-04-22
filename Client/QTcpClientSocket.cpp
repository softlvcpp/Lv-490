#include "QTcpClientSocket.h"
#include <QHostAddress>
const size_t TIME_CONNECTION= 30;// time to wait for a connection

QTcpClientSocket::QTcpClientSocket()
{
}
bool QTcpClientSocket::Init(const std::string& host_name, const unsigned short port)
{
	m_port = port;
	m_qtcp_socket.reset(new QTcpSocket());
	if (m_qtcp_socket == nullptr)
	{
		return false;
	}
	QHostAddress addr;
	if (!addr.setAddress(host_name.c_str()))
	{
		return false;
	}
	m_host_name = host_name;
	return true;
}

bool QTcpClientSocket::Connect()
{

	m_qtcp_socket->connectToHost(QString::fromStdString(m_host_name), m_port);
	if (m_qtcp_socket->waitForConnected(TIME_CONNECTION))
	{
		return true;
	}
	return false;
}

bool QTcpClientSocket::Disconnect()
{
	if (m_qtcp_socket->state() == QAbstractSocket::ConnectedState)
	{
		m_qtcp_socket->disconnectFromHost();
		m_qtcp_socket->close();
		return true;
	}
	return false;
}

bool QTcpClientSocket::Send(const std::string& message)
{
	if (m_qtcp_socket->state() == QAbstractSocket::ConnectedState) {
		size_t message_size = message.size();
		m_qtcp_socket->write(reinterpret_cast<char*>(&message_size), sizeof(int));
		m_qtcp_socket->write(message.c_str(), message_size);
		m_qtcp_socket->flush();
		return true;
	}
	return false;
}


const std::string QTcpClientSocket::LastError() const // return last error
{
	return m_qtcp_socket->errorString().toStdString();
}

QTcpClientSocket::~QTcpClientSocket()
{}

