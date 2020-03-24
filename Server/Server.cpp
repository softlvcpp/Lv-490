#include "Server.h"


Server::Server(): m_name{_wcsdup(L"TCPServer_LV490")}
{
}

bool Server::Run(int argc, char** argv)
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = s_instance->m_name.get();
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
	ServiceTable[1].lpServiceName = nullptr;
	ServiceTable[1].lpServiceProc = nullptr;
	if (argc == 2)
	{
		if (strcmp(argv[1], "install") == 0)
		{
			if (!s_instance->Install())
			{
				//TODO: Log that can not install service
				return false;
			}
		}
		else if (strcmp(argv[1], "uninstall") == 0)
		{
			if (!s_instance->Uninstall())
			{
				//TODO: Log that can not uninstall service
				return false;
			}
		}
		else if (strcmp(argv[1], "start") == 0)
		{
			if (!s_instance->Start())
			{
				//TODO: Log that can not start service
				return false;
			}
		}
		else if (strcmp(argv[1], "stop") == 0)
		{
			if (!s_instance->Stop())
			{
				//TODO: Log that can not stop service
				return false;
			}
		}
		else if (strcmp(argv[1], "restart") == 0)
		{
			if (!s_instance->Restart())
			{
				//TODO: Log that can not restart service
				return false;
			}
		}

	}
	else
	{
		if (!StartServiceCtrlDispatcher(ServiceTable))
		{
			//TODO: Log that cannot start service control dispatcher
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
	}
	s_instance->m_service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	s_instance->m_service_status.dwCurrentState = SERVICE_START_PENDING;
	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	s_instance->m_service_status.dwWin32ExitCode = 0;
	s_instance->m_service_status.dwServiceSpecificExitCode = 0;
	s_instance->m_service_status.dwCheckPoint = 0;
	s_instance->m_service_status.dwWaitHint = 0;

	s_instance->m_service_status_handle = RegisterServiceCtrlHandlerExW(s_instance->m_name.get(), (LPHANDLER_FUNCTION_EX)ControlHandler, nullptr);
	if (s_instance->m_service_status_handle == (SERVICE_STATUS_HANDLE)0) {
		//TODO: Log that can not register control handler
		return;
	}

	if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
	{
		//TODO: Log that cannot set service status
		return;
	}

	bool init_result = s_instance->InitService();
	if (init_result == false)
	{
		//TODO: Log that can not init service
		return;
	}

	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	s_instance->m_service_status.dwCurrentState = SERVICE_RUNNING;
	s_instance->m_service_status.dwWin32ExitCode = 0;
	s_instance->m_service_status.dwCheckPoint = 0;

	if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
	{
		//TODO: Log that can not set service status
		return;
	}


	std::thread main_thread(&Server::Main, s_instance);
	{
		std::condition_variable condition_var;
		std::mutex mutex;
		std::unique_lock<std::mutex> condition_lock(mutex);
		condition_var.wait(condition_lock, []() { return s_instance->m_service_status.dwCurrentState != SERVICE_RUNNING; });
	}

}

void Server::ControlHandler(unsigned long request)
{
	switch (request)
	{
	case SERVICE_CONTROL_STOP:

		if (s_instance->m_service_status.dwCurrentState != SERVICE_RUNNING)
			return;

		////TODO: Log about service stop

		SetEvent(s_instance->m_service_stop_event);
		s_instance->m_service_status.dwControlsAccepted = 0;
		s_instance->m_service_status.dwCurrentState = SERVICE_STOPPED;
		s_instance->m_service_status.dwWin32ExitCode = 0;
		s_instance->m_service_status.dwCheckPoint = 4;

		if (SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status) == FALSE)
		{
			//TODO: Log that cannot set service status
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
			//TODO: Log that can not set service status

		}
		//TODO: Log that can not create event :(

		return false;
	}
	return true;
}

