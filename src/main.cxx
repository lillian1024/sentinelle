#include <csignal>

#include "core/event/event-manager.hh"
#include "core/orchestrator/orchestrator.hh"
#include "utils/config/config-manager.hh"

/**
 * \brief Handle the stop signals to cleanly stop the software.
 */
void stopSignalHandler(int)
{
    core::orchestrator::Orchestrator::instance().StopOrchestrator();
}

int main()
{
    // Load settings
    utils::config::ConfigManager::instance().reloadSettings();

    // Init managers
    utils::logger::Logger::instance().Init();
    core::event::EventManager::instance().startEventManager();

    core::orchestrator::Orchestrator::instance().LoadConfiguration();
    // Start managers
    core::orchestrator::Orchestrator::instance().StartOrchestrator();

    signal(SIGINT, stopSignalHandler);
    signal(SIGTERM, stopSignalHandler);

    // Wait for the Orchestrator to stop
    core::orchestrator::Orchestrator::instance().AttachToRunning();

    // Stop the application
    core::orchestrator::Orchestrator::instance().CleanOrchestrator();

    core::event::EventManager::instance().stopEventManager();

    // Wait for the EventManager to stop
    core::event::EventManager::instance().AttachToRunning();

    // cv::destroyAllWindows();

    return 0;
}
