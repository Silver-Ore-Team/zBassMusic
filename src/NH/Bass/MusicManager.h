#pragma once

#include <NH/Bass/MusicTheme.h>
#include <vector>

namespace NH::Bass
{

    class MusicManager
    {
        Logger* log = CreateLogger("zBassMusic::MusicManager");
        std::unordered_map<HashString, std::shared_ptr<MusicTheme>> m_Themes;

    public:
        void AddTheme(HashString id, std::shared_ptr<MusicTheme> theme);

        [[nodiscard]] std::shared_ptr<MusicTheme> GetTheme(HashString id);

        std::vector<std::pair<HashString, std::shared_ptr<MusicTheme>>> GetThemesForZone(HashString zone);
    };
}
