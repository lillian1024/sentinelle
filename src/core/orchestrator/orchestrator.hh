#pragma once

#include "core/components/sources/source.hh"
#include "utils/singleton/singleton.hh"
#include <memory>
#include <vector>

namespace core
{
    namespace orchestrator
    {
        class Orchestrator : public utils::Singleton<Orchestrator>
        {
            public:
                void LoadConfiguration();

                void StartOrchestrator();

                void OrchestrateSource(components::source::Source& source);

                void UpdateGroupActivation(components::source::Source& source);

                static const std::string ORCHESTRATOR_CATEGORY_NAME;
            protected:
                static void startSourceOrchestration(components::source::Source& source);

                void logConfig();
                void logSourceConfig();
                void logAnalizerConfig();

                std::vector<std::unique_ptr<components::source::Source>> sources;
        };
    }
}
