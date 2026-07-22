#include "logger.hh"
#include "utils/config/config-manager.hh"
#include "utils/thread/thread-manager.hh"
#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>

namespace utils
{
    namespace logger
    {
        bool Logger::Log(std::string category_name, std::string message, LogLevel level)
        {
            if (level > logging_level)
            {
                return false;
            }

            time_t timestamp;
            time(&timestamp);

            output_stream << ctime(&timestamp) << ": ";

            output_stream << "[" << level << "]";

            if (!thread::ThreadManager::instance().IsMainThread())
            {
                output_stream << "[Thread" << thread::ThreadManager::instance().GetCurrentThreadId() << "]";
            }

            output_stream << "[" << category_name << "]: ";

            output_stream << message << "\n";

            return true;
        }

        void Logger::Init()
        {
            Log("Logger", "New logging session initialized.", LogLevel::INFO);
        }

        Logger::Logger()
            : logging_level(),
            output_stream(getLogFileBuffer())
        { }

        Logger::~Logger()
        {
            if (file_stream.has_value() && file_stream->is_open())
            {
                file_stream->close();
            }
        }

        std::streambuf* Logger::getLogFileBuffer()
        {
            std::string log_file = config::ConfigManager::instance().getGeneralSettings().getLogFile();

            if (log_file.length() == 0)
            {
                return std::cout.rdbuf();
            }

            std::streambuf* buf;
            file_stream = std::ofstream();

            file_stream->open(log_file);
            buf = file_stream->rdbuf();

            return buf;
        }
    }
}
