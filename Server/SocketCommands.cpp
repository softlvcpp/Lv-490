#include "SocketCommands.h"

bool AddSocketConnection::Execute(SocketState& socket_state)//return bool
{
	//WSAData wsaData;//config sockets
	//if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	//{
	//	//log: "Server: Error at WSAStartup()\n"
	//}

	// create new socket, use the Internet address family (AF_INET), streaming sockets (SOCK_STREAM), and the TCP/IP protocol (IPPROTO_TCP).
	SOCKET new_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// check for errors to ensure that the new_socket is a valid socket.	
	if (INVALID_SOCKET == new_socket)
	{
		socket_state.log_msg = "Server: Error at socket(): " + WSAGetLastError();
		WSACleanup();//clean configuration
		return false;
	}

	//create and configurate a socket settings for our new socket
	sockaddr_in serverService;
	//AF_INET - Internet address family
	serverService.sin_family = AF_INET;
	//the IP address is INADDR_ANY to accept connections on all interfaces.
	serverService.sin_addr.s_addr = inet_addr(m_server_configuration->ipadress.c_str());
	//set port
	serverService.sin_port = htons(std::stoi(m_server_configuration->listenport.c_str()));
	
	//bind the socket for client's requests
	if (SOCKET_ERROR == ::bind(new_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		socket_state.log_msg = "Server: Error at bind(): " + WSAGetLastError();
		closesocket(new_socket);
		WSACleanup();
		return false;
	}

	// Listen on the Socket for incoming connections.
	// This socket accepts only one connection (no pending connections 
	// from other clients). This sets the backlog parameter.
	if (SOCKET_ERROR == listen(new_socket, SOMAXCONN))
	{
		socket_state.log_msg = "Server: Error at listen(): " + WSAGetLastError();
		closesocket(new_socket);
		WSACleanup();
		return false;
	}

	SocketState new_socket_state;
	new_socket_state.id = new_socket;
	new_socket_state.state = LISTEN;
	socket_state = new_socket_state;
	return true;
}

bool RemoveSocket::Execute(SocketState& socket_state)
{
	closesocket(socket_state.id);	
	socket_state.state = LISTEN;
	return true;
}

bool AcceptConnection::Execute(SocketState& socket_state)
{	
	struct sockaddr_in client;		// Address of sending partner
	int client_length = sizeof(client);

	SOCKET accepted_socket = accept(server_socket, (struct sockaddr*) & client, &client_length);
	if (INVALID_SOCKET == accepted_socket)
	{
		return false;
	}

	
	socket_state.id = accepted_socket;
	socket_state.state = ACCEPTED;
	return true;
}

bool ReceiveMessage::Execute(SocketState& socket_state)
{
	std::ofstream test_output("D:/programing/softserve/Lv-490/output.txt", std::ios::app);

	SOCKET current_socket = socket_state.id;

	while (true)
	{
		//get message size from client
		int msg_size = 0;
		int bytes_received = recv(current_socket, (char*)&msg_size, sizeof(int), 0);
		if (SOCKET_ERROR == bytes_received)
		{
			return false;
		}
		socket_state.buffer.resize(msg_size + 1);
		bytes_received = recv(current_socket, (char*)socket_state.buffer.c_str(), BUFFER_SIZE, 0);

		/*size_t msg_end = socket_state.buffer.find('\0');
		socket_state.buffer.erase(socket_state.buffer.begin() + msg_end, socket_state.buffer.end());*/

		if (SOCKET_ERROR == bytes_received)
		{
			return false;
		}

		if (bytes_received == 0)
		{
			RemoveSocket remove_socket;
			remove_socket.Execute(socket_state);
			return false;
		}
		else
		{
			//потім потрібно буде парсити цю стрічку, але на даному етапі просто вивід в консоль є
			test_output << "Server: Recieved: " << bytes_received << " bytes of \"" << socket_state.buffer << "\" message.\n";

			if (socket_state.buffer == "exit")
			{
				RemoveSocket remove_socket;
				remove_socket.Execute(socket_state);
				return true;
			}
		}
	}
	return true;
}

void StartConnection::DoRecv(SocketState new_conection)
{
	ReceiveMessage receive_message;	
	receive_message.Execute(new_conection);
	closesocket(new_conection.id);
}

bool StartConnection::Execute(SocketState& socket_state)
{		
	while (true)
	{
		SocketState new_conection;
		AcceptConnection accept_connection(socket_state.id);
		accept_connection.Execute(new_conection);
		if(new_conection.state == ACCEPTED)
		{
			new_conection.state = RECEIVE;
			m_thread_pool->ExecuteTask(&StartConnection::DoRecv, this, new_conection);
		}
	}
	return true;
}

bool Command::InitThreadPool(ThreadPool* main_pool)
{
	m_thread_pool = std::shared_ptr<ThreadPool>(main_pool); 
	return true;
}

bool Command::InitThreadPool(std::shared_ptr<ThreadPool> main_pool)
{
	m_thread_pool = main_pool;
	return true;
}

bool Command::InitConfiguration(CXMLParser::OutDocument* server_configuration)
{
	m_server_configuration = std::shared_ptr<CXMLParser::OutDocument>(server_configuration);
	return true;
}

bool Command::InitConfiguration(std::shared_ptr<CXMLParser::OutDocument> server_configuration)
{
	m_server_configuration = server_configuration;
	return true;
}
