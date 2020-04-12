#pragma once
#include <string>
class ClientSocket
{
public:
	virtual bool Init(const std::string& host_name, const unsigned short port) = 0;//initialize
	virtual bool Connect() = 0;//connect to host
	virtual bool Disconnect()=0;//disconnect to host, cloce socket
	virtual bool Send(const std::string& message)=0;//send information
	virtual const std::string LastError() const = 0;//return last socket error

};

