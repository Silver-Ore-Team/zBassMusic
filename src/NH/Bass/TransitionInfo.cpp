#include "TransitionInfo.h"

#include "Transition.h"
#include "MidiFile.h"
#include "MusicTheme.h"

#include <utility>
#include <regex>

namespace NH::Bass
{
    const Transition& TransitionInfo::GetTransition(const std::string& targetTheme) const
    {
        for (const auto& [key, transition]: m_FilteredTransitions)
        {
            String regexString = String(key.c_str())
                // interpret comma as an OR operator, "THEME_1,THEME_2" -> "THEME_1|THEME_2"
                .Replace(",", "|")
                // interpret a space as zero or more whitespace characters
                .Replace(" ", "\\s*")
                // nasty hack to avoid replacement of .* -> ..* by the next step
                .Replace(".*", "*")
                // interpret wildcard (*) as any match (.*) in the regex syntax
                .Replace("*", ".*");

            const std::regex regex(regexString);
            if (std::smatch match; std::regex_match(targetTheme, match, regex))
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

    void TransitionInfo::AddTransitionEffect(const TransitionEffect effect, const double duration, const std::string& filter)
    {
        Transition* transition = filter.empty() ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->Effect = effect;
        transition->EffectDuration = duration;
    }

    void TransitionInfo::AddMidiFile(const MidiFile& file, const std::string& filter)
    {
        Transition* transition = filter.empty() ? &m_DefaultTransition : GetFilteredTransition(filter);
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
        Transition* transition = filter.empty() ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->TimePoints.push_back(timePoint);
    }

    void TransitionInfo::AddJingle(std::shared_ptr<AudioFile> jingle, const double delay, const std::string& filter)
    {
        Transition* transition = filter.empty() ? &m_DefaultTransition : GetFilteredTransition(filter);
        transition->Jingle = std::move(jingle);
        transition->JingleDelay = delay;
    }

    Transition* TransitionInfo::GetFilteredTransition(const std::string& filter)
    {
        if (!m_FilteredTransitions.contains(filter))
        {
            m_FilteredTransitions.emplace(filter, m_DefaultTransition);
        }
        return &m_FilteredTransitions[filter];
    }
}