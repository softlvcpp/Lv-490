#pragma once
#include <QTcpSocket>
#include "ClientSocket.h"
class QTcpClientSocket: public ClientSocket
{
public:
	explicit QTcpClientSocket();
	bool Init(const std::string& host_name, const unsigned short port);
	bool Connect();
	bool Disconnect();
	bool Send(const std::string &message);
	const std::string LastError() const;
	~QTcpClientSocket();
private:
	QScopedPointer<QTcpSocket> m_qtcp_socket;
	std::string m_host_name;
	unsigned short m_port;
};

