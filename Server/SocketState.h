#pragma once

#include <winsock2.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <Mstcpip.h>

constexpr int BUFFER_SIZE = 512;//default buffer size

enum
{
	LISTEN,
	ACCEPTED,
	RECEIVE
};

typedef struct
{
	SOCKET id;			//socket handle
	int	state;			//receiving? 0 - LISTEN, 1 - ACCEPTED, 2 - RECEIVEING
	std::string buffer;
	std::string log_msg;
} SocketState;