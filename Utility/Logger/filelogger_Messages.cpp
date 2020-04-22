#include "Other/pch.h"
#include "filelogger_messages.h"

#include <iomanip>
#include <cassert>

namespace filelog
{

    FileLogMessage::FileLogMessage(FileLogger& output)
        : log490::LogMessage(output), timePosition(0)
    { }

    FileLogMessage::FileLogMessage(FileLogger& output, LogLevel level, log490::signature_t signature, log490::line_t line)
        : LogMessage(output, static_cast<log490::level_t>(level), signature, line)
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
            << Utils::getLogLevelStr(static_cast<LogLevel>(mLogData.msgLevel))     // Log level
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
        if (output.logThisLevel(static_cast<log490::level_t>(level)))
        {
            msg = FileLogMessage{ output, level, signature, line };
            callIndex = 1;
        }
        else
            callIndex = 0;
    }

    FilteredFileLogMessage::FilteredFileLogMessage(FilteredFileLogMessage&& source) noexcept
        : callIndex{ source.callIndex }, msg(std::move(source.msg))
    {
        source.callIndex = 0;
    }

    FilteredFileLogMessage& FilteredFileLogMessage::operator=(FilteredFileLogMessage&& right) noexcept
    {
        if (this != &right)
        {
            //RuntimeLogFilter::operator=(std::move(right));
            callIndex = right.callIndex;
            msg = std::move(right.msg);

            right.callIndex = 0;
        }
        return *this;
    }

    FilteredFileLogMessage& FilteredFileLogMessage::operator<<(MsgEndl mendl)
    {
        mendl.flush(msg);
        callIndex = 0;
        return *this;
    }

    FileLogMessage& FilteredFileLogMessage::getMessage()
    {
        return msg;
    }

    bool Utils::LogDataLess::operator()(const LogData& left, const LogData& right)
    {
        return ((left.msgHighResTime() - right.msgHighResTime()).count() < 0ull);
        //return difftime(left.msgTime(), right.msgTime()) > 0;  
    }

#pragma endregion FileLogMessage and filtering


}