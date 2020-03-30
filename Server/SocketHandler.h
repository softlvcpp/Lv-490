#pragma once

#include <memory>
#include "SocketCommands.h"

using namespace std;

constexpr int USERNAME_LEN = 20;

class SocketHandler
{
public:
	SocketHandler();
	~SocketHandler();
	
	bool AddCommand(Command* command);
	bool AddCommand(shared_ptr<Command> command);

	bool Run(ThreadPool* thread_pool);
	bool Run(std::shared_ptr<ThreadPool> thread_pool);

	bool set_configuration(CXMLParser::OutDocument* server_configuration);
	bool set_configuration(std::shared_ptr<CXMLParser::OutDocument> server_configuration);

	bool InitLoger(const std::string& directory_name);

private:
	std::string m_log_directory_name{ "Lv-490_logs" };
	std::shared_ptr<filelog::FileLogger> m_socket_logger;

	std::shared_ptr<CXMLParser::OutDocument> m_server_configuration;

	vector<shared_ptr<Command>> m_commands;
	SocketState m_socket_state;
};

