#include "Other/pch.h"
#include "Filelogger/filelogger.h"
#include "filelogger_messages.h"
#include "filelogger_StreamModes.h"
#include <assert.h>

#include <filesystem>
namespace fs = std::filesystem;

/* Constants */
constexpr auto NOLOGS_LVL_STRING = "NOLOGS";
constexpr auto PROD_LVL_STRING = "PROD";
constexpr auto DEBUG_LVL_STRING = "DEBUG";
constexpr auto TRACE_LVL_STRING = "TRACE";
constexpr auto OUT_OF_RANGE_LVL_STRING = "INVALIDLVL";

constexpr auto DEFAULT_PARAM_NAME_TEMPLATE = "logfile.txt";
constexpr auto DEFAULT_PARAM_USE_EXACT_SAME = false;
constexpr auto DEFAULT_PARAM_RT_LEVEL = filelog::LogLevel::Prod;
constexpr size_t DEFAULT_PARAM_MAX_FILE_SIZE = 4 * 1024 * 1024;
constexpr auto DEFAULT_PARAM_STREAMINGMODE = filelog::FileLogger::StreamingMode::Sync;
constexpr auto DEFAULT_PARAM_IS_FORCE_FLUSHED = false;
#ifdef WIN32
constexpr auto DEFAULT_PARAM_DEFAULT_DIR = R"(C:/Lv-490_Files/serv_LOGS)";
#else
/* Will need to choose other location for Linux */
constexpr auto DEFAULT_PARAM_DEFAULT_DIR = R"(.\)";
#endif // WIN32 

namespace filelog
{

#pragma region Constructors/Destructor
    // --------------------------------------------------------------
    // Constructors/Destructor
    FileLogger::FileLogger() : mFileManager{ *this }
    {
        defaultInit();
    }

    FileLogger::FileLogger(const char* filePathTemplate, LogLevel lvl, size_t fileSize, StreamingMode strmMode)
        : FileLogger(filePathTemplate, false, lvl, fileSize, strmMode)
	{ }

    FileLogger::FileLogger(const char* fTemplate, bool useExactName, LogLevel lvl, size_t fileSize, StreamingMode strmMode) : mFileManager{ *this }
    {
        try
        {
            init(fTemplate, useExactName, lvl, fileSize, strmMode);
        }
        catch (std::exception & e)
        {
            interruptedException = e;
            stop(false);
            setInterrupted();
        }
    }

	FileLogger::~FileLogger()
    {
        stopLogging();
    }

    FileLogger::FileLogger(FileLogger&& moveSource) noexcept
        : mFileManager(*this)
    {
        moveToThis(std::move(moveSource));
    }

    FileLogger& FileLogger::operator=(FileLogger&& right) noexcept
    {
        if (this != &right)
            moveToThis(std::move(right));
        return (*this);
    }

#pragma endregion Constructors/Destructor

#pragma region Init/Cleanup
    // --------------------------------------------------------------
    // Init/cleanup methods 
    void FileLogger::init(const char* fTemplate, bool useExactName, LogLevel lvl, size_t fileSize, StreamingMode fAsyncMode)
    {
        if (fileSize < (log490::Utils::FixedMessageBuffer::bufferSize + 1))
            fileSize = log490::Utils::FixedMessageBuffer::bufferSize + 1;

        setRTLevel(static_cast<log490::level_t>(lvl));
        mFileManager.setMaxFileSize(fileSize);

        alwaysFlush = false;

        // Checks if passed a proper path, and initialize deconstructed path
        if (!mFileManager.setupFilenameTemplate(fTemplate))
        {
            setInterrupted();
            return;
        }
        // Try to open file with deconstructed path and with respect to filename template
        if (!mFileManager.openFile(fTemplate, useExactName))
        {
            setInterrupted();
            return;
        }

        interrupted.store(false);
        fLoggerStop.store(false);
        // Create streaming object
        FileLogStreamerCreatorParams strmParams{ streamingMode, *this };
        streamer = std::move(FileLogStreamerCreator{}.create(strmParams));

        auto initMessage = FilteredFileLogMessage{ *this, static_cast<filelog::LogLevel>(LogLevel::Trace), LOG_FUNCTION_MACRO, LOG_LINE_MACRO };
        if (initMessage.getMessage().getData().message())
        {
            if (fs::file_size(mFileManager.getCurrentFilename()) != 0)
                mFileManager.getStream().write(L"\n", 1);
            initMessage << "Initialized logger!";
        }
    }

