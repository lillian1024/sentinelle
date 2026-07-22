#pragma once

#include "utils/singleton/singleton.hh"
#include <cstddef>
#include <mutex>

namespace utils
{
    namespace thread
    {
        class ThreadManager : public Singleton<ThreadManager>
        {
            public:
                bool CanCreateNewThread();

                template< class F, class... Args >
                bool CreateThread(bool join, F&& f, Args&&... args);

                template< class F, class... Args >
                void CreateThreadOrExec(bool join, F&& f, Args&&... args);

                inline std::size_t GetThreadCount()
                {
                    return thread_count;
                }
            protected:
                std::size_t getThreadLimit();

                template< class F, class... Args >
                static void startNewThread(F&& f, Args&&... args);

                std::size_t thread_count = 0;

                std::mutex thread_count_mutex;
        };
    }
}
