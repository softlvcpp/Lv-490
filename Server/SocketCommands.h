#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ThreadPool.h"
#include "SocketState.h"
#include "SocketDeleter.h"

#include "..\Utility\Logger\LoggerDLL.h"
#include "..\Utility\XML_Parser\XML_Parser.h"

#pragma comment(lib, "Ws2_32.lib")

class Command
{
public:
	Command() {};
	virtual bool Execute(SOCKET_shared_ptr& socket_state) = 0;
	
	bool InitConfiguration(CXMLParser::OutDocument* server_configuration);
	bool InitConfiguration(std::shared_ptr<CXMLParser::OutDocument> server_configuration);

protected:
	std::shared_ptr<CXMLParser::OutDocument> m_server_configuration;
	SocketWrapper socket_wrapper;
};

class AddSocketConnection : public Command
{
public:
	AddSocketConnection() {};
	bool Execute(SOCKET_shared_ptr& socket_state) override;
};

class AcceptConnection : public Command
{
public:
	AcceptConnection(SOCKET_shared_ptr server_socket) : m_server_socket(server_socket) {};
	bool Execute(SOCKET_shared_ptr& socket_state) override;
private:
	SOCKET_shared_ptr m_server_socket;
};

class ReceiveMessage : public Command
{
public:
	ReceiveMessage() {};
	bool Execute(SOCKET_shared_ptr& socket_state) override;
};

class StartConnection : public Command
{
public:
	StartConnection() {};
	bool Execute(SOCKET_shared_ptr& socket_state) override;

	bool InitThreadPool(ThreadPool* main_pool);
	bool InitThreadPool(std::shared_ptr<ThreadPool> main_pool);

private:
	std::shared_ptr<ThreadPool> m_thread_pool;
	void DoRecv(SOCKET_shared_ptr& new_conection);
};