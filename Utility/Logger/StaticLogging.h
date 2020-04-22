#pragma once
#include "Filelogger/filelogger.h"
#include "filelogger_messages.h"

namespace filelog
{

    namespace globalutils
    {
        template <typename T>
        constexpr FileLogger& toRef(T& obj);
    }

#ifndef NGLOG_DEFAULT_DEFINES
    // Instance define
    #define GLOG_CASTED_INTANCE (filelog::globalutils::toRef(filelog::GlobalLogging::instance))

    // Levels defines
    #define SLOG_GLOBAL(level) filelog::FilteredFileLogMessage{ \
	    GLOG_CASTED_INTANCE, \
	    static_cast<filelog::LogLevel>(level), \
	    LOG_FUNCTION_MACRO, LOG_LINE_MACRO }

    #ifndef GLOG_DEFAULT_LEVEL 
    #define GLOG_DEFAULT_LEVEL filelog::LogLevel::Trace
    #endif

    #define GLOG_P SLOG_GLOBAL(filelog::LogLevel::Prod)
    #define GLOG_D SLOG_GLOBAL(filelog::LogLevel::Debug)
    #define GLOG_T SLOG_GLOBAL(filelog::LogLevel::Trace)

    // Utility defines
    #define GLOG_IS_INTERRUPTED() GLOG_CASTED_INTANCE.isInterrupted()
    #define GLOG_LAST_EXCEPTION() GLOG_CASTED_INTANCE.getException()
    #define GLOG_EXISTS() GlobalLogging::exists()
    #define GLOG_RESET_FROM_CONFIG(config) GlobalLogging::resetInstance(GlobalLogging::configToParams(config))
#endif // !NGLOG_DEFAULT_DEFINES

    template<typename TConfig>
    struct DefaultConfigAccessor;
    struct ConfigToFileLoggerParams;

	struct GlobalLogging
	{
        constexpr static const char* DEFAULT_FOLDER = "C:\\Lv-490_Files\\Logs";
        constexpr static const char* DEFAULT_FILENAME = "logfile.txt";
        constexpr static filelog::LogLevel DEFAULT_GLOG_LEVEL = filelog::LogLevel::Debug;

        template <typename TConfig, typename TAccessor = DefaultConfigAccessor<TConfig>>
        static FileLoggerCreatorParams configToParams(TConfig& conf)
        {
            auto params = ConfigToFileLoggerParams::createServerParams(conf);
            params.logStoringDirectory = DEFAULT_FOLDER;
            params.useExactSameName = true;
            return params;
        }
        static bool resetIfParamsNotSame(FileLoggerCreatorParams& params)
        {
            if (
                (lastParams.getRTLevel() != params.getRTLevel()) || 
                (lastParams.isForceFlushed != params.isForceFlushed) ||
                (lastParams.logStoringDirectory != params.logStoringDirectory) ||
                (lastParams.maxFileSize != params.maxFileSize) ||
                (lastParams.nameTemplate != params.nameTemplate) ||
                (lastParams.streamingMode != params.streamingMode) ||
                (lastParams.useExactSameName != params.useExactSameName))
            {
                return !filelog::GlobalLogging::resetInstance(params);
            }
            else
                return !GLOG_IS_INTERRUPTED();
        }
        static bool resetInstance(FileLoggerCreatorParams& params)
        {
            instance = FileLoggerCreator{}.create(params); 
            return GLOG_CASTED_INTANCE.isInterrupted();
        }
        static void setLevel(filelog::LogLevel lvl)
        {
            if (lvl > filelog::LogLevel::Trace)
                lvl = filelog::LogLevel::Trace;
            else if (lvl < filelog::LogLevel::NoLogs)
                lvl = filelog::LogLevel::NoLogs;

            GLOG_CASTED_INTANCE.setRTLevel(static_cast<log490::level_t>(lvl));
        }
        static bool exists()
        {
            return instance.get() != nullptr;
        }
        static const FileLoggerCreatorParams& checkLastParams()
        {
            return lastParams;
        }

		inline static std::unique_ptr<logbase::ILogger> instance;
    private:
        inline static FileLoggerCreatorParams lastParams;
        inline static struct StaticConstructor
        {
            StaticConstructor()
            {
            #ifdef GLOG_NO_DEFAULT_CONGIG
                return;
            #endif
                FileLoggerCreatorParams params;
                params.logStoringDirectory = DEFAULT_FOLDER;
                params.nameTemplate = DEFAULT_FILENAME;
                params.useExactSameName = true;
                params.setRTLevel(DEFAULT_GLOG_LEVEL);
                instance = FileLoggerCreator{}.create(params);
            }
        } stcon;
	};

#ifndef NGLOG_DEFAULT_DEFINES

#endif // !NGLOG_DEFAULT_DEFINES

    template <typename T>
    constexpr filelog::FileLogger& globalutils::toRef(T& obj)
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

    template<typename TConfig>
    struct DefaultConfigAccessor
    {
        static std::string getFilename(TConfig& config)
        {
            return config.get_filename();
        }
        static log490::level_t getLogLevel(TConfig& config)
        {
            return static_cast<log490::level_t>(config.get_loglevel());
        }
        static bool getFlush(TConfig& config)
        {
            return static_cast<bool>(config.get_flush());
        }
    };

    struct ConfigToFileLoggerParams
    {
        template <typename TConfig, typename TAccessor = DefaultConfigAccessor<TConfig>>
        static FileLoggerCreatorParams createServerParams(TConfig& conf)
        {
            FileLoggerCreatorParams output;
            output.nameTemplate = TAccessor::getFilename(conf);
            output.setRTLevel(TAccessor::getLogLevel(conf));
            output.isForceFlushed = TAccessor::getFlush(conf);

            return output;
        }

        template <typename TConfig, typename TAccessor = DefaultConfigAccessor<TConfig>>
        static void readServerConfig(FileLoggerCreatorParams& targetParams, TConfig& conf)
        {
            targetParams.nameTemplate = TAccessor::getFilename(conf);
            targetParams.setRTLevel(TAccessor::getLogLevel(conf));
            targetParams.isForceFlushed = TAccessor::getFlush(conf);
        }
    };

}
