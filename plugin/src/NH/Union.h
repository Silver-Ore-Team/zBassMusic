#pragma once

#include <Union/String.h>

#undef ERROR

namespace NH
{
	namespace Log {
		extern bool zSpyReady;

		enum class Level
		{
			DEBUG = 0,
			INFO = 1,
			WARN = 2,
			ERROR = 3
		};

		void Message(Level level, Union::StringUTF8 channel, Union::StringUTF8 message);

		inline void Debug(const Union::StringUTF8& channel, const Union::StringUTF8& message) { Message(Level::DEBUG, channel, message); }
		inline void Debug(const Union::StringUTF8& message) { Debug("DEFAULT", message); }
		inline void Info(const Union::StringUTF8& channel, const Union::StringUTF8& message) { Message(Level::INFO, channel, message); }
		inline void Info(const Union::StringUTF8& message) { Info("DEFAULT", message); }
		inline void Warn(const Union::StringUTF8& channel, const Union::StringUTF8& message) { Message(Level::WARN, channel, message); }
		inline void Warn(const Union::StringUTF8& message) { Warn("DEFAULT", message); }
		inline void Error(const Union::StringUTF8& channel, const Union::StringUTF8& message) { Message(Level::ERROR, channel, message); }
		inline void Error(const Union::StringUTF8& message) { Error("DEFAULT", message); }
	}
}
