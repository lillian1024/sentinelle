#include "orchestrator.hh"
#include "core/components/sources/source.hh"
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
            logSourceConfig();

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
                "All sources started successfully.",
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
                utils::logger::Logger::instance().Log("Source", "Unable to start the source!", utils::logger::Logger::LogLevel::ERROR);

                return;
            }
            else
            {
                utils::logger::Logger::instance().Log("Source", "Source started successfully.", utils::logger::Logger::LogLevel::INFO);
            }

            while (!stop)
            {
                if (source.isInCooldown())
                {
                    // TODO: correctly retrieve the last frame from the stream
                    std::this_thread::sleep_until(source.getCooldownStopTime());
                }

                auto prev_time = source.getLastFrameTime();

                std::optional<cv::Mat> image = source.getImage();
                source.setLastFrameNow();

                if (!image.has_value())
                {
                    continue;
                }

                auto current_time = source.getLastFrameTime();

                float time_interval_sec = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - prev_time).count();

                float prev_fps;

                if (time_interval_sec == 0.0)
                {
                    prev_fps = 0;
                }
                else
                {
                    prev_fps = 1 / (time_interval_sec);
                }

                if (source.getShowDebugView())
                {
                    std::ostringstream box_string_builder;

                    box_string_builder << "FPS: ";
                    box_string_builder << prev_fps;

                    cv::putText(*image, box_string_builder.str(), cv::Point(5, 75), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,255), 1);

                    cv::imshow("image", *image);

                    int k = cv::waitKey(10);
                    if (k == 113){
                        stop = true;
                    }
                }
            }

            source.releaseSource();
            if (source.getShowDebugView())
            {
                cv::destroyAllWindows();
            }

            utils::logger::Logger::instance().Log("Source", "Source closed successfully.", utils::logger::Logger::LogLevel::INFO);
        }

        void Orchestrator::logSourceConfig()
        {
            size_t i = 0;
            for (auto& source_setting: utils::config::ConfigManager::instance().getGeneralSettings().getSourceSettings().sources)
            {
                std::ostringstream sb;

                sb << "Source ";
                sb << i;
                sb << ":\n";

                utils::logger::Logger::instance().LogPlain(sb.str(), utils::logger::Logger::LogLevel::DEBUG);

                auto* url_source = dynamic_cast<utils::config::data::sources::UrlSourceSettings*>(source_setting.get());

                if (url_source == nullptr)
                {
                    utils::logger::Logger::instance().LogPlain("\tUnkown source type\n", utils::logger::Logger::LogLevel::DEBUG);
                    continue;
                }

                utils::logger::Logger::instance().LogPlain(source_setting->dumpSettings(), utils::logger::Logger::LogLevel::DEBUG);

                i++;
            }
        }
    }
}
