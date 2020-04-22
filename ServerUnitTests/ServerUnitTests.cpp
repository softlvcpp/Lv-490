#include "pch.h"


#include "CppUnitTest.h"

#include "../Server/SocketState.h"
#include "../Server/ThreadPool.h"
#include "../Server/ThreadPool.cpp"
#include "../Server/SocketDeleter.h"
#include "../Server/SocketDeleter.cpp"
#include "../Server/SocketCommands.h"
#include "../Server/SocketCommands.cpp"
#include "../Server/SocketHandler.h"
#include "../Server/SocketHandler.cpp"

#include "../Server/Server.h"
#include "../Server/Server.cpp"
#include "../Server/SmartServiceHandle.h"
#include "../Server/ServerService.h"
#include "../Server/ServerService.cpp"

std::shared_ptr<ServerService> ServerService::s_instance = nullptr;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;

class MockParser : public XMLServer
{
public:
	MOCK_METHOD0(get_ipadress, string());
	MOCK_METHOD0(get_listenport, int());
};

class MockServerService : public ServerService
{
public:
	MOCK_METHOD0(Install, bool());
	MOCK_METHOD0(Uninstall, bool());
	MOCK_METHOD0(Start, bool());
	MOCK_METHOD0(Stop, bool());
	MOCK_METHOD0(Restart, bool());
	MOCK_METHOD0(ReadConfig, bool());
	MOCK_METHOD0(InitLogger, bool());

	static ServerService& get_instance()
	{
		if (s_instance == nullptr)
		{
			s_instance = std::make_unique<MockServerService>();
		}
		return *s_instance;
	}
	static ServerService* get_instance_ptr()
	{
		if (s_instance == nullptr)
		{
			s_instance = std::make_unique<MockServerService>();
		}
		return s_instance.get();
	}
	MockServerService() = default;
	MockServerService(const MockServerService&) = default;
};

