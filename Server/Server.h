#pragma once
#include <memory>

#include "ThreadPool.h"
#include "SocketHandler.h"
#include "../Utility/XML_Parser/XMLServer.h"
class Server
{
	ThreadPool m_thread_pool;
	SocketHandler m_socket_handler;
public:
	Server(const XMLServer& config, std::string log_dir_name);
	void Run();
};