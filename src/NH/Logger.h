#pragma once

#include "NH/Commons.h"

#include <Union/String.h>
#include <Union/Array.h>

namespace NH
{

    enum class LoggerLevel : size_t
    {
        None = 0,
        Fatal = 1,
        Error = 2,
        Warn = 3,
        Info = 4,
        Debug = 5,
        Trace = 6
    };

    constexpr const char* LoggerLevelToString(LoggerLevel level)
    {
        constexpr const char* values[] = { "NONE", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE" };
        return values[static_cast<size_t>(level)];
    }

    constexpr const char* LoggerLevelToDisplayString(LoggerLevel level)
    {
        constexpr const char* values[] = { " NONE", "FATAL", "ERROR", " WARN", " INFO", "DEBUG", "TRACE" };
        return values[static_cast<size_t>(level)];
    }

    LoggerLevel StringToLoggerLevel(String level);

    class Logger;

    class ILoggerAdapter
    {
        friend Logger;

    protected:
        LoggerLevel m_Level;

        explicit ILoggerAdapter(const LoggerLevel level = LoggerLevel::Debug) : m_Level(level) {}
        virtual ~ILoggerAdapter() = default;

        [[nodiscard]] bool CanLog(const LoggerLevel level) const { return level <= m_Level; }

        virtual void Message(LoggerLevel level, const String& channel, const String& message) = 0;

    public:
        void SetLoggerLevel(const LoggerLevel level) { m_Level = level; };
    };

    class Logger
    {
        const String m_LoggerName;
        Union::Array<ILoggerAdapter*> m_Adapters;

    public:
        friend class LoggerFactory;

        explicit Logger(const String& name) : m_LoggerName(name) {};

        explicit Logger(const String& name, const Union::Array<ILoggerAdapter*>& adapters) : m_LoggerName(name), m_Adapters(adapters) {};

        void Message(LoggerLevel level, const String& message);

        void PrintRaw(LoggerLevel level, const String& message) const;

        template<typename... Args>
        void Message(const LoggerLevel level, const char* format, Args... args) { Message(level, String::Format(format, args...)); }

        void Fatal(const String& message) { Message(LoggerLevel::Fatal, message); };

        template<typename... Args>
        void Fatal(const char* format, Args... args) { Fatal(String::Format(format, args...)); }

        void Error(const String& message) { Message(LoggerLevel::Error, message); };

        template<typename... Args>
        void Error(const char* format, Args... args) { Error(String::Format(format, args...)); }

        void Warning(const String& message) { Message(LoggerLevel::Warn, message); };

        template<typename... Args>
        void Warning(const char* format, Args... args) { Warning(String::Format(format, args...)); }

        void Info(const String& message) { Message(LoggerLevel::Info, message); };

        template<typename... Args>
        void Info(const char* format, Args... args) { Info(String::Format(format, args...)); }

        void Debug(const String& message) { Message(LoggerLevel::Debug, message); };

        template<typename... Args>
        void Debug(const char* format, Args... args) { Debug(String::Format(format, args...)); }

        void Trace(const String& message) { Message(LoggerLevel::Trace, message); };

        template<typename... Args>
        void Trace(const char* format, Args... args) { Trace(String::Format(format, args...)); }

        [[nodiscard]] Union::Array<ILoggerAdapter*> GetAdapters() const { return m_Adapters; };

        template<class T>
        T* GetAdapter() const
        {
            for (int i = 0; i < m_Adapters.GetCount(); i++)
            {
                ILoggerAdapter* adapter = m_Adapters[i];
                if (T* ptr = dynamic_cast<T*>(adapter))
                {
                    return ptr;
                }
            }
            return nullptr;
        }
    };

    class UnionConsoleLoggerAdapter final : public ILoggerAdapter
    {
    public:
        static LoggerLevel DEFAULT_LEVEL;

        struct Color
        {
            String Channel;
            String Level;
            String Message;
        };

        explicit UnionConsoleLoggerAdapter(LoggerLevel level = LoggerLevel::Debug);

        void SetLevelColor(LoggerLevel level, Color color);

    protected:
        void Message(LoggerLevel level, const String& channel, const String& message) override;

    private:
        Color m_ColorTable[7];
    };

    class ZSpyLoggerAdapter final : public ILoggerAdapter
    {
    private:
        String m_Prefix;

    public:
        static LoggerLevel DEFAULT_LEVEL;

        explicit ZSpyLoggerAdapter(const LoggerLevel level = LoggerLevel::Debug, const String& prefix = "N") : ILoggerAdapter(level), m_Prefix(prefix) {}

    protected:
        void Message(LoggerLevel level, const String& channel, const String& message) override;
    };

    class LoggerFactory
    {
        static LoggerFactory* s_Instance;
        Union::Array<Logger*> m_Loggers{};

        LoggerFactory() = default;

    public:
        Logger* Create(const String& name);

        Union::Array<Logger*> GetLoggers() { return m_Loggers.Share(); };

        static LoggerFactory* GetInstance()
        {
            if (!s_Instance)
            {
                s_Instance = new LoggerFactory();
            }
            return s_Instance;
        }
    };

    Logger* CreateLogger(const String& name);
}