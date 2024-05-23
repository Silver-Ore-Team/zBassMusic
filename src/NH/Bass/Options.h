#pragma once

#include "Union/String.h"

namespace NH::Bass
{
    struct OptionsType
    {
        float TransitionTime;
        bool ForceDisableReverb;
        bool ForceFadeTransition;
        bool CreateMainParserCMusicTheme;
        Union::StringUTF8 LoggerLevelUnion = "DEBUG";
        Union::StringUTF8 LoggerLevelZSpy = "DEBUG";
    };

    extern OptionsType* Options;
}
