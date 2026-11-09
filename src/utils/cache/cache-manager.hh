#include "utils/singleton/singleton.hh"
#include <string>

namespace utils
{
    namespace cache
    {
        /**
         * \brief This class alows the system to store and retrieve important files that would be too heavy to download at every start.
         */
        class CacheManager : public Singleton<CacheManager>
        {
            public:
                /**
                 * \brief Return the cache path of the provided file in the provided category.
                 */
                std::string getPath(std::string category, std::string file_name);

                /**
                 * \brief Check if the provided file exist and can be read in the cache directory.
                 */
                bool hasFile(std::string category, std::string file_name);
            private:
                /**
                 * \brief Create the cache directory of the application if it does not exist. Does nothing if the directory already exist.
                 * \details This function will throw if the cache directory cannot be created.
                 */
                void checkOrCreateCachePath();

                /**
                 * \brief Return the cache root directory.
                 */
                std::string getCachePath();

                /**
                 * \brief Helper function to check if a file exist.
                 */
                bool fileExist(std::string path);
                /**
                 * \brief Helper function to check that a file can be read.
                 */
                bool canRead(std::string path);
                /**
                 * \brief Helper function to check if a path is a directory.
                 */
                bool isDir(std::string path);
        };
    }
}
