#pragma once

#include <Union/String.h>

namespace NH
{
	namespace Bass
	{
		struct BassOptions
		{
			float TransitionTime = 2000.0f;
			bool ForceDisableReverb = false;
			bool ForceFadeTransition = false;
			bool CreateMainParserCMusicTheme = true;
			Union::StringUTF8 LoggerLevelUnion = "INFO";
			Union::StringUTF8 LoggerLevelZSpy = "DEBUG";
		};

		extern BassOptions Options;
	}
}