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
}

SocketHandler::~SocketHandler()
{
	//closing connections setings
	LOG_T << "Server: Closing Connection";
	WSACleanup();
	m_socket_logger->join();	
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

//bool SocketHandler::InitLoger(const std::string& directory_name)
//{
//	std::string log_file_path = "C:/Lv-490_Files/serv_LOGS/";
//	if (!CreateDirectoryA(log_file_path.c_str(), nullptr))
//	{
//		if (GetLastError() == ERROR_PATH_NOT_FOUND)
//		{
//			return false;
//		}
//	}
//	log_file_path += m_server_configuration->get_filename();
//	filelog::LogLevel log_level = filelog::LogLevel::NoLogs;
//	int level = m_server_configuration->get_loglevel();
//	switch (level)
//	{
//	case 0:
//		log_level = filelog::LogLevel::NoLogs;
//		break;
//	case 1:
//		log_level = filelog::LogLevel::Prod;
//		break;
//	case 2:
//		log_level = filelog::LogLevel::Debug;
//		break;
//	case 3:
//		log_level = filelog::LogLevel::Trace;
//		break;
//	default:
//		break;
//	}
//	m_socket_logger = std::shared_ptr<filelog::FileLogger>(new filelog::FileLogger(log_file_path.c_str(), true, log_level));
//	return true;
//}