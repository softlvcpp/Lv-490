#include "Other/pch.h"
#include "filelogger_StreamModes.h"

#include <cassert>

namespace filelog
{

    FileLogStreamer::FileLogStreamer(FileLogger& logger) : mlogger{ logger }
    {
    }

    FileLogStreamer::FileLogStreamer(FileLogStreamer&& streamer) noexcept : mlogger(streamer.mlogger)
    {
    }

    FileLogStreamer& FileLogStreamer::operator=(FileLogStreamer&& streamer) noexcept
    {
        if (this != &streamer)
        {
            this->mlogger = streamer.mlogger;
        }
        return *this;
    }

    void FileLogStreamer::cleanup()
    {
    }

    void FileLogStreamer::logger_SetException(const std::exception& exc)
    {
        mlogger.get().interruptedException = exc;
        mlogger.get().setInterrupted();

    }
    void FileLogStreamer::logger_SetInterrupted()
    {
        mlogger.get().setInterrupted();
    }
    bool FileLogStreamer::streamLogMessage(LogData& data)
    {
        return mlogger.get().streamLogMessage(data);
    }


    std::unique_ptr<FileLogStreamer> FileLogStreamerCreator::create(FileLogStreamerCreatorParams& params)
    {
        switch (params.mode)
        {
        case FileLogger::StreamingMode::Sync:
            return std::make_unique<SingleThreadedStreamer>(params.targetLogger.get());
        case FileLogger::StreamingMode::Async:
            return std::make_unique<MultiThreadedStreamer>(params.targetLogger.get());
        default:
            return std::unique_ptr<FileLogStreamer>(nullptr);
        }
    }

#pragma region SingleThreadedStreamer

    SingleThreadedStreamer::SingleThreadedStreamer(FileLogger& logger) : FileLogStreamer{ logger }
    {
    }
    SingleThreadedStreamer::SingleThreadedStreamer(SingleThreadedStreamer&& streamer) noexcept : FileLogStreamer{ std::move(streamer) }
    {
    }

    SingleThreadedStreamer& SingleThreadedStreamer::operator=(SingleThreadedStreamer&& streamer) noexcept
    {
        if (this != &streamer)
            this->mlogger = streamer.mlogger;
        return *this;
    }

    bool SingleThreadedStreamer::proccessData(LogData& data)
    {
        if (mlogger.get().isInterrupted())
            return false;
        try
        {
            bool returnVal = FileLogStreamer::streamLogMessage(data);
            data.message().reset(nullptr);
            return returnVal;
        }
        catch (std::exception & e)
        {
            FileLogStreamer::logger_SetException(e);
            return false;
        }
    }

#pragma endregion SingleThreadedStreamer
#pragma region MultiThreadedStreamer

    MultiThreadedStreamer::MultiThreadedStreamer(FileLogger& logger) : FileLogStreamer{ logger }
    {
        init();
    }

    MultiThreadedStreamer::MultiThreadedStreamer(MultiThreadedStreamer&& streamer) noexcept : FileLogStreamer{ std::move(streamer) }
    {
        moveToThis(std::move(streamer));
    }

    MultiThreadedStreamer& MultiThreadedStreamer::operator=(MultiThreadedStreamer&& right) noexcept
    {
        if (this != &right)
            moveToThis(std::move(right));
        return *this;
    }

    void MultiThreadedStreamer::init()
    {
        this->stopWriting = false;
        fileStreamThread = std::thread{ &MultiThreadedStreamer::threadEntry, this };
        while (!mlogger.get().isInterrupted() && !fileStreamThread.joinable()); // Spin lock, wait from few nanoseconds to max ~10ms
    }

    void MultiThreadedStreamer::moveToThis(MultiThreadedStreamer&& source)
    {
        source.stopWriting = true;
        source.messageQueue.cancelWaits();
        if (source.fileStreamThread.joinable())
            source.fileStreamThread.join();

        this->messageQueue = std::move(source.messageQueue);
    }

    bool MultiThreadedStreamer::proccessData(LogData& data)
    {
        if (mlogger.get().isStoped() && !messageQueue.empty())
            messageQueue.clear();
        else if (!mlogger.get().isStoped() && fileStreamThread.joinable())
        {
            messageQueue.emplace(std::move(data));
            return true;
        }
        return false;
    }

    void MultiThreadedStreamer::cleanup()
    {
        if (fileStreamThread.joinable())
            this->fileStreamThread.join();
        messageQueue.clear();
    }


    void MultiThreadedStreamer::logWriteLoop()
    {
        while (!mlogger.get().isStoped() && !stopWriting)
        {
            LogData msg;
            if (messageQueue.waitPopMove(msg))
            {
                if (!FileLogStreamer::streamLogMessage(msg))
                {
                    FileLogStreamer::logger_SetInterrupted();
                    return;
                }
                msg.message().reset(nullptr);
            }
        }

        if (!stopWriting)
        {
            auto queueSnapshot = std::move(messageQueue);
            LogData msg;
            while (queueSnapshot.waitPopMove(msg))
            {
                bool succesful = FileLogStreamer::streamLogMessage(msg);
                msg.message().reset(nullptr);
                if (!succesful)
                {
                    FileLogStreamer::logger_SetInterrupted();
                    return;
                }
            }
        }     
    }

    void MultiThreadedStreamer::threadEntry() noexcept
    {
        try
        {
            logWriteLoop();
        }
        catch (std::exception & e)          // Should not throw any errors if done right
        {
            FileLogStreamer::logger_SetException(e);
            assert(false);
        }
    }

#pragma endregion MultiThreadedStreamer

}