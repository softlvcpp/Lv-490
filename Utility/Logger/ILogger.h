#pragma once
#include <stdint.h>
#include <time.h>
#include <memory>

#define LOG_LINE_TYPE uint64_t
#define LOG_LEVEL_TYPE uint64_t

#ifndef LOG_FUNCTION_MACRO
#define LOG_FUNCTION_MACRO __FUNCTION__
#endif 

#ifndef LOG_LINE_MACRO
#define LOG_LINE_MACRO static_cast<LOG_LINE_TYPE>(__LINE__)
#endif 

namespace logbase
{

	class ILogger
	{
	public:
		virtual ~ILogger() { }
	};

	struct CompileTimeLoggingInfo
	{
		CompileTimeLoggingInfo(const char* mF, LOG_LINE_TYPE mLine)
			: mFunction{ mF }, mLine { mLine } { }
		const char* mFunction;
		const LOG_LINE_TYPE mLine;
	};

	struct LoggerCreationParamsBase
	{
		virtual ~LoggerCreationParamsBase() { }
		LOG_LEVEL_TYPE minimumLevel;
	};

	class LoggerCreatorBase
	{
	public:
		virtual ~LoggerCreatorBase() { }
		virtual std::unique_ptr<ILogger> create(LoggerCreationParamsBase& params) = 0;
	};
	
}

#define CAPTURE_CALL_INFO logbase::CompileTimeLoggingInfo( LOG_FUNCTION_MACRO, LOG_LINE_MACRO )
