#include "SocketCommands.h"

bool AddSocketConnection::Execute(SocketState& socket_state)//return bool
{
	WSAData wsaData;//config sockets
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		//log: "Time Server: Error at WSAStartup()\n"
	}

	// create new socket, use the Internet address family (AF_INET), streaming sockets (SOCK_STREAM), and the TCP/IP protocol (IPPROTO_TCP).
	SOCKET new_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// check for errors to ensure that the new_socket is a valid socket.	
	if (INVALID_SOCKET == new_socket)
	{
		//log: "Time Server: Error at socket(): " + WSAGetLastError();
		WSACleanup();//clean configuration
		return false;
	}

	//create and configurate a socket settings for our new socket
	sockaddr_in serverService;
	//AF_INET - Internet address family
	serverService.sin_family = AF_INET;
	//the IP address is INADDR_ANY to accept connections on all interfaces.
	serverService.sin_addr.s_addr = INADDR_ANY;
	//set port
	serverService.sin_port = htons(PORT);

	//bind the socket for client's requests
	if (SOCKET_ERROR == bind(new_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		//log: "Server: Error at bind(): " + WSAGetLastError()
		closesocket(new_socket);
		WSACleanup();
		return false;
	}

	// Listen on the Socket for incoming connections.
	// This socket accepts only one connection (no pending connections 
	// from other clients). This sets the backlog parameter.
	if (SOCKET_ERROR == listen(new_socket, SOMAXCONN))
	{
		//log: "Server: Error at listen(): " + WSAGetLastError()
		closesocket(new_socket);
		WSACleanup();
		return false;
	}

	////set the socket to be in non-blocking mode.
	//unsigned long flag = 1;
	//if (ioctlsocket(new_socket, FIONBIO, &flag) != 0)
	//{
	//	//log: "Server: Error at ioctlsocket(): " + WSAGetLastError()
	//}

	SocketState new_socket_state;
	new_socket_state.id = new_socket;
	new_socket_state.state = LISTEN;
	socket_state = new_socket_state;
	return true;
}

bool RemoveSocket::Execute(SocketState& socket_state)
{
	closesocket(socket_state.id);	
	socket_state.state = -1;
	return true;
}

bool AcceptConnection::Execute(SocketState& socket_state)
{	
	struct sockaddr_in client;		// Address of sending partner
	int client_length = sizeof(client);

	SOCKET accepted_socket = accept(server_socket, (struct sockaddr*) & client, &client_length);
	if (INVALID_SOCKET == accepted_socket)
	{
		//log: "Server: Error at accept(): " + WSAGetLastError()
		return false;
	}

	//log: "Server: Client " + inet_ntoa(from.sin_addr) + ":" + ntohs(from.sin_port) + " is connected.";
		
	//tcp_keepalive KeepAlive;
	//DWORD dJunk;

	//// Use socket level keep alive for about 5 minutes
	//// Unless this is done Microsoft will not close the socket
	//// in the event of a cable / VPN disconnection for 2 hours.
	//KeepAlive.onoff = 1;
	//KeepAlive.keepalivetime = 6000;
	//KeepAlive.keepaliveinterval = 6000;

	//if (SOCKET_ERROR == WSAIoctl(accepted_socket, SIO_KEEPALIVE_VALS, &KeepAlive, sizeof(KeepAlive), NULL, 0, &dJunk, NULL, NULL))
	//{
	//	std::cout << "error" << "\n";
	//}
		
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
		socket_state.buffer.resize(BUFFER_SIZE);
		int bytes_received = recv(current_socket, (char*)socket_state.buffer.c_str(), BUFFER_SIZE, 0);
		size_t msg_end = socket_state.buffer.find('\0');
		socket_state.buffer.erase(socket_state.buffer.begin() + msg_end, socket_state.buffer.end());

		if (SOCKET_ERROR == bytes_received)
		{
			//log: "Server: Error at recv(): " + WSAGetLastError();		
			//RemoveSocket remove_socket;
			//remove_socket.Execute(socket_state);
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

			if (bytes_received > 0)
			{
				if (socket_state.buffer == "exit")
				{
					RemoveSocket remove_socket;
					remove_socket.Execute(socket_state);
					return true;
				}
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