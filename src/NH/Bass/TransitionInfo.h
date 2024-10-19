#pragma once

#include "Transition.h"

#include <unordered_map>
#include <utility>

namespace NH::Bass
{
    class MidiFile;

    class TransitionInfo
    {
        std::string m_SourceTheme;
        Transition m_DefaultTransition;
        std::unordered_map<std::string, Transition> m_FilteredTransitions;

    public:
        explicit TransitionInfo(std::string sourceTheme)
            : m_SourceTheme(std::move(sourceTheme))
        {}

        void AddTransitionEffect(TransitionEffect effect, double duration = 0, const std::string& filter = "");
        void AddMidiFile(const MidiFile& file, const std::string& filter = "");
        void AddTimePoint(const Transition::TimePoint& timePoint, const std::string& filter = "");
        void AddJingle(std::shared_ptr<AudioFile> jingle, double delay, const std::string& filter = "");

        [[nodiscard]] const Transition& GetTransition(const std::string& targetTheme) const;
        [[nodiscard]] const Transition& GetDefaultTransition() const;
        [[nodiscard]] const std::string& GetSourceTheme() const { return m_SourceTheme; }

    private:
        Transition* GetFilteredTransition(const std::string& filter);
    };

}
