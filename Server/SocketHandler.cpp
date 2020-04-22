#include "pch.h"
#include "SocketHandler.h"

#define LOG_T GLOG_T
#define LOG_D GLOG_D
#define LOG_P GLOG_P

constexpr int LOW_BYTE = 2;
constexpr int HIGH_BYTE = 2;

SocketHandler::SocketHandler()
{	
	//configurate sockets
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(LOW_BYTE, HIGH_BYTE), &wsaData))
	{		
		LOG_T << "Server: Error at WSAStartup()";
	}		
	LOG_T << "Socket configured";
}

SocketHandler::~SocketHandler()
{
	//closing connections setings
	if (SOCKET_ERROR == WSACleanup())
	{
		LOG_T << "Server: bad socket configuration cleanup";
	}
	LOG_T << "Server: Closing Connection";
}

bool SocketHandler::AddCommand(Command* command)
{
	if (command == nullptr) return false;
	shared_ptr<Command> new_command(command);
	m_commands.push_back(new_command);
	return true;
}

bool SocketHandler::AddCommand(shared_ptr<Command> command)
{
	if (command == nullptr) return false;
	m_commands.push_back(command);
	return true;
}


bool SocketHandler::Run()
{
	for (auto iter : m_commands)
	{
		iter->InitConfiguration(m_server_configuration);

		if(iter->Execute(m_server_socket) == false)
		{
			LOG_T << m_server_socket->log_msg.c_str();
			return false;
		}
	}
	return true;
}


bool SocketHandler::set_configuration(XMLServer* server_configuration)
{
	if (server_configuration == nullptr) return false;
	m_server_configuration = std::shared_ptr<XMLServer>(server_configuration);
	return true;
}

bool SocketHandler::set_configuration(std::shared_ptr<XMLServer> server_configuration)
{
	if (server_configuration == nullptr) return false;
	m_server_configuration = server_configuration;
	return true;
}
