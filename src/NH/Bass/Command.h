#pragma once

#include <NH/Logger.h>

#include <deque>
#include <chrono>
#include <memory>
#include <utility>
#include <mutex>

namespace NH::Bass
{
    enum class CommandResult
    {
        DONE,       // Remove command from the queue
        RETRY,      // Keep command in the front of a queue
        DEFER       // Keep command in the back of a queue
    };

    class Engine;

    struct Command
    {
        virtual CommandResult Execute(Engine& engine) = 0;
    };

    class OnTimeCommand : public Command
    {
        using TimePointType = std::chrono::high_resolution_clock::time_point;

        TimePointType m_TimePoint;
        std::shared_ptr<Command> m_Command;
        bool m_ForceOrder = false;

    public:
        OnTimeCommand(TimePointType timePoint, std::shared_ptr<Command> command, bool forceOrder = false)
                : m_TimePoint(timePoint), m_Command(std::move(command)), m_ForceOrder(forceOrder) {}

        CommandResult Execute(Engine& engine) override;
    };

    class CommandQueue
    {
        Logger* log = CreateLogger("zBassMusic::CommandQueue");
        std::deque<std::shared_ptr<Command>> m_Commands;
        std::deque<std::shared_ptr<Command>> m_DeferredCommands;
        std::mutex m_Mutex;
        std::mutex m_DeferredMutex;

    public:
        void AddCommand(std::shared_ptr<Command> command);

        /**
         * Method to add commands within an executing command.
         * We can't use AddCommand() directly, because it would cause a deadlock.
         * @param command
         */
        void AddCommandDeferred(std::shared_ptr<Command> command);

        void AddCommandOnFront(std::shared_ptr<Command> command);

        void Update(Engine& engine);
    };
}
