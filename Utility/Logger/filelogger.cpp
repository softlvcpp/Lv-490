#include "Other/pch.h"
#include "Filelogger/filelogger.h"

#include <filesystem>
#include <cassert>
#include <iostream>

namespace fs = std::filesystem;

namespace filelog
{

#pragma region Constructors/Destructor
    // --------------------------------------------------------------
    // Constructors/Destructor
    FileLogger::FileLogger()
    {
        setRTLevel(LogLevel::NoLogs);
        maxFileSize = 0;
        currentFileSize = 0;
        setJoined();
    }

    FileLogger::FileLogger(const char* filePathTemplate, LogLevel lvl, size_t fileSize)
        : FileLogger(filePathTemplate, false, lvl, fileSize)
	{ }

    FileLogger::FileLogger(const char* fTemplate, bool useExactName, LogLevel lvl, size_t fileSize)
    {
        try
        {
            init(fTemplate, useExactName, lvl, fileSize);
        }
        catch (std::exception & e)
        {
            interruptedException = e;
            setInterrupted();
        }
    }

	FileLogger::~FileLogger()
    {
        stop(false);
    }

#pragma endregion Constructors/Destructor

#pragma region Init/Cleanup
    // --------------------------------------------------------------
    // Init/cleanup methods 
    void FileLogger::init(const char* fTemplate, bool useExactName, LogLevel lvl, size_t fileSize)
    {
        if (fileSize < (log490::Utils::FixedMessageBuffer::bufferSize + 1))
            fileSize = log490::Utils::FixedMessageBuffer::bufferSize + 1;

        setRTLevel(lvl);
        maxFileSize = fileSize;
        currentFileSize = 0;

        // Checks if passed a proper path
        if (!validateFilenameTemplate(fTemplate))
        {
            setInterrupted();
            return;
        }

        std::ios_base::iostate exceptionMask = outputStream.exceptions() | std::ios::failbit;
        outputStream.exceptions(exceptionMask);
        if (!openFile(fTemplate, useExactName))
        {
            setInterrupted();
            return;
        }
        currentFileSize += fs::file_size(currentFileName);

        interrupted.store(false);
        joined.store(true);
        fLoggerStop.store(false);
        fOnJoinWriteRemaining.store(false);
        fileStreamThread = std::thread{ &FileLogger::threadEntry, this };
        while (!interrupted && joined); // Spin lock with max ~10ms wait
    }

    void FileLogger::stop(bool writeRemaining)
    {
        fOnJoinWriteRemaining.store(writeRemaining);
        fLoggerStop.store(true);

        messageQueue.cancelWaits();
        if (fileStreamThread.joinable())
            this->fileStreamThread.join();
        if (outputStream.is_open())
            this->outputStream.close();

        setJoined();
    }
#pragma endregion Init/Cleanup

#pragma region Public methods
    // --------------------------------------------------------------
    // Public methods

    bool FileLogger::restart(const char* filePathTemplate, bool useExactName, LogLevel lvl, size_t fileSize, bool fjoin)
    {
        stop(fjoin);
        init(filePathTemplate, useExactName, lvl, fileSize);
        return !isInterrupted();
    }

    bool FileLogger::isInterrupted() const
    {
        return interrupted.load();
    }

    const std::exception& FileLogger::getException()
    {
        return interruptedException;
    }

    bool FileLogger::isJoined() const
    {
        return joined.load();
    }

    const char* FileLogger::getCurrentFilename() const
    {
        std::scoped_lock lock{ filenameAccessMutex };
        return currentFileName.c_str();;
    }

#pragma endregion Public methods

#pragma region Private methods
    // --------------------------------------------------------------
    // Private methods
    bool FileLogger::streamLogMessage(LogData& data)
    {
        auto& buff = data.message()->messageBuffer;

        auto bytesToWrite = static_cast<size_t>(buff.length() * CHAR_TYPE_SIZE);
        this->currentFileSize += bytesToWrite;
        if (currentFileSize >= maxFileSize)
        {     
            outputStream.close();

            if (!openFile(currentFileName.c_str(), false, bytesToWrite))
                return false;
          
            currentFileSize = fs::file_size(currentFileName) + bytesToWrite;
        }

        outputStream.write(buff.c_str(), buff.length());
        outputStream.write(FLOG_STR(\n), 1);
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
        stop(true);
    }

    void FileLogger::logWriteLoop()
    {
        while (!this->fLoggerStop)
        {
            LogData msg;
            if (messageQueue.waitPop(msg))
            //if (messageQueue.waitPopMove(msg))
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
            while (queueSnapshot.tryPop(msg))
            //while (queueSnapshot.tryPopMove(msg)) // will return false if empty
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
            this->interrupted.store(false);
            this->joined.store(false);
            logWriteLoop();
        }
        catch (std::exception & e)          // Should not throw any errors if done right
        {
            interruptedException = e;
            setInterrupted();
            assert(false);
        }
	}

    bool FileLogger::openFile(const char* fileName, bool useExact, size_t neccessarySpace, std::ios::openmode mode)
    {        
        if (useExact)
        {
            if (fs::exists(fileName))   
            {
                std::string lastModified;
                if (getLastModifiedMatchesTemplate(lastModified))                       // Try to find last modified file
                    setFilename((fileFull(lastModified.c_str(), neccessarySpace)) ?     // If found then continue it(path/name_DATE-TIME_COUNT.extension
                        filenameRegexCheck().c_str() :
                        lastModified.c_str());
                else
                    setFilename((fileFull(fileName, neccessarySpace)) ?                 // Else write in file of format path/name.extension
                        filenameRegexCheck().c_str() :
                        fileName);
            }
            else
                setFilename(fileName);                                                  
        }
        else
            setFilename(filenameRegexCheck().c_str());                                  // Create new file

        outputStream.open(currentFileName, mode);
        return outputStream.is_open();
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
        currentFileName = "";
    }

#pragma endregion Private methods

#pragma region FileLogMessage and filtering

    FileLogMessage::FileLogMessage(FileLogger& output) 
        : log490::LogMessage(output), timePosition(0) 
    { }

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
    {                                                               // threadID-DATE TIME-[LEVEL]-[FUNCTION]
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
        : RuntimeLogFilter(source), msg(std::move(source.msg))
    { }

    FilteredFileLogMessage& FilteredFileLogMessage::operator=(FilteredFileLogMessage&& right) noexcept
    {
        if (this != &right)
        {
            RuntimeLogFilter::operator=(std::move(right));
            msg = std::move(right.msg);
        }
        return *this;
    }

    FilteredFileLogMessage& FilteredFileLogMessage::operator<<(MsgEndl mendl)
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
        return ((left.msgHighResTime() - right.msgHighResTime()).count() < 0ull);
        //return difftime(left.msgTime(), right.msgTime()) > 0;  
    }

#pragma endregion FileLogMessage and filtering
}
