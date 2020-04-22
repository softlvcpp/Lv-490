#pragma once
#ifndef FILELOGGER_H
#define FILELOGGER_H
#include "../Essentials/ExportConfig.h"
#include "../ILogger.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <fstream>
#include <regex>
#include "../Essentials/LogFundamentals.h"

#define CAT(A, B) A##B
#define FLOG_STR(text) CAT(L, #text)

#define FLOG_CHAR_TYPE_INT 1

namespace filelog
{

#if FLOG_CHAR_TYPE_INT == 0
    using char_type = char;
    using filestream_type = std::ofstream;
    using stream_base = std::ostream;
#elif FLOG_CHAR_TYPE_INT == 1
    using char_type = wchar_t;
    using filestream_type = std::wofstream;
    using stream_base = std::wostream;
#endif

    using LogData = log490::LogData;
    using LogMessage = log490::LogMessage;
    using Logger = log490::Logger;
    using MsgEndl = log490::MsgEndl;

    enum class LogLevel : log490::level_t
    {
        NoLogs = 0,
        Prod,
        Debug,
        Trace
    };

    /* Forward declarations */
    class FileLogMessage;
    class FilteredFileLogMessage;
    class FileLogger;
    class FileLoggerFilestreamManager;
    class FileLogStreamer;
    class SingleThreadedStreamer;
    class MultiThreadedStreamer;

    namespace Utils
    {
        LOGGER_API const char* getLogLevelStr(LogLevel lvl);
        struct LogDataLess
        {
            LOGGER_API bool operator()(const LogData& left, const LogData& right);
        };
    }

    class FileLoggerFilestreamManager
    {
    public:
        constexpr static const char* DATETIME_STRING_FORMAT = "%d%m%Y %H-%M-%S";             // Filename datetime suffix format  
        constexpr static size_t DATETIME_FORMAT_CHAR_COUNT = sizeof("01012000 00-00-00");    // Filename datetime suffix char count

        LOGGER_API FileLoggerFilestreamManager(FileLogger& logger);
        LOGGER_API FileLoggerFilestreamManager(FileLoggerFilestreamManager&&) noexcept;
        LOGGER_API FileLoggerFilestreamManager& operator=(FileLoggerFilestreamManager&&) noexcept;

        LOGGER_API stream_base& getStream();
        LOGGER_API bool getFileIsOpen();
        LOGGER_API const std::string& getCurrentFilename() const;

        LOGGER_API void setMaxFileSize(size_t value);
        LOGGER_API void setFilename(const char* newFileName);

        LOGGER_API bool setupFilenameTemplate(const std::string& temp);
        LOGGER_API bool openFile(const char* fileName, bool useExact = false, size_t neccessarySpace = 0, std::ios::openmode mode = std::ios::out | std::ios::app);
        LOGGER_API bool checkedRotate(size_t bytesToWrite);
        LOGGER_API void closeFile();
    private:
        /* Checks filename against filenameTemplateRegex */
        bool fileMatchesTemplate(const char_type* filename);
        /* Creates filename narrow filename, and modifies it if it's a duplicate to current */
        std::string filenameRegexCheck();
        /* Tries to find last modified file that matches regex template */
        bool getLastModifiedMatchesTemplate(std::string& resultPath);
        /* Creates filename with time suffix */
        std::string createNarrowFileName();
        /* Checks if file is full */
        bool fileFull(const char* fileName, size_t shouldHaveLeft = 0);
        /* Used in move constructor, operator */
        void moveToThis(FileLoggerFilestreamManager&& source);
    private:
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

        size_t currentFileSize;                    // currently written size
        size_t maxFileSize = 4 * 1024 * 1024;      // max file size
        filestream_type outputStream;

        std::reference_wrapper<FileLogger> mlogger;
    };

    class FileLogger : public Logger, public logbase::ILogger
    {
    public:
        constexpr static size_t DEFAULT_FILE_SIZE = 4194304;
        constexpr static size_t CHAR_TYPE_SIZE = sizeof(char_type);

        enum class StreamingMode
        {
            Sync,
            Async
        };

        /*  Creates filelogger object and set's it to joined state. Use when want to deffer start */
        LOGGER_API FileLogger();

        /* Creates filelogger object and starts separate filestreaming thread.
           
           filePathTemplate - specifies file template: filename.txt -> filename_01012000 00-00-00.txt
           lvl - specifies maximum logging level
           fileSize - maximum size of the single file
        */
        LOGGER_API FileLogger(const char* filePathTemplate, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE, StreamingMode strmMode = StreamingMode::Sync);

