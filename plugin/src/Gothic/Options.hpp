#include <NH/BassOptions.h>

namespace GOTHIC_NAMESPACE
{
	void ApplyOptions()
	{
		if (GetGameVersion() != ENGINE)
		{
			return;
		}


		{
			char* value = zoptions->ReadString("BASSMUSIC", "LoggerLevelUnion", "INFO").ToChar();
			NH::Bass::Options->LoggerLevelUnion = Union::StringUTF8(value);
			NH::Log::Info("ApplyOptions", Union::StringUTF8("LoggerLevelUnion = ") + NH::Bass::Options->LoggerLevelUnion);
		}
		{
			char* value = zoptions->ReadString("BASSMUSIC", "LoggerLevelZSpy", "DEBUG").ToChar();
			NH::Bass::Options->LoggerLevelZSpy = Union::StringUTF8(value);
			NH::Log::Info("ApplyOptions", Union::StringUTF8("LoggerLevelZSpy = ") + NH::Bass::Options->LoggerLevelZSpy);
		}

		NH::Bass::Options->TransitionTime = zoptions->ReadReal("BASSMUSIC", "TransitionTime", 2000.0f);
		NH::Log::Info("ApplyOptions", Union::StringUTF8("TransitionTime = ") + Union::StringUTF8(NH::Bass::Options->TransitionTime));
		NH::Bass::Options->ForceDisableReverb = zoptions->ReadBool("BASSMUSIC", "ForceDisableReverb", false);
		NH::Log::Info("ApplyOptions", Union::StringUTF8("ForceDisableReverb = ") + Union::StringUTF8(NH::Bass::Options->ForceDisableReverb ? "TRUE" : "FALSE"));
		NH::Bass::Options->ForceFadeTransition = zoptions->ReadBool("BASSMUSIC", "ForceFadeTransition", false);
		NH::Log::Info("ApplyOptions", Union::StringUTF8("ForceFadeTransition = ") + Union::StringUTF8(NH::Bass::Options->ForceFadeTransition ? "TRUE" : "FALSE"));
		NH::Bass::Options->CreateMainParserCMusicTheme = zoptions->ReadBool("BASSMUSIC", "CreateMainParserCMusicTheme", true);
		NH::Log::Info("ApplyOptions", Union::StringUTF8("CreateMainParserCMusicTheme = ") + Union::StringUTF8(NH::Bass::Options->CreateMainParserCMusicTheme ? "TRUE" : "FALSE"));
	}
}
