#include "QTcpClientSocket.h"

QTcpClientSocket::QTcpClientSocket():m_qtcp_socket(new QTcpSocket())
{
}

bool QTcpClientSocket::Connect(const std::string& host_name, const unsigned short port)
{
	const size_t time_connection = 30;// time to wait for a connection
	m_qtcp_socket->connectToHost(QString::fromStdString(host_name), port);
	if (m_qtcp_socket->waitForConnected(time_connection))
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

bool QTcpClientSocket::Send(const std::string &message) 
{
	if (m_qtcp_socket->state() == QAbstractSocket::ConnectedState) {
		const size_t message_size = message.size();
		m_qtcp_socket->write(std::to_string(message_size).c_str(),sizeof(int));
		m_qtcp_socket->write(message.c_str(), message_size);
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

