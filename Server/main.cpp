#include "ServerService.h"
#include "../Utility/XML_Parser/XML_Parser.h"
std::shared_ptr<ServerService> ServerService::s_instance{ nullptr };

int main(int argc, char** argv)
{
	ServerService& server = ServerService::get_instance();
	server.Run(argc, argv);

    /*MessageBox(
        NULL,
        (LPCWSTR)L"Resource not available\nDo you want to try again?",
        (LPCWSTR)L"Account Details",
        MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2
    );*/

}