#include "ClientSocket.h"

ClientSocket::ClientSocket():socket(new QTcpSocket())
{
}

bool ClientSocket::connect(const QString& host_name, const unsigned int port)
{

	socket->connectToHost(host_name, port);
	if (socket->waitForConnected(3000))
	{
		return true;
	}
	return false;
}

bool ClientSocket::disconnect()
{
	if (socket->state() == QAbstractSocket::ConnectedState)
	{
		socket->disconnectFromHost();
		socket->close();
		return true;
	}
	return false;
}

bool ClientSocket::send(const std::string message)
{
	if (socket->state() == QAbstractSocket::ConnectedState) {
		int message_size = message.size();
		socket->write(std::to_string(message_size).c_str(),sizeof(int));
		socket->write(message.c_str(), message_size);
		return true;
	}
	return false;
}


std::string ClientSocket::lastError() const
{
	return socket->errorString().toStdString();
}

ClientSocket::~ClientSocket()
{
	socket->deleteLater();
}

