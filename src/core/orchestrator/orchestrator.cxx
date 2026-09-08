#include "orchestrator.hh"
#include "core/components/chains/chain.hh"
#include "core/components/sources/source.hh"
#include "core/event/events/modules/trigger/triggered-event.hh"
#include "core/event/events/modules/trigger/untriggered-event.hh"
#include "core/event/event-manager.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
#include "utils/logger/logger.hh"
#include "utils/thread/thread-manager.hh"
#include <chrono>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/ovx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/utils/logger.defines.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <thread>

#define SOURCE_DISABLE_CHECK_INTERVAL_MILLIS 20

namespace core
{
    namespace orchestrator
    {
        const std::string Orchestrator::ORCHESTRATOR_CATEGORY_NAME = "Orchestrator";

        void Orchestrator::LoadConfiguration()
        {
            utils::logger::Logger::instance().LogPlain(cv::getBuildInformation(), utils::logger::Logger::LogLevel::DEBUG);

            logConfig();

            auto& sources_config = utils::config::ConfigManager::instance().getGeneralSettings().getSourceSettings();

            for (size_t i = 0; i < sources_config.sources.size(); i++)
            {
                auto* source_config = sources_config.sources[i].get();

                std::unique_ptr<components::source::Source> source = components::source::Source::getSourceFromConfig(*source_config);

                sources.push_back(std::move(source));
            }

            auto middle_ends = utils::config::ConfigManager::instance().getGeneralSettings().getMiddleEndSettings().getMiddleEnds();

            for (auto mid : middle_ends)
            {
                mid.get().Init();
            }
        }

        void Orchestrator::StartOrchestrator()
        {
            for (size_t i = 0; i < sources.size(); i++)
            {
                auto& source = *sources[i].get();

                if (!utils::thread::ThreadManager::instance().CanCreateNewThread())
                {
                    utils::logger::Logger::instance().Log(ORCHESTRATOR_CATEGORY_NAME,
                        "Thread limit reached! Unable to start sources threads! sequential mode will be used.\n\tWarning: seqential sources mode will severly degrade performance!",
                        utils::logger::Logger::LogLevel::WARNING);

                    throw std::runtime_error("Sequential source mode is not implemented yet!");
                }
                else
                {
                    std::optional<std::thread> t = utils::thread::ThreadManager::instance().CreateThread(false, startSourceOrchestration, std::ref(source));

                    if (!t.has_value())
                    {
                        throw std::runtime_error("Unable to start the source thread! Please report this error.");
                    }
                }
            }

            utils::logger::Logger::instance().Log(ORCHESTRATOR_CATEGORY_NAME,
                "All sources initialized successfully.",
                utils::logger::Logger::LogLevel::INFO);

            auto middle_ends = utils::config::ConfigManager::instance().getGeneralSettings().getMiddleEndSettings().getMiddleEnds();

            for (auto mid : middle_ends)
            {
                mid.get().Start();
            }

            utils::logger::Logger::instance().Log(ORCHESTRATOR_CATEGORY_NAME,
                "All middle ends started successfully.",
                utils::logger::Logger::LogLevel::INFO);
        }

        void Orchestrator::startSourceOrchestration(components::source::Source& source)
        {
            Orchestrator::instance().OrchestrateSource(source);
        }

        void Orchestrator::OrchestrateSource(components::source::Source& source)
        {
            source.startResource();

            if (!source.isSourceOpen())
            {
                std::ostringstream sb;

                sb << "Unable to start the source (";
                sb << source.getName();
                sb << ")!";

                utils::logger::Logger::instance().Log("Source", sb.str(), utils::logger::Logger::LogLevel::ERROR);

                return;
            }
            else
            {
                std::ostringstream sb;

                sb << "Source  ";
                sb << source.getName();
                sb << " started successfully.";

                utils::logger::Logger::instance().Log("Source", sb.str(), utils::logger::Logger::LogLevel::INFO);
            }

            while (!source.isStopping())
            {
                if (!source.isEnabled())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(SOURCE_DISABLE_CHECK_INTERVAL_MILLIS));

                    continue;
                }

                if (source.isInCooldown())
                {
                    std::this_thread::sleep_until(source.getCooldownStopTime());
                }

