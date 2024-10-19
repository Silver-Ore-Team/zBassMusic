#include "Executor.h"

namespace NH
{
    CommonExecutors Executors = {.IO = ThreadPool("IO", std::thread::hardware_concurrency())};

    ThreadPool::ThreadPool(const String& name, const size_t threads)
        : Executor(), log(CreateLogger(String::Format("zBassMusic::ThreadPool({0})", name))), m_Name(name)
    {
        for (size_t i = 0; i < threads; i++)
        {
            auto& thread = m_Threads.emplace_back([this] {
                log->Info("[thread-{0}] started", std::format("{}", std::this_thread::get_id()).c_str());
                EventLoop();
                log->Info("[thread-{0}] exiting", std::format("{}", std::this_thread::get_id()).c_str());
            });
            thread.detach();
        }
    }

    ThreadPool::~ThreadPool()
    {
        m_Stop = true;
        for (auto& thread: m_Threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

    void ThreadPool::AddTask(TaskFN&& task)
    {
        std::lock_guard lock(m_TasksMutex);
        m_Tasks.push_back(std::move(task));
    }

    void ThreadPool::AddTask(const TaskFN& task)
    {
        std::lock_guard lock(m_TasksMutex);
        m_Tasks.push_back(task);
    }

    void ThreadPool::EventLoop()
    {
        while (!m_Stop)
        {
            TaskFN task;
            {
                std::lock_guard lock(m_TasksMutex);
                if (!m_Tasks.empty())
                {
                    task = std::move(m_Tasks.front());
                    m_Tasks.pop_front();
                }
            }

            if (task)
            {
                log->Trace("[thread-{0}] processing task", std::format("{}", std::this_thread::get_id()).c_str());
                task();
                log->Trace("[thread-{0}] task done", std::format("{}", std::this_thread::get_id()).c_str());
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

}// namespace NH