#pragma once

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

	bool Run();
	bool set_configuration(XMLServer* server_configuration);
	bool set_configuration(std::shared_ptr<XMLServer> server_configuration);

	bool InitLoger(const std::string& directory_name);

private:
	std::string m_log_directory_name{ "Lv-490_logs" };
	std::shared_ptr<filelog::FileLogger> m_socket_logger;

	std::shared_ptr<DatabaseManager> m_database;
	std::shared_ptr<XMLServer> m_server_configuration;

	vector<shared_ptr<Command>> m_commands;
	SOCKET_shared_ptr m_server_socket;
};

