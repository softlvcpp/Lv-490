#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <fstream>
#include <mutex>
#include <condition_variable>

#include <Windows.h>

#include "../Utility/Logger/LoggerDLL.h"

/* 
 * The class is implemented Singleton for two reasons :
 * 1) You can not have two instances of the same service running simultaneously
 * 2) Different implementation is impossible(no)
 */
class Server
{
private:

private:
	std::unique_ptr<wchar_t> m_name;
	SERVICE_STATUS m_service_status{};
	SERVICE_STATUS_HANDLE m_service_status_handle{};
	HANDLE m_service_stop_event{ INVALID_HANDLE_VALUE };

	static std::shared_ptr<Server> s_instance;
public:
	static bool Run(int argc, char** argv); 
	static Server& get_instance();
	Server(const Server&) = delete;
	Server operator=(const Server&) = delete;
	~Server();

	//This is, basicly, the "main" function of the service
	virtual void Main();
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

};

