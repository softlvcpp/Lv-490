#pragma once
#include "ConcQueue.h"
#include "Filelogger/filelogger.h"

namespace filelog
{

    class FileLogStreamer
    {
    public:
        LOGGER_API FileLogStreamer(FileLogger& logger);
        LOGGER_API FileLogStreamer(FileLogStreamer&& streamer) noexcept;
        LOGGER_API FileLogStreamer& operator=(FileLogStreamer&& streamer) noexcept;
        virtual bool proccessData(LogData& data) = 0;
        LOGGER_API virtual void cleanup();
    protected:
        void logger_SetException(const std::exception&);
        void logger_SetInterrupted();
        bool streamLogMessage(LogData&);

        std::reference_wrapper<FileLogger> mlogger;
    };

    struct FileLogStreamerCreatorParams
    {
        FileLogger::StreamingMode mode;
        std::reference_wrapper<FileLogger> targetLogger;
    };
    class FileLogStreamerCreator
    {
    public:
        LOGGER_API virtual std::unique_ptr<FileLogStreamer> create(FileLogStreamerCreatorParams& params);
    };

    class SingleThreadedStreamer final : public FileLogStreamer
    {
    public:
        LOGGER_API SingleThreadedStreamer(FileLogger& logger);
        LOGGER_API SingleThreadedStreamer(SingleThreadedStreamer&& streamer) noexcept;
        LOGGER_API SingleThreadedStreamer& operator=(SingleThreadedStreamer&& streamer) noexcept;
        LOGGER_API bool proccessData(LogData& data) override;
    };

    class MultiThreadedStreamer final : public FileLogStreamer
    {
    public:
        LOGGER_API MultiThreadedStreamer(FileLogger& logger);
        LOGGER_API MultiThreadedStreamer(MultiThreadedStreamer&& streamer) noexcept;
        LOGGER_API MultiThreadedStreamer& operator=(MultiThreadedStreamer&& streamer) noexcept;
        LOGGER_API bool proccessData(LogData& data) override;
        LOGGER_API void cleanup() override;
    private:
        void threadEntry() noexcept;
        void logWriteLoop();
        void init();
        void moveToThis(MultiThreadedStreamer&& source);
    private:
        std::atomic<bool> stopWriting;
        std::thread fileStreamThread;
        log490::ConcurrentQueue<LogData> messageQueue;
    };

}