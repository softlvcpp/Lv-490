#include "stdafx.h"
extern "C" UINT __stdcall GetPortNumberCustomAction(
	MSIHANDLE hInstall
)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;
	//TODO:delete when done
	std::string port_number = "8080";
	std::string server_name = "TCPServer_Lv-490";

	std::ofstream ofs{ "D:\\dev\\Lv-490\\ServerInstaller\\bin\\Debug/out.txt" };

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
	
	//For debug only
	ofs << port_number << "\n" << server_name;

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
