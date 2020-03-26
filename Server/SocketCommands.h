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
//#include <Mstcpip.h>
#include "ThreadPool.h"

#pragma comment(lib, "Ws2_32.lib")

//using namespace std;

typedef struct
{
	SOCKET id;			//socket handle
	int	state;			//receiving? 0 - not accepted
	std::string buffer;
} SocketState;

constexpr int PORT = 8080;//default port
constexpr int BUFFER_SIZE = 512;//default buffer size
constexpr int LISTEN = 0;
constexpr int ACCEPTED = 1;
constexpr int RECEIVE = 2;

class Command
{
public:
	Command() {};
	virtual bool Execute(SocketState& socket_state) = 0;
	bool InitThreadPool(ThreadPool* main_pool) { m_thread_pool = std::shared_ptr<ThreadPool>(main_pool); return true; };
protected:
	std::shared_ptr<ThreadPool> m_thread_pool;//make_shared не хоче працювати зі ThreadPool
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
	AcceptConnection(SOCKET serv_socket):server_socket(serv_socket) {};
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