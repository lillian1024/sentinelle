#pragma once

#include "utils/singleton/singleton.hh"
#include <cstddef>
#include <mutex>
#include <optional>
#include <thread>

namespace utils
{
    namespace thread
    {
        template< class F, class... Args >
        concept ParamFunctor = requires(F f, Args&&... args) { f(args...); };

        class ThreadManager : public Singleton<ThreadManager>
        {
            public:
                ThreadManager();

                bool CanCreateNewThread();

                template< class F, class... Args >
                requires ParamFunctor<F, Args...>
                std::optional<std::thread> CreateThread(bool join, F&& f, Args&&... args);

                template< class F, class... Args >
                requires ParamFunctor<F, Args...>
                std::optional<std::thread> CreateThreadOrExec(bool join, F&& f, Args&&... args);

                inline std::size_t GetThreadCount()
                {
                    return thread_count;
                }

                std::thread::id GetCurrentThreadId();

                bool IsMainThread();
            protected:
                std::size_t getThreadLimit();

                template< class F, class... Args >
                requires ParamFunctor<F, Args...>
                static void startNewThread(F&& f, Args&&... args);

                std::thread::id main_thread_id;

                std::size_t thread_count;

                std::mutex thread_count_mutex;
        };
    }
}

#include "thread-manager.hxx"
