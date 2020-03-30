#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <fstream>
#include <mutex>
#include <condition_variable>

#include "SocketHandler.h"

#include <Windows.h>

//#include "../Utility/Logger/LoggerDLL.h"

#define USERNAME_LEN 20
/*
 * This class is implemented as singletone
 * The class functions don`t throw exceptions
 */
class Server
{
// File IO members and service commands
private:
	std::string m_install_command  { "install" };
	std::string m_uninstall_command{ "uninstall" };
	std::string m_start_command    { "start" };
	std::string m_stop_command     { "stop" };
	std::string m_restart_command  { "restart" };

	std::string m_log_file_name{ "serverlog.log" };
	std::string m_log_directory_name{ "Lv-490_logs" };
	std::unique_ptr<filelog::FileLogger> m_logger;
	filelog::LogLevel m_log_level;
	std::unique_ptr<wchar_t> m_name;
//Windows Service members
private:
	SERVICE_STATUS m_service_status{};
	SERVICE_STATUS_HANDLE m_service_status_handle{};
	HANDLE m_service_stop_event{ INVALID_HANDLE_VALUE };

	static std::shared_ptr<Server> s_instance;
// Parser members
private:
	CXMLParser::XMLParser m_parser;
	std::string m_config_file_name{ "D:/programing/softserve/Lv-490/Server/config.xml" };
	bool ReadConfig();
	std::ofstream fout{ "D:/programing/softserve/Lv-490/temp_log.txt", std::ios::app };

// Server members
private:
	std::string m_server_IP;
	std::string m_server_listenport;
	unsigned m_max_threads;
public:
	static bool Run(int argc, char** argv);
	static Server& get_instance();
	void set_log_dir_name(std::string log_dir_name);
	void set_config_file_name(std::string file_name);

	~Server();

	//This is, basicly, the "main" function of the service
	virtual void Main();

	Server(const Server&) = delete;
	Server operator=(const Server&) = delete;
private:
	Server();
	static void ServiceMain(int argc, char** argv);
	static void ControlHandler(unsigned long request);
	static bool InitService();


	bool Install();
	bool Uninstall();
	bool Start();
	bool Stop();
	bool Restart();


	bool InitLogger();
};

