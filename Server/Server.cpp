#include "Server.h"
#define LOG_T SLOG_TRACE(*s_instance->m_logger)
#define LOG_D SLOG_DEBUG(*s_instance->m_logger)
#define LOG_P SLOG_PROD(*s_instance->m_logger)



Server::Server() : m_name{ _wcsdup(L"TCPServer_LV490") }{}

bool Server::Run(int argc, char** argv)
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = s_instance->m_name.get();
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
	ServiceTable[1].lpServiceName = nullptr;
	ServiceTable[1].lpServiceProc = nullptr;
	s_instance->InitLogger();
	if (argc == 2)
	{
		if (argv[1] == s_instance->m_install_command)
		{
			if (!s_instance->Install())
			{
				LOG_T << "Unable to install windows service";
				return false;
			}
		}
		else if (argv[1] == s_instance->m_uninstall_command)
		{
			if (!s_instance->Uninstall())
			{
				LOG_T << "Unable to uninstall windows service.";
				return false;
			}
		}
		else if (argv[1] == s_instance->m_start_command)
		{
			if (!s_instance->Start())
			{
				LOG_T << "Unable to start windows service.";
				return false;
			}
		}
		else if (argv[1] == s_instance->m_stop_command)
		{
			if (!s_instance->Stop())
			{
				LOG_T << "Unable to stop windows service.";
				return false;
			}
		}
		else if (argv[1] == s_instance->m_restart_command)
		{
			if (!s_instance->Restart())
			{
				LOG_T << "Unable to restart windows service.";
				return false;
			}
		}

	}
	else
	{
		if (!StartServiceCtrlDispatcher(ServiceTable))
		{
			LOG_T << "Unable to start Service Control Dispatcher.";
			return false;
		}
	}
	return true;
}

void Server::ServiceMain(int argc, char** argv)
{
	if (s_instance == nullptr)
	{
		s_instance = std::shared_ptr<Server>(new Server);
		
		if (!s_instance->InitLogger())
		{
			return;
		}
	}
	s_instance->m_service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	s_instance->m_service_status.dwCurrentState = SERVICE_START_PENDING;
	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	s_instance->m_service_status.dwWin32ExitCode = 0;
	s_instance->m_service_status.dwServiceSpecificExitCode = 0;
	s_instance->m_service_status.dwCheckPoint = 0;
	s_instance->m_service_status.dwWaitHint = 0;

	s_instance->m_service_status_handle = RegisterServiceCtrlHandlerExW(s_instance->m_name.get(), (LPHANDLER_FUNCTION_EX)ControlHandler, nullptr);
	if (s_instance->m_service_status_handle == (SERVICE_STATUS_HANDLE)0) {
		LOG_T << "Unable to register Control Handler.";
		return;
	}

	if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
	{
		LOG_T << "Unable to set service status.";
		return;
	}

	bool init_result = s_instance->InitService();
	if (init_result == false)
	{
		LOG_T << "Unable to initialize service.";
		return;
	}

	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	s_instance->m_service_status.dwCurrentState = SERVICE_RUNNING;
	s_instance->m_service_status.dwWin32ExitCode = 0;
	s_instance->m_service_status.dwCheckPoint = 0;

	if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
	{
		LOG_T << "Unable to set service status.";
		return;
	}



	std::thread main_thread(&Server::Main, s_instance);
	{
		std::condition_variable condition_var;
		std::mutex mutex;
		std::unique_lock<std::mutex> condition_lock(mutex);
		condition_var.wait(condition_lock, []() { return s_instance->m_service_status.dwCurrentState != SERVICE_RUNNING; });
	}

	s_instance->m_logger->join();



}

void Server::ControlHandler(unsigned long request)
{
	switch (request)
	{
	case SERVICE_CONTROL_STOP:

		if (s_instance->m_service_status.dwCurrentState != SERVICE_RUNNING)
			return;


		SetEvent(s_instance->m_service_stop_event);
		s_instance->m_service_status.dwControlsAccepted = 0;
		s_instance->m_service_status.dwCurrentState = SERVICE_STOPPED;
		s_instance->m_service_status.dwWin32ExitCode = 0;
		s_instance->m_service_status.dwCheckPoint = 4;

		if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
		{
			LOG_T << "Unable to set service status.";
		}
		break;
	}
}

bool Server::InitService()
{
	s_instance->m_service_stop_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (s_instance->m_service_stop_event == NULL)
	{
		// Error creating event
		// Tell service controller we are stopped and exit
		s_instance->m_service_status.dwControlsAccepted = 0;
		s_instance->m_service_status.dwCurrentState = SERVICE_STOPPED;
		s_instance->m_service_status.dwWin32ExitCode = GetLastError();
		s_instance->m_service_status.dwCheckPoint = 1;

		if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
		{
			LOG_T << "Unable to set service status.";
		}
		LOG_T << "Unable to create service stop event.";

		return false;
	}

	return true;
}

