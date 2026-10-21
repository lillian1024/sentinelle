#pragma once

namespace utils
{
    template <typename T>
    class Singleton
    {
    public:
        /**
         * \brief Get the instance of the class.
         */
        static T& instance()
        {
            static T instance_;
            return instance_;
        }

        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
    protected:
        Singleton() = default;
    };
}
