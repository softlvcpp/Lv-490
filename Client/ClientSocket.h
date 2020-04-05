#pragma once
#include <string>
class ClientSocket
{
public:
	virtual bool Connect(const std::string& host_name, const unsigned short port) = 0;//connect to host
	virtual bool Disconnect()=0;//disconnect to host, cloce socket
	virtual bool Send(const std::string& message)=0;//send information
};

