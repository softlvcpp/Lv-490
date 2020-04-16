#include "pch.h"
#define _HAS_TR1_NAMESPACE 1
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "gmock-global/gmock-global.h"

//#include "gtest/gtest.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::_;

namespace ServerUnitTests
{
	TEST_CLASS(ServerUnitTests)
	{
	public:



		//Danylo's tests
		BEGIN_TEST_METHOD_ATTRIBUTE(LoginTestSuccess)
			TEST_OWNER(L"dan_sad")
		END_TEST_METHOD_ATTRIBUTE()
		TEST_METHOD(TestMethod1)
		{

		}

	};
}
