#pragma once

#include "NH/Bass/MusicTheme.h"

#pragma warning(push, 1)
#include <vector>
#pragma warning(pop)

namespace NH::Bass
{

    class MusicManager
    {
        Logger* log = CreateLogger("zBassMusic::MusicManager");
        std::unordered_map<HashString, std::shared_ptr<MusicTheme>> m_Themes;

    public:
        void AddTheme(HashString id, const std::shared_ptr<MusicTheme>& theme);
        void AddJingle(HashString id, const String& jingleFilename, double delay, HashString filter = ""_hs);
        void RefreshTheme(HashString id);

        [[nodiscard]] std::shared_ptr<MusicTheme> GetTheme(HashString id);
        std::vector<std::pair<HashString, std::shared_ptr<MusicTheme>>> GetThemesForZone(HashString zone);
    };
}
