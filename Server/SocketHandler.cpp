#include "SocketHandler.h"

#define LOG_T SLOG_TRACE(*m_socket_logger)
#define LOG_D SLOG_DEBUG(*m_socket_logger)
#define LOG_P SLOG_PROD(*m_socket_logger)

SocketHandler::SocketHandler()
{		
	WSAData wsaData;//config sockets
	InitDatabase(false);//ToDO: check mistakes
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{		
		//LOG_T << "Server: Error at WSAStartup()";
	}	
	
}

SocketHandler::~SocketHandler()
{
	//closing connections and Winsock.
	LOG_T << "Server: Closing Connection";
	closesocket(m_socket_state.id);
	WSACleanup();
	LOG_T << "logger end";
	m_socket_logger->join();
	
}

bool SocketHandler::AddCommand(Command* command)
{
	shared_ptr<Command> new_command(command);
	m_commands.push_back(new_command);
	return true;
}

bool SocketHandler::AddCommand(shared_ptr<Command> command)
{
	m_commands.push_back(command);
	return true;
}
bool SocketHandler::InitDatabase(bool is_tables)
{
	std::shared_ptr<DatabaseManager> m_data_base = shared_ptr<DatabaseManager>(new DatabaseManager());
	if (!m_data_base->Connect())
		return false;
	if (is_tables) 
	{
		if (!m_data_base->CreateTables())
			return false;
	}
	return true;
}

bool SocketHandler::Run(ThreadPool* thread_pool)
{
	
	for (auto iter : m_commands)
	{
		iter->InitThreadPool(thread_pool);
		iter->InitConfiguration(m_server_configuration);
		iter->InitDatabase(m_data_base.get());
		if(iter->Execute(m_socket_state) == false)
		{
			LOG_T << m_socket_state.log_msg.c_str();
			return false;
		}
	}
	return true;
}

bool SocketHandler::Run(std::shared_ptr<ThreadPool> thread_pool)// database
{
	for (auto iter : m_commands)
	{
		iter->InitThreadPool(thread_pool);
		iter->InitConfiguration(m_server_configuration);
		iter->InitDatabase(m_data_base.get());
		if (iter->Execute(m_socket_state) == false)
		{
			LOG_T << m_socket_state.log_msg.c_str();
		}
	}
	return true;
}

bool SocketHandler::set_configuration(CXMLParser::OutDocument* server_configuration)
{
	m_server_configuration = std::shared_ptr<CXMLParser::OutDocument>(server_configuration);
	return true;
}

bool SocketHandler::set_configuration(std::shared_ptr<CXMLParser::OutDocument> server_configuration)
{
	m_server_configuration = server_configuration;
	return true;
}

bool SocketHandler::InitLoger(const std::string& directory_name)
{
	std::string log_file_path = "C:/Lv-490_Files/serv_LOGS/";
	if (!CreateDirectoryA(log_file_path.c_str(), nullptr))
	{
		if (GetLastError() == ERROR_PATH_NOT_FOUND)
		{
			return false;
		}
	}
	log_file_path += m_server_configuration->filename;
	filelog::LogLevel log_level = filelog::LogLevel::NoLogs;
	int level = std::stoi(m_server_configuration->loglevel);
	switch (level)
	{
	case 0:
		log_level = filelog::LogLevel::NoLogs;
		break;
	case 1:
		log_level = filelog::LogLevel::Prod;
		break;
	case 2:
		log_level = filelog::LogLevel::Debug;
		break;
	case 3:
		log_level = filelog::LogLevel::Trace;
		break;
	default:
		break;
	}
	m_socket_logger = std::shared_ptr<filelog::FileLogger>(new filelog::FileLogger(log_file_path.c_str(), true, log_level));
	return true;
}