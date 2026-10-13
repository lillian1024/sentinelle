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
        class Orchestrator : public utils::Singleton<Orchestrator>, public event::EventHandler
        {
            public:
                void LoadConfiguration();

                void StartOrchestrator();
                void StopOrchestrator();

                void OrchestrateSource(components::source::Source& source);

                components::source::Source* GetSourceByName(std::string name);

                bool IsRunning();

                void AttachToRunning();

                static const std::string ORCHESTRATOR_CATEGORY_NAME;

                virtual void handleEvent(event::Event& event);
            protected:
                void RunSourceLoop(components::source::Source& source);

                void startSource(components::source::Source& source);

                static void startSourceOrchestration(components::source::Source& source);

                bool hasSourceRunning();

                void logConfig();
                void logSourceConfig();
                void logAnalizerConfig();

                std::atomic_bool stop;

                std::vector<std::unique_ptr<components::source::Source>> sources;

                std::vector<std::thread> source_threads;
        };
    }
}
