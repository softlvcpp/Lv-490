#pragma once
#ifndef LOGGERDLL_H
#define LOGGERDLL_H

#define SLOG_NO_DEFAULT_MACROS
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


// Message object that does nothing
#ifndef VOID_LOG
#define VOID_LOG log490::VoidMessage::voidMsg
#endif // !VOID_LOG

#ifndef SLOG_NOTHING
#define SLOG_NOTHING VOID_LOG
#endif // !SLOD_NOTHING

#include "Filelogger/filelogger.h"
#include "Filelogger/FileLogAdapters.h"

#if CTLVL_PROD <= CTIME_LOG_LEVEL
#ifndef SLOG_PROD
#define SLOG_PROD(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_PROD))
#endif
#else
#ifndef SLOG_PROD
#define SLOG_PROD(logger) VOID_LOG
#endif
#endif

#if CTLVL_DEBUG <= CTIME_LOG_LEVEL
#ifndef SLOG_DEBUG
#define SLOG_DEBUG(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_DEBUG))
#endif
#else
#ifndef SLOG_DEBUG
#define SLOG_DEBUG(logger) VOID_LOG
#endif
#endif

#if CTLVL_TRACE <= CTIME_LOG_LEVEL
#ifndef SLOG_TRACE
#define SLOG_TRACE(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_TRACE))
#endif
#else
#ifndef SLOG_TRACE
#define SLOG_TRACE(logger) VOID_LOG
#endif
#endif

#endif // !LOGGERDLL_H