        /* Creates filelogger object and starts separate filestreaming thread.

          filePathTemplate - specifies file template: filename.txt -> filename_01012000 00-00-00.txt
          useExactName - specifies if should try to use exact name first. If file with that name exist, 
          and is too big it will still create new files like this -> filename_01012000 00-00-00.txt
          lvl - specifies maximum logging level
          fileSize - maximum size of the single file
        */
        LOGGER_API FileLogger(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE, StreamingMode strmMode = StreamingMode::Sync);

        /* Destroys filelogger, stops streaming loop and joins streaming thread */
        LOGGER_API ~FileLogger() override;

        /* FileLogger move constructor */
        LOGGER_API FileLogger(FileLogger&& moveSource) noexcept;

        /* FileLogger move assignment operator */
        LOGGER_API FileLogger& operator=(FileLogger&& right) noexcept;

        /* Returns true and enqueues data to filestreaming thread if thread is active, othwerwise returns false */
        bool LOGGER_API sendLogMessage(LogData& data) override;

        /* Starts ignoring incoming messages,
           and, if asynchronious mode set, writes all remaining messages into file
        */
        void LOGGER_API stopLogging();

        /* [ DEPRECATED ] Starts ignoring incoming messages,
           and, if asynchronious mode set, writes all remaining messages into file
        */
        void LOGGER_API join();

        /* Stops and restarts logger, with given arguments.
           See also: FileLogger(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize) */
        bool LOGGER_API restart(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize = DEFAULT_FILE_SIZE, StreamingMode strmMode = StreamingMode::Sync);

        /* Returns true if logger's loop has exited with exception */
        bool LOGGER_API isInterrupted() const;

        /* Returns exception that logger has encoutered if it was interrupted */
        LOGGER_API const std::exception& getException();

        /* Returns true if logger is set to ignore all messages */
        bool LOGGER_API isStoped() const;

        /* Returns true if file-stream flushes on each message */
        LOGGER_API bool flushIsForced() const;

        /* Set if flush should happen on each received message */
        LOGGER_API void setForceFlush(bool value);

        /* Forces stream to flush */
        LOGGER_API void flushNow();

    private:
        void init(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize, StreamingMode fAsyncMode);
        void stop(bool writeRemaining);

        void setInterrupted();
        bool streamLogMessage(LogData& data);

        void defaultInit();
        void moveToThis(FileLogger&& source);
    private:
        friend class FileLogStreamer;

        std::unique_ptr<FileLogStreamer> streamer; // streamer object that is called in sendLogMessage
        FileLoggerFilestreamManager mFileManager;  // file manager object

        StreamingMode streamingMode;

        std::atomic<bool> streamerHasStoped;       // [flag] checks if streamer has stoped
        std::atomic<bool> interrupted;             // [flag] interrupted due to exception   
        std::atomic<bool> fLoggerStop;             // [flag] thread broke loop and joined 
        std::atomic<bool> alwaysFlush;             // [flag] determines if file manager will flush filestream on each written message

        mutable std::mutex streamAccessMutex;      // write/read mutex for stream
       
        std::exception interruptedException;       // exception that is saved when logger is interrupted
    };

#pragma region FileLoggerFactory
    /* Filelogger factory class */
    struct FileLoggerCreator : logbase::LoggerCreatorBase
    {
        LOGGER_API std::unique_ptr<logbase::ILogger> create(logbase::LoggerCreationParamsBase& params) override;
    };

    /* Filelogger factory parameters class */
    struct FileLoggerCreatorParams : logbase::LoggerCreationParamsBase
    {
        LOGGER_API FileLoggerCreatorParams();

        LOGGER_API void setRTLevel(LogLevel lvl);
        LOGGER_API void setRTLevel(log490::level_t lvl);

        LOGGER_API LogLevel getRTLevel();
        LOGGER_API log490::level_t getRTLevelInt();

        std::string nameTemplate;
        bool useExactSameName;
        size_t maxFileSize;
        FileLogger::StreamingMode streamingMode;
        
        bool isForceFlushed;
        std::string logStoringDirectory;
    private:
        LogLevel rtLevel;
    };
#pragma endregion FileLoggerFactory
}

#endif // FILELOGGER_H
