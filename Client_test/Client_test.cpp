#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdio>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512

int  main(int argc, char** argv)
{
	WSADATA wsa_data;
	SOCKET connect_socket = INVALID_SOCKET;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	const char* sendbuf = "LV-490 Ñ++. If you see this, the message was delivered correctly.";
	char recvbuf[DEFAULT_BUFLEN];
	int return_result;
	int receive_buffer_length = DEFAULT_BUFLEN;

	// Validate the parameters
	if (argc != 3) {
		return 1;
	}

	return_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (return_result != 0) {
		std::cout << "Could not initialize socket";
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	return_result = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (return_result != 0) {
		std::cout << "getaddrinfo failed \n";
		WSACleanup();
		return 1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		connect_socket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (connect_socket == INVALID_SOCKET) {
			std::cout << "Socket failed";
			WSACleanup();
			return 1;
		}

		return_result = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (return_result == SOCKET_ERROR) {
			closesocket(connect_socket);
			connect_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (connect_socket == INVALID_SOCKET) {
		std::cout << "Cannot connect to the server";
		WSACleanup();
		return 1;
	}

	return_result = send(connect_socket, sendbuf, (int)strlen(sendbuf), 0);
	if (return_result == SOCKET_ERROR) {
		std::cout << "Send failed";
		closesocket(connect_socket);
		WSACleanup();
		return 1;
	}

	return_result = shutdown(connect_socket, SD_SEND);
	if (return_result == SOCKET_ERROR) {
		std::cout << "Shutdown failed";
		closesocket(connect_socket);
		WSACleanup();
		return 1;
	}

	do {

		return_result = recv(connect_socket, recvbuf, receive_buffer_length, 0);
		if (result > 0)
			printf("Bytes received: %d\n", result);
		else if (result == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (result > 0);

	// cleanup
	closesocket(connect_socket);
	WSACleanup();

	return 0;
}