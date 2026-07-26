#include "cache-manager.hh"
#include "utils/config/config-manager.hh"
#include "utils/logger/logger.hh"
#include <filesystem>
#include <optional>
#include <string>

#include <unistd.h>

#define CACHE_CATEGORY_NAME "CacheManager"

namespace utils
{
    namespace cache
    {
        bool CacheManager::fileExist(std::string path)
        {
            return std::filesystem::is_regular_file(path);
        }

        bool CacheManager::canRead(std::string path)
        {
            return (access(path.c_str(), R_OK) != -1);
        }

        bool CacheManager::isDir(std::string path)
        {
            return std::filesystem::is_directory(path);
        }

        void CacheManager::checkOrCreateCachePath()
        {
            std::string cachePath = getCachePath();

            if (!isDir(cachePath))
            {
                if (fileExist(cachePath))
                {
                    logger::Logger::instance().Log(CACHE_CATEGORY_NAME, "Unable to initialize cahce: cache path is not a directory!", logger::Logger::LogLevel::ERROR);
                }

                std::filesystem::create_directories(cachePath);
            }
        }

        std::string CacheManager::getCachePath()
        {
            return config::ConfigManager::instance().getGeneralSettings().getCachePath();
        }

        std::string CacheManager::getPath(std::string category, std::string file_name)
        {
            checkOrCreateCachePath();

            std::string path = config::ConfigManager::instance().getGeneralSettings().getCachePath() + "/" + category + "/" + file_name;

            return path;
        }

        bool CacheManager::hasFile(std::string category, std::string file_name)
        {
            checkOrCreateCachePath();

            std::string path = config::ConfigManager::instance().getGeneralSettings().getCachePath() + "/" + category + "/" + file_name;

            if (access(path.c_str(), R_OK) == -1)
            {
                return false;
            }

            return true;
        }
    }
}
