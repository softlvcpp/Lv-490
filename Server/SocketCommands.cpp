#include "pch.h"

#include "ThreadPool.h"
#include "SocketCommands.h"

constexpr int BUFFER_SIZE = 512;//default buffer size
constexpr char DEFAULT_IP[] = "127.0.0.1";//default ip address
constexpr int DEFAULT_PORT = 8080;//default port number

bool AddSocketConnection::Execute(SOCKET_shared_ptr& socket_state)//return bool
{
	//create socket wrapper with autodeleter
	socket_state = m_socket_wrapper.MakeSharedSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	//check for errors to ensure that the new_socket is a valid socket.	
	if (INVALID_SOCKET == socket_state->id)
	{
		GLOG_T << "Server: Error at socket(): " + to_string(WSAGetLastError());
		return false;
	}

	//create and configurate a socket settings for our new socket
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	//set IP

	serverService.sin_addr.s_addr = inet_addr((INADDR_NONE == inet_addr(m_server_configuration->get_ipadress().c_str()))?
																DEFAULT_IP: m_server_configuration->get_ipadress().c_str());
	//set port
	int port = m_server_configuration->get_listenport();
	if(port <= 0)
	{
		port = DEFAULT_PORT;
	}

	serverService.sin_port = htons(port);
	
	//bind the socket for client's requests

	if (SOCKET_ERROR == ::bind(socket_state->id, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		GLOG_T << "Server: Error at bind(): " + to_string(WSAGetLastError());
		return false;
	}

	//listen on the socket for incoming connections.
	if (SOCKET_ERROR == listen(socket_state->id, SOMAXCONN))
	{
		GLOG_T << "Server: Error at listen(): " + to_string(WSAGetLastError());
		return false;
	}
	GLOG_T << "Server socket created";
	return true;
}

bool AcceptConnection::Execute(SOCKET_shared_ptr& socket_state)
{	
	//address of sending partner
	struct sockaddr_in client;		
	int client_length = sizeof(client);

	//create socket wrapper with autodeleter for accepted client
	socket_state = m_socket_wrapper.MakeSharedSocket(m_server_socket->id, (struct sockaddr*) & client, &client_length);

	if (INVALID_SOCKET == socket_state->id)
	{
		GLOG_T << "Server: Error at socket(): " + to_string(WSAGetLastError());
		return false;
	}

	GLOG_T << "Client connected";

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
		int msg_size;
		incomming_buffer.resize(BUFFER_SIZE);
		int bytes_received = recv(current_socket, reinterpret_cast<char*>(&msg_size), sizeof(int), 0);

		if (SOCKET_ERROR == bytes_received || bytes_received == 0)
		{
			return false;
		}
		
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
			GLOG_T << "Client disconected";
			return false;
		}
		else
		{		
			GLOG_T << "Message received";
			GLOG_T << "Client sented " + to_string(bytes_received) + " bytes";
			//write information in database
			std::string xml_string = socket_state->buffer;
			XMLServer xml_parser;
			xml_parser.PrepareToDBManager(xml_string);	
			m_data_base->setClient(xml_parser.get_mac_address(), xml_parser.get_ip_address(), xml_parser.get_total_ram(), 
				xml_parser.get_cpu_numbers(), 6, xml_parser.get_cpu_speed(),xml_parser.get_os());
								
			time_t timer;
			time(&timer);
			for (size_t i = 0; i < xml_parser.get_hard_disk_free().size(); ++i)
			{				
				m_data_base->setDisk(xml_parser.get_mac_address(), xml_parser.get_os(), xml_parser.get_hard_disk_total_size()[i], 
					xml_parser.get_hard_disk_type_list()[i], xml_parser.get_hard_disk_media_type()[i], xml_parser.get_hard_disk_total_size()[i], 
					xml_parser.get_hard_disk_used()[i], xml_parser.get_hard_disk_free()[i], timer);
			}
			for (auto i : xml_parser.get_processes())
			{
				m_data_base->setProcess(xml_parser.get_mac_address(), xml_parser.get_os(), i.second, i.first, timer);
			}
			if (socket_state->buffer == "exit")
			{	
				return true;
			}
		}
	}
	return true;
}

bool ReceiveMessage::InitDatabase(std::shared_ptr<DatabaseManager> m)
{
	if (m == nullptr) return false;
	m_data_base = std::shared_ptr<DatabaseManager>(m);
	return true;
}

void StartConnection::DoRecv(SOCKET_shared_ptr& new_conection)
{	
	ReceiveMessage receive_message;	
	receive_message.InitDatabase(m_data_base);
	receive_message.Execute(new_conection);
}

StartConnection::StartConnection()
{
	//set database connection
	m_data_base = std::shared_ptr<DatabaseManager>(new DatabaseMicrosoftSQLServer());
	m_data_base->Connect();
}

bool StartConnection::Execute(SOCKET_shared_ptr& socket_state)
{		
	while (true)
	{
		SOCKET_shared_ptr new_conection;
		AcceptConnection accept_connection(socket_state);		
		if (accept_connection.Execute(new_conection) == false)
		{
			GLOG_T << "Bad cliend connection";
			return false;
		}
		if(new_conection->state == State::ACCEPTED)
		{
			new_conection->state = State::RECEIVE;
			//add message receiving in thread pool
			m_thread_pool->ExecuteTask(&StartConnection::DoRecv, this, new_conection);
		}
	}
	return true;
}

bool StartConnection::InitThreadPool(ThreadPool* main_pool)
{
	if (main_pool == nullptr)
	{
		GLOG_T << "Bad thread pool pointer";
		return false;
	}
	m_thread_pool = std::shared_ptr<ThreadPool>(main_pool); 
	return true;
}


bool StartConnection::InitThreadPool(std::shared_ptr<ThreadPool> main_pool) 
{
	if (main_pool == nullptr)
	{
		GLOG_T << "Bad thread pool pointer";
		return false;
	}
	m_thread_pool = main_pool;
	return true;
}

bool Command::InitConfiguration(XMLServer* server_configuration)
{
	if (server_configuration == nullptr)
	{
		GLOG_T << "Bad XMLServer pointer";
		return false;
	}
	m_server_configuration = std::shared_ptr<XMLServer>(server_configuration);
	return true;
}

bool Command::InitConfiguration(std::shared_ptr<XMLServer> server_configuration)
{	
	if (server_configuration == nullptr)
	{
		GLOG_T << "Bad XMLServer pointer";
		return false;
	}
	m_server_configuration = server_configuration;
	return true;
}
