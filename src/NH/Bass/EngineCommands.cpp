#include "EngineCommands.h"

#include "NH/Bass/Engine.h"
#include "NH/Bass/Options.h"

#include <algorithm>

namespace NH::Bass
{
    Logger* ChangeZoneCommand::log = CreateLogger("zBassMusic::ChangeZoneCommand");
    Logger* ScheduleThemeChangeCommand::log = CreateLogger("zBassMusic::ScheduleThemeChangeCommand");
    Logger* PlayThemeInstantCommand::log = CreateLogger("zBassMusic::PlayThemeInstantCommand");

    CommandResult ChangeZoneCommand::Execute(Engine& engine)
    {
        log->Info("Music zone changed: {0}", m_Zone.c_str());
        engine.SetCurrentZone(m_Zone);
        const auto themes = engine.GetMusicManager().GetThemesForZone(m_Zone);
        if (themes.empty())
        {
            log->Warning("No themes found for zone {0}", m_Zone.c_str());
            return CommandResult::DONE;
        }
        static std::chrono::high_resolution_clock::time_point lastTimeChange;
        std::chrono::high_resolution_clock::time_point timeChange = std::chrono::high_resolution_clock::now();
        
        if (lastTimeChange > timeChange)
        {
            engine.GetCommandQueue().AddCommand(std::make_shared<OnTimeCommand>(lastTimeChange, std::make_shared<ScheduleThemeChangeCommand>(themes[0].first)));
        }
        else 
        {
            engine.GetCommandQueue().AddCommand(std::make_shared<ScheduleThemeChangeCommand>(themes[0].first));
        }
        lastTimeChange = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(static_cast<int64_t>(themes[0].second->GetTransitionInfo().GetDefaultTransition().EffectDuration + 50));
        return CommandResult::DONE;
    }

    CommandResult ScheduleThemeChangeCommand::Execute(Engine& engine)
    {
        log->Info("Scheduling theme: {0}", m_ThemeId.c_str());
        const auto theme = engine.GetMusicManager().GetTheme(m_ThemeId);
        if (!theme)
        {
            log->Error("Theme {0} doesn't exist", m_ThemeId.c_str());
            return CommandResult::DONE;
        }

        // Lazy loading: trigger load if not already loaded
        if (NH::Bass::Options->LazyLoading && !engine.GetMusicManager().IsThemeLoaded(m_ThemeId))
        {
            if (engine.GetMusicManager().IsThemeFailed(m_ThemeId))
            {
                log->Error("Theme {0} failed to load. Cannot schedule.", m_ThemeId.c_str());
                return CommandResult::DONE;
            }
            if (!engine.GetMusicManager().IsThemeLoading(m_ThemeId))
            {
                log->Debug("Theme {0} not loaded yet. Triggering lazy load.", m_ThemeId.c_str());
                engine.GetMusicManager().LoadTheme(m_ThemeId);
            }
            // Re-queue this command to execute after loading completes
            engine.GetCommandQueue().AddCommand(std::make_shared<ScheduleThemeChangeCommand>(m_ThemeId));
            return CommandResult::DONE;
        }

        const auto activeTheme = engine.m_ActiveTheme;
        const bool anyChannelPlaying = std::ranges::any_of(engine.m_Channels, [](const std::shared_ptr<Channel>& channel) {
            return channel->IsPlaying();
        });

        if (activeTheme && anyChannelPlaying) theme->Schedule(engine, activeTheme);
        else theme->Play(engine);
        engine.m_ActiveTheme = theme;

        return CommandResult::DONE;
    }

    CommandResult PlayThemeInstantCommand::Execute(Engine& engine)
    {
        log->Info("Playing theme: {0} instantly, because PlayThemeInstantCommand forced it.", m_ThemeId.c_str());
        const auto theme = engine.GetMusicManager().GetTheme(m_ThemeId);
        if (!theme)
        {
            log->Error("Theme {0} doesn't exist", m_ThemeId.c_str());
            return CommandResult::DONE;
        }

        // Lazy loading: trigger load if not already loaded
        if (NH::Bass::Options->LazyLoading && !engine.GetMusicManager().IsThemeLoaded(m_ThemeId))
        {
            if (engine.GetMusicManager().IsThemeFailed(m_ThemeId))
            {
                log->Error("Theme {0} failed to load. Cannot play.", m_ThemeId.c_str());
                return CommandResult::DONE;
            }
            if (!engine.GetMusicManager().IsThemeLoading(m_ThemeId))
            {
                log->Debug("Theme {0} not loaded yet. Triggering lazy load.", m_ThemeId.c_str());
                engine.GetMusicManager().LoadTheme(m_ThemeId);
            }
            // Re-queue this command to execute after loading completes
            engine.GetCommandQueue().AddCommand(std::make_shared<PlayThemeInstantCommand>(m_ThemeId));
            return CommandResult::DONE;
        }

        if (const auto activeTheme = engine.GetActiveTheme()) { activeTheme->Stop(engine); }
        theme->Play(engine);
        engine.SetActiveTheme(theme);

        return CommandResult::DONE;
    }
}