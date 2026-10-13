#include "core/event/event-manager.hh"
#include "core/orchestrator/orchestrator.hh"
#include "utils/config/config-manager.hh"
#include <csignal>

void stopSignalHandler(int)
{
    core::orchestrator::Orchestrator::instance().StopOrchestrator();
}

int main()
{
    utils::config::ConfigManager::instance().reloadSettings();

    utils::logger::Logger::instance().Init();
    core::event::EventManager::instance().startEventManager();

    core::orchestrator::Orchestrator::instance().LoadConfiguration();
    core::orchestrator::Orchestrator::instance().StartOrchestrator();

    signal(SIGINT, stopSignalHandler);
    signal(SIGTERM, stopSignalHandler);

    core::orchestrator::Orchestrator::instance().AttachToRunning();

    core::orchestrator::Orchestrator::instance().CleanOrchestrator();

    core::event::EventManager::instance().stopEventManager();

    core::event::EventManager::instance().AttachToRunning();

    //cv::destroyAllWindows();

    return 0;
}
