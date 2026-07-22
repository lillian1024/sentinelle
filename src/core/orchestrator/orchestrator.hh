#pragma once

#include "core/components/sources/source.hh"
#include "utils/singleton/singleton.hh"
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

                static const std::string ORCHESTRATOR_MSG_PREFIX;
            protected:
                static void startSourceOrchestration(components::source::Source& source);

                std::vector<components::source::Source> sources;
        };
    }
}
