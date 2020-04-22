#pragma once
#include <memory>

#include "ThreadPool.h"
#include "SocketHandler.h"
class Server
{
	ThreadPool m_thread_pool;
	SocketHandler m_socket_handler;
public:
	Server(XMLServer& config);
	void Run();
};