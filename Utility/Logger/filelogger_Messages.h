#pragma once
#include "Filelogger/filelogger.h"

namespace filelog
{

    class FileLogMessage : public LogMessage
    {
    public:
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

    class FilteredFileLogMessage
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
        template <typename T>
        struct FilteredCall
        {
            static void resolveCall(FilteredFileLogMessage& target, const T& data);
        private:
            using tcall = void (*)(FileLogMessage&, const T& data);
            static void falseCall(FileLogMessage& msg, const T& data);
            static void trueCall(FileLogMessage& msg, const T& data);
            inline static tcall callTable[2]
            {
                &FilteredCall<T>::falseCall,
                &FilteredCall<T>::trueCall,
            };
        };

        size_t callIndex;
        FileLogMessage msg;
    };

    template <typename T>
    inline FilteredFileLogMessage& FilteredFileLogMessage::operator<<(const T& data)
    {
        FilteredCall<T>::resolveCall((*this), data);
        return (*this);
    }
    template <typename T>
    inline FilteredFileLogMessage& FilteredFileLogMessage::operator<<(T&& data)
    {
        FilteredCall<T>::resolveCall((*this), data);
        return (*this);
    }
    template<typename T>
    inline void FilteredFileLogMessage::FilteredCall<T>::resolveCall(FilteredFileLogMessage& target, const T& data)
    {
        (*callTable[target.callIndex])(target.msg, data);
    }
    template<typename T>
    inline void FilteredFileLogMessage::FilteredCall<T>::falseCall(FileLogMessage& msg, const T& data) { }
    template<typename T>
    inline void FilteredFileLogMessage::FilteredCall<T>::trueCall(FileLogMessage& msg, const T& data)
    {
        msg << data;
    }

}