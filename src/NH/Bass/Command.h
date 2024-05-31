#pragma once

#include <NH/Logger.h>

#include <deque>
#include <chrono>
#include <memory>
#include <utility>
#include <mutex>
#include <functional>

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
        virtual ~Command() = default;
    };

    class FunctionCommand : public Command
    {
        std::function<CommandResult(Engine&)> m_Function;
    public:
        explicit FunctionCommand(std::function<CommandResult(Engine&)> function) : m_Function(std::move(function)) {}
        CommandResult Execute(Engine& engine) override { return m_Function(engine); }
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
        // Commands for one-off actions may stay in queue while returning RETRY or DEFER
        std::deque<std::shared_ptr<Command>> m_Commands;
        // Commands added during execution of a command. They are added to the main queue after execution.
        std::deque<std::shared_ptr<Command>> m_DeferredCommands;
        // Commands which lifetime is expected to last several frames.
        // They are executed from another queue to avoid locking with one-off commands that use RETRY.
        std::vector<std::shared_ptr<Command>> m_PerFrameCommands;
        std::vector<std::shared_ptr<Command>> m_PerFrameCommandsDeferred;
        std::mutex m_Mutex;
        std::mutex m_DeferredMutex;
        std::mutex m_PerFrameMutex;
        std::mutex m_PerFrameDeferredMutex;

    public:
        void AddCommand(std::shared_ptr<Command> command);

        void AddPerFrameCommand(std::shared_ptr<Command> command);

        void Update(Engine& engine);
    };
}
