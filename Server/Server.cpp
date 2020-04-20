#include "pch.h"
#include "Server.h"

Server::Server(const XMLServer& config, std::string log_dir_name) :
	m_thread_pool{ std::stoi(config.get_maxworkingthreads()) }
{
	m_socket_handler.set_configuration(std::make_shared<XMLServer>(config));
	m_socket_handler.InitLoger(log_dir_name);
	AddSocketConnection* add_socket_connection = new AddSocketConnection;
	m_socket_handler.AddCommand(add_socket_connection);
	StartConnection* start_connection = new StartConnection;
	start_connection->InitThreadPool(&m_thread_pool);
	m_socket_handler.AddCommand(start_connection);
}
	

void Server::Run()
{
	m_socket_handler.Run();
}
