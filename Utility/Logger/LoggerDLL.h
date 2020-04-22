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
#include "filelogger_messages.h"
#include "Filelogger/FileLogAdapters.h"
#include "StaticLogging.h"

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

namespace filelog
{
    template <typename T>
    constexpr FileLogger& toRef(T& obj)
    {
        if constexpr (std::is_same<T, filelog::FileLogger*>::value)
            return *obj;
        else if constexpr (std::is_same<T, std::unique_ptr<filelog::FileLogger>>::value)
            return *obj.get();
        else if constexpr (std::is_same<T, logbase::ILogger*>::value)
            return *static_cast<filelog::FileLogger*>(obj);
        else if constexpr (std::is_same<T, std::unique_ptr<logbase::ILogger>>::value)
            return *static_cast<filelog::FileLogger*>(obj.get());
        else if constexpr (true)
            return obj;
    }

#ifndef SLOG_LEVEL
#define SLOG_LEVEL(logger, level) filelog::FilteredFileLogMessage{ \
    filelog::toRef(logger), \
    static_cast<filelog::LogLevel>(level), \
    LOG_FUNCTION_MACRO, LOG_LINE_MACRO }
#endif
}

#ifndef SLOG_ENDL
#define SLOG_ENDL filelog::MsgEndl()
#endif


#if CTLVL_PROD <= CTIME_LOG_LEVEL
#ifndef SLOG_PROD
/* Creates message with Prod level value */
#define SLOG_PROD(logger) SLOG_LEVEL(logger, static_cast<log490::level_t>(CTLVL_PROD))
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
#define SLOG_DEBUG(logger) SLOG_LEVEL(logger, static_cast<log490::level_t>(CTLVL_DEBUG))
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
#define SLOG_TRACE(logger) SLOG_LEVEL(logger, static_cast<log490::level_t>(CTLVL_TRACE))
#endif
#else
#ifndef SLOG_TRACE
/* Creates VOID_LOG */
#define SLOG_TRACE(logger) VOID_LOG
#endif
#endif

#endif // !LOGGERDLL_H

