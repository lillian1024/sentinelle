#include "orchestrator.hh"
#include "core/components/sources/source.hh"
#include "utils/config/config-manager.hh"
#include "utils/thread/thread-manager.hh"
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <thread>

namespace core
{
    namespace orchestrator
    {
        const std::string Orchestrator::ORCHESTRATOR_MSG_PREFIX = "[Orchestrator]: ";

        void Orchestrator::LoadConfiguration()
        {
            auto& sources_config = utils::config::ConfigManager::instance().getGeneralSettings().getSourceSettings();

            for (size_t i = 0; i < sources_config.sources.size(); i++)
            {
                auto* source_config = sources_config.sources[i].get();

                components::source::Source source = components::source::Source::getSourceFromConfig(*source_config);
            }
        }

        void Orchestrator::StartOrchestrator()
        {
            for (auto& source: sources)
            {
                if (!utils::thread::ThreadManager::instance().CanCreateNewThread())
                {
                    std::cout << ORCHESTRATOR_MSG_PREFIX << "Thread limit reached! Unable to start sources threads! sequential mode will be used.\n\tWarning: seqential sources mode will severly degrade performance!" << '\n';

                    throw std::runtime_error("Sequential source mode is not implemented yet!");
                }
                else
                {
                    std::optional<std::thread> t = utils::thread::ThreadManager::instance().CreateThread(false, startSourceOrchestration, source);

                    if (!t.has_value())
                    {
                        throw std::runtime_error("Unable to start the source thread! Please report this error.");
                    }
                }
            }
        }

        void Orchestrator::startSourceOrchestration(components::source::Source& source)
        {
            Orchestrator::instance().OrchestrateSource(source);
        }

        void Orchestrator::OrchestrateSource(components::source::Source& source)
        {

        }
    }
}
