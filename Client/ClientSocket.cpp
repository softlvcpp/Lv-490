#include "ClientSocket.h"

ClientSocket::ClientSocket():m_socket(new QTcpSocket())
{
}

bool ClientSocket::connect(const QString& host_name, const unsigned int port)//connect to host
{

	m_socket->connectToHost(host_name, port);
	if (m_socket->waitForConnected(30))
	{
		return true;
	}
	return false;
}

bool ClientSocket::disconnect()//disconnect to host, cloce socket
{
	if (m_socket->state() == QAbstractSocket::ConnectedState)
	{
		m_socket->disconnectFromHost();
		m_socket->close();
		return true;
	}
	return false;
}

bool ClientSocket::send(const std::string message) //send information
{
	if (m_socket->state() == QAbstractSocket::ConnectedState) {
		int message_size = message.size();
		m_socket->write(std::to_string(message_size).c_str(),sizeof(int));
		m_socket->write(message.c_str(), message_size);
		return true;
	}
	return false;
}


std::string ClientSocket::lastError() const // return last error
{
	return m_socket->errorString().toStdString();
}

ClientSocket::~ClientSocket()
{}

