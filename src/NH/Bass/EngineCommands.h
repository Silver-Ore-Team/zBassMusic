#pragma once

#include <NH/HashString.h>
#include <NH/Logger.h>
#include <NH/Bass/Command.h>
#include <NH/Bass/IEngine.h>

namespace NH::Bass
{
    class ChangeZoneCommand : public Command
    {
        static Logger* log;
        HashString m_Zone;
    public:
        explicit ChangeZoneCommand(HashString zone) : m_Zone(zone) {};
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
}