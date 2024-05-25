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
        std::lock_guard<std::mutex> lock(m_Mutex);
        log->Info("Adding command to queue");
        m_Commands.push_back(std::move(command));
    }

    void CommandQueue::AddCommandDeferred(std::shared_ptr<Command> command)
    {
        std::lock_guard<std::mutex> lock(m_DeferredMutex);
        m_DeferredCommands.push_back(std::move(command));
    }

    void CommandQueue::AddCommandOnFront(std::shared_ptr<Command> command)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        log->Info("Adding command to queue");
        m_Commands.push_front(std::move(command));
    }

    void CommandQueue::Update(Engine& engine)
    {
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