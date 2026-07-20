#pragma once

namespace utils
{
    template <typename T>
    class Singleton
    {
    public:
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
