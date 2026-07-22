#include "core/orchestrator/orchestrator.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <thread>

int main()
{
    utils::logger::Logger::instance().Init();

    std::cout << "Server name: " << utils::config::ConfigManager::instance().getGeneralSettings().getServerName() << std::endl;
    std::cout << "Nb sources: " << utils::config::ConfigManager::instance().getGeneralSettings().getSourceSettings().sources.size() << std::endl;

    core::orchestrator::Orchestrator::instance().LoadConfiguration();
    core::orchestrator::Orchestrator::instance().StartOrchestrator();

    // TODO: to change
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }

    return 0;
}
