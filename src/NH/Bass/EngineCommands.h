#pragma once

#include "NH/HashString.h"
#include "NH/Logger.h"
#include "NH/Bass/Command.h"
#include "NH/Bass/IEngine.h"

namespace NH::Bass
{
    class ChangeZoneCommand : public Command
    {
        static Logger* log;
        String m_Zone;
    public:
        explicit ChangeZoneCommand(const String& zone) : m_Zone(zone) {};
        CommandResult Execute(Engine& engine) override;
    };

    class ScheduleThemeChangeCommand : public Command
    {
        static Logger* log;
        HashString m_ThemeId;
    public:
        explicit ScheduleThemeChangeCommand(HashString themeId) : m_ThemeId(themeId) {};
        CommandResult Execute(Engine& engine) override;
    };

    class PlayThemeInstantCommand : public Command
    {
        static Logger* log;
        HashString m_ThemeId;
    public:
        explicit PlayThemeInstantCommand(HashString themeId) : m_ThemeId(themeId) {};
        CommandResult Execute(Engine& engine) override;
    };
}