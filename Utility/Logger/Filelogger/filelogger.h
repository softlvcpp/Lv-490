#pragma once
#ifndef FILELOGGER_H
#define FILELOGGER_H
#include "../Essentials/ExportConfig.h"

#ifndef SLOG_LEVEL
#define SLOG_LEVEL(logger, level) filelog::FilteredFileLogMessage{ logger, level, LOG_FUNCTION_MACRO, LOG_LINE_MACRO }
#endif

#ifndef SLOG_ENDL
#define SLOG_ENDL filelog::FileMsgEndl()
#endif

#include <thread>
#include <atomic>
#include <string>
#include <fstream>
#include "../Essentials/ConcPriorityQueue.h"
#include "../Essentials/LogFundamentals.h"
#include "../Essentials/RuntimeFilter.h"

#define FLOG_CHAR_TYPE wchar_t
#define CAT(A, B) A##B
#define FLOG_STR(text) CAT(L, #text)

namespace filelog
{

    using LogData = log490::LogData;
    using LogMessage = log490::LogMessage;
    using Logger = log490::Logger;
    using LogLevel = log490::LogLevel;
    using MsgEndl = log490::MsgEndl;

    struct FileMsgEndl;
    class FileLogMessage;
    class FileLogger;

    LOGGER_API LogMessage& operator<<(LogMessage& left, FileMsgEndl&& right);
    LOGGER_API LogMessage& operator<<(LogMessage& left, FileMsgEndl& right);

    struct FailedToOpenFileException : std::runtime_error
    {
        LOGGER_API FailedToOpenFileException(const char* what = "");
    };

    struct LogDataLess
    {
        bool operator()(const LogData& left, const LogData& right);
    };

    class FileLogMessage : public LogMessage
    {
    public:
        using char_type = FLOG_CHAR_TYPE;
        LOGGER_API FileLogMessage(FileLogger& output);
        LOGGER_API FileLogMessage(FileLogger& output, LogLevel level, log490::signature_t signature, log490::line_t line);

        LOGGER_API FileLogMessage(FileLogMessage&& output) noexcept;
        LOGGER_API FileLogMessage& operator=(FileLogMessage&& output) noexcept;

        LOGGER_API virtual ~FileLogMessage();
    private:
        constexpr static const char_type timeFormat[] = FLOG_STR(00/00/0000 00:00:00);
        void initPrefix();
        void updateTime();
        size_t timePosition;
    };

    struct LOGGER_API FileMsgEndl : MsgEndl
    {
        virtual void flush(LogMessage&) const override;
        void flush(FileLogMessage&) const;
    };

    class FileLogger : public Logger
    {
    public:
        using char_type = FLOG_CHAR_TYPE;
        using stream_type = std::wofstream;                                                             

        constexpr static LOGGER_API const char* DATETIME_STRING_FORMAT = "%d%m%Y %H-%M-%S";             // Filename datetime suffix format  
        constexpr static LOGGER_API size_t DATETIME_FORMAT_CHAR_COUNT = sizeof("01012000 00-00-00");    // Filename datetime suffix char count
        constexpr static LOGGER_API size_t DEFAULT_FILE_SIZE = 4194304;
        constexpr static LOGGER_API size_t CHAR_TYPE_SIZE = sizeof(char_type);

        /* Creates filelogger object and starts separate filestreaming thread */
        LOGGER_API FileLogger(const char* filePathTemplate, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE);
        //FileLogger(const wchar_t* filePathTemplate, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE);

        /* Destroys filelogger, stops streaming loop and joins streaming thread */
        LOGGER_API ~FileLogger() override;

        /* Returns true and enqueues data to filestreaming thread if thread is active, othwerwise returns false */
        bool LOGGER_API sendLogMessage(LogData& data) override;

        /* Starts ignoring incoming messages,
           writes all remaining messages into file
           and then joins streaming thread
        */
        void LOGGER_API join();

        /* Returns true if logger's loop has exited with exception */
        bool LOGGER_API isInterrupted() const;

        /* Returns true if join was called succesfully */
        bool LOGGER_API isJoined() const;
    private:
        void logWriteLoop();
        void threadEntry() noexcept;
        std::wstring createWideFileName();
        std::string createNarrowFileName();

        void setInterrupted();
        void setJoined();
        bool streamLogMessage(LogData& data);
        bool validateFilenameTemplate(const std::string& temp);
    private:
        std::atomic<bool> interrupted;             // [flag] interrupted due to exception
        std::atomic<bool> joined;                  // [flag] thread is properly joined      
        std::atomic<bool> fLoggerStop;             // [flag] thread broke loop and joined
        std::atomic<bool> fOnJoinWriteRemaining;   // [flag] should write all remaining messages on join     
        std::thread consumerStreamer;
        log490::ConcurrentPriorityQueue<LogData, std::vector<LogData>, LogDataLess> messageQueue;

        std::string filenamePathTemplate_Narrow;   // filename template in narrow char                          
        std::string fileExtension_Narrow;          // file extension in const narrow char
        std::string folder_Narrow;                 // file parent folder in narrow char

        std::wstring filenamePathTemplate_Wide;    // filename template in wide char       
        std::wstring fileExtension_Wide;           // file extension in const wide char
        std::wstring folder_Wide;                  // file parent folder in narrow char

        stream_type outputStream;                  // log file output stream
        size_t currentFileSize;                    // currently written size
        size_t maxFileSize = 4 * 1024 * 1024;      // max file size
    };

    class FilteredFileLogMessage : public log490::RuntimeLogFilter
    {
    public:
        LOGGER_API FilteredFileLogMessage(FileLogger& output, LogLevel level, log490::signature_t signature, log490::line_t line);

        LOGGER_API FilteredFileLogMessage(FilteredFileLogMessage&&) noexcept;
        LOGGER_API FilteredFileLogMessage& operator=(FilteredFileLogMessage&&) noexcept;

        template <typename T>
        inline FilteredFileLogMessage& operator<<(const T& data) { return static_cast<FilteredFileLogMessage&>(RuntimeLogFilter::operator<<(data)); }
        template <typename T>
        inline FilteredFileLogMessage& operator<<(T&& data) { return static_cast<FilteredFileLogMessage&>(RuntimeLogFilter::operator<<(data)); }
        LOGGER_API FilteredFileLogMessage& operator<<(const FileMsgEndl& endl);
        LOGGER_API FilteredFileLogMessage& operator<<(FileMsgEndl&& endl);

        LOGGER_API FileLogMessage& getMessage();
    private:
        FileLogMessage msg;
    };

}

#endif // FILELOGGER_H
