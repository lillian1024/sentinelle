#pragma once

#include <optional>
#include <thread>

namespace utils
{
    namespace thread
    {
        template< class F, class... Args >
        requires utils::thread::ParamFunctor<F, Args...>
        std::optional<std::thread> ThreadManager::CreateThread(bool join, F&& f, Args&&... args)
        {
            thread_count_mutex.lock();

            if (!CanCreateNewThread())
            {
                thread_count_mutex.unlock();

                return std::nullopt;
            }

            thread_count++;

            thread_count_mutex.unlock();

            std::thread nt(startNewThread<std::decay_t<F>, std::decay_t<Args>...>, f, args...);

            if (nt.joinable())
            {
                if (join)
                {
                    nt.join();
                }
                else
                {
                    nt.detach();
                }
            }

            return nt;
        }

        template< class F, class... Args >
        requires utils::thread::ParamFunctor<F, Args...>
        std::optional<std::thread> ThreadManager::CreateThreadOrExec(bool join, F&& f, Args&&... args)
        {
            std::optional<std::thread> t = CreateThread(join, f, args...);

            if (!t.has_value())
            {
                f(args...);

                return std::nullopt;
            }

            return t;
        }

        template< class F, class... Args >
        requires utils::thread::ParamFunctor<F, Args...>
        void ThreadManager::startNewThread(F&& f, Args&&... args)
        {
            f(args...);

            ThreadManager::instance().thread_count_mutex.lock();

            ThreadManager::instance().thread_count--;

            ThreadManager::instance().thread_count_mutex.unlock();
        }
    }
}
