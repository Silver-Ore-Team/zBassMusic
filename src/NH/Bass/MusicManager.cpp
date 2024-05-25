#include "MusicManager.h"

namespace NH::Bass
{
    void MusicManager::AddTheme(HashString id, std::shared_ptr<MusicTheme> theme)
    {
        m_Themes.emplace(id, theme);
        m_Themes.at(id)->LoadAudioFiles(Executors.IO);
        log->Info("New theme {0}", id);
        log->PrintRaw(LoggerLevel::Debug, m_Themes.at(id)->ToString());
    }

    std::vector<std::pair<HashString, std::shared_ptr<MusicTheme>>> MusicManager::GetThemesForZone(HashString zone)
    {
        std::vector<std::pair<HashString, std::shared_ptr<MusicTheme>>> themes;
        for (auto& [id, theme] : m_Themes)
        {
            if (theme->HasZone(zone))
            {
                themes.emplace_back(id, theme);
            }
        }
        return std::move(themes);
    }

    std::shared_ptr<MusicTheme> MusicManager::GetTheme(HashString id)
    {
        if (!m_Themes.contains(id))
        {
            return {};
        }
        return m_Themes.at(id);
    }
}