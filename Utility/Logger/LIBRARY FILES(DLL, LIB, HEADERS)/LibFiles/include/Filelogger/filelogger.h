#pragma once
#ifndef FILELOGGER_H
#define FILELOGGER_H
#include "../Essentials/ExportConfig.h"

#ifndef SLOG_LEVEL
#define SLOG_LEVEL(logger, level) filelog::FilteredFileLogMessage{ logger, level, LOG_FUNCTION_MACRO, LOG_LINE_MACRO }
#endif

#ifndef SLOG_ENDL
#define SLOG_ENDL filelog::MsgEndl()
#endif

#include <thread>
#include <atomic>
#include <string>
#include <fstream>
#include <regex>
//#include "../Essentials/ConcPriorityQueue.h"
#include "../Essentials/ConcurrentMultiQueue.h"
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

    class FileLogMessage;
    class FileLogger;

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

    class FileLogger : public Logger
    {
    public:
        using char_type = FLOG_CHAR_TYPE;
        using stream_type = std::wofstream;                                                         

        constexpr static LOGGER_API const char* DATETIME_STRING_FORMAT = "%d%m%Y %H-%M-%S";             // Filename datetime suffix format  
        constexpr static LOGGER_API size_t DATETIME_FORMAT_CHAR_COUNT = sizeof("01012000 00-00-00");    // Filename datetime suffix char count
        constexpr static LOGGER_API size_t DEFAULT_FILE_SIZE = 4194304;
        constexpr static LOGGER_API size_t CHAR_TYPE_SIZE = sizeof(char_type);

        /*  Creates filelogger object and set's it to joined state. Use when want to deffer start */
        LOGGER_API FileLogger();

        /* Creates filelogger object and starts separate filestreaming thread.
           
           filePathTemplate - specifies file template: filename.txt -> filename_01012000 00-00-00.txt
           lvl - specifies maximum logging level
           fileSize - maximum size of the single file
        */
        LOGGER_API FileLogger(const char* filePathTemplate, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE);

        /* Creates filelogger object and starts separate filestreaming thread.

          filePathTemplate - specifies file template: filename.txt -> filename_01012000 00-00-00.txt
          useExactName - specifies if should try to use exact name first. If file with that name exist, 
          and is too big it will still create new files like this -> filename_01012000 00-00-00.txt
          lvl - specifies maximum logging level
          fileSize - maximum size of the single file
        */
        LOGGER_API FileLogger(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE);

        /* Destroys filelogger, stops streaming loop and joins streaming thread */
        LOGGER_API ~FileLogger() override;

        /* Returns true and enqueues data to filestreaming thread if thread is active, othwerwise returns false */
        bool LOGGER_API sendLogMessage(LogData& data) override;

        /* Starts ignoring incoming messages,
           writes all remaining messages into file
           and then joins streaming thread
        */
        void LOGGER_API join();

        /* Stops and restarts logger, with given arguments.
           See also: FileLogger(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize) */
        bool LOGGER_API restart(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE, bool join = false);

        /* Returns true if logger's loop has exited with exception */
        bool LOGGER_API isInterrupted() const;

        /* Returns exception that logger has encoutered if it was interrupted */
        LOGGER_API const std::exception& getException();

        /* Returns true if join was called succesfully */
        bool LOGGER_API isJoined() const;

        /* Returns currently open file's name */
        LOGGER_API const char* getCurrentFilename() const;

        /* Returns true if file-stream flushes on each message */
        LOGGER_API bool flushIsForced() const;

        /* Set if flush should happen on each received message */
        LOGGER_API void setForceFlush(bool value);

        /* Forces stream to flush */
        LOGGER_API void flushNow();

    private:
        void logWriteLoop();
        void threadEntry() noexcept;
        std::string createNarrowFileName();

        void init(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE);
        void stop(bool writeRemaining);

        void setFilename(const char* newFileName);
        bool openFile(const char* fileName, bool useExact = false, size_t neccessarySpace = 0, std::ios::openmode mode = std::ios::out | std::ios::app);
        void setInterrupted();
        void setJoined();
        bool streamLogMessage(LogData& data);
        bool validateFilenameTemplate(const std::string& temp);
        bool fileFull(const char* fileName, size_t shouldHaveLeft = 0);
        bool fileMatchesTemplate(const char_type* filename);
        std::string filenameRegexCheck();

        bool getLastModifiedMatchesTemplate(std::string& resultPath);
    private:
        std::atomic<bool> forceFlush;
        std::atomic<bool> interrupted;             // [flag] interrupted due to exception
        std::atomic<bool> joined;                  // [flag] thread is properly joined      
        std::atomic<bool> fLoggerStop;             // [flag] thread broke loop and joined
        std::atomic<bool> fOnJoinWriteRemaining;   // [flag] should write all remaining messages on join     
        std::thread fileStreamThread;              // thread that streams incoming messages in parallel
        //log490::ConcurrentPriorityQueue<LogData, std::vector<LogData>, LogDataLess> messageQueue; // concurrent blocking message queue
        log490::ConcurrentMultiQueue messageQueue;

        struct DeconstructedFilename
        {
            template<typename T>
            struct DFStrings
            {
                using str = std::basic_string<T>;
                // Path pieces without DATE-TIME_COUNT suffix
                str pathWithNoExt;                 // full path without extension
                str folder;                        // full path without file-name
                str fileNameWithNoExt;             // filename with no extension
                str extension;                     // just extension
                str fullPath;                      // full path, example path/logger.log
            };

            DFStrings<char> asString;              // info as ASCII string 
            DFStrings<wchar_t> asWstring;          // info as UNICODE string
        } filenameInfo;                            // any neccessary information about file template

        std::string currentFileName;               // currently used file name
        std::wregex filenameTemplateRegex;         // path_DATE-TIME_COUNT.extension
        std::regex filenameTNoCounterRegex;        // path_DATE-TIME.extension
        mutable std::mutex filenameAccessMutex;    // write/read mutex for filename
        mutable std::mutex streamAccessMutex;    // write/read mutex for stream

        std::exception interruptedException;       // exception that is saved when logger is interrupted

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
        inline FilteredFileLogMessage& operator<<(const T& data);
        template <typename T>
        inline FilteredFileLogMessage& operator<<(T&& data);
        LOGGER_API FilteredFileLogMessage& operator<<(MsgEndl endl);

        LOGGER_API FileLogMessage& getMessage();
    private:
        FileLogMessage msg;
    };

    template <typename T>
    inline FilteredFileLogMessage& FilteredFileLogMessage::operator<<(const T& data) { return static_cast<FilteredFileLogMessage&>(RuntimeLogFilter::operator<<(data)); }
    template <typename T>
    inline FilteredFileLogMessage& FilteredFileLogMessage::operator<<(T&& data) { return static_cast<FilteredFileLogMessage&>(RuntimeLogFilter::operator<<(data)); }

}

#endif // FILELOGGER_H
