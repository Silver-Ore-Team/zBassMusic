#include "Command.h"

namespace NH::Bass
{
    CommandResult OnTimeCommand::Execute(Engine& engine)
    {
        if (std::chrono::high_resolution_clock::now() >= m_TimePoint)
        {
            return m_Command->Execute(engine);
        }
        return m_ForceOrder ? CommandResult::RETRY : CommandResult::DEFER;
    }

    void CommandQueue::AddCommand(std::shared_ptr<Command> command)
    {
        std::lock_guard<std::mutex> lock(m_DeferredMutex);
        log->Trace("Adding command to queue");
        m_DeferredCommands.push_back(std::move(command));
    }

    void CommandQueue::AddCommandOnFront(std::shared_ptr<Command> command)
    {
        std::lock_guard<std::mutex> lock(m_DeferredMutex);
        log->Trace("Adding command to queue");
        m_DeferredCommands.push_front(std::move(command));
    }

    void CommandQueue::AddPerFrameCommand(std::shared_ptr<Command> command)
    {
        std::lock_guard<std::mutex> lock(m_PerFrameDeferredMutex);
        m_PerFrameCommandsDeferred.push_back(std::move(command));
    }

    void CommandQueue::Update(Engine& engine)
    {
        {
            std::lock_guard<std::mutex> lock(m_PerFrameDeferredMutex);
            m_PerFrameCommands.insert(m_PerFrameCommands.end(), m_PerFrameCommandsDeferred.begin(), m_PerFrameCommandsDeferred.end());
            m_PerFrameCommandsDeferred.clear();
        }

        {
            std::lock_guard<std::mutex> lock(m_PerFrameMutex);
            std::vector<std::shared_ptr<Command>> finishedCommands{};
            for (auto& command: m_PerFrameCommands)
            {
                if (command->Execute(engine) == CommandResult::DONE)
                {
                    finishedCommands.push_back(command);
                }
            }
            std::erase_if(m_PerFrameCommands, [&finishedCommands](const std::shared_ptr<Command>& command)
            {
                return std::find(finishedCommands.begin(), finishedCommands.end(), command) != finishedCommands.end();
            });
        }

        {
            std::lock_guard<std::mutex> lock(m_DeferredMutex);
            m_Commands.insert(m_Commands.end(), m_DeferredCommands.begin(), m_DeferredCommands.end());
            m_DeferredCommands.clear();
        }

        {
            std::lock_guard<std::mutex> lock(m_DeferredMutex);
            m_Commands.insert(m_Commands.end(), m_DeferredCommands.begin(), m_DeferredCommands.end());
            m_DeferredCommands.clear();
        }

        std::lock_guard<std::mutex> lock(m_Mutex);
        std::shared_ptr<Command> commandToDefer;
        bool exitEarly = false;
        while (!exitEarly && !m_Commands.empty())
        {
            auto result = m_Commands.front()->Execute(engine);
            switch (result)
            {
                case CommandResult::DONE:
                    m_Commands.pop_front();
                    break;
                case CommandResult::RETRY:
                    // Return, so we don't execute the next command
                    exitEarly = true;
                    break;
                case CommandResult::DEFER:
                    commandToDefer = m_Commands.front();
                    m_Commands.pop_front();
                    break;
            }
        }
        if (commandToDefer)
        {
            m_Commands.push_back(commandToDefer);
        }
    }
}