#pragma once

#include "utils/singleton/singleton.hh"
#include <fstream>
#include <optional>
#include <ostream>
#include <string>

namespace utils
{
    namespace logger
    {
        class Logger : public Singleton<Logger>
        {
            public:
                enum class LogLevel
                {
                    CRITICAL = 0,
                    ERROR = 1,
                    WARNING = 2,
                    INFO = 3,
                    DEBUG = 4,
                    TRACE = 5,
                };

                Logger();
                ~Logger();

                static std::optional<LogLevel> ParseLogLevel(std::string log_level_str)
                {
                    if (log_level_str == "CRITICAL")
                    {
                        return Logger::LogLevel::CRITICAL;
                    }
                    else if (log_level_str == "ERROR")
                    {
                        return Logger::LogLevel::ERROR;
                    }
                    else if (log_level_str == "WARNING")
                    {
                        return Logger::LogLevel::WARNING;
                    }
                    else if (log_level_str == "INFO")
                    {
                        return Logger::LogLevel::INFO;
                    }
                    else if (log_level_str == "DEBUG")
                    {
                        return Logger::LogLevel::DEBUG;
                    }
                    else if (log_level_str == "TRACE")
                    {
                        return Logger::LogLevel::TRACE;
                    }

                    return std::nullopt;
                }

                void Init();

                bool Log(std::string category_name, std::string message, LogLevel level);

                void LogPlain(std::string message);
                void LogPlain(std::string message, LogLevel level);
            protected:
                std::streambuf* getLogFileBuffer();

                LogLevel logging_level;

                std::ostream output_stream;
                std::optional<std::ofstream> file_stream = std::nullopt;
        };

        inline std::ostream& operator<<(std::ostream& os, const Logger::LogLevel& obj)
        {
            switch (obj)
            {
                case Logger::LogLevel::CRITICAL:
                    os << "CRITICAL";
                    break;
                case Logger::LogLevel::ERROR:
                    os << "ERROR";
                    break;
                case Logger::LogLevel::WARNING:
                    os << "WARNING";
                    break;
                case Logger::LogLevel::INFO:
                    os << "INFO";
                    break;
                case Logger::LogLevel::DEBUG:
                    os << "DEBUG";
                    break;
                case Logger::LogLevel::TRACE:
                    os << "TRACE";
                    break;
            }

            return os;
        }
    }
}
