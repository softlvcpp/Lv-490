#include "SocketHandler.h"

#define LOG_T SLOG_TRACE(*m_logger)
#define LOG_D SLOG_DEBUG(*m_logger)
#define LOG_P SLOG_PROD(*m_logger)

SocketHandler::SocketHandler(const std::string& file_name, const std::string& directory_name):m_log_file_name(file_name), m_log_directory_name(directory_name)
{	
	InitLoger();	
	WSAData wsaData;//config sockets
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		LOG_T << "Server: Error at WSAStartup()";
	}	
}

SocketHandler::~SocketHandler()
{
	//closing connections and Winsock.
	LOG_T << "Server: Closing Connection";
	closesocket(m_socket_state.id);
	WSACleanup();
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

bool SocketHandler::Run(ThreadPool* thread_pool)
{
	for (auto iter : m_commands)
	{
		WSAData wsaData;//config sockets
		if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			LOG_T << "Server: Error at WSAStartup()";
		}
		iter->InitThreadPool(thread_pool);
		if(iter->Execute(m_socket_state) == false)
		{
			LOG_T << m_socket_state.log_msg.c_str();
		}
	}
	return true;
}

bool SocketHandler::InitLoger()
{
	std::string log_file_path = "C:/Users/";
	char user_name[USERNAME_LEN];
	unsigned long len = USERNAME_LEN;
	if (!GetUserNameA(user_name, &len))
	{
		return false;
	}
	log_file_path += user_name;
	log_file_path += "/";
	log_file_path += m_log_directory_name;
	log_file_path += "/";
	if (!CreateDirectoryA(log_file_path.c_str(), nullptr))
	{
		if (GetLastError() == ERROR_PATH_NOT_FOUND)
		{
			return false;
		}
	}
	log_file_path += m_log_file_name;
	m_logger = std::unique_ptr<filelog::FileLogger>(new filelog::FileLogger(log_file_path.c_str(), filelog::LogLevel::Trace));
	return true;
}