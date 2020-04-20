#include "pch.h"
#include "ServerService.h"
#include "SmartServiceHandle.h"

#define LOG_T SLOG_TRACE(*s_instance->m_logger)
#define LOG_D SLOG_DEBUG(*s_instance->m_logger)
#define LOG_P SLOG_PROD(*s_instance->m_logger)


ServerService::ServerService() : m_name{ _wcsdup(L"TCPServer_Lv490") } {}

bool ServerService::ReadConfig()
{
	char exe_file_path[MAX_PATH];
	if (GetModuleFileNameA(NULL, exe_file_path, ARRAYSIZE(exe_file_path)) == 0)
	{
		LOG_T << "Unable to get executable path. Error " << GetLastError();
		return false;
	}
	m_config_file_path = exe_file_path;
	m_config_file_path.erase(m_config_file_path.rfind('\\') + 1, m_config_file_path.length() - m_config_file_path.rfind('\\')- 1);
	m_config_file_path += "config.xml";

	if (!m_parser.ReadConfigs(m_config_file_path))
	{
		return false;
	}
	m_log_file_name = m_parser.get_filename();
	m_log_level = static_cast<filelog::LogLevel>(m_parser.get_loglevel()[0] - '0');

	std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(m_parser.get_servername());
	m_name.reset(_wcsdup(name.c_str()));
	return true;
}

bool ServerService::Run(int argc, char** argv)
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = s_instance->m_name.get();
	ServiceTable[0].lpServiceProc = reinterpret_cast<LPSERVICE_MAIN_FUNCTION>(ServiceMain);
	ServiceTable[1].lpServiceName = nullptr;
	ServiceTable[1].lpServiceProc = nullptr;
	if (s_instance->ReadConfig() == false)
	{
		return false;
	}
	if (!s_instance->InitLogger())
	{
		return false;
	}
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

void ServerService::ServiceMain(int argc, char** argv)
{
	//std::ofstream out{ R"(C:\Lv-490_Files\serv_LOGS\out.txt)", std::ios::app };
	if (s_instance == nullptr)
	{
		s_instance = std::shared_ptr<ServerService>(new ServerService);
		if (s_instance->ReadConfig() == false)
		{
			//out << "Cannot read config";
			return;
		}
		if (!s_instance->InitLogger())
		{
			//out << "Cannot init logger";
			return;
		}

	}
	//out << __LINE__ << '\n';

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
	//out << __LINE__ << '\n';

	if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
	{
		LOG_T << "Unable to set service status.";
		return;
	}
	//out << __LINE__ << '\n';

	bool init_result = s_instance->InitService();
	if (init_result == false)
	{
		LOG_T << "Unable to initialize service.";
		return;
	}
	//out << __LINE__ << '\n';

	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	s_instance->m_service_status.dwCurrentState = SERVICE_RUNNING;
	s_instance->m_service_status.dwWin32ExitCode = 0;
	s_instance->m_service_status.dwCheckPoint = 0;

	if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
	{
		LOG_T << "Unable to set service status.";
		return;
	}
	//out << __LINE__ << '\n';

	std::thread main_thread(&ServerService::Main, s_instance);
	{
		std::condition_variable condition_var;
		std::mutex mutex;
		std::unique_lock<std::mutex> condition_lock(mutex);

		condition_var.wait(condition_lock, []() { return s_instance->m_service_status.dwCurrentState != SERVICE_RUNNING; });
	}

	s_instance->m_logger->join();
}

void ServerService::ControlHandler(unsigned long request)
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

bool ServerService::InitService()
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

bool ServerService::Install()
{
	// Get current .exe path
	wchar_t exe_file_path[MAX_PATH];
	if (GetModuleFileName(NULL, exe_file_path, ARRAYSIZE(exe_file_path)) == 0)
	{
		LOG_T << "Unable to get executable path. Error " << GetLastError();
		return false;
	}

	SmartServiceHandle SCManager = HandleManager::MakeSmart(OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE));
	if (!SCManager) {
		LOG_T << "Unable to open Service Control Manager. Error " << GetLastError();
		return false;
	}

	SmartServiceHandle service = HandleManager::MakeSmart(
		CreateService(
			SCManager.get(),
			s_instance->m_name.get(),
			s_instance->m_name.get(),
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			exe_file_path,
			NULL, NULL, NULL, NULL, NULL
		));


	if (!service) {
		int err = GetLastError();
		switch (err) {
		case ERROR_ACCESS_DENIED:
			LOG_T << "Unable to create service. Access denied.  Error " << GetLastError();
			break;
		case ERROR_CIRCULAR_DEPENDENCY:
			LOG_T << "Unable to create service. Circular dependency. Error " << GetLastError();
			break;
		case ERROR_DUPLICATE_SERVICE_NAME:
			LOG_T << "Unable to create service. Duplicate service name. Error " << GetLastError();
			break;
		case ERROR_INVALID_HANDLE:
			LOG_T << "Unable to create service. Invalid handle. Error " << GetLastError();;
			break;
		case ERROR_INVALID_NAME:
			LOG_T << "Unable to create service. Invalid name. Error " << GetLastError();
			break;
		case ERROR_INVALID_PARAMETER:
			LOG_T << "Unable to create service. Invalid parameter. Error " << GetLastError();
			break;
		case ERROR_INVALID_SERVICE_ACCOUNT:
			LOG_T << "Unable to create service. invalid service account. Error " << GetLastError();
			break;
		case ERROR_SERVICE_EXISTS:
			LOG_T << "Unable to create service. Service exists. Error " << GetLastError();
			break;
		default:
			LOG_T << "Unable to create service. Undefined error. Error " << GetLastError();
			break;
		}
		return false;
	}
	LOG_P << "Service installed.";
	return true;
}

