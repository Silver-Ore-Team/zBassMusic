#pragma once

#include <NH/Commons.h>
#include <NH/Bass/IEngine.h>
#include <vector>

namespace NH::Bass
{
    class MusicTheme;

    enum class TransitionEffect { NONE, CROSSFADE };
    struct Transition
    {
        struct TimePoint
        {
            double Start{};
            double Duration{};
            TransitionEffect Effect = TransitionEffect::NONE;
            double NextStart = Start;
            double NextDuration = Duration;
            TransitionEffect NextEffect = TransitionEffect::NONE;
        };
        TransitionEffect Effect = TransitionEffect::NONE;
        double EffectDuration = 0;
        std::vector<TimePoint> TimePoints;
        std::shared_ptr<MusicTheme> Jingle;
        double JingleDelay = 0;

        [[nodiscard]] std::optional<TimePoint> NextAvailableTimePoint(double position);
    };
}
