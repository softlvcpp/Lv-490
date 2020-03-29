#include "ClientSocket.h"

ClientSocket::ClientSocket()
{
	socket = new QTcpSocket();
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
		socket->write(std::to_string(message.size()).c_str());
		//socket->waitForBytesWritten(3000);
		socket->flush();
		socket->write(message.c_str(), message.size());
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

