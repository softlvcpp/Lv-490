#include "pch.h"
#define _HAS_TR1_NAMESPACE 1
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "gmock-global/gmock-global.h"

//#include "gtest/gtest.h"

#include "CppUnitTest.h"

#include "../Server/pch.h"
#include "../Server/ThreadPool.h"
#include "../Server/ThreadPool.cpp"
#include "../Server/SocketState.h"
#include "../Server/SocketCommands.h"
#include "../Server/SocketCommands.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::_;

namespace ServerUnitTests
{
	TEST_CLASS(ServerUnitTests)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			Command* c;
		}



		//Danylo's tests

	};
}
