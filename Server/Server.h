#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <fstream>
#include <mutex>
#include <condition_variable>

#include "SocketHandler.h"

#include <Windows.h>

#include "../Utility/Logger/LoggerDLL.h"

#include "../Utility/XML_Parser/XML_Parser.h"

#define USERNAME_LEN 20
/*
 * The class is implemented Singleton for two reasons :
 * 1) You can not have two instances of the same service running simultaneously
 * 2) Different implementation is impossible(no)
 */
class Server
{
private:
	std::string m_install_command  { "install" };
	std::string m_uninstall_command{ "uninstall" };
	std::string m_start_command    { "start" };
	std::string m_stop_command     { "stop" };
	std::string m_restart_command  { "restart" };

	std::string m_log_file_name{ "serverlog.log" };
	std::string m_log_directory_name{ "Lv-490_logs" };
	std::unique_ptr<filelog::FileLogger> m_logger;

	std::unique_ptr<wchar_t> m_name;
	SERVICE_STATUS m_service_status{};
	SERVICE_STATUS_HANDLE m_service_status_handle{};
	HANDLE m_service_stop_event{ INVALID_HANDLE_VALUE };

	static std::shared_ptr<Server> s_instance;
private:
	XML_Parser parser;

public:
	static bool Run(int argc, char** argv);
	static Server& get_instance();
	void set_name(std::wstring name);
	void set_log_file_name(std::string log_file_name);
	void set_log_dir_name(std::string log_dir_name);

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

