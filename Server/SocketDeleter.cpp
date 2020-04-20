#include "pch.h"
#include "SocketDeleter.h"

void SocketDeleter::operator()(SocketState* used_socket)
{
    if (SOCKET_ERROR != used_socket->id)
    {
        closesocket(used_socket->id);        
    }
}

SOCKET_unique_ptr SocketWrapper::MakeUniqueSocket(int af, int type, int protocol)
{
    SOCKET new_socket = socket(af, type, protocol);

    if (INVALID_SOCKET == new_socket)
    {
        return nullptr;
    }

    SocketState* new_socket_state = new SocketState();
    new_socket_state->id = new_socket;
    new_socket_state->state = LISTEN;
    return SOCKET_unique_ptr(new_socket_state);
}

SOCKET_unique_ptr SocketWrapper::MakeUniqueSocket(SOCKET s, sockaddr* addr, int* addrlen)
{
    SOCKET accepted_socket = accept(s, addr, addrlen);

    if (INVALID_SOCKET == accepted_socket)
    {
        return nullptr;
    }

    SocketState* new_socket_state = new SocketState();
    new_socket_state->id = accepted_socket;
    new_socket_state->state = ACCEPTED;
    return SOCKET_unique_ptr(new_socket_state);
}

SOCKET_shared_ptr SocketWrapper::MakeSharedSocket(int af, int type, int protocol)
{
    SOCKET new_socket = socket(af, type, protocol);

    if (INVALID_SOCKET == new_socket)
    {
        return nullptr;
    }

    SocketState new_socket_state;
    new_socket_state.id = new_socket;
    new_socket_state.state = LISTEN;

    auto return_ptr = SOCKET_shared_ptr(new SocketState{ new_socket, LISTEN }, SocketDeleter());
    return return_ptr;
}

SOCKET_shared_ptr SocketWrapper::MakeSharedSocket(SOCKET s, sockaddr* addr, int* addrlen)
{
    SOCKET accepted_socket = accept(s, addr, addrlen);

    if (INVALID_SOCKET == accepted_socket)
    {
        return nullptr;
    }

    SocketState* new_socket_state = new SocketState();
    new_socket_state->id = accepted_socket;
    new_socket_state->state = ACCEPTED;
    return SOCKET_shared_ptr(new_socket_state, SocketDeleter());
}
