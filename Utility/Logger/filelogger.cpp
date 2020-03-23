#include "Other/pch.h"
#include "Filelogger/filelogger.h"

#ifndef INCLUDE_CHECK
#include <filesystem>
#else
#include <experimental/filesystem>
#include <iomanip>
#endif
#include <regex>
#include <cassert>
#include <iostream>

namespace filelog
{

    FailedToOpenFileException::FailedToOpenFileException(const char * desc) : std::runtime_error(desc) { }

    FileLogger::FileLogger(const char* fTemplate, LogLevel lvl, size_t fileSize)
    {
        setRTLevel(lvl);
        maxFileSize = fileSize;
        currentFileSize = 0;

        if (!validateFilenameTemplate(fTemplate))
        {
            setInterrupted();
            //throw FailedToOpenFileException("Invalid file template, invalid path format or path does not exist");
            return;
        }
 
        std::string initialFileName = createNarrowFileName();
        outputStream.open(initialFileName.c_str());
        if (!outputStream.is_open())
        {
            setInterrupted();
            //throw FailedToOpenFileException((std::string{ "Failed to create file, file path: " } + initialFileName.c_str()).c_str());
            return;
        }

        interrupted.store(false);

        fLoggerStop.store(false);
        fOnJoinWriteRemaining.store(false);
        consumerStreamer = std::thread{ &FileLogger::threadEntry, this };
    }


	FileLogger::~FileLogger()
    {
        fLoggerStop.store(true);
        fOnJoinWriteRemaining.store(false);
        messageQueue.cancelWaits();
        if(consumerStreamer.joinable())
            this->consumerStreamer.join();
        this->outputStream.close();
    }

    bool FileLogger::streamLogMessage(LogData& data)
    {
        auto& buff = data.message()->messageBuffer;

        this->currentFileSize += static_cast<size_t>(buff.length() * CHAR_TYPE_SIZE);
        if (currentFileSize >= maxFileSize)
        {
            using namespace std::string_literals;
            outputStream.close();
            //auto t = time(0);
            //outputStream.open("log_s" + std::to_string(t) + ".txt");
            outputStream.open(createNarrowFileName());
            if (outputStream.is_open() == false)
                return false;
            currentFileSize = static_cast<size_t>(buff.length());
        }

        outputStream.write(buff.c_str(), buff.length());
        outputStream.write(FLOG_STR(\n), 1);
        return true;
    }

    bool FileLogger::validateFilenameTemplate(const std::string& temp)
    {
        #ifndef INCLUDE_CHECK
        namespace fs = std::filesystem;
        #else
        namespace fs = std::experimental::filesystem;
        #endif

        fs::path p{ temp };
        auto folder = p.parent_path();

        //static std::regex thisFolder{ "./" };
        //static std::regex parentFolder{ "../" };
        static std::regex parentFolder{ R"(^((\.\/)|(\.\.\/))*?$)" };
  
        if(
            (*folder.c_str() != L'\0' && !fs::exists(folder) &&         
                !(std::regex_match(folder.string(), parentFolder))) 
            || !p.has_extension())
            return false;

        fileExtension_Narrow = p.extension().string();
        folder_Narrow = folder.string();

        p.replace_extension();
        filenamePathTemplate_Narrow = p.string();
        return true;
    }

    bool FileLogger::sendLogMessage(LogData &data)
    {
        if(interrupted && !messageQueue.empty())
            messageQueue.clear();
        else if(!joined)
        {
            messageQueue.emplace(std::move(data));
            return true;
        }      
        return false;
    }

    void FileLogger::join()
    {
        fOnJoinWriteRemaining.store(true);
        fLoggerStop.store(true);
        messageQueue.cancelWaits();
        if (consumerStreamer.joinable())
            this->consumerStreamer.join();
        outputStream.close();

        setJoined();
    }

    bool FileLogger::isInterrupted() const
    {
        return interrupted.load();
    }

    bool LOGGER_API FileLogger::isJoined() const
    {
        return joined.load();
    }

    void FileLogger::logWriteLoop()
    {
        while (!this->fLoggerStop)
        {
            LogData msg;
            if (messageQueue.waitPopMove(msg))
            {
                if(!streamLogMessage(msg))
                {
                    setInterrupted();
                    return;
                }
                msg.message().reset(nullptr);
            } 
        }

        if (fOnJoinWriteRemaining)
        {
            auto queueSnapshot = std::move(messageQueue);
            LogData msg;
            while (queueSnapshot.tryPopMove(msg)) // will return false if empty
            {
                bool succesful = streamLogMessage(msg);
                msg.message().reset(nullptr);
                if (!succesful)
                {
                    setInterrupted();
                    return;
                }
            }
        }
    
        setJoined();
    }

	void FileLogger::threadEntry() noexcept
	{
        try
        {
            logWriteLoop();
        }
        catch (...)
        {
            assert(false);
            setInterrupted();
        }
	}

    std::wstring FileLogger::createWideFileName()
    {
        time_t timeEnc = time(0);
        tm timeStruct;
        log490::Utils::getUTCTime(timeStruct, timeEnc);

        using namespace std::string_literals;
        return filenamePathTemplate_Wide + L"__"
            + std::to_wstring(timeStruct.tm_mday) + std::to_wstring(timeStruct.tm_mon) + std::to_wstring(timeStruct.tm_year + 1900)
            + std::to_wstring(timeStruct.tm_hour) + L"-" + std::to_wstring(timeStruct.tm_min) + L"-" + std::to_wstring(timeStruct.tm_sec)
            + fileExtension_Wide;
    }

