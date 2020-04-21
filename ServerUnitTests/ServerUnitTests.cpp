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

			Assert::IsTrue(is_connected == true, L"Initialize should return 1");
		}


		//Danylo's tests

	};
}
