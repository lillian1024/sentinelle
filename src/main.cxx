#include "core/event/event-manager.hh"
#include "core/orchestrator/orchestrator.hh"
#include "utils/config/config-manager.hh"
#include <chrono>
#include <thread>

int main()
{
    utils::config::ConfigManager::instance().reloadSettings();

    utils::logger::Logger::instance().Init();
    core::event::EventManager::instance().startEventManager();

    core::orchestrator::Orchestrator::instance().LoadConfiguration();
    core::orchestrator::Orchestrator::instance().StartOrchestrator();

    // TODO: to change
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }

    return 0;
}