    std::string FileLogger::createNarrowFileName()
    {
        time_t timeEnc = time(0);
        tm timeStruct;
        log490::Utils::getUTCTime(timeStruct, timeEnc);

        char timeSuffix[DATETIME_FORMAT_CHAR_COUNT + 1];
        strftime(timeSuffix, DATETIME_FORMAT_CHAR_COUNT, "%d%m%Y %H-%M-%S", &timeStruct);

        using namespace std::string_literals;
        return filenamePathTemplate_Narrow + "__"
                + timeSuffix
                + fileExtension_Narrow;
    }

    void FileLogger::setInterrupted()
    {
        interrupted = true;
        messageQueue.clear();
        setJoined();
    }

    void FileLogger::setJoined()
    {
        joined.store(true);
    }


    FileLogMessage::FileLogMessage(FileLogger& output) 
        : log490::LogMessage(output), timePosition(0)
    {
    }

    FileLogMessage::FileLogMessage(FileLogger& output, LogLevel level, log490::signature_t signature, log490::line_t line)
        : LogMessage(output, level, signature, line)
    {
        initPrefix();
    }

    FileLogMessage::FileLogMessage(FileLogMessage&& output) noexcept
        : LogMessage(std::move(output))
    {
        timePosition = output.timePosition;
    }

    FileLogMessage& FileLogMessage::operator=(FileLogMessage&& output) noexcept
    {
        timePosition = output.timePosition;
        return static_cast<FileLogMessage&>(LogMessage::operator=(std::move(output)));
    }

    FileLogMessage::~FileLogMessage()
    {
        if (!flushed) 
        {         
            updateTime();                                           // Update message time
            mLogger.get().sendLogMessage(mLogData);                 // Enqueue message to logger
        }
    }

    void FileLogMessage::initPrefix()
    {
        mLogData.message()->messageStream << mLogData.threadID()    // Thread id
            << "-";
        timePosition = mLogData.message()->messageBuffer.length();
        mLogData.message()->messageStream << timeFormat             // Padding for time
            << "-["
            << log490::Utils::getLogLevelStr(mLogData.msgLevel)     // Log level
            << "]-["
            << mLogData.callerSignature                             // Caller function
            << "] ";
    }

    void FileLogMessage::updateTime()
    {
        auto lastPos = mLogData.message()->messageBuffer.length();
        mLogData.message()->messageStream.seekp(lastPos - timePosition, std::ios_base::end);

        mLogData.updateTime();

        mLogData.message()->messageStream << std::put_time(&mLogData.msgUTCTime(), FLOG_STR(%d/%m/%Y %T)); // Inserts time and date - 00/00/0000 00:00:00
        constexpr size_t  timeFormatOffset = sizeof(timeFormat) / sizeof(char_type) - 1;
        mLogData.message()->messageStream.seekp(lastPos - (timePosition + timeFormatOffset));

        assert(lastPos == mLogData.message()->messageBuffer.length());
    }


    LogMessage& operator<<(LogMessage& left, FileMsgEndl&& right)
    {
        right.flush(left);
        return left;
    }

    LogMessage& operator<<(LogMessage& left, FileMsgEndl& right)
    {
        right.flush(left);
        return left;
    }

    FilteredFileLogMessage::FilteredFileLogMessage(FileLogger& output, LogLevel level, log490::signature_t signature, log490::line_t line)
        : msg(output)
    {
        if (output.logThisLevel(level))
        {
            msg = FileLogMessage{ output, level, signature, line };
            RuntimeLogFilter::setIndex(&msg.getData().message()->messageStream);
        }
    }

    FilteredFileLogMessage::FilteredFileLogMessage(FilteredFileLogMessage&& source) noexcept
        : msg(std::move(source.msg))
    {
        if (msg.hasMessage())
        {
            RuntimeLogFilter::setIndex(&msg.getData().message()->messageStream);
        }
    }

    FilteredFileLogMessage& FilteredFileLogMessage::operator=(FilteredFileLogMessage&& right) noexcept
    {
        if (this != &right)
        {
            msg = std::move(right.msg);
            RuntimeLogFilter::setIndex(&msg.getData().message()->messageStream);
        }
        return *this;
    }

    FilteredFileLogMessage& FilteredFileLogMessage::operator<<(const FileMsgEndl& mendl)
    {
        mendl.flush(msg);
        setIndex(nullptr);
        return *this;
    }

    FilteredFileLogMessage& FilteredFileLogMessage::operator<<(FileMsgEndl&& mendl)
    {
        mendl.flush(msg);
        setIndex(nullptr);
        return *this;
    }

	FileLogMessage& FilteredFileLogMessage::getMessage()
	{
        return msg;
	}

    bool LogDataLess::operator()(const LogData& left, const LogData& right)
    {
        return difftime(left.msgTime(), right.msgTime()) > 0;
    }

    void FileMsgEndl::flush(LogMessage& msg) const
    {
        throw std::runtime_error("Only FileLogMessage objects are supported");
    }

    void FileMsgEndl::flush(FileLogMessage& msg) const
    {
        MsgEndl::flush(msg);
    }

}
