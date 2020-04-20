#pragma once

#include <winsock2.h>

enum State
{
	LISTEN,
	ACCEPTED,
	RECEIVE
};

typedef struct
{
	SOCKET id;			//socket handle
	State	state;			//receiving? 0 - LISTEN, 1 - ACCEPTED, 2 - RECEIVEING
	std::string buffer;
	std::string log_msg;
} SocketState;