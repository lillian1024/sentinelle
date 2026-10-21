#pragma once

#include "core/components/sources/source.hh"
#include "core/event/event-handler.hh"
#include "utils/singleton/singleton.hh"
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <thread>

namespace core
{
    namespace orchestrator
    {
        /**
         * \brief This class is a Singleton responsible for managing the sources and middle-ends of the program.
         * \details This class is started after the configuration is loaded and the event manager is started. It will start and monitor all the sources from the configuration, allowing source error reporting and source auto restart. I will also start and stop the middle-ends without monitoring their statuses (will not auto restart).
         */
        class Orchestrator : public utils::Singleton<Orchestrator>, public event::EventHandler
        {
            public:
                /**
                 * \brief Initialize the Orchestrator with the values of the configuration.
                 */
                void LoadConfiguration();

                /**
                 * \brief Start the sources and middle-ends loaded from LoadConfiguration().
                 */
                void StartOrchestrator();
                /**
                 * \brief Request the sources to stop. This will not stop the middle-ends. Mark the Orchestrator as stopping.
                 */
                void StopOrchestrator();
                /**
                 * \brief Stops the sources and middle-ends and blocks until everything has stopped. Mark the Orchestrator as stopping as soon as the function is called.
                 */
                void CleanOrchestrator();

                /**
                 * \brief Get a loaded source using its name.
                 * \param name The name of the source to get
                 * \return The pointer to the requested source if found or nullptr if not found.
                 */
                components::source::Source* GetSourceByName(std::string name);

                /**
                 * \brief Get the running status of the Orchestrator.
                 * \details The Orchestrator is considered as running when StartOrchestrator() is called until the orchestrator is marked as stopping (StopOrchestrator(), CleanOrchestrator()).
                 * \return If the Orchestrator is running
                 */
                bool IsRunning();

                /**
                 * \brief Blocks until the Orchestrator (and all sources and middle-ends) has stopped.
                 */
                void AttachToRunning();

                /**
                 * \brief Orchestrator name for logging.
                 */
                static const std::string ORCHESTRATOR_CATEGORY_NAME;

                /**
                 * \brief Handle StopSourceEvent for source's auto restart.
                 */
                virtual void handleEvent(event::Event& event);
            protected:
                /**
                 * \brief Handles a single source start and stop process.
                 * \details This function handles starting the source and source's handle loop as well as error and stop handling.
                 * \param source The source to handle
                 */
                void OrchestrateSource(components::source::Source& source);

                /**
                 * \brief Run the source processing loop.
                 * \details Block until the source stops.
                 * \param source The source to process
                 */
                void RunSourceLoop(components::source::Source& source);

                /**
                 * \brief Create the source thread that execute startSourceOrchestration().
                 */
                void createSourceThread(components::source::Source& source);

                /**
                 * \brief Helper function to call OrchestrateSource() from a static context.
                 */
                static void startSourceOrchestration(components::source::Source& source);

                /**
                 * \brief Return if at least one of the sources is running.
                 */
                bool hasSourceRunning();

                /**
                 * \prief Logs the entire configuration.
                 */
                void logConfig();
                /**
                 * \prief Logs the source configuration.
                 */
                void logSourceConfig();
                /**
                 * \prief Logs the analizer configuration.
                 */
                void logAnalizerConfig();

                /**
                 * \brief If Orchestrator is stopping.
                 * \details Will block the auto restart of sources.
                 */
                std::atomic_bool stop;

                /**
                 * \brief List of all sources managed by the Orchestrator.
                 */
                std::vector<std::unique_ptr<components::source::Source>> sources;

                /**
                 * \brief List of the threads managing the sources.
                 */
                std::vector<std::thread> source_threads;
        };
    }
}
