#ifndef LOGFUNDAMENTALS_H
#define LOGFUNDAMENTALS_H

#include "ExportConfig.h"

#ifndef _WIN32
#ifndef  _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // ! _CRT_SECURE_NO_WARNINGS
#endif

#include <stdint.h>
#include <ostream>
#include <memory>
#include <thread>

#ifndef LOG_FUNCTION_MACRO
#define LOG_FUNCTION_MACRO __FUNCTION__
#endif 

#ifndef LOG_LINE_MACRO
#define LOG_LINE_MACRO static_cast<uint64_t>(__LINE__)
#endif 

namespace log490 {

    using level_t = int8_t;
    using line_t = uint32_t;
    using logtimestruct_t = tm;
    using logtime_t = time_t;
    using signature_t = const char*;
    using logchar_t = wchar_t;
    using stream_t = std::wostream;

    template <typename T>
    using uptr_t = std::unique_ptr<T>;

    enum class LogLevel : level_t
    {
        NoLogs = 0,
        Prod,
        Debug,
        Trace
    };

    namespace Utils
    {
        void getLocaltime(logtimestruct_t& to, logtime_t& from);
        void getUTCTime(logtimestruct_t& to, logtime_t& from);
        const char* getLogLevelStr(LogLevel lvl);

        class FixedMessageBuffer : public std::basic_streambuf<logchar_t, std::char_traits<logchar_t>>
        {
        public:
            constexpr static LOGGER_API size_t bufferSize = 512 * 1 * sizeof(logchar_t);

            LOGGER_API FixedMessageBuffer();
            LOGGER_API std::streamsize length();//   const { return pptr() - pbase(); }
            LOGGER_API std::streamsize capacity();// const { return bufferSize; }
            LOGGER_API bool empty();//             const { return length() == 0; }
            LOGGER_API bool full();// const { return length() == capacity(); }

            // Unput one character.
            LOGGER_API int_type sunputc();

            // Peek at last inserted character.
            LOGGER_API int peek() const;

            LOGGER_API const logchar_t* c_str() const;
        protected:
            std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override;
            std::streampos seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override;
        private:
            using base_class = std::basic_streambuf<logchar_t, std::char_traits<logchar_t>>;
            logchar_t buffer[bufferSize + 1];
        };

        struct FixedBuffStream
        {
            FixedBuffStream();
            FixedMessageBuffer messageBuffer;
            stream_t messageStream;
        };
    };

    struct LogData
    {   
        LOGGER_API LogData();
        LOGGER_API LogData(LogLevel level, signature_t signature, line_t line) noexcept;
        LOGGER_API LogData(LogData&&) noexcept;
        virtual LOGGER_API ~LogData() { }

        LOGGER_API LogData& operator=(LogData&&) noexcept;

        LOGGER_API std::thread::id& threadID();
        LOGGER_API logtime_t& msgTime();
        LOGGER_API logtimestruct_t& msgUTCTime();
        LOGGER_API uptr_t<Utils::FixedBuffStream>& message();

        LOGGER_API inline const std::thread::id& threadID() const;
        LOGGER_API inline const logtime_t& msgTime() const;
        LOGGER_API inline const logtimestruct_t& msgUTCTime() const;
        LOGGER_API inline const uptr_t<Utils::FixedBuffStream>& message() const;

        LOGGER_API void updateTime();

        LogLevel msgLevel;
        signature_t callerSignature;
        line_t msgLine;
    private:
        std::thread::id _threadID;
        logtime_t _msgTime;
        logtimestruct_t _msgUTCTime;
        uptr_t<Utils::FixedBuffStream> _message;
    };

    class LOGGER_API Logger
    {
    public:
        Logger();
        virtual ~Logger() { }
        virtual bool sendLogMessage(LogData& data) = 0;

        virtual void setRTLevel(LogLevel level);
        virtual LogLevel getRTLevel() const;
        virtual bool logThisLevel(LogLevel lvl) const;
    private:
        LogLevel runtimeLevel;
    };

    class LOGGER_API DummyLogger : Logger
    {
    public:
        bool sendLogMessage(LogData& data)  override { return false; }
        void setRTLevel(LogLevel level) override {  }
        LogLevel getRTLevel() const override { return LogLevel::NoLogs; }
        bool logThisLevel(LogLevel lvl) const override { return false; }
        static DummyLogger& instance() { return sInstance; }
    private:
        static DummyLogger sInstance;
    };

