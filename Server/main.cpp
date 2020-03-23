#include "Server.h"

std::shared_ptr<Server> Server::s_instance{ nullptr };

int main(int argc, char** argv)
{
	Server& server = Server::get_instance();
	
	server.Run(argc, argv);
}