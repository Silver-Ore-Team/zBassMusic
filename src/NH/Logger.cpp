#include "Logger.h"

#include <ZenGin/zGothicAPI.h>

#include <utility>

namespace NH
{
    LoggerLevel StringToLoggerLevel(String level)
    {
        String values[] = { "NONE", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE" };
        size_t count = sizeof(values) / sizeof(String);
        for (size_t i = 0; i < count; i++)
        {
            if (level.MakeUpper() == values[i])
            {
                return (LoggerLevel)i;
            }
        }
        return LoggerLevel::None;
    }

    void Logger::Message(LoggerLevel level, const String& message)
    {
        if (level == LoggerLevel::None)
        {
            return;
        }

        size_t count = m_Adapters.GetCount();
        for (size_t i = 0; i < count; i++)
        {
            ILoggerAdapter* adapter = m_Adapters[i];
            if (adapter->CanLog(level))
            {
                adapter->Message(level, m_LoggerName, message);
            }
        }
    }

    UnionConsoleLoggerAdapter::UnionConsoleLoggerAdapter(LoggerLevel level) : ILoggerAdapter(level)
    {
        SetLevelColor(LoggerLevel::Fatal, Color{ "\x1B[31m", "\x1B[41;1m\x1B[37;1m", "\x1B[41m\x1B[37;1m" });
        SetLevelColor(LoggerLevel::Error, Color{ "\x1B[31m", "\x1B[41;1m\x1B[37;1m", "\x1B[31;1m" });
        SetLevelColor(LoggerLevel::Warn, Color{ "\x1B[33m", "\x1B[43;1m\x1B[37m", "\x1B[0m\x1B[33;1m" });
        SetLevelColor(LoggerLevel::Info, Color{ "\x1B[30;1m", "\x1B[47;2m\x1B[39m", "\x1B[0m" });
        SetLevelColor(LoggerLevel::Debug, Color{ "\x1B[32m", "\x1B[42;1m\x1B[37;1m", "\x1B[0m\x1B[32;1m" });
        SetLevelColor(LoggerLevel::Trace, Color{ "\x1B[35m", "\x1B[35;1m\x1B[30m", "\x1B[35;1m" });
    }

    void UnionConsoleLoggerAdapter::SetLevelColor(LoggerLevel level, Color color)
    {
        m_ColorTable[(size_t)level] = std::move(color);
    }

    void UnionConsoleLoggerAdapter::Message(LoggerLevel level, const String& channel, const String& message)
    {
        Color color = m_ColorTable[(size_t)level];

        String::Format("\x1B[0m{0} {1} \x1B[0m {2}[{3}]\x1B[0m {4}{5}\x1B[0m",
                       color.Level, LoggerLevelToDisplayString(level),
                       color.Channel, channel,
                       color.Message, message)
                .StdPrintLine();
    }

    void ZSpyLoggerAdapter::Message(LoggerLevel level, const String& channel, const String& message)
    {
        String formattedMessage = String(m_Prefix) + ":\t" + channel + ": [" + LoggerLevelToString(level) + "]" + message;

        switch (GetGameVersion())
        {
        case Engine_G1:
            Gothic_I_Classic::zerr->Message(Gothic_I_Classic::zSTRING(formattedMessage.ToChar()));
            break;
        case Engine_G1A:
            Gothic_I_Addon::zerr->Message(Gothic_I_Addon::zSTRING(formattedMessage.ToChar()));
            break;
        case Engine_G2:
            Gothic_II_Classic::zerr->Message(Gothic_II_Classic::zSTRING(formattedMessage.ToChar()));
            break;
        case Engine_G2A:
            Gothic_II_Addon::zerr->Message(Gothic_II_Addon::zSTRING(formattedMessage.ToChar()));
            break;
        }
    }

    LoggerLevel UnionConsoleLoggerAdapter::DEFAULT_LEVEL = LoggerLevel::Debug;
    LoggerLevel ZSpyLoggerAdapter::DEFAULT_LEVEL = LoggerLevel::Debug;

    LoggerFactory* LoggerFactory::s_Instance = null;

    Logger* LoggerFactory::Create(const String& name)
    {

        for (auto* logger: m_Loggers)
        {
            if (logger->m_LoggerName == name)
            {
                return logger;
            }
        }

        auto* logger = new NH::Logger(name, {
                new NH::UnionConsoleLoggerAdapter(UnionConsoleLoggerAdapter::DEFAULT_LEVEL),
                new NH::ZSpyLoggerAdapter(ZSpyLoggerAdapter::DEFAULT_LEVEL, "B")
        });
        m_Loggers.Insert(logger);
        return logger;
    }

    Logger* CreateLogger(const String& name)
    {
        return LoggerFactory::GetInstance()->Create(name);
    }
}