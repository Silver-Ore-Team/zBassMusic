#include "Transition.h"

#include "NH/Bass/MusicTheme.h"

namespace NH::Bass
{
    Transition Transition::EMPTY = {};

    std::optional<Transition::TimePoint> Transition::NextAvailableTimePoint(double position) const
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

    String Transition::ToString() const
    {
        static String effects[] = {"NONE", "CROSSFADE"};
        return String("Transition {\n")
                + "\tEffect: " + effects[(size_t)Effect] + ",\n"
                + "\tEffectDuration: " + String(EffectDuration) + ",\n"
                + "\tTimePoints: " + "not_impl" + ",\n"
                + "\tJingle: " + "not_impl" + ",\n"
                + "\tJingleDelay: " + String(JingleDelay) + "\n"
            + "}";
    }
}