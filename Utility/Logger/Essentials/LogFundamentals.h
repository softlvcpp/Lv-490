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

namespace log490 {

    using level_t = int8_t;
    using line_t = uint32_t;
    using logtimestruct_t = tm;
    using logtime_t = time_t;
    using highres_t = std::chrono::steady_clock;
    using signature_t = const char*;
    using logchar_t = wchar_t;
    using stream_t = std::wostream;

    template <typename T>
    using uptr_t = std::unique_ptr<T>;

    namespace Utils
    {
        void getLocaltime(logtimestruct_t& to, logtime_t& from);
        void getUTCTime(logtimestruct_t& to, logtime_t& from);

        //template<typename char_type>
        class FixedMessageBuffer : public std::basic_streambuf<logchar_t, std::char_traits<logchar_t>>
        {
        public:
            constexpr static LOGGER_API size_t bufferSize = 512 * 1 * sizeof(char_type);

            LOGGER_API FixedMessageBuffer();
            /* Returns put pointer offset */
            LOGGER_API std::streamsize length();
            /* Returns constant bufferSize */
            LOGGER_API std::streamsize capacity();
            /* Checks if put pointer is at 0 */
            LOGGER_API bool empty();
            /* Checks if put pointer is at pbase + bufferSize */
            LOGGER_API bool full();

            /* Unput one character. */
            LOGGER_API int_type sunputc();

            /* Peek at last inserted character. */
            LOGGER_API int peek() const;

            LOGGER_API const logchar_t* c_str() const;
        protected:
            std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override;
            std::streampos seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override;
        private:
            using base_class = std::basic_streambuf<logchar_t, std::char_traits<logchar_t>>;
            logchar_t buffer[bufferSize + 1];
        };

        //template <typename char_type, typename stream_type>
        struct FixedBuffStream
        {
            FixedBuffStream();
            FixedMessageBuffer messageBuffer;   // fixed size character buffer
            stream_t messageStream;             // ostream buffer handler
        };
    };


    struct LogData
    {   
        LOGGER_API LogData();
        LOGGER_API LogData(level_t level, signature_t signature, line_t line) noexcept;
        LOGGER_API LogData(LogData&&) noexcept;
        virtual LOGGER_API ~LogData() { }

        LOGGER_API LogData& operator=(LogData&&) noexcept;

        /* Returns stored message id */
        LOGGER_API std::thread::id& threadID();
        /* Returns stored local time as time_t int */
        LOGGER_API logtime_t& msgTime();
        /* Returns stored local time in nanosecond */
        LOGGER_API std::chrono::time_point<highres_t>& msgHighResTime();
        /* Returns stored utc time as struct */
        LOGGER_API logtimestruct_t& msgUTCTime();
        /* Returns allocated buffer pointer */
        LOGGER_API uptr_t<Utils::FixedBuffStream>& message();

        /* Returns stored message id */
        LOGGER_API const std::thread::id& threadID() const;
        /* Returns stored local time as time_t int */
        LOGGER_API const logtime_t& msgTime() const;
        LOGGER_API const std::chrono::time_point<highres_t>& msgHighResTime() const;
        /* Returns stored utc time as struct */
        LOGGER_API const logtimestruct_t& msgUTCTime() const;
        /* Returns allocated buffer pointer */
        LOGGER_API const uptr_t<Utils::FixedBuffStream>& message() const;

        /* Updates stored local and utc time */
        LOGGER_API void updateTime();

        level_t msgLevel;                              // message level 
        signature_t callerSignature;                    // calling function signature
        line_t msgLine;                                 // message line
    private:
        std::thread::id _threadID;                      // caller function thread id
        logtime_t _msgTime;                             // message local time
        logtimestruct_t _msgUTCTime;                    // message utc time
        std::chrono::time_point<highres_t> _msgHighResTime;
        uptr_t<Utils::FixedBuffStream> _message;        // message buffer and stream-handler
    };

    class LOGGER_API Logger
    {
    public:
        Logger();
        virtual ~Logger() { }

        /* Sends log data to target stream */
        virtual bool sendLogMessage(LogData& data);
        /* Sets runtime log level(not atomic) */
        virtual void setRTLevel(level_t level);
        /* Returns current runtime log level(not atomic) */
        virtual level_t getRTLevel() const;
        /* Checks if logger accepts this level value */
        virtual bool logThisLevel(level_t lvl) const;

        static Logger dummyInstance;
    private:
        level_t runtimeLevel;                          // runtime log level value
    };

    /* Base class for messages, exists just for polymorpism */
    struct LOGGER_API Message
    {
    public:
        virtual ~Message() { }
        virtual void flush() = 0;
        virtual void free() = 0;
        virtual bool isFlushed() const = 0;
    };

    /* Special message type that does nothing to enable  */
    class VoidMessage : public Message
    {
    public:
        template<typename T>
        inline const VoidMessage& operator<<(const T& data) const { return *this; }
        template<typename T>
        inline const VoidMessage& operator<<(T&& data) const { return *this; }

        void flush() override;
        void free() override;
        bool isFlushed() const override;

        LOGGER_API static const VoidMessage voidMsg;
    };

    class LogMessage;

    /* Class that allows you to immidiately send the message before destuction */
    struct LOGGER_API MsgEndl
    {
        virtual ~MsgEndl() {}
        virtual void flush(LogMessage&) const;
        virtual void cancel(LogMessage&) const;
    };

    /* Special class stores anything that is neccessary for  */
    class LogMessage : Message
    {
    public:
        LOGGER_API LogMessage(Logger& lgr);
        LOGGER_API LogMessage(Logger& output, level_t level, signature_t signature, line_t line);
        LOGGER_API LogMessage(LogMessage&& source) noexcept;
        LOGGER_API virtual ~LogMessage();

        LOGGER_API void flush() override;
        LOGGER_API void free() override;
        LOGGER_API bool isFlushed() const override;

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


#endif // BASELOGGER_H