    void FileLogger::defaultInit()
    {
        setRTLevel(static_cast<log490::level_t>(LogLevel::NoLogs));
        mFileManager = FileLoggerFilestreamManager{ *this };
        mFileManager.setMaxFileSize(0);
        alwaysFlush = false;
        streamingMode = StreamingMode::Sync;
        streamerHasStoped = true;
        streamer = std::move(std::make_unique<SingleThreadedStreamer>(*this));
    }

    void FileLogger::moveToThis(FileLogger&& source)
    {        
        std::scoped_lock lck{ this->streamAccessMutex,source.streamAccessMutex };

        // Move inner variables
        this->fLoggerStop = source.fLoggerStop.load();
        this->interrupted = source.interrupted.load();
        this->interruptedException = std::move(source.interruptedException);
        this->streamingMode = source.streamingMode; 
        // Move filemanager
        this->mFileManager = std::move(source.mFileManager);
        // Move streamer
        auto moveStreamer = [this, &source](auto sPtr) -> void
        {
            using ptr_type = decltype(sPtr);
            this->streamer = std::move(std::make_unique<std::remove_pointer<ptr_type>::type>(*this));

            ptr_type mStrm = static_cast<ptr_type>(streamer.get());
            (*mStrm) = std::move(*sPtr);
        };

        switch (streamingMode)
        {
        case filelog::FileLogger::StreamingMode::Sync:
            assert(typeid(source.streamer.get()) == typeid(SingleThreadedStreamer*));
            moveStreamer(static_cast<SingleThreadedStreamer*>(source.streamer.get()));
            break;
        case filelog::FileLogger::StreamingMode::Async:
            assert(typeid(source.streamer.get()) == typeid(MultiThreadedStreamer*));
            moveStreamer(static_cast<MultiThreadedStreamer*>(source.streamer.get()));
            break;
        default:
            break;
        }

        this->streamerHasStoped = source.streamerHasStoped.load();   
        source.defaultInit();
    }

    void FileLogger::stop(bool succesfulStop) // Does not use bool anymore, but is still left just in case
    {   
        if (succesfulStop && !isStoped())
        {
            auto initMessage = FilteredFileLogMessage{ *this, static_cast<filelog::LogLevel>(LogLevel::Trace), LOG_FUNCTION_MACRO, LOG_LINE_MACRO };
            if (initMessage.getMessage().getData().message())
                initMessage << "Succesfully stoping logger!";
        }
        fLoggerStop.store(true); 
        if(streamer != nullptr)
            streamer->cleanup();
        streamerHasStoped = true;
        mFileManager.closeFile();
    }
#pragma endregion Init/Cleanup

#pragma region Public methods
    // --------------------------------------------------------------
    // Public methods



	bool FileLogger::restart(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize, StreamingMode strmMode)
    {
        stop(true);
        init(filePathTemplate, useExactName, lvl, fileSize, strmMode);
        return !isInterrupted();
    }

    void FileLogger::stopLogging()
    {
        stop(true);
    }

    void FileLogger::join()
    {
        stopLogging();
    }

    bool FileLogger::isInterrupted() const
    {
        return interrupted.load();
    }

    const std::exception& FileLogger::getException()
    {
        return interruptedException;
    }

    bool FileLogger::isStoped() const
    {
        return streamerHasStoped || fLoggerStop;
    }

    bool FileLogger::flushIsForced() const
    {
        return alwaysFlush.load();
    }

    void FileLogger::setForceFlush(bool value)
    {
        alwaysFlush.store(value);
    }

    void FileLogger::flushNow()
    {
        std::unique_lock lock{ streamAccessMutex };
        mFileManager.getStream() << std::flush;
    }

