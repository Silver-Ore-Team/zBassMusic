#include "EngineCommands.h"

#include "NH/Bass/Engine.h"
#include "NH/Bass/Options.h"

namespace NH::Bass
{
    Logger* ChangeZoneCommand::log = CreateLogger("zBassMusic::ChangeZoneCommand");
    Logger* ScheduleThemeChangeCommand::log = CreateLogger("zBassMusic::ScheduleThemeChangeCommand");
    Logger* PlayThemeInstantCommand::log = CreateLogger("zBassMusic::PlayThemeInstantCommand");

    CommandResult ChangeZoneCommand::Execute(Engine& engine)
    {
        log->Info("Music zone changed: {0}", m_Zone);
        const auto themes = engine.GetMusicManager().GetThemesForZone(m_Zone);
        if (themes.empty())
        {
            log->Warning("No themes found for zone {0}", m_Zone);
            return CommandResult::DONE;
        }
        engine.GetCommandQueue().AddCommand(std::make_shared<ScheduleThemeChangeCommand>(themes[0].first));
        return CommandResult::DONE;
    }

    CommandResult ScheduleThemeChangeCommand::Execute(Engine& engine)
    {
        log->Info("Scheduling theme: {0}", m_ThemeId);
        auto theme = engine.GetMusicManager().GetTheme(m_ThemeId);
        if (!theme)
        {
            log->Error("Theme {0} doesn't exist", m_ThemeId);
            return CommandResult::DONE;
        }

        auto activeTheme = engine.m_ActiveTheme;
        bool anyChannelPlaying = false;
        for (auto& channel : engine.m_Channels)
        {
            if (channel->IsPlaying()) { anyChannelPlaying = true; break; }
        }

        if (activeTheme && anyChannelPlaying) theme->Schedule(engine, activeTheme);
        else theme->Play(engine);
        engine.m_ActiveTheme = theme;

        return CommandResult::DONE;
    }

    CommandResult PlayThemeInstantCommand::Execute(Engine& engine)
    {
        log->Info("Playing theme: {0} instantly, because PlayThemeInstantCommand forced it.", m_ThemeId);
        auto theme = engine.GetMusicManager().GetTheme(m_ThemeId);
        if (!theme)
        {
            log->Error("Theme {0} doesn't exist", m_ThemeId);
            return CommandResult::DONE;
        }

        auto activeTheme = engine.GetActiveTheme();
        if (activeTheme) { activeTheme->Stop(engine); }
        theme->Play(engine);
        engine.SetActiveTheme(theme);

        return CommandResult::DONE;
    }
}