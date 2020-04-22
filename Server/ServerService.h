#pragma once

#include "Server.h"


/*
 * This class is implemented as singletone
 * The class functions don`t throw exceptions
 */
class ServerService
{

// File IO members and service commands
private:
	std::string_view m_install_command  { "install" };
	std::string_view m_uninstall_command{ "uninstall" };
	std::string_view m_start_command    { "start" };
	std::string_view m_stop_command     { "stop" };
	std::string_view m_restart_command  { "restart" };

	////REDO Logger
	//std::string m_log_file_name{ "serverlog.log" };
	//std::string m_log_directory_name{ "Lv-490_logs" };
	//filelog::LogLevel m_log_level;

	//std::unique_ptr<filelog::FileLogger m_logger;
	std::unique_ptr<wchar_t> m_name;

	std::string m_all_files_dir_name{ "Lv-490_Files" };

// Server members
private:
	std::unique_ptr<Server> m_server;

//Windows Service members
protected:
	SERVICE_STATUS m_service_status{};
	SERVICE_STATUS_HANDLE m_service_status_handle{};
	HANDLE m_service_stop_event{ INVALID_HANDLE_VALUE };

	static std::shared_ptr<ServerService> s_instance;
	ServerService();

// Parser members
private:
	XMLServer m_parser;
	std::string m_config_file_path;

	[[nodiscard]] virtual bool ReadConfig();

public:
	[[nodiscard]] static bool Run(int argc, char** argv);
	static ServerService& get_instance();

	virtual ~ServerService();

	//This is the "main" function of the service
	virtual void Main();

	ServerService(const ServerService&) = delete;
	ServerService operator=(const ServerService&) = delete;
private:
	static void ServiceMain(int argc, char** argv);
	static void ControlHandler(unsigned long request);
	[[nodiscard]] static bool InitService();
	[[nodiscard]] virtual bool InitLogger();

public:
	[[nodiscard]] virtual bool Install();
	[[nodiscard]] virtual bool Uninstall();
	[[nodiscard]] virtual bool Start();
	[[nodiscard]] virtual bool Stop();
	[[nodiscard]] virtual bool Restart();


};
