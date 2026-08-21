#include "logger.hh"
#include "utils/config/config-manager.hh"
#include "utils/thread/thread-manager.hh"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

namespace utils
{
    namespace logger
    {
        void Logger::LogPlain(std::string message)
        {
            output_stream << message;
        }

        void Logger::LogPlain(std::string message, LogLevel level)
        {
            if (level > logging_level)
            {
                return;
            }

            output_stream << message;
        }

        bool Logger::Log(std::string category_name, std::string message, LogLevel level)
        {
            if (level > logging_level)
            {
                return false;
            }

            time_t timestamp;
            time(&timestamp);

            output_stream << std::put_time(std::localtime(&timestamp), "%d-%m-%Y %X") << ": ";

            output_stream << "[" << level << "]";

            if (!thread::ThreadManager::instance().IsMainThread())
            {
                output_stream << "[Thread-" << thread::ThreadManager::instance().GetCurrentThreadId() << "]";
            }

            output_stream << "[" << category_name << "]: ";

            output_stream << message << std::endl;

            return true;
        }

        void Logger::Init()
        {
            logging_level = config::ConfigManager::instance().getGeneralSettings().getLoggingLevel();

            std::ostringstream sb;

            sb << "New logging session initialized at log_level: ";
            sb << logging_level;

            Log("Logger", sb.str(), LogLevel::INFO);
        }

        Logger::Logger()
            : logging_level(LogLevel::INFO),
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
