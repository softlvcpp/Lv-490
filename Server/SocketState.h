#pragma once

#include "pch.h"

constexpr int BUFFER_SIZE = 512;//default buffer size
constexpr char DEFAULT_IP[] = "127.0.0.1";//default ip address
constexpr int DEFAULT_PORT = 8080;//default port number

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