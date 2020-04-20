#pragma once
#pragma comment(lib, "Ws2_32.lib")

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

#include <winsock2.h>
#include <Windows.h>

#include "../Utility/XML_Parser/XMLServer.h"
#include "../Utility/Logger/LoggerDLL.h"
#include "../Utility/DatabaseManager/DatabaseManager.h"

#include "ThreadPool.h"
#include "SocketCommands.h"

