#include "stdafx.h"
extern "C" UINT __stdcall GetPortNumberCustomAction(
	MSIHANDLE hInstall
)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;
	XMLServer parser;

	// Should use constexpr std::string_view, but interfaces don't allow
	const std::string server_displayname  { "Unused" };
	const std::string ip                  { "127.0.0.1" };
	const std::string blocking            { "0" };
	const std::string socket_timeout      { "5" };
	const std::string filename            { "serverlog.txt" };
	const std::string loglevel            { "3" };
	const std::string flush               { "0" };
	const std::string period_time         { "30" };
	const std::string max_working_threads { "127.0.0.1" };
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
