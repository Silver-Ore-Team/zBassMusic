#pragma once

#include "NH/Bass/IEngine.h"
#include "NH/ToString.h"

#include <vector>

namespace NH::Bass
{
    class MusicTheme;

    enum class TransitionEffect { NONE = 0, CROSSFADE = 1 };
    struct Transition final : HasToString
    {
        static Transition EMPTY;

        struct TimePoint
        {
            double Start{};
            double Duration{};
            TransitionEffect Effect = TransitionEffect::NONE;
            double NextStart = Start;
            double NextDuration = Duration;
            TransitionEffect NextEffect = Effect;
        };
        TransitionEffect Effect = TransitionEffect::NONE;
        double EffectDuration = 0;
        std::vector<TimePoint> TimePoints;
        std::shared_ptr<AudioFile> Jingle;
        double JingleDelay = 0;

        [[nodiscard]] std::optional<TimePoint> NextAvailableTimePoint(double position) const;

        [[nodiscard]] std::string ToString() const override;
    };
}
