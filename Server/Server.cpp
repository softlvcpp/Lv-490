#include "pch.h"
#include "Server.h"


Server::Server(XMLServer& config) :
	m_thread_pool{ config.get_maxworkingthreads() }
{
	m_socket_handler.set_configuration(&config);
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
