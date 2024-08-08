#include "TransitionInfo.h"
#include "Transition.h"

#include <NH/Bass/MidiFile.h>
#include <NH/Bass/MusicTheme.h>

#include <utility>

namespace NH::Bass
{
    const Transition& TransitionInfo::GetTransition(const std::string& targetTheme) const
    {
        for (const auto& [key, transition]: m_FilteredTransitions)
        {
            String regex = String(key.c_str()).Replace(",", "|").Replace(" ", "\\s*").Replace("*", ".*");;
            if (regex.IsMatchesMask(targetTheme.c_str()))
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

    void TransitionInfo::AddTransitionEffect(TransitionEffect effect, double duration, const std::string& filter)
    {
        Transition* transition = filter.empty() ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->Effect = effect;
        transition->EffectDuration = duration;
    }

    void TransitionInfo::AddMidiFile(const MidiFile& file, const std::string& filter)
    {
        Transition* transition = filter == "" ? &m_DefaultTransition : GetFilteredTransition(filter);
        Transition::TimePoint timePoint{-1};
        for (const auto& [key, start, end] : file.GetTones())
        {
            if (key == 69) // A4
            {
                if (timePoint.Start > -1) { transition->TimePoints.push_back(timePoint); }
                timePoint = { start, end - start, TransitionEffect::CROSSFADE };
            }
            else if (key == 70 && timePoint.Start > -1) // A4
            {
                timePoint.NextStart = start;
                timePoint.NextDuration = end - start;
                timePoint.NextEffect = TransitionEffect::CROSSFADE;
                transition->TimePoints.push_back(timePoint);
                timePoint = {-1};
            }
        }
        if (timePoint.Start > -1) { transition->TimePoints.push_back(timePoint); }
    }

    void TransitionInfo::AddTimePoint(const Transition::TimePoint& timePoint, const std::string& filter)
    {
        Transition* transition = filter == "" ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->TimePoints.push_back(timePoint);
    }

    void TransitionInfo::AddJingle(std::shared_ptr<AudioFile> jingle, const double delay, const std::string& filter)
    {
        Transition* transition = filter == "" ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->Jingle = std::move(jingle);
        transition->JingleDelay = delay;
    }

    Transition* TransitionInfo::GetFilteredTransition(std::string filter)
    {
        if (!m_FilteredTransitions.contains(filter))
        {
            m_FilteredTransitions.emplace(filter, m_DefaultTransition);
        }
        return &m_FilteredTransitions[filter];
    }
}