#include <iostream>
#include "LoggerDLL.h"

void logMillion()
{
	filelog::FileLogger logger{ "../LogFiles/logfile.txt", log490::LogLevel::Debug, 1024 * 1024 * 1024 };
	for (int i = 0; i < 1000000; ++i)
		SLOG_PROD(logger) << std::to_string(i).c_str() << "_______________________________________ (0_0)";
	logger.join();
}

void logMillion_x(size_t n)
{
	filelog::FileLogger logger{ "../LogFiles/logfile.txt", log490::LogLevel::Trace, 1024 * 1024 * 1024 };
	std::vector<std::thread> threads;
	for (size_t i = 0; i < n; ++i)
	{
		threads.emplace_back(std::thread{			
			[&logger]()
			{
				for (int i = 0; i < 1000000; ++i)
					SLOG_PROD(logger) << std::to_string(i).c_str() << "___________________MT__________________ (0_0)";
			}
			});
	}
	for (auto& t : threads) t.join();
	logger.join();
}

void logMutliMillion_NoLogs()
{
	constexpr size_t threadCounter = 10;

	filelog::FileLogger logger{ "../LogFiles/logfile.txt", log490::LogLevel::NoLogs };
	std::vector<std::thread> threads;
	for (size_t i = 0; i < threadCounter; ++i)
	{
		threads.emplace_back(std::thread{
			[&logger]()
			{
				for (int i = 0; i < 1000000; ++i)
					SLOG_PROD(logger) << std::to_string(i).c_str() << "___________________MT__________________ (0_0)";
			}
			});
	}
	for (auto& t : threads) t.join();
	logger.join();
}




int main(int argc, char** argv)
{
	filelog::FileLogger logger{ "../LogFiles/logfile_one.txt", log490::LogLevel::Debug, 1024 * 1024 * 1024 };
	SLOG_TRACE(logger) << "_______________________________________ (0_0)";

	auto msg{ SLOG_DEBUG(logger) };
	msg << "aaaa";

	msg << "bbbb" << "cccc" << SLOG_ENDL;
	msg << "bbbb" << "cccc" << SLOG_ENDL;

	logger.join();

	{
		auto start = std::chrono::high_resolution_clock::now();
		logMillion();
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "ST: " << (end - start).count() / 1000000 << '\n';
	}
	{
		auto start = std::chrono::high_resolution_clock::now();
		logMutliMillion_NoLogs();
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "MT: " << (end - start).count() / 1000000 << '\n';
	}
	{
		auto start = std::chrono::high_resolution_clock::now();
		logMillion_x(4);
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "MT: " << (end - start).count() / 1000000 << '\n';
	}
	std::cin.get();
}