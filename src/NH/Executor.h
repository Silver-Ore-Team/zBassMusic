#pragma once

#include "NH/Logger.h"

#pragma warning(push, 1)
#include <cstdint>
#include <vector>
#include <thread>
#include <functional>
#include <deque>
#include <mutex>
#pragma warning(pop)

namespace NH
{
    using TaskFN = std::function<void()>;

    struct Executor
    {
        virtual void AddTask(TaskFN&& task) = 0;
        virtual void AddTask(const TaskFN& task) = 0;
        virtual ~Executor() = default;
    };

    class InstantExecutor : public Executor
    {
    public:
        void AddTask(TaskFN&& task) override { task(); }
        void AddTask(const TaskFN& task) override { task(); }
    };

    class ThreadPool : public Executor
    {
        NH::Logger* log;
        String m_Name;
        std::vector<std::thread> m_Threads;
        std::deque<TaskFN> m_Tasks;
        std::mutex m_TasksMutex;
        bool m_Stop = false;

    public:
        explicit ThreadPool(const String& name, size_t threads = std::thread::hardware_concurrency());

        ~ThreadPool();

        void AddTask(TaskFN&& task) override;

        void AddTask(const TaskFN& task) override;

    private:
        ThreadPool(ThreadPool const&);
        ThreadPool& operator=(ThreadPool const&);

        void EventLoop();
    };

    struct CommonExecutors {
        ThreadPool IO;
    };

    extern CommonExecutors Executors;
}