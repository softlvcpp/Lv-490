#pragma once
#include "../Utility/Logger/LoggerDLL.h"
static filelog::FileLogger g_logger("clientlog.txt",true,filelog::LogLevel::Trace);
#define L_TRACE SLOG_TRACE(g_logger)
#define L_DEBUG SLOG_DEBUG(g_logger)
#define L_PROD SLOG_PROD(g_logger)

