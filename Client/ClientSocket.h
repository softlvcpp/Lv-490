#pragma once
#include <QTcpSocket>
class ClientSocket
{
private:
	QScopedPointer<QTcpSocket> socket;
public:
	ClientSocket();
	bool connect(const QString& host_name, const unsigned int port);
	bool disconnect();
	bool send(const std::string message);
	std::string lastError() const;
	~ClientSocket();
};

