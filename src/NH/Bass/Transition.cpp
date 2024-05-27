#include "Transition.h"

#include <NH/Bass/MusicTheme.h>

namespace NH::Bass
{
    std::optional<Transition::TimePoint> Transition::NextAvailableTimePoint(double position)
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
}