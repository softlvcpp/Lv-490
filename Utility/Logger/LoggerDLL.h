#pragma once
#ifndef LOGGERDLL_H
#define LOGGERDLL_H

#include "Essentials/LogFundamentals.h"

//
// Compile time logs
#ifndef CTLVL_NOLOGS
#define CTLVL_NOLOGS  0 
#endif 

#ifndef CTLVL_PROD
#define CTLVL_PROD    1 
#endif

#ifndef CTLVL_DEBUG
#define CTLVL_DEBUG   2
#endif 

#ifndef CTLVL_TRACE
#define CTLVL_TRACE   3
#endif 
//
//

#ifndef CTIME_LOG_LEVEL
/* Current compile time log level */
#define CTIME_LOG_LEVEL CTLVL_TRACE
#endif // !CTIME_LOG_LEVEL


//
//
//
//
//
//
//
//
//
//
//

#ifndef VOID_LOG
/* Message object that does nothing */
#define VOID_LOG log490::VoidMessage::voidMsg
#endif // !VOID_LOG

#ifndef SLOG_NOTHING
/* Message macro that does cretes VOID_LOG */
#define SLOG_NOTHING VOID_LOG
#endif // !SLOD_NOTHING

#include "Filelogger/filelogger.h"
#include "Filelogger/FileLogAdapters.h"

#define LOGGER_OPERATOR(typeSig) template <>\
inline filelog::FilteredFileLogMessage& filelog::FilteredFileLogMessage::operator<<<typeSig##>

LOGGER_OPERATOR(std::string)(std::string&& param)
{
	(*this) << param.c_str();
	return *this;
}
LOGGER_OPERATOR(std::string)(const std::string& param)
{
	(*this) << param.c_str();
	return *this;
}


#if CTLVL_PROD <= CTIME_LOG_LEVEL
#ifndef SLOG_PROD
/* Creates message with Prod level value */
#define SLOG_PROD(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_PROD))
#endif
#else
#ifndef SLOG_PROD
/* Creates VOID_LOG */
#define SLOG_PROD(logger) VOID_LOG
#endif
#endif

#if CTLVL_DEBUG <= CTIME_LOG_LEVEL
#ifndef SLOG_DEBUG
/* Creates message with Debug level value */
#define SLOG_DEBUG(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_DEBUG))
#endif
#else
#ifndef SLOG_DEBUG
/* Creates VOID_LOG */
#define SLOG_DEBUG(logger) VOID_LOG
#endif
#endif

#if CTLVL_TRACE <= CTIME_LOG_LEVEL
#ifndef SLOG_TRACE
/* Creates message with Trace level value */
#define SLOG_TRACE(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_TRACE))
#endif
#else
#ifndef SLOG_TRACE
/* Creates VOID_LOG */
#define SLOG_TRACE(logger) VOID_LOG
#endif
#endif

#endif // !LOGGERDLL_H
