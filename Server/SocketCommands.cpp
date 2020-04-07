#include "SocketCommands.h"

bool AddSocketConnection::Execute(SocketState& socket_state)//return bool
{
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
		RemoveSocket::RemoveUsedSocket(new_socket);
		return false;
	}

	// Listen on the Socket for incoming connections.
	// This socket accepts only one connection (no pending connections 
	// from other clients). This sets the backlog parameter.
	if (SOCKET_ERROR == listen(new_socket, SOMAXCONN))
	{
		socket_state.log_msg = "Server: Error at listen(): " + WSAGetLastError();
		RemoveSocket::RemoveUsedSocket(new_socket);
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
	RemoveSocket::RemoveUsedSocket(socket_state.id);
	socket_state.state = LISTEN;
	return true;
}

bool RemoveSocket::RemoveUsedSocket(SOCKET& server_socket)
{
	closesocket(server_socket);
	WSACleanup();
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
	SOCKET current_socket = socket_state.id;

	while (true)
	{
		//get message size from client
		
		socket_state.buffer.clear();
		std::string incomming_buffer;
		incomming_buffer.resize(BUFFER_SIZE);
		size_t bytes_received = recv(current_socket, const_cast<char*>(incomming_buffer.c_str()), sizeof(int), 0);

		if (SOCKET_ERROR == bytes_received)
		{
			RemoveSocket::RemoveUsedSocket(socket_state.id);
			return false;
		}

		int msg_size = std::stoi(incomming_buffer);

		if (msg_size == 0)
		{
			RemoveSocket::RemoveUsedSocket(socket_state.id);
			return false;
		}

		//get main message from client
		bytes_received = 0;
		while(bytes_received < msg_size)
		{
			bytes_received += recv(current_socket, const_cast<char*>(incomming_buffer.c_str()), BUFFER_SIZE, 0);
			socket_state.buffer.append(incomming_buffer);
		}
		socket_state.buffer.erase(socket_state.buffer.begin() + msg_size, socket_state.buffer.end());

		if (SOCKET_ERROR == bytes_received)
		{
			RemoveSocket::RemoveUsedSocket(socket_state.id);
			return false;
		}

		if (bytes_received == 0)
		{
			RemoveSocket::RemoveUsedSocket(socket_state.id);
			return false;
		}
		else
		{
			std::ofstream test_output("C:/Lv-490_Files/output.txt", std::ios::app);
						
			//потім потрібно буде записувати прийняту інформацію в бд, але на даному етапі просто вивід в консоль є
			test_output << "Server: Recieved: " << bytes_received << " bytes of \"" << socket_state.buffer << "\" message.\n";

			std::string xml_string = socket_state.buffer;

			CXMLParser::XMLParser xml_parser;
			xml_parser.PrepareToDBManager(xml_string);

			test_output << "Cpu numbers: " << xml_parser.get_cpunumbers() << '\n';
			test_output << "Cpu speed: " << xml_parser.get_cpuspeed() << '\n';
			test_output << "Cpu vendor: " << xml_parser.get_cpuvendor() << '\n';
			for (size_t i = 0; i < xml_parser.get_harddisk_free().size(); ++i)
			{
				test_output << xml_parser.get_harddisk_type_list()[i] << '\n';
				test_output << "Hard disk free: " << xml_parser.get_harddisk_free()[i] << '\n';
				test_output << "Hard disk total size: " << xml_parser.get_harddisk_totalsize()[i] << '\n';
				test_output << "Hard disk used: " << xml_parser.get_harddisk_used()[i] << '\n';
			}
			test_output << "Ip: " << xml_parser.get_ipaddress() << '\n';
			test_output << "Mac: " << xml_parser.get_macaddress() << '\n';
			test_output << "Ram: " << xml_parser.get_totalram() << '\n';

			test_output.close();

			if (socket_state.buffer == "exit")
			{
				RemoveSocket::RemoveUsedSocket(socket_state.id);
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
			//add message receiving in thread pool
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
