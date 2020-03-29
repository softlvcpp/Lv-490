#pragma once

#include <memory>
#include "SocketCommands.h"

using namespace std;

constexpr int USERNAME_LEN = 20;

class SocketHandler
{
public:
	SocketHandler(const std::string& directory_name);
	~SocketHandler();
	
	bool AddCommand(Command* command);
	bool AddCommand(shared_ptr<Command> command);

	bool Run(ThreadPool* thread_pool);
	bool Run(std::shared_ptr<ThreadPool> thread_pool);

	bool set_configuration(CXMLParser::outDocument* server_configuration);
	bool set_configuration(std::shared_ptr<CXMLParser::outDocument> server_configuration);

	bool InitLoger();

private:
	std::string m_log_directory_name{ "Lv-490_logs" };
	std::unique_ptr<filelog::FileLogger> m_logger;

	std::shared_ptr<CXMLParser::outDocument> m_server_configuration;

	vector<shared_ptr<Command>> m_commands;
	SocketState m_socket_state;
};