                std::optional<cv::Mat> image = source.getImage();
                source.setLastFrameNow();

                if (!image.has_value())
                {
                    continue;
                }

                bool trigger;

                for (size_t i = 0; i < source.getSourceConfig().getProcess().size(); i++)
                {
                    auto chain = source.getSourceConfig().getProcess()[i];

                    if (chain == nullptr)
                    {
                        continue;
                    }

                    chain->process(image.value(), source, trigger);
                }

                if (trigger != source.isTriggered())
                {
                    source.setTriggered(trigger);

                    if (trigger)
                    {
                        std::ostringstream sb;

                        sb << "Source ";
                        sb << source.getName();
                        sb << " has been triggered!";

                        utils::logger::Logger::instance().Log(ORCHESTRATOR_CATEGORY_NAME, sb.str(), utils::logger::Logger::LogLevel::INFO);

                        event::EventManager::instance().registerEvent(std::make_unique<event::modules::TriggeredEvent>(source));
                    }
                    else
                    {
                        std::ostringstream sb;

                        sb << "Source ";
                        sb << source.getName();
                        sb << " has been untriggered!";

                        utils::logger::Logger::instance().Log(ORCHESTRATOR_CATEGORY_NAME, sb.str(), utils::logger::Logger::LogLevel::TRACE);

                        event::EventManager::instance().registerEvent(std::make_unique<event::modules::UnTriggeredEvent>(source));
                    }

                    UpdateGroupActivation(source);
                }
            }

            source.releaseSource();
            cv::destroyAllWindows();

            utils::logger::Logger::instance().Log("Source", "Source closed successfully.", utils::logger::Logger::LogLevel::INFO);
        }

        void Orchestrator::UpdateGroupActivation(components::source::Source& source)
        {
            // TODO: Manage groups
            source.setActive(source.isTriggered());
        }

        components::source::Source* Orchestrator::GetSourceByName(std::string name)
        {
            for (size_t i = 0; i < sources.size(); i++)
            {
                auto source = sources[i].get();

                if (source == nullptr)
                {
                    continue;
                }

                if (source->getName() == name)
                {
                    return source;
                }
            }

            return nullptr;
        }

        void Orchestrator::logConfig()
        {
            std::ostringstream sb;

            sb << "Server name: " << utils::config::ConfigManager::instance().getGeneralSettings().getServerName() << std::endl;
            sb << "Nb sources: " << utils::config::ConfigManager::instance().getGeneralSettings().getSourceSettings().sources.size() << std::endl;
            sb << "Logger level: " << utils::config::ConfigManager::instance().getGeneralSettings().getLoggingLevel() << std::endl;

            utils::logger::Logger::instance().LogPlain(sb.str(), utils::logger::Logger::LogLevel::DEBUG);
            logSourceConfig();
            logAnalizerConfig();
        }

        void Orchestrator::logSourceConfig()
        {
            for (auto& source_setting: utils::config::ConfigManager::instance().getGeneralSettings().getSourceSettings().sources)
            {
                std::ostringstream sb;

                sb << "Source ";
                sb << source_setting->getName();
                sb << ":\n";

                utils::logger::Logger::instance().LogPlain(sb.str(), utils::logger::Logger::LogLevel::DEBUG);

                auto* url_source = dynamic_cast<utils::config::data::sources::UrlSourceSettings*>(source_setting.get());

                if (url_source == nullptr)
                {
                    utils::logger::Logger::instance().LogPlain("\tUnkown source type\n", utils::logger::Logger::LogLevel::DEBUG);
                    continue;
                }

                utils::logger::Logger::instance().LogPlain(source_setting->dumpSettings(), utils::logger::Logger::LogLevel::DEBUG);
            }
        }

        void Orchestrator::logAnalizerConfig()
        {
            utils::logger::Logger::instance().LogPlain(utils::config::ConfigManager::instance().getGeneralSettings().getAnalizersSettings().dumpInfo(), utils::logger::Logger::LogLevel::DEBUG);
            utils::logger::Logger::instance().LogPlain(utils::config::ConfigManager::instance().getGeneralSettings().getChainsSettings().dumpInfo(), utils::logger::Logger::LogLevel::DEBUG);
        }
    }
}