    bool FileLogger::sendLogMessage(LogData& data)
    {
        if (!isStoped())
            return streamer->proccessData(data);
        return false;
    }

#pragma endregion Public methods

#pragma region Private methods
    // --------------------------------------------------------------
    // Private methods
    bool FileLogger::streamLogMessage(LogData& data)
    {
        std::unique_lock lock{ streamAccessMutex };
        auto& buff = data.message()->messageBuffer;

        auto bytesToWrite = static_cast<size_t>(buff.length() * CHAR_TYPE_SIZE);
       
        if (!mFileManager.checkedRotate(bytesToWrite))
            return false;

        mFileManager.getStream().write(buff.c_str(), buff.length());
        if (alwaysFlush)
            mFileManager.getStream() << std::flush;
        else
            mFileManager.getStream().write(FLOG_STR(\n), 1);
        return true;
    }

    void FileLogger::setInterrupted()
    {
        interrupted = true;
    }

#pragma endregion Private methods

    const char* Utils::getLogLevelStr(LogLevel lvl)
    {
        switch (lvl)
        {
        case LogLevel::NoLogs: return NOLOGS_LVL_STRING;
        case LogLevel::Prod: return PROD_LVL_STRING;
        case LogLevel::Debug: return DEBUG_LVL_STRING;
        case LogLevel::Trace: return TRACE_LVL_STRING;
        }

        return OUT_OF_RANGE_LVL_STRING;
    }

    FileLoggerCreatorParams::FileLoggerCreatorParams()
    {
        nameTemplate = DEFAULT_PARAM_NAME_TEMPLATE;
        useExactSameName = DEFAULT_PARAM_USE_EXACT_SAME;
        setRTLevel(DEFAULT_PARAM_RT_LEVEL);
        maxFileSize = DEFAULT_PARAM_MAX_FILE_SIZE;
        streamingMode = DEFAULT_PARAM_STREAMINGMODE;
        isForceFlushed = DEFAULT_PARAM_IS_FORCE_FLUSHED;
        logStoringDirectory = DEFAULT_PARAM_DEFAULT_DIR;
    }

    void FileLoggerCreatorParams::setRTLevel(LogLevel lvl)
    {
        this->minimumLevel = static_cast<log490::level_t>(this->rtLevel = lvl);
    }
    void FileLoggerCreatorParams::setRTLevel(log490::level_t lvl)
    {
        this->rtLevel = static_cast<LogLevel>(this->minimumLevel = lvl);
    }
    LogLevel FileLoggerCreatorParams::getRTLevel() 
    { 
        return this->rtLevel; 
    }
    log490::level_t  FileLoggerCreatorParams::getRTLevelInt() 
    {   
        return static_cast<log490::level_t>(this->minimumLevel);
    }

    std::unique_ptr<logbase::ILogger> FileLoggerCreator::create(logbase::LoggerCreationParamsBase& params)
    {
        std::unique_ptr<FileLogger> outputObject;
        if (typeid(params) == typeid(logbase::LoggerCreationParamsBase))
        {
            outputObject = std::move(std::make_unique<FileLogger>());
            outputObject->setRTLevel(static_cast<log490::level_t>(params.minimumLevel));
        }
        else if (typeid(params) == typeid(FileLoggerCreatorParams))
        {
            FileLoggerCreatorParams& fcParams = static_cast<FileLoggerCreatorParams&>(params);

            std::string fTemplate;
            if (fs::path(fcParams.nameTemplate).root_directory().c_str()[0] != 0)
                fTemplate = fcParams.nameTemplate;
            else
            {
                if(fcParams.logStoringDirectory.back() == '/')
                    fTemplate = fcParams.logStoringDirectory + fcParams.nameTemplate;
                else
                    fTemplate = fcParams.logStoringDirectory + "/" + fcParams.nameTemplate;
            }

            outputObject = std::move(std::make_unique<FileLogger>(
                fTemplate.c_str(),
                fcParams.useExactSameName,
                fcParams.getRTLevel(),
                fcParams.maxFileSize,
                fcParams.streamingMode)
            );
            outputObject->setForceFlush(fcParams.isForceFlushed);
        }

        return outputObject;
    }

}