bool Server::Install()
{
	wchar_t exe_file_path[MAX_PATH];
	if (GetModuleFileName(NULL, exe_file_path, ARRAYSIZE(exe_file_path)) == 0)
	{
		//TODO: Log that cannot get name of current .exe file
		return false;
	}

	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!SCManager) {
		//TODO: Log that can not open SCM
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
			//TODO: Log that eccess denied
			break;
		case ERROR_CIRCULAR_DEPENDENCY:
			//TODO: Log that can not create service because of circular dependency
			break;
		case ERROR_DUPLICATE_SERVICE_NAME:
			//TODO: Log that a service with this name alredy is installed
			break;
		case ERROR_INVALID_HANDLE:
			//TODO: Log that SCM handle is invalid
			break;
		case ERROR_INVALID_NAME:
			//TODO: Log that service name is invalid
			break;
		case ERROR_INVALID_PARAMETER:
			//TODO: Log that the parameters are invalid
			break;
		case ERROR_INVALID_SERVICE_ACCOUNT:
			//TODO: 
			break;
		case ERROR_SERVICE_EXISTS:
			//TODO: Log that this service alredy exists
			break;
		default:
			//TODO: Log Undefined error
			break;
		}
		CloseServiceHandle(SCManager);
		return false;
	}
	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	//TODO: Log that service was installed
	return true;
}

bool Server::Uninstall()
{
	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!SCManager) {
		// TODO: Log that can not open SCM
		return false;
	}
	SC_HANDLE service = OpenService(SCManager, s_instance->m_name.get(), SERVICE_STOP | DELETE);
	if (!service) {
		// TODO: Log that cannot remove service
		CloseServiceHandle(SCManager);
		return false;
	}

	DeleteService(service);
	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	// TODO: Log that the server was uninstalled
	return true;
}

bool Server::Start()
{
	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!SCManager)
	{
		//TODO: Log that cannot open SCM
		return false;
	}
	SC_HANDLE service = OpenService(SCManager, s_instance->m_name.get(), SERVICE_START);

	if (!StartService(service, 0, NULL)) {
		//TODO: Log that cannot start service
		CloseServiceHandle(SCManager);
		return false;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	//TODO:Log that successfuly started service
	return true;
}

bool Server::Stop()
{
	SERVICE_STATUS_PROCESS ssp;
	unsigned long bytes_needed;

	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!SCManager)
	{
		//TODO: Log that cannot open SCM
		return false;
	}
	SC_HANDLE service = OpenService(
		SCManager,
		s_instance->m_name.get(),
		SERVICE_STOP |
		SERVICE_QUERY_STATUS);
	if (service == nullptr)
	{
		//TODO: Log that cannot open the service
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
		//TODO: Log that can not get the process status

		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}


	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		//TODO: Log that service is stopped alredy
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}

	if (ssp.dwCurrentState == SERVICE_STOP_PENDING)
	{

		std::mutex mutex;
		std::unique_lock<std::mutex> condition_lock(mutex);
		std::condition_variable cond_var;

		cond_var.wait(condition_lock, [&]() {return ssp.dwCurrentState == SERVICE_STOPPED; });
	}

	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status);

	if (!ControlService(
		service,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		//TODO: Log that can not send service a stop signal
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
	return true;
}

bool Server::Restart()
{
	SERVICE_STATUS_PROCESS ssp;
	unsigned long bytes_needed;
	SC_HANDLE SCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!SCManager)
	{
		//TODO: Log that cannot open SCM
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
		//TODO: Log that cannot open the service
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
		//TODO: Log that can not get the process status
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}


	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		//TODO: Log that service is stopped alredy
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}

	if (ssp.dwCurrentState == SERVICE_STOP_PENDING)
	{
		return true;
	}

	s_instance->m_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	SetServiceStatus(s_instance->m_service_status_handle, &s_instance->m_service_status);

	if (!ControlService(
		service,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		//TODO: Log that can not send service a stop signal
		CloseServiceHandle(service);
		CloseServiceHandle(SCManager);
		return false;
	}

	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		if (!QueryServiceStatusEx(
			service,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&bytes_needed))
		{
			//TODO: Log that can not get service status 
			return false;
		}
		Sleep(1000);
	}
	if (!StartService(service, 0, NULL)) {
		//TODO: Log that cannot start service
		std::cout << GetLastError();
		CloseServiceHandle(SCManager);
		return false;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(SCManager);
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
}

void Server::Main()
{
	while (true)
	{
	}
}
