// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H


#define _HAS_TR1_NAMESPACE 1
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "gmock-global/gmock-global.h"

#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>

#include <memory>
#include <string_view>
#include <iostream>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <functional>
#include <future>
#include <vector>
#include <thread>
#include <queue>
#include <codecvt>
#include <ctime>


#endif //PCH_H
