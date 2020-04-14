#pragma once

#include "SocketState.h"

//functor for delete used socket
class SocketDeleter
{
public:
    void operator()(SocketState* used_socket);
};

using SOCKET_unique_ptr = std::unique_ptr<SocketState, SocketDeleter>;
using SOCKET_shared_ptr = std::shared_ptr<SocketState>;

class SocketWrapper
{
public:
    SOCKET_unique_ptr MakeUniqueSocket(int af, int type, int protocol);
    SOCKET_unique_ptr MakeUniqueSocket(SOCKET s, sockaddr* addr, int* addrlen);

    SOCKET_shared_ptr MakeSharedSocket(int af, int type, int protocol);
    SOCKET_shared_ptr MakeSharedSocket(SOCKET s, sockaddr* addr, int* addrlen);
};