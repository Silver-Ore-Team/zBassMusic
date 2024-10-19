#pragma once

#include "NH/Logger.h"
#include "Command.h"

#include <utility>

namespace NH::Bass
{
    class ChangeZoneCommand final : public Command
    {
        static Logger* log;
        std::string m_Zone;
    public:
        explicit ChangeZoneCommand(std::string zone) : m_Zone(std::move(zone)) {};
        CommandResult Execute(Engine& engine) override;
    };

    class ScheduleThemeChangeCommand final : public Command
    {
        static Logger* log;
        std::string m_ThemeId;
    public:
        explicit ScheduleThemeChangeCommand(std::string themeId) : m_ThemeId(std::move(themeId)) {};
        CommandResult Execute(Engine& engine) override;
    };

    class PlayThemeInstantCommand final : public Command
    {
        static Logger* log;
        std::string m_ThemeId;
    public:
        explicit PlayThemeInstantCommand(std::string themeId) : m_ThemeId(std::move(themeId)) {};
        CommandResult Execute(Engine& engine) override;
    };
}