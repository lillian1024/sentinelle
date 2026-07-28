#include "orchestrator.hh"
#include "core/components/analizers/analizer.hh"
#include "core/components/sources/source.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
#include "utils/io_data/types/io_data_mat.hh"
#include "utils/io_data/types/io_data_string.hh"
#include "utils/logger/logger.hh"
#include "utils/thread/thread-manager.hh"
#include <cstddef>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core/utils/logger.defines.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <thread>

namespace core
{
    namespace orchestrator
    {
        const std::string Orchestrator::ORCHESTRATOR_CATEGORY_NAME = "Orchestrator";

        void Orchestrator::LoadConfiguration()
        {
            logConfig();

            auto& sources_config = utils::config::ConfigManager::instance().getGeneralSettings().getSourceSettings();

            for (size_t i = 0; i < sources_config.sources.size(); i++)
            {
                auto* source_config = sources_config.sources[i].get();

                std::unique_ptr<components::source::Source> source = components::source::Source::getSourceFromConfig(*source_config);

                sources.push_back(std::move(source));
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
        }

        void Orchestrator::startSourceOrchestration(components::source::Source& source)
        {
            Orchestrator::instance().OrchestrateSource(source);
        }

        void Orchestrator::OrchestrateSource(components::source::Source& source)
        {
            bool stop = false;

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

            while (!stop)
            {
                if (source.isInCooldown())
                {
                    // TODO: correctly retrieve the last frame from the stream
                    std::this_thread::sleep_until(source.getCooldownStopTime());
                }

                std::optional<cv::Mat> image = source.getImage();
                source.setLastFrameNow();

                if (!image.has_value())
                {
                    continue;
                }

                // TODO: change this to configured analizer sequence!
                // Start of DEBUG

                //components::analizer::dnn::AnalizerDNNGenericIdent ana("test");
                components::analizer::Analizer* ana = utils::config::ConfigManager::instance().getGeneralSettings().getAnalizersSettings().getAnalizer("general_ident");

                std::map<std::string, utils::io_data::IOData*> inputs;

                std::unique_ptr<utils::io_data::IOData> image_input = std::make_unique<utils::io_data::IODataMat>(*image);

                inputs.insert({"image", image_input.get()});

                auto outputs = ana->process(inputs, source);

                if (outputs.empty())
                {
                    utils::logger::Logger::instance().Log("Analizer", "Unable to get outputs: error while processing!", utils::logger::Logger::LogLevel::ERROR);

                    continue;
                }

                auto* debug_data_ptr = outputs["debug-image"].get();

                auto* debug_data = dynamic_cast<utils::io_data::IODataMat*>(debug_data_ptr);

                if (debug_data == nullptr)
                {
                    utils::logger::Logger::instance().Log("Analizer", "Unable to get outputs: missmatched types!", utils::logger::Logger::LogLevel::ERROR);

                    continue;
                }

                auto debug_image = debug_data->getData();

                if (source.getShowDebugView())
                {
                    components::analizer::Analizer* display_ana = utils::config::ConfigManager::instance().getGeneralSettings().getAnalizersSettings().getAnalizer("show_debug");

                    std::map<std::string, utils::io_data::IOData*> show_inputs;

                    std::unique_ptr<utils::io_data::IOData> show_image_input = std::move(outputs["debug-image"]);
                    std::unique_ptr<utils::io_data::IOData> show_name_input = std::make_unique<utils::io_data::IODataString>("test");

                    show_inputs.insert({"image", show_image_input.get()});
                    show_inputs.insert({"name", show_name_input.get()});

                    display_ana->process(show_inputs, source);
                }

                // End of DEBUG
            }

            source.releaseSource();
            if (source.getShowDebugView())
            {
                cv::destroyAllWindows();
            }

            utils::logger::Logger::instance().Log("Source", "Source closed successfully.", utils::logger::Logger::LogLevel::INFO);
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
        }
    }
}
