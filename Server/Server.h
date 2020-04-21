#pragma once
#include <memory>

#include "ThreadPool.h"
#include "SocketHandler.h"
class Server
{
	ThreadPool m_thread_pool;
	SocketHandler m_socket_handler;
public:
	Server(XMLServer& config, const std::string& log_dir_name);
	void Run();
};