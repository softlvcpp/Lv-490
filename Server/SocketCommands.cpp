#include "SocketCommands.h"

bool AddSocketConnection::Execute(SOCKET_shared_ptr& socket_state)//return bool
{
	//create socket wrapper with autodeleter
	socket_state = socket_wrapper.MakeSharedSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//check for errors to ensure that the new_socket is a valid socket.	
	if (INVALID_SOCKET == socket_state->id)
	{
		socket_state->log_msg = "Server: Error at socket(): " + WSAGetLastError();
		return false;
	}

	//create and configurate a socket settings for our new socket
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	//set IP
	serverService.sin_addr.s_addr = inet_addr(m_server_configuration->get_ipadress().c_str());
	//set port
	serverService.sin_port = htons(std::stoi(m_server_configuration->get_listenport().c_str()));
	
	//bind the socket for client's requests
	if (SOCKET_ERROR == ::bind(socket_state->id, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		socket_state->log_msg = "Server: Error at bind(): " + WSAGetLastError();
		return false;
	}

	//listen on the socket for incoming connections.
	if (SOCKET_ERROR == listen(socket_state->id, SOMAXCONN))
	{
		socket_state->log_msg = "Server: Error at listen(): " + WSAGetLastError();
		return false;
	}
	return true;
}

bool AcceptConnection::Execute(SOCKET_shared_ptr& socket_state)
{	
	//address of sending partner
	struct sockaddr_in client;		
	int client_length = sizeof(client);

	//create socket wrapper with autodeleter for accepted client
	socket_state = socket_wrapper.MakeSharedSocket(m_server_socket->id, (struct sockaddr*) & client, &client_length);

	if (INVALID_SOCKET == socket_state->id)
	{
		socket_state->log_msg = "Server: Error at socket(): " + WSAGetLastError();
		return false;
	}

	return true;
}

bool ReceiveMessage::Execute(SOCKET_shared_ptr& socket_state)
{	
	SOCKET current_socket = socket_state->id;

	while (true)
	{
		//get message size from client
		socket_state->buffer.clear();
		std::string incomming_buffer;
		incomming_buffer.resize(BUFFER_SIZE);
		int bytes_received = recv(current_socket, const_cast<char*>(incomming_buffer.c_str()), sizeof(int), 0);

		if (SOCKET_ERROR == bytes_received || bytes_received == 0)
		{
			return false;
		}

		int msg_size = std::stoi(incomming_buffer);

		if (msg_size == 0)
		{
			return false;
		}

		//get main message from client
		bytes_received = 0;
		while (bytes_received < msg_size)
		{
			bytes_received += recv(current_socket, const_cast<char*>(incomming_buffer.c_str()), BUFFER_SIZE, 0);
			socket_state->buffer.append(incomming_buffer);
		}
		if (msg_size <= socket_state->buffer.size())
		{
			socket_state->buffer.erase(socket_state->buffer.begin() + msg_size, socket_state->buffer.end());
		}
				
		if (SOCKET_ERROR == bytes_received || bytes_received == 0)
		{
			return false;
		}
		else
		{
			std::ofstream test_output("C:/Lv-490_Files/output.txt", std::ios::app);
						
			//this information must be write into data base, when it will be implemented, please delete file output 
			test_output << "Server: Recieved: " << bytes_received << " bytes of \"" << socket_state->buffer << "\" message.\n";

			std::string xml_string = socket_state->buffer;

			XMLServer xml_parser;
			xml_parser.PrepareToDBManager(xml_string);		
					

			test_output << "Cpu numbers: " << xml_parser.get_cpu_numbers() << '\n';
			test_output << "Cpu speed: " << xml_parser.get_cpu_speed() << '\n';
			test_output << "Cpu vendor: " << xml_parser.get_cpu_vendor() << '\n';
			for (size_t i = 0; i < xml_parser.get_hard_disk_free().size(); ++i)
			{
				test_output << xml_parser.get_hard_disk_type_list()[i] << '\n';
				test_output << "Hard disk free: " << xml_parser.get_hard_disk_free()[i] << '\n';
				test_output << "Hard disk total size: " << xml_parser.get_hard_disk_total_size()[i] << '\n';
				test_output << "Hard disk used: " << xml_parser.get_hard_disk_used()[i] << '\n';
			}
			test_output << "Ip: " << xml_parser.get_ip_address() << '\n';
			test_output << "Mac: " << xml_parser.get_mac_address() << '\n';
			test_output << "Ram: " << xml_parser.get_total_ram() << '\n';

			test_output.close();

			if (socket_state->buffer == "exit")
			{	
				return true;
			}
		}
	}
	return true;
}

void StartConnection::DoRecv(SOCKET_shared_ptr& new_conection)
{
	ReceiveMessage receive_message;	
	receive_message.Execute(new_conection);
}

bool StartConnection::Execute(SOCKET_shared_ptr& socket_state)
{	
	while (true)
	{
		SOCKET_shared_ptr new_conection;
		AcceptConnection accept_connection(socket_state);
		accept_connection.Execute(new_conection);
		if(new_conection->state == ACCEPTED)
		{
			new_conection->state = RECEIVE;
			//add message receiving in thread pool
			m_thread_pool->ExecuteTask(&StartConnection::DoRecv, this, new_conection);
		}
	}
	return true;
}

bool StartConnection::InitThreadPool(ThreadPool* main_pool)
{
	m_thread_pool = std::shared_ptr<ThreadPool>(main_pool); 
	return true;
}

bool StartConnection::InitThreadPool(std::shared_ptr<ThreadPool> main_pool)
{
	m_thread_pool = main_pool;
	return true;
}

bool Command::InitConfiguration(XMLServer* server_configuration)
{
	m_server_configuration = std::shared_ptr<XMLServer>(server_configuration);
	return true;
}

bool Command::InitConfiguration(std::shared_ptr<XMLServer> server_configuration)
{
	m_server_configuration = server_configuration;
	return true;
}
