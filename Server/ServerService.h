#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <fstream>
#include <mutex>
#include <condition_variable>

#include "Server.h"

#include <Windows.h>






#define USERNAME_LEN 20
/*
 * This class is implemented as singletone
 * The class functions don`t throw exceptions
 */
class ServerService
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
	filelog::LogLevel m_log_level;

	std::unique_ptr<filelog::FileLogger> m_logger;
	std::unique_ptr<wchar_t> m_name;

	std::string m_all_files_dir_name{ "Lv-490_Files" };
	std::ofstream m_file_output{ "C:/Lv-490_Files/temp_log.txt", std::ios::app };

// Server members
private:
	std::unique_ptr<Server> m_server;

//Windows Service members
private:
	SERVICE_STATUS m_service_status{};
	SERVICE_STATUS_HANDLE m_service_status_handle{};
	HANDLE m_service_stop_event{ INVALID_HANDLE_VALUE };

	static std::shared_ptr<ServerService> s_instance;

// Parser members
private:
	CXMLParser::XMLParser m_parser;
	std::string m_config_file_name{ "C:/Lv-490_Files/config.xml" };
 
	bool ReadConfig();

public:
	static bool Run(int argc, char** argv);
	static ServerService& get_instance();
	void set_log_dir_name(std::string log_dir_name);
	void set_config_file_name(std::string file_name);

	~ServerService();

	//This is, basicly, the "main" function of the service
	virtual void Main();

	ServerService(const ServerService&) = delete;
	ServerService operator=(const ServerService&) = delete;
private:
	ServerService();
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
