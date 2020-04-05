#include "Server.h"

Server::Server(const CXMLParser::OutDocument& config, std::string log_dir_name) :
	m_thread_pool{ std::stoi(config.maxworkingthreads) }
{
	m_socket_handler.set_configuration(std::make_shared<CXMLParser::OutDocument>(config));
	m_socket_handler.InitLoger(log_dir_name);
	m_socket_handler.AddCommand(new AddSocketConnection);
	m_socket_handler.AddCommand(new StartConnection);

}
	

void Server::Run()
{
	m_socket_handler.Run(&m_thread_pool);
}