namespace ServerUnitTests
{
	TEST_CLASS(ServerUnitTests)
	{
	public:

		BEGIN_TEST_METHOD_ATTRIBUTE(TestAddConnection1)
			TEST_OWNER(L"Oleh_Dmytrash")
			END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(TestAddConnection1)
		{
			std::shared_ptr<MockParser> mock_parser(new MockParser());			
			AddSocketConnection add_socket_connection;
			SocketWrapper sock_wrapper;
			auto socket = sock_wrapper.MakeSharedSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			EXPECT_CALL(*mock_parser, get_ipadress()).Times(1).WillOnce(Return("127.0.0.1"));
			EXPECT_CALL(*mock_parser, get_listenport()).Times(1).WillOnce(Return(8080));		

			mock_parser->set_ipadress(mock_parser->get_ipadress());
			mock_parser->set_listenport(mock_parser->get_listenport());

			add_socket_connection.InitConfiguration(mock_parser);
			bool is_connected = add_socket_connection.Execute(socket);

			Assert::IsTrue(is_connected == true, L"Execute should return true");
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(TestAddConnection2)
			TEST_OWNER(L"Oleh_Dmytrash")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(TestAddConnection2)
		{
			std::shared_ptr<MockParser> mock_parser(new MockParser());
			AddSocketConnection add_socket_connection;
			SocketWrapper sock_wrapper;
			auto socket = sock_wrapper.MakeSharedSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			
			add_socket_connection.InitConfiguration(mock_parser);
			bool is_connected = add_socket_connection.Execute(socket);
                        Assert::IsTrue(is_connected == true, L"Execute should return true");
		}


		//Danylos tests

		BEGIN_TEST_METHOD_ATTRIBUTE(SERVICE_INSTALL_METHOD)
			TEST_OWNER(L"Danylo_Sadivnychyi")
		END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(SERVICE_INSTALL_METHOD)
		{
			int argc = 2;
			const char* argv[]{ "Exe_name", "install" };

			MockServerService* service = dynamic_cast<MockServerService*>(MockServerService::get_instance_ptr());//std:make_unique<MockServerService>(MockServerService::get_instance_ptr());
			ON_CALL(*service, Install()).WillByDefault(Return(true));
			EXPECT_CALL(*service, ReadConfig()).Times(1).WillOnce(Return(true));
			ON_CALL(*service, InitLogger()).WillByDefault(Return(true));

			bool run_result = service->Run(argc, const_cast<char**>(argv));
			Assert::IsTrue(run_result, L"Run must return true");
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(SERVICE_UNINSTALL_METHOD)
			TEST_OWNER(L"Danylo_Sadivnychyi")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(SERVICE_UNINSTALL_METHOD)
		{
			int argc = 2;
			const char* argv[]{ "Exe_name", "uninstall" };

			MockServerService* service = dynamic_cast<MockServerService*>(MockServerService::get_instance_ptr());//std:make_unique<MockServerService>(MockServerService::get_instance_ptr());
			ON_CALL(*service, Install()).WillByDefault(Return(true));
			EXPECT_CALL(*service, ReadConfig()).Times(1).WillOnce(Return(true));
			ON_CALL(*service, InitLogger()).WillByDefault(Return(true));

			bool run_result = service->Run(argc, const_cast<char**>(argv));
			Assert::IsTrue(run_result, L"Run must return true");
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(SERVICE_START_METHOD)
			TEST_OWNER(L"Danylo_Sadivnychyi")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(SERVICE_START_METHOD)
		{
			int argc = 2;
			const char* argv[]{ "Exe_name", "start" };

			MockServerService* service = dynamic_cast<MockServerService*>(MockServerService::get_instance_ptr());//std:make_unique<MockServerService>(MockServerService::get_instance_ptr());
			ON_CALL(*service, Install()).WillByDefault(Return(true));
			EXPECT_CALL(*service, ReadConfig()).Times(1).WillOnce(Return(true));
			ON_CALL(*service, InitLogger()).WillByDefault(Return(true));

			bool run_result = service->Run(argc, const_cast<char**>(argv));
			Assert::IsTrue(run_result, L"Run must return true");
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(SERVICE_STOP_METHOD)
			TEST_OWNER(L"Danylo_Sadivnychyi")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(SERVICE_STOP_METHOD)
		{
			int argc = 2;
			const char* argv[]{ "Exe_name", "stop" };

			MockServerService* service = dynamic_cast<MockServerService*>(MockServerService::get_instance_ptr());//std:make_unique<MockServerService>(MockServerService::get_instance_ptr());
			ON_CALL(*service, Install()).WillByDefault(Return(true));
			EXPECT_CALL(*service, ReadConfig()).Times(1).WillOnce(Return(true));
			ON_CALL(*service, InitLogger()).WillByDefault(Return(true));

			bool run_result = service->Run(argc, const_cast<char**>(argv));
			Assert::IsTrue(run_result, L"Run must return true");
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(SERVICE_RESTART_METHOD)
			TEST_OWNER(L"Danylo_Sadivnychyi")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(SERVICE_RESTART_METHOD)
		{
			int argc = 2;
			const char* argv[]{ "Exe_name", "restart" };

			MockServerService* service = dynamic_cast<MockServerService*>(MockServerService::get_instance_ptr());//std:make_unique<MockServerService>(MockServerService::get_instance_ptr());
			ON_CALL(*service, Install()).WillByDefault(Return(true));
			EXPECT_CALL(*service, ReadConfig()).Times(1).WillOnce(Return(true));
			ON_CALL(*service, InitLogger()).WillByDefault(Return(true));

			bool run_result = service->Run(argc, const_cast<char**>(argv));
			Assert::IsTrue(run_result, L"Run must return true");
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(SERVICE_BAD_INPUT_METHOD)
			TEST_OWNER(L"Danylo_Sadivnychyi")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(SERVICE_BAD_INPUT_METHOD)
		{
			int argc = 2;
			const char* argv[]{ "Exe_name", "sdgflkjhfvd" };

			MockServerService* service = dynamic_cast<MockServerService*>(MockServerService::get_instance_ptr());//std:make_unique<MockServerService>(MockServerService::get_instance_ptr());
			ON_CALL(*service, Install()).WillByDefault(Return(true));
			EXPECT_CALL(*service, ReadConfig()).Times(1).WillOnce(Return(true));
			ON_CALL(*service, InitLogger()).WillByDefault(Return(true));

			bool run_result = service->Run(argc, const_cast<char**>(argv));
			Assert::IsTrue(run_result == false, L"Run must return false");
		}
	};
}
