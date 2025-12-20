#include "MusicManager.h"

namespace NH::Bass
{
    void MusicManager::AddTheme(const std::string& id, const std::shared_ptr<MusicTheme>& theme)
    {
        m_Themes[id] = theme;
        if (!m_lazyLoading)
        {
            m_Themes[id]->LoadAudioFiles(Executors.IO);
        }
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
            if (!m_lazyLoading)
            {
                m_Themes[id]->LoadAudioFiles(Executors.IO);
            }
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
        // Track access for LRU
        if (IsThemeLoaded(id))
        {
            TouchTheme(id);
        }
        return m_Themes[id];
    }

    // Used only for lazy loading
    void MusicManager::LoadTheme(const std::string& id)
    {
        if (m_Themes.contains(id))
        {
            log->Info("Loading theme {0}", id.c_str());
            m_Themes[id]->LoadAudioFiles(Executors.IO, [this](const std::string& themeId) { OnThemeReady(themeId); });
        }
        else
        {
            log->Error("Cannot load theme {0}: theme does not exist", id.c_str());
        }
    }

    void MusicManager::OnThemeReady(const std::string& id)
    {
        if (!m_Themes.contains(id) || !m_lazyLoading)
        {
            return;
        }
        ++m_LoadedThemesCount;
        m_NeedsEviction = true;  // Signal main thread to evict
        log->Trace("OnThemeReady: {0} is READY;", id.c_str());
        // Do NOT call TouchTheme here it would be called from EvictOldThemes on Engine::Update
    }

    void MusicManager::TouchTheme(const std::string& id)
    {
        // If already in LRU, remove from current position
        if (m_LRUIterators.contains(id))
        {
            m_LRUOrder.erase(m_LRUIterators[id]);
        }
        m_LRUOrder.push_back(id);
        m_LRUIterators[id] = std::prev(m_LRUOrder.end());
    }

    void MusicManager::EvictOldThemes()
    {
        // Ensure all loaded themes are tracked in LRU (newly ready themes from OnThemeReady)
        for (const auto& [id, theme] : m_Themes)
        {
            if (IsThemeLoaded(id) && !m_LRUIterators.contains(id))
            {
                TouchTheme(id);  // Add newly loaded themes to LRU
            }
        }

        if (m_LoadedThemesCount <= m_MaxLoadedThemes)
        {
            return;
        }

        // Evict from front (oldest) until we're under the limit
        while (!m_LRUOrder.empty() && m_LoadedThemesCount > m_MaxLoadedThemes)
        {
            const std::string& oldestId = m_LRUOrder.front();

            // Should never happen, but just in case
            if (IsThemePlaying(oldestId))
            {
                log->Debug("Skipping eviction of theme {0} because it is currently playing.", oldestId.c_str());
                // Move to back to avoid repeated checks
                TouchTheme(oldestId);
                continue;
            }

            // Only evict if it's actually loaded
            if (m_Themes.contains(oldestId) && IsThemeLoaded(oldestId))
            {
                log->Info("Evicting theme from memory: {0}", oldestId.c_str());
                m_Themes[oldestId]->ReleaseAudioBuffers();
                --m_LoadedThemesCount;
            }

            // Remove from LRU tracking
            m_LRUIterators.erase(oldestId);
            m_LRUOrder.pop_front();
        }
    }

    bool MusicManager::IsThemeLoaded(const std::string& id) const
    {
        if (!m_Themes.contains(id))
        {
            return false;
        }
        return m_Themes.at(id)->IsAudioFileReady(AudioFile::DEFAULT);
    }

    bool MusicManager::IsThemeLoading(const std::string& id) const
    {
        if (!m_Themes.contains(id))
        {
            return false;
        }
        const auto& file = m_Themes.at(id)->GetAudioFile(AudioFile::DEFAULT);
        return file.Status == AudioFile::StatusType::LOADING;
    }

    bool MusicManager::IsThemePlaying(const std::string& id) const
    {
        if (!m_Themes.contains(id))
        {
            return false;
        }
        return m_Themes.at(id)->IsPlaying();
    }

}