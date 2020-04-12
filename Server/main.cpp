#include "ServerService.h"
#include "../Utility/XML_Parser/XMLServer.h"
std::shared_ptr<ServerService> ServerService::s_instance{ nullptr };

int main(int argc, char** argv)
{
	ServerService& server = ServerService::get_instance();
	server.Run(argc, argv);

}