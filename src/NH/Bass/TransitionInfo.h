#pragma once

#include "Transition.h"
#include <NH/Commons.h>
#include <NH/HashString.h>

#include <unordered_map>

namespace NH::Bass
{
    class MidiFile;

    class TransitionInfo
    {
        HashString m_SourceTheme;
        Transition m_DefaultTransition;
        std::unordered_map<HashString, Transition> m_FilteredTransitions;

    public:
        explicit TransitionInfo(HashString sourceTheme)
            : m_SourceTheme(sourceTheme)
        {}

        void AddTransitionEffect(TransitionEffect effect, double duration = 0, HashString filter = ""_hs);
        void AddMidiFile(MidiFile& file, HashString filter = ""_hs);
        void AddTimePoint(Transition::TimePoint timePoint, HashString filter = ""_hs);
        void AddJingle(std::shared_ptr<MusicTheme> jingle, double delay, HashString filter = ""_hs);

        [[nodiscard]] const Transition& GetTransition(HashString targetTheme) const;
        [[nodiscard]] HashString GetSourceTheme() const { return m_SourceTheme; }

    private:
        Transition* GetFilteredTransition(HashString filter);
    };

}
