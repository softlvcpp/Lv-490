#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "SocketState.h"
#include "SocketDeleter.h"

class Command
{
public:
	Command() {};
	virtual bool Execute(SOCKET_shared_ptr& socket_state) = 0;
	
	bool InitConfiguration(XMLServer* server_configuration);
	bool InitConfiguration(std::shared_ptr<XMLServer> server_configuration);
protected:
	std::shared_ptr<XMLServer> m_server_configuration;
	SocketWrapper socket_wrapper;
};

class AddSocketConnection : public Command
{
public:
	AddSocketConnection() {};
	bool Execute(SOCKET_shared_ptr& socket_state) override;
};

class AcceptConnection : public Command
{
public:
	AcceptConnection(SOCKET_shared_ptr server_socket) : m_server_socket(server_socket) {};
	bool Execute(SOCKET_shared_ptr& socket_state) override;
private:
	SOCKET_shared_ptr m_server_socket;
};

class ReceiveMessage : public Command
{
public:
	ReceiveMessage() {};
	bool Execute(SOCKET_shared_ptr& socket_state) override;
	bool InitDatabase(std::shared_ptr<DatabaseManager> m);
private:
	std::shared_ptr<DatabaseManager> m_data_base;
};

class StartConnection : public Command
{
public:
	StartConnection();
	bool Execute(SOCKET_shared_ptr& socket_state) override;

	bool InitThreadPool(ThreadPool* main_pool);
	bool InitThreadPool(std::shared_ptr<ThreadPool> main_pool);

private:
	std::shared_ptr<ThreadPool> m_thread_pool;
	void DoRecv(SOCKET_shared_ptr& new_conection);
	std::shared_ptr<DatabaseManager> db;	
};