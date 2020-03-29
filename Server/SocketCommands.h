#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <Mstcpip.h>
#include "ThreadPool.h"

#include "..\Utility\Logger\LoggerDLL.h"
#include "..\Utility\XML_Parser\XML_Parser.h"

#pragma comment(lib, "Ws2_32.lib")

//using namespace std;

typedef struct
{
	SOCKET id;			//socket handle
	int	state;			//receiving? 0 - LISTEN, 1 - ACCEPTED, 2 - RECEIVEING
	std::string buffer;
	std::string log_msg;
} SocketState;

constexpr int BUFFER_SIZE = 512;//default buffer size

enum
{
	LISTEN,
	ACCEPTED,
	RECEIVE
};

class Command
{
public:
	Command() {};
	virtual bool Execute(SocketState& socket_state) = 0;
	
	bool InitThreadPool(ThreadPool* main_pool);
	bool InitThreadPool(std::shared_ptr<ThreadPool> main_pool);

	bool InitConfiguration(CXMLParser::outDocument* server_configuration);
	bool InitConfiguration(std::shared_ptr<CXMLParser::outDocument> server_configuration);

protected:
	std::shared_ptr<ThreadPool> m_thread_pool;
	std::shared_ptr<CXMLParser::outDocument> m_server_configuration;
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