    struct LOGGER_API Message
    {
    public:
        virtual ~Message() { }
    };

    class VoidMessage : public Message
    {
    public:
        template<typename T>
        inline const VoidMessage& operator<<(const T& data) const { return *this; }
        template<typename T>
        inline const VoidMessage& operator<<(T&& data) const { return *this; }
        LOGGER_API static const VoidMessage voidMsg;
    };

    class LogMessage;

    struct LOGGER_API MsgEndl
    {
        virtual ~MsgEndl() {}
        virtual void flush(LogMessage&) const;
    protected:
        void setFlushed(LogMessage&) const;
    };

    class LogMessage : Message
    {
    public:
        friend struct MsgEndl;

        LOGGER_API LogMessage(Logger& lgr);
        LOGGER_API LogMessage(Logger& output, LogLevel level, signature_t signature, line_t line);
        LOGGER_API LogMessage(LogMessage&& source) noexcept;
        LOGGER_API virtual ~LogMessage();

        LOGGER_API bool hasMessage();
        LOGGER_API const LogData& getData() const;
        LOGGER_API LogData& getDataRef();
        LOGGER_API Logger& getLogger();

        LOGGER_API LogMessage& operator=(LogMessage&&) noexcept;

        template<typename T>
        LogMessage& operator<<(T data);
        LOGGER_API LogMessage& operator<<(const char* str);
        LOGGER_API LogMessage& operator<<(const wchar_t* str);
        LOGGER_API LogMessage& operator<<(std::string&& str);
        LOGGER_API LogMessage& operator<<(const std::string& str);
        LOGGER_API LogMessage& operator<<(std::wstring&& str);
        LOGGER_API LogMessage& operator<<(const std::wstring& str);

        LOGGER_API LogMessage& operator<<(MsgEndl& mendl);
        LOGGER_API LogMessage& operator<<(MsgEndl&& mendl);

    protected:

        bool flushed;
        LogData mLogData;
        std::reference_wrapper<Logger> mLogger;
    };

    template<typename T>
    inline LogMessage& LogMessage::operator<<(T data)
    {
        this->mLogData.message()->messageStream << data;
        return *this;
    }

}

// Log levels as macros

#ifndef SLOG_NO_DEFAULT_MACROS

#ifndef CTLVL_NOLOGS
#define CTLVL_NOLOGS 0 
#endif 

#ifndef CTLVL_PROD
#define CTLVL_PROD 1 
#endif

#ifndef CTLVL_DEBUG
#define CTLVL_DEBUG 2
#endif 

#ifndef CTLVL_TRACE
#define CTLVL_TRACE 3
#endif 


// Compile time log level
#ifndef CTIME_LOG_LEVEL
#define CTIME_LOG_LEVEL CTLVL_TRACE
#endif // !CTIME_LOG_LEVEL

#ifndef SLOG_ENDL
#define SLOG_ENDL log490::MsgEndl{}
#endif

// Message object that does nothing
#ifndef VOID_LOG
#define VOID_LOG log490::VoidMessage::voidMsg
#endif // !VOID_LOG

// Logger calls by levels
#ifndef SLOG_LEVEL
#define SLOG_LEVEL(logger, level) log490::LogMessage{ logger, level, LOG_FUNCTION_MACRO, LOG_LINE_MACRO }
#endif

#if CTLVL_PROD <= CTIME_LOG_LEVEL
#ifndef SLOG_PROD
#define SLOG_PROD(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_PROD))
#endif
#else
#ifndef SLOG_PROD
#define SLOG_PROD(logger) VOID_LOG
#endif
#endif

#if CTLVL_DEBUG <= CTIME_LOG_LEVEL
#ifndef SLOG_DEBUG
#define SLOG_DEBUG(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_DEBUG))
#endif
#else
#ifndef SLOG_DEBUG
#define SLOG_DEBUG(logger) VOID_LOG
#endif
#endif

#if CTLVL_TRACE <= CTIME_LOG_LEVEL
#ifndef SLOG_TRACE
#define SLOG_TRACE(logger) SLOG_LEVEL(logger, static_cast<log490::LogLevel>(CTLVL_TRACE))
#endif
#else
#ifndef SLOG_TRACE
#define SLOG_TRACE(logger) VOID_LOG
#endif
#endif

#endif // !SLOG_NO_DEFAULT_MACROS

#endif // BASELOGGER_H
