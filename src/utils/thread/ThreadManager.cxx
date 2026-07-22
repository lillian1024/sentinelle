#include "ThreadManager.hh"
#include "utils/config/config-manager.hh"
#include <cstddef>
#include <thread>

namespace utils
{
    namespace thread
    {
        std::size_t ThreadManager::getThreadLimit()
        {
            return config::ConfigManager::instance().getGeneralSettings().getThreadLimit();
        }

        bool ThreadManager::CanCreateNewThread()
        {
            return thread_count < getThreadLimit();
        }

        template< class F, class... Args >
        bool ThreadManager::CreateThread(bool join, F&& f, Args&&... args)
        {
            thread_count_mutex.lock();

            if (!CanCreateNewThread())
            {
                thread_count_mutex.unlock();

                return false;
            }

            thread_count++;

            std::thread nt(startNewThread, f, args...);

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

            thread_count_mutex.unlock();

            return true;
        }

        template< class F, class... Args >
        void ThreadManager::CreateThreadOrExec(bool join, F&& f, Args&&... args)
        {
            if (!CreateThread(join, f, args...))
            {
                f(args...);
            }
        }

        template< class F, class... Args >
        void ThreadManager::startNewThread(F&& f, Args&&... args)
        {
            f(args...);

            ThreadManager::instance().thread_count_mutex.lock();

            ThreadManager::instance().thread_count--;

            ThreadManager::instance().thread_count_mutex.unlock();
        }
    }
}
