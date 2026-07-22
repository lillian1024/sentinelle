#include "thread-manager.hh"
#include "utils/config/config-manager.hh"
#include <cstddef>

namespace utils
{
    namespace thread
    {
        ThreadManager::ThreadManager()
            : main_thread_id(GetCurrentThreadId()),
              thread_count(0),
              thread_count_mutex()
        { }

        std::size_t ThreadManager::getThreadLimit()
        {
            return config::ConfigManager::instance().getGeneralSettings().getThreadLimit();
        }

        bool ThreadManager::CanCreateNewThread()
        {
            return thread_count < getThreadLimit();
        }

        std::thread::id ThreadManager::GetCurrentThreadId()
        {
            return std::this_thread::get_id();
        }

        bool ThreadManager::IsMainThread()
        {
            return GetCurrentThreadId() == main_thread_id;
        }
    }
}
