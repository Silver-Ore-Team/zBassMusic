#include "MusicManager.h"

namespace NH::Bass
{
    void MusicManager::AddTheme(const std::string& id, const std::shared_ptr<MusicTheme>& theme)
    {
        m_Themes[id] = theme;
        m_Themes[id]->LoadAudioFiles(Executors.IO);
        log->Info("New theme {0}", id.c_str());
        log->PrintRaw(LoggerLevel::Debug, m_Themes[id]->ToString().c_str());
    }

    void MusicManager::AddJingle(const std::string& id, const String& jingleFilename, const double delay, const std::string& filter)
    {
        if (m_Themes.contains(id))
        {
            m_Themes[id]->AddJingle(jingleFilename.ToChar(), delay, filter);
        }
        else
        {
            log->Error("Theme {0} does not exist", id.c_str());
        }
    }

    void MusicManager::RefreshTheme(const std::string& id)
    {
        if (m_Themes.contains(id))
        {
            m_Themes[id]->LoadAudioFiles(Executors.IO);
            log->Info("Refresh theme {0}", id.c_str());
            log->PrintRaw(LoggerLevel::Debug, m_Themes[id]->ToString().c_str());
        }
    }

    std::vector<std::pair<std::string, std::shared_ptr<MusicTheme>>> MusicManager::GetThemesForZone(const std::string& zone)
    {
        std::vector<std::pair<std::string, std::shared_ptr<MusicTheme>>> themes;
        for (auto& [id, theme] : m_Themes)
        {
            if (theme->HasZone(zone))
            {
                themes.emplace_back(id, theme);
            }
        }
        return themes;
    }

    std::shared_ptr<MusicTheme> MusicManager::GetTheme(const std::string& id)
    {
        if (!m_Themes.contains(id))
        {
            return {};
        }
        return m_Themes[id];
    }
}