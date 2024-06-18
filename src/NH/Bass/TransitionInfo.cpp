#include "TransitionInfo.h"
#include "Transition.h"
#include "NH/Bass/MidiFile.h"
#include "NH/Bass/MusicTheme.h"

#pragma warning(push, 1)
#include <utility>
#pragma warning(pop)

namespace NH::Bass
{
    const Transition& TransitionInfo::GetTransition(HashString targetTheme) const
    {
        for (const auto& [key, transition]: m_FilteredTransitions)
        {
            String regex = String(key.GetValue()).Replace(",", "|").Replace(" ", "\\s*").Replace("*", ".*");;
            if (regex.IsMatchesMask(targetTheme.GetValue()))
            {
                return transition;
            }
        }
        return m_DefaultTransition;
    }

    const Transition& TransitionInfo::GetDefaultTransition() const
    {
        return m_DefaultTransition;
    }

    void TransitionInfo::AddTransitionEffect(TransitionEffect effect, double duration, HashString filter)
    {
        Transition* transition = filter == ""_hs ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->Effect = effect;
        transition->EffectDuration = duration;
    }

    void TransitionInfo::AddMidiFile(MidiFile& file, HashString filter)
    {
        Transition* transition = filter == ""_hs ? &m_DefaultTransition : GetFilteredTransition(filter);
        Transition::TimePoint timePoint{-1};
        for (const auto& tone : file.GetTones())
        {
            if (tone.key == 69) // A4
            {
                if (timePoint.Start > -1) { transition->TimePoints.push_back(timePoint); }
                timePoint = { tone.start, tone.end - tone.start, TransitionEffect::CROSSFADE };
            }
            else if (tone.key == 70 && timePoint.Start > -1) // A4
            {
                timePoint.NextStart = tone.start;
                timePoint.NextDuration = tone.end - tone.start;
                timePoint.NextEffect = TransitionEffect::CROSSFADE;
                transition->TimePoints.push_back(timePoint);
                timePoint = {-1};
            }
        }
        if (timePoint.Start > -1) { transition->TimePoints.push_back(timePoint); }
    }

    void TransitionInfo::AddTimePoint(Transition::TimePoint timePoint, HashString filter)
    {
        Transition* transition = filter == ""_hs ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->TimePoints.push_back(timePoint);
    }

    void TransitionInfo::AddJingle(std::shared_ptr<AudioFile> jingle, double delay, HashString filter)
    {
        Transition* transition = filter == ""_hs ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->Jingle = std::move(jingle);
        transition->JingleDelay = delay;
    }

    Transition* TransitionInfo::GetFilteredTransition(HashString filter)
    {
        if (!m_FilteredTransitions.contains(filter))
        {
            m_FilteredTransitions.emplace(filter, m_DefaultTransition);
        }
        return &m_FilteredTransitions[filter];
    }
}