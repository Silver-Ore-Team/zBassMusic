#include "Transition.h"

#include "NH/Bass/MusicTheme.h"

namespace NH::Bass
{
    Transition Transition::EMPTY = {};

    std::optional<Transition::TimePoint> Transition::NextAvailableTimePoint(const double position) const
    {
        for (const auto& timePoint : TimePoints)
        {
            if (position >= timePoint.Start && position < timePoint.Start + timePoint.Duration)
            {
                return timePoint;
            }
        }
        return std::nullopt;
    }

    std::string Transition::ToString() const
    {
        static const std::string effects[] = {"NONE", "CROSSFADE"};
        return std::string("Transition {\n")
                + "\tEffect: " + effects[static_cast<size_t>(Effect)] + ",\n"
                + "\tEffectDuration: " + String(EffectDuration).ToChar() + ",\n"
                + "\tTimePoints: " + "not_impl" + ",\n"
                + "\tJingle: " + "not_impl" + ",\n"
                + "\tJingleDelay: " + String(JingleDelay).ToChar() + "\n"
            + "}";
    }
}