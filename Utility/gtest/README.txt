UnitTestProject->Properties(Зверху обов'язково поставте All configurations, All platforms)
{
	C++->General->Additional include directories-> Шлях до папки, в якій лежить цей файлик
	Linker->General->Additional Library directories-> Ця сама папка/libs
	Linker->Input-> Вводите там gmock.lib; gtest.lib;
}

Далі в самому файлі, де вивикористовуєте gmock, пишете:
#define _HAS_TR1_NAMESPACE 1
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "gmock-global/gmock-global.h"

Після цього всього мало б працювати.