#pragma once
#include "filelogger.h"

namespace filelog
{

	struct CompileTimeLogData
	{
		log490::signature_t functionSignature;
		log490::line_t messageConstructLine;
	};

	class StaticFileLogMessage final
	{
	public:
		inline StaticFileLogMessage()
			: targetLogger{ (FileLogger&)log490::DummyLogger::instance()}, messageLevel{ filelog::LogLevel::NoLogs }
		{ }
		inline StaticFileLogMessage(FileLogger& targetLogger)
			: targetLogger{ targetLogger }, messageLevel{ targetLogger.getRTLevel() }
		{ }	
		inline FilteredFileLogMessage makeMessage(CompileTimeLogData data)
		{	
			return FilteredFileLogMessage{ targetLogger, messageLevel, data.functionSignature, data.messageConstructLine };
		}
		inline bool usesThisLogger(filelog::FileLogger& logger)
		{
			return &targetLogger.get() == &logger;
		}
		inline filelog::LogLevel getLogLevel() const { return messageLevel; }
	private:
		std::reference_wrapper<FileLogger> targetLogger;
		filelog::LogLevel messageLevel;
	};

}

#define LOGMSG(staticMessage) staticMessage.makeMessage(filelog::CompileTimeLogData{ LOG_FUNCTION_MACRO, LOG_LINE_MACRO })