#pragma once

#include <NH/Bass/MusicTheme.h>
#include <vector>

namespace NH::Bass
{

    class MusicManager
    {
        Logger* log = CreateLogger("zBassMusic::MusicManager");
        std::unordered_map<std::string, std::shared_ptr<MusicTheme>> m_Themes;

    public:
        void AddTheme(const std::string& id, const std::shared_ptr<MusicTheme>& theme);
        void AddJingle(const std::string& id, const String& jingleFilename, double delay, const std::string& filter = "");
        void RefreshTheme(const std::string& id);

        [[nodiscard]] std::shared_ptr<MusicTheme> GetTheme(const std::string& id);
        std::vector<std::pair<std::string, std::shared_ptr<MusicTheme>>> GetThemesForZone(const std::string& zone);
    };
}
