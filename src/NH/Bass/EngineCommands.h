#pragma once

#include <NH/Logger.h>
#include <NH/Bass/Command.h>
#include <NH/Bass/IEngine.h>

#include <utility>

namespace NH::Bass
{
    class ChangeZoneCommand : public Command
    {
        static Logger* log;
        std::string m_Zone;
    public:
        explicit ChangeZoneCommand(std::string zone) : m_Zone(std::move(zone)) {};
        CommandResult Execute(Engine& engine) override;
    };

    class ScheduleThemeChangeCommand : public Command
    {
        static Logger* log;
        std::string m_ThemeId;
    public:
        explicit ScheduleThemeChangeCommand(std::string themeId) : m_ThemeId(std::move(themeId)) {};
        CommandResult Execute(Engine& engine) override;
    };

    class PlayThemeInstantCommand : public Command
    {
        static Logger* log;
        std::string m_ThemeId;
    public:
        explicit PlayThemeInstantCommand(std::string themeId) : m_ThemeId(std::move(themeId)) {};
        CommandResult Execute(Engine& engine) override;
    };
}