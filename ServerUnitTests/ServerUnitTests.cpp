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
using ::testing::_;

class MockSocketWrapper : public XMLServer
{	
public:
	MOCK_METHOD0(get_ipadress, string());
};

namespace ServerUnitTests
{
	TEST_CLASS(ServerUnitTests)
	{
	public:

		BEGIN_TEST_METHOD_ATTRIBUTE(TestMethod1)
			TEST_OWNER(L"Oleh_Dmytrash")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(TestMethod1)
		{
			//MockSocketWrapper mock_socket_wrapper;
			AddSocketConnection add_socket_connection;

		}


		//Danylo's tests

	};
}
