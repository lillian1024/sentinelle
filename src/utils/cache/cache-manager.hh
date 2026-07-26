#include "utils/singleton/singleton.hh"
#include <optional>
#include <string>

namespace utils
{
    namespace cache
    {
        class CacheManager : public Singleton<CacheManager>
        {
            public:
                std::string getPath(std::string category, std::string file_name);

                bool hasFile(std::string category, std::string file_name);
            private:
                void checkOrCreateCachePath();

                std::string getCachePath();

                bool fileExist(std::string path);
                bool canRead(std::string path);
                bool isDir(std::string path);
        };
    }
}
