#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "ThreadPool.h"
#include "SocketState.h"

#include "..\Utility\Logger\LoggerDLL.h"
#include "..\Utility\XML_Parser\XML_Parser.h"

#pragma comment(lib, "Ws2_32.lib")

class Command
{
public:
	Command() {};
	virtual bool Execute(SocketState& socket_state) = 0;
	
	bool InitThreadPool(ThreadPool* main_pool);
	bool InitThreadPool(std::shared_ptr<ThreadPool> main_pool);

	bool InitConfiguration(CXMLParser::OutDocument* server_configuration);
	bool InitConfiguration(std::shared_ptr<CXMLParser::OutDocument> server_configuration);

protected:
	std::shared_ptr<ThreadPool> m_thread_pool;
	std::shared_ptr<CXMLParser::OutDocument> m_server_configuration;
};

class AddSocketConnection : public Command
{
public:
	AddSocketConnection() {};
	bool Execute(SocketState& socket_state);
};

class RemoveSocket : public Command
{
public:
	RemoveSocket() {};
	bool Execute(SocketState& socket_state);
	static bool RemoveUsedSocket(SOCKET& server_socket);
};

class AcceptConnection : public Command
{
public:
	AcceptConnection(SOCKET server_socket):server_socket(server_socket) {};
	bool Execute(SocketState& socket_state);
private:
	SOCKET server_socket;
};

class ReceiveMessage : public Command
{
public:
	ReceiveMessage() {};
	bool Execute(SocketState& socket_state);
};

class StartConnection : public Command
{
public:
	StartConnection() {};
	bool Execute(SocketState& socket_state);
private:
	void DoRecv(SocketState new_conection);
};