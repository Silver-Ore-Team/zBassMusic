#include "MusicManager.h"

namespace NH::Bass
{
    void MusicManager::AddTheme(HashString id, const std::shared_ptr<MusicTheme>& theme)
    {
        m_Themes[id] = theme;
        m_Themes[id]->LoadAudioFiles(Executors.IO);
        log->Info("New theme {0}", id);
        log->PrintRaw(LoggerLevel::Debug, m_Themes[id]->ToString());
    }

    void MusicManager::AddJingle(HashString id, const String& jingleFilename, double delay, HashString filter)
    {
        if (m_Themes.contains(id))
        {
            m_Themes[id]->AddJingle(jingleFilename, delay, filter);
        }
        else
        {
            log->Error("Theme {0} does not exist", String(id));
        }
    }

    void MusicManager::RefreshTheme(HashString id)
    {
        if (m_Themes.contains(id))
        {
            m_Themes[id]->LoadAudioFiles(Executors.IO);
            log->Info("Refresh theme {0}", id);
            log->PrintRaw(LoggerLevel::Debug, m_Themes[id]->ToString());
        }
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
        return m_Themes[id];
    }
}