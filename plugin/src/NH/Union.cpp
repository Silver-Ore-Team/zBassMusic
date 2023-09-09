#include "Union.h"
#include <NH/BassOptions.h>
#include <ZenGin/zGothicAPI.h>

namespace NH
{
	namespace Log
	{

		inline bool ShouldLog(Level level, const Union::StringUTF8& config)
		{
			if (config.IsEmpty())
			{
				return true;
			}
			if (level == Level::DEBUG) return config == "DEBUG";
			if (level == Level::INFO) return config == "DEBUG" || config == "INFO";
			if (level == Level::WARN) return config == "DEBUG" || config == "INFO" || config == "WARN";
			if (level == Level::ERROR) return config == "DEBUG" || config == "INFO" || config == "WARN" || config == "ERROR";
			return false;
		}

		void Message(Level level, const Union::StringUTF8& channel, const Union::StringUTF8& message)
		{
			if (ShouldLog(level, NH::Bass::Options->LoggerLevelUnion)) {		
				Union::StringUTF8 output = "";
				switch (level)
				{
				case Level::DEBUG:
					output = Union::StringUTF8::Format("\x1B[1;97;45mzBassMusic DEBUG \x1B[0m\x1B[95m %s: %s", channel, message);
					break;
				case Level::INFO:
					output = Union::StringUTF8::Format("\x1B[1m\x1B[37m\x1B[44mzBassMusic  INFO \x1B[0m\x1B[94m %s: %s", channel, message);
					break;
				case Level::WARN:
					output = Union::StringUTF8::Format("\x1B[1m\x1B[37m\x1B[43mzBassMusic  WARN \x1B[0m\x1B[93m %s: %s", channel, message);
					break;
				case Level::ERROR:
					output = Union::StringUTF8::Format("\x1B[1m\x1B[97m\x1B[41mzBassMusic ERROR \x1B[0m\x1B[91m %s: %s", channel, message);
					break;
				}
 
				const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
				CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
				GetConsoleScreenBufferInfo(handle, &consoleInfo);
				const auto color = consoleInfo.wAttributes;

				DWORD dw;
				WriteConsoleA(handle, output.ToChar(), output.GetLength(), &dw, nullptr);
				static constexpr std::string_view newLine{"\n"};
				WriteConsoleA(handle, newLine.data(), newLine.size(), &dw, nullptr);

				SetConsoleTextAttribute(handle, color);
			}

			if (ShouldLog(level, NH::Bass::Options->LoggerLevelZSpy)) {
				if (GetGameVersion() == Engine_G1)
				{
					auto msg = Gothic_I_Classic::zSTRING("B:\tBASSMUSIC: ") + channel.ToChar() + ": " + message.ToChar();
					Gothic_I_Classic::zerr->Message(msg);
				}

				if (GetGameVersion() == Engine_G2A)
				{
					auto msg = Gothic_II_Addon::zSTRING("B:\tBASSMUSIC: ") + channel.ToChar() + ": " + message.ToChar();
					Gothic_II_Addon::zerr->Message(msg);
				}
			}
		}
	}
}