bool Server::Install()
{
	wchar_t exe_file_path[MAX_PATH];
	if (GetModuleFileName(NULL, exe_file_path, ARRAYSIZE(exe_file_path)) == 0)
	{
		LOG_T << "Unable to get executable path.";
		return false;
	}

	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!SCManager) {
		LOG_T << "Unable to open Service Control Manager.";
		return false;
	}

	SC_HANDLE service = CreateService(
		SCManager,
		s_instance->m_name.get(),
		s_instance->m_name.get(),
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		exe_file_path,
		NULL, NULL, NULL, NULL, NULL
	);


	if (!service) {
		int err = GetLastError();
		switch (err) {
		case ERROR_ACCESS_DENIED:
			LOG_T << "Unable to create service. Access denied";
			break;
		case ERROR_CIRCULAR_DEPENDENCY:
			LOG_T << "Unable to create service. Circular dependency";
			break;
		case ERROR_DUPLICATE_SERVICE_NAME:
			LOG_T << "Unable to create service. Duplicate service name";
			break;
		case ERROR_INVALID_HANDLE:
			LOG_T << "Unable to create service. Invalid handle";
			break;
		case ERROR_INVALID_NAME:
			LOG_T << "Unable to create service. Invalid name";
			break;
		case ERROR_INVALID_PARAMETER:
			LOG_T << "Unable to create service. Invalid parameter";
			break;
		case ERROR_INVALID_SERVICE_ACCOUNT:
			LOG_T << "Unable to create service. invalid service account";
			break;
		case ERROR_SERVICE_EXISTS:
			LOG_T << "Unable to create service. Service exists";
			break;
		default:
			LOG_T << "Unable to create service. Undefined error";
			break;
		}
		CloseServiceHandle(SCManager);
		return false;
	}
	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	LOG_P << "Service installed.";
	return true;
}

bool Server::Uninstall()
{
	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!SCManager) {
		LOG_T << "Unable to open Service Control Manager.";
		return false;
	}
	SC_HANDLE service = OpenService(SCManager, s_instance->m_name.get(), SERVICE_STOP | DELETE);
	if (!service) {
		LOG_T << "Unable to uninstall service.";
		CloseServiceHandle(SCManager);
		return false;
	}

	DeleteService(service);
	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	LOG_P << "Service uninstalled.";
	return true;
}

bool Server::Start()
{
	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!SCManager)
	{
		LOG_T << "Unable to open Service Control Manager";
		return false;
	}
	SC_HANDLE service = OpenService(SCManager, s_instance->m_name.get(), SERVICE_START);

	if (!StartService(service, 0, NULL)) {
		LOG_T << "Unable to start service";
		CloseServiceHandle(SCManager);
		return false;
	}

	LOG_P << "Service started";
	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	
	return true;
}

bool Server::Stop()
{
	SERVICE_STATUS_PROCESS ssp;
	unsigned long bytes_needed;

	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!SCManager)
	{
		LOG_T << "Unable to open Service Control Manager.";
		return false;
	}
	SC_HANDLE service = OpenService(
		SCManager,
		s_instance->m_name.get(),
		SERVICE_STOP |
		SERVICE_QUERY_STATUS);

	if (service == nullptr)
	{
		LOG_T << "Unable to open service.";

		CloseServiceHandle(SCManager);
		return false;
	}

	if (!QueryServiceStatusEx(
		service,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&bytes_needed))
	{
		LOG_T << "Unable to get service status.";
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}


	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		LOG_T << "Unable to stop service. The service is stopped alredy.";
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}

	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status);

	if (!ControlService(
		service,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		LOG_T << "Unable to stop service.";
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	LOG_P << "Service stopped";

	return true;
}

bool Server::Restart()
{
	SERVICE_STATUS_PROCESS ssp;
	unsigned long bytes_needed;
	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!SCManager)
	{
		LOG_T << "Unable to open Service Control Manager.";
		return false;
	}
	SC_HANDLE service = OpenService(
		SCManager,
		s_instance->m_name.get(),
		SERVICE_STOP |
		SERVICE_START |
		SERVICE_QUERY_STATUS);
	if (service == nullptr)
	{
		LOG_T << "Unable to open service.";
		CloseServiceHandle(SCManager);
		return false;
	}

	if (!QueryServiceStatusEx(
		service,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&bytes_needed))
	{
		LOG_T << "Unable to get service status.";
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}


	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		LOG_T << "Unable to stop service. The service is stopped alredy.";
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}
	
	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status);

	if (!ControlService(
		service,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		LOG_T << "Unable to stop service.";
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}

	unsigned sleep_time_ms = 1000;
	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		if (!QueryServiceStatusEx(
			service,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&bytes_needed))
		{
			LOG_T << "Unable to get service status.";
			return false;
		}
		Sleep(sleep_time_ms);
	}
	if (!StartService(service, 0, NULL)) {
		LOG_T << "Unable to start service.";
		std::cout << GetLastError();
		CloseServiceHandle(SCManager);
		return false;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	return true;
}

bool Server::InitLogger()
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

Server& Server::get_instance()
{
	if (s_instance.get() == nullptr)
	{
		s_instance = std::shared_ptr<Server>(new Server);
	}
	return *s_instance;
}

Server::~Server()
{
	CloseHandle(m_service_stop_event);
}

void Server::Main()
{	
	ThreadPool thread_pool(std::thread::hardware_concurrency()/*замість цього треба вставити параметри з парсера*/);
	SocketHandler socket_handler(m_log_directory_name);
	
	//socket_handler.set_configuration(parser); //тут впишеш вказівник, або розумний вказівник на парсер

	socket_handler.AddCommand(new AddSocketConnection);
	socket_handler.AddCommand(new StartConnection);

	socket_handler.Run(&thread_pool);
}

void Server::set_name(std::wstring name)
{
	m_name.reset(_wcsdup(name.c_str()));
}

void Server::set_log_file_name(std::string log_file_name)
{
	m_log_file_name = log_file_name;
}

void Server::set_log_dir_name(std::string log_dir_name)
{
	m_log_directory_name = log_dir_name;
}
