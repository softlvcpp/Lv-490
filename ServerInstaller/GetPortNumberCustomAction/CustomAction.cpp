#include "stdafx.h"
#include "XMLInstallerParser.h"

BOOL IsProcessElevated()
{
	BOOL fIsElevated = FALSE;
	HANDLE hToken = NULL;
	TOKEN_ELEVATION elevation;
	DWORD dwSize;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		printf("\n Failed to get Process Token :%d.", GetLastError());
		goto Cleanup;  // if Failed, we treat as False
	}


	if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
	{
		printf("\nFailed to get Token Information :%d.", GetLastError());
		goto Cleanup;// if Failed, we treat as False
	}

	fIsElevated = elevation.TokenIsElevated;

Cleanup:
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}
	return fIsElevated;
}

extern "C" UINT __stdcall GetPortNumberCustomAction(
	MSIHANDLE hInstall
)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;
	std::ofstream a{ "D:\\dev\\Lv-490\\ServerInstaller\\bin\\Debug\\out.txt" };


	XMLInstallerParser parser;
	std::string target_dir;
	// Should use constexpr std::string_view, but interfaces don't allow
	constexpr std::string_view server_displayname  { "Unused" };
	constexpr std::string_view ip                  { "127.0.0.1" };
	constexpr std::string_view blocking            { "0" };
	constexpr std::string_view socket_timeout      { "5" };
	constexpr std::string_view filename            { "serverlog.txt" };
	constexpr std::string_view loglevel            { "3" };
	constexpr std::string_view flush               { "0" };
	constexpr std::string_view period_time         { "30" };
	constexpr std::string_view max_working_threads { "127.0.0.1" };
	std::string port_number = "8080";
	std::string server_name = "TCPServer_Lv-490";

	parser.set_serverdisplayname(server_displayname);
	parser.set_ipadress(ip);
	parser.set_blocking(blocking);
	parser.set_socket_timeout(socket_timeout);
	parser.set_filename(filename);
	parser.set_loglevel(loglevel);
	parser.set_flush(flush);
	parser.set_period_time(period_time);
	parser.set_maxworkingthreads(max_working_threads);

	hr = WcaInitialize(hInstall, "GetPortNumberCustomAction");
	ExitOnFailure(hr, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.

	LPWSTR port_str = nullptr;
	hr = WcaGetProperty(L"PORTNUMBERProperty", &port_str);
	ExitOnFailure(hr, "Failure in port number setting");
	if (port_str[0] != L'\0' && port_str != NULL)
	{
		std::wstring_view  port_str_view{ port_str };
		port_number = std::string(port_str_view.begin(), port_str_view.end());
	}

	LPWSTR server_name_str = nullptr;
	hr = WcaGetProperty(L"SERVERNAMEProperty", &server_name_str);
	ExitOnFailure(hr, "Failure in port number setting");
	if (server_name_str[0] != L'\0' && server_name_str != NULL)
	{
		std::wstring_view  server_name_view{ server_name_str };
		server_name = std::string(server_name_view.begin(), server_name_view.end());
	}
	
	parser.set_listenport(port_number);
	parser.set_servername(server_name);
	
	WCHAR path[MAX_PATH] = { 0 };
	DWORD action_data_len = MAX_PATH;
	hr = MsiGetProperty(hInstall, L"CustomActionData", path, &action_data_len);
	ExitOnFailure(hr, "Cannot access INSTALLFOLDER");

	std::wstring_view  target_dir_view{ path };
	target_dir = std::string(target_dir_view.begin(), target_dir_view.end());

	target_dir += "config.xml";

	//a << IsProcessElevated() << "\n" << target_dir <<"\n"<< *port_str << "\n" << *server_name_str;

	if (!parser.WriteConfig(/*"C:\\Program Files (x86)\\Lv-490_server\\config.xml"*/target_dir))
	{
		MessageBox(
			NULL,
			(LPCWSTR)L"Oops!",
			(LPCWSTR)L"Cannot write to file",
			MB_ICONWARNING | MB_OK | MB_DEFBUTTON2
		);
	}

LExit:
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}


// DllMain - Initialize and cleanup WiX custom action utils.
extern "C" BOOL WINAPI DllMain(
	__in HINSTANCE hInst,
	__in ULONG ulReason,
	__in LPVOID
)
{
	switch (ulReason)
	{
	case DLL_PROCESS_ATTACH:
		WcaGlobalInitialize(hInst);
		break;

	case DLL_PROCESS_DETACH:
		WcaGlobalFinalize();
		break;
	}

	return TRUE;
}
