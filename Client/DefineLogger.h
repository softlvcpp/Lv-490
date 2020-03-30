#pragma once
#include "../Utility/Logger/LoggerDLL.h"
static filelog::FileLogger logger("clientlog.txt", filelog::LogLevel::Trace);
#define L_TRACE SLOG_TRACE(logger)
#define L_DEBUG SLOG_DEBUG(logger)
#define L_PROD SLOG_PROD(logger)

