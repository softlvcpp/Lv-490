#pragma once
#include <QTcpSocket>
#include "ClientSocket.h"
class QTcpClientSocket: ClientSocket
{
public:
	explicit QTcpClientSocket();
	bool Connect(const std::string& host_name, const unsigned short port);
	bool Disconnect();
	bool Send(const std::string &message);
	const std::string LastError() const;
	~QTcpClientSocket();
private:
	QScopedPointer<QTcpSocket> m_qtcp_socket;
};

