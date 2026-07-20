#include "utils/config/config-manager.hh"
#include <iostream>
int main()
{
    std::cout << "Server name: " << utils::config::ConfigManager::instance().getGeneralSettings().getServerName() << std::endl;
    std::cout << "Nb sources: " << utils::config::ConfigManager::instance().getGeneralSettings().getSourceSettings().sources.size() << std::endl;

    return 0;
}
