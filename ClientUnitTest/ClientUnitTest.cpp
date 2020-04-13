#include "pch.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gmock-global/gmock-global.h>
#include "CppUnitTest.h"
#include <QtNetwork/qtcpsocket.h>
//#include "../Client/QTcpClientSocket.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;

bool foo() {
	return true;
}


MOCK_GLOBAL_FUNC1(sqrt, double(double));
namespace ClientUnitTest
{
	TEST_CLASS(ClientUnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			
			EXPECT_GLOBAL_CALL(sqrt, sqrt(5)).Times(1).WillOnce(Return(3));
			double retVal = sqrt(5);
			Assert::IsTrue(retVal == 3, L"ComputeSquareRootTest should return 3");

		}
	};
}
