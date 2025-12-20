#pragma once

#include "NH/Bass/MusicTheme.h"

#include <vector>
#include <list>

namespace NH::Bass
{
    class MusicManager
    {
        Logger* log = CreateLogger("zBassMusic::MusicManager");
        std::unordered_map<std::string, std::shared_ptr<MusicTheme>> m_Themes;
        bool m_lazyLoading = true;
        std::list<std::string> m_LRUOrder;  // Most recent at back, oldest at front
        std::unordered_map<std::string, std::list<std::string>::iterator> m_LRUIterators;
        size_t m_MaxLoadedThemes = 5;
        std::atomic<size_t> m_LoadedThemesCount{0};  // Counter for loaded themes (atomic for thread-safe increment)
        std::atomic<bool> m_NeedsEviction{false};  // Signaled by OnThemeReady, checked by Engine::Update
    public:
        void AddTheme(const std::string& id, const std::shared_ptr<MusicTheme>& theme);
        void AddJingle(const std::string& id, const String& jingleFilename, double delay, const std::string& filter = "");
        void RefreshTheme(const std::string& id);

        void LoadTheme(const std::string& id);
        bool IsThemeLoaded(const std::string& id) const;
        bool IsThemeLoading(const std::string& id) const;
        bool IsThemeFailed(const std::string& id) const;
        bool IsThemePlaying(const std::string& id) const;
        void OnThemeReady(const std::string& id); // Callback when a theme finishes loading (for lazy loading)
        void SetMaxLoadedThemes(size_t maxThemes) { m_MaxLoadedThemes = maxThemes; };
        void SetLazyLoading(bool lazy) { m_lazyLoading = lazy; };
        [[nodiscard]] bool ShouldEvict() { return m_NeedsEviction.exchange(false); }
        void EvictOldThemes();

        [[nodiscard]] std::shared_ptr<MusicTheme> GetTheme(const std::string& id);
        std::vector<std::pair<std::string, std::shared_ptr<MusicTheme>>> GetThemesForZone(const std::string& zone);
    private:
        void TouchTheme(const std::string& id);
    };
}
