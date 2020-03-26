#pragma once

#include <memory>
#include "SocketCommands.h"
#include "..\Utility\Logger\LoggerDLL.h"

using namespace std;

constexpr int USERNAME_LEN = 20;

class SocketHandler
{
public:
	SocketHandler(const std::string& file_name, const std::string& directory_name);
	~SocketHandler();
	
	bool AddCommand(Command* command);
	bool AddCommand(shared_ptr<Command> command);
	bool Run(ThreadPool* thread_pool);

	bool InitLoger();

private:
	std::string m_log_file_name{ "serverlog.log" };
	std::string m_log_directory_name{ "Lv-490_logs" };
	std::unique_ptr<filelog::FileLogger> m_logger;

	vector<shared_ptr<Command>> m_commands;
	SocketState m_socket_state;
};