bool ServerService::Uninstall()
{
	SmartServiceHandle SCManager = HandleManager::MakeSmart(OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE));
	if (!SCManager) {
		LOG_T << "Unable to open Service Control Manager.";
		return false;
	}
	SmartServiceHandle service = HandleManager::MakeSmart(OpenService(SCManager.get(), s_instance->m_name.get(), SERVICE_STOP | DELETE));
	if (!service) {
		LOG_T << "Unable to uninstall service.";
		return false;
	}

	DeleteService(service.get());
	LOG_P << "Service uninstalled.";
	return true;
}

bool ServerService::Start()
{
	SmartServiceHandle SCManager = HandleManager::MakeSmart(OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE));
	if (!SCManager)
	{
		LOG_T << "Unable to open Service Control Manager. Error " << GetLastError();
		return false;
	}
	SmartServiceHandle service = HandleManager::MakeSmart(OpenService(SCManager.get(), s_instance->m_name.get(), SERVICE_START));

	if (!StartService(service.get(), 0, NULL)) {
		LOG_T << "Unable to start service. Error " << GetLastError();
		return false;
	}

	LOG_P << "Service started";
	return true;
}

bool ServerService::Stop()
{
	SERVICE_STATUS_PROCESS ssp;
	unsigned long bytes_needed;

	SmartServiceHandle SCManager = HandleManager::MakeSmart(OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
	if (!SCManager)
	{
		LOG_T << "Unable to open Service Control Manager. Error " << GetLastError();
		return false;
	}
	SmartServiceHandle service = HandleManager::MakeSmart(OpenService(
		SCManager.get(),
		s_instance->m_name.get(),
		SERVICE_STOP |
		SERVICE_QUERY_STATUS));

	if (service == nullptr)
	{
		LOG_T << "Unable to open service. Error " << GetLastError();
		return false;
	}

	if (!QueryServiceStatusEx(
		service.get(),
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&bytes_needed))
	{
		LOG_T << "Unable to get service status. Error " << GetLastError();
		return false;
	}


	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		LOG_T << "Unable to stop service. The service is stopped alredy. Error " << GetLastError();
		return false;
	}

	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status);

	if (!ControlService(
		service.get(),
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		LOG_T << "Unable to stop service. Error " << GetLastError();
		return false;
	}

	LOG_P << "Service stopped";

	return true;
}

bool ServerService::Restart()
{
	SERVICE_STATUS_PROCESS ssp;
	unsigned long bytes_needed;
	SmartServiceHandle SCManager = HandleManager::MakeSmart(OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE));
	if (!SCManager)
	{
		LOG_T << "Unable to open Service Control Manager. Error " << GetLastError();
		return false;
	}
	SmartServiceHandle service = HandleManager::MakeSmart(OpenService(
		SCManager.get(),
		s_instance->m_name.get(),
		SERVICE_STOP |
		SERVICE_START |
		SERVICE_QUERY_STATUS));
	if (service == nullptr)
	{
		LOG_T << "Unable to open service. Error " << GetLastError();
		return false;
	}

	if (!QueryServiceStatusEx(
		service.get(),
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&bytes_needed))
	{
		LOG_T << "Unable to get service status. Error " << GetLastError();
		return false;
	}


	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		LOG_T << "Unable to stop service. The service is stopped alredy. Error " << GetLastError();
		return false;
	}

	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status);

	if (!ControlService(
		service.get(),
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		LOG_T << "Unable to stop service. Error " << GetLastError();
		return false;
	}

	unsigned sleep_time_ms = 1000;
	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		if (!QueryServiceStatusEx(
			service.get(),
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&bytes_needed))
		{
			LOG_T << "Unable to get service status. Error " << GetLastError();
			return false;
		}
		Sleep(sleep_time_ms);
	}
	if (!StartService(service.get(), 0, NULL)) {
		LOG_T << "Unable to start service. Error " << GetLastError();
		std::cout << GetLastError();
		return false;
	}

	return true;
}

bool ServerService::InitLogger()
{
	std::string log_file_path = "C:/Lv-490_Files/serv_LOGS/";
	if (!CreateDirectoryA(log_file_path.c_str(), nullptr))
	{
		if (GetLastError() == ERROR_PATH_NOT_FOUND)
		{
			return false;
		}
	}
	log_file_path += m_log_file_name;
	m_logger = std::unique_ptr<filelog::FileLogger>(new filelog::FileLogger(log_file_path.c_str(), true, filelog::LogLevel::Trace));
	return true;
}

ServerService& ServerService::get_instance()
{
	if (s_instance.get() == nullptr)
	{
		s_instance = std::shared_ptr<ServerService>(new ServerService);
	}
	return *s_instance;
}

ServerService::~ServerService()
{
	CloseHandle(m_service_stop_event);
	CloseHandle(s_instance->m_service_status_handle);
}

void ServerService::Main()
{
	s_instance->m_server = std::make_unique<Server>(s_instance->m_parser, s_instance->m_log_directory_name);
	s_instance->m_server->Run();
}