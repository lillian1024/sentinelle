#include "source.hh"
#include "utils/config/data/sources/source-settings.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
#include <opencv2/core/mat.hpp>
#include <optional>
#include <stdexcept>

namespace core
{
    namespace components
    {
        namespace source
        {
            Source Source::getSourceFromConfig(utils::config::data::sources::SourceSettings& config)
            {
                switch (config.getType()) {
                    case utils::config::data::sources::SourceSettings::SourceType::URL:
                    {
                        auto* url_config = dynamic_cast<utils::config::data::sources::UrlSourceSettings*>(&config);

                        if (url_config == nullptr)
                        {
                            throw std::runtime_error("[ConfigManager]: Unable to cast source config to the correct type! Please report this error.");
                        }

                        cv::VideoCapture video_stream(url_config->getUrl());

                        return Source(video_stream, url_config->getActiveFps(), url_config->getPassiveFps());
                    }
                }

                throw std::runtime_error("[ConfigManager]: Unkown source config type! Please report this error.");
            }

            Source::Source(cv::VideoCapture video, float active_fps, float passive_fps)
                : video_stream(video),
                  active_fps(active_fps),
                  passive_fps(passive_fps),
                  is_active(false),
                  is_triggered(false),
                  last_frame(0) { }

            Source::~Source()
            {
                video_stream.release();
            }

            std::optional<cv::Mat> Source::getImage()
            {
                cv::Mat image;

                bool is_success = video_stream.read(image);

                if (!is_success)
                {
                    return std::nullopt;
                }

                return image;
            }

            float Source::getCurrentFPS()
            {
                return is_active ? active_fps : passive_fps;
            }

            std::clock_t Source::getLastFrameClocks()
            {
                return last_frame;
            }

            bool Source::isInCooldown()
            {
                std::clock_t c_stop = getCooldownStopClocks();

                std::clock_t now = clock();

                return now >= c_stop;
            }

            std::clock_t Source::getCooldownStopClocks()
            {
                float c_fps = getCurrentFPS();

                if (c_fps == 0.0)
                {
                    return last_frame;
                }

                float f_interval_sec = 1.0/c_fps;

                std::clock_t f_interval_clocks = f_interval_sec * CLOCKS_PER_SEC;

                return last_frame + f_interval_clocks;
            }

            float Source::getRemainingCoolSecs()
            {
                std::clock_t c_stop = getCooldownStopClocks();

                std::clock_t now = clock();

                return float(c_stop - now) / CLOCKS_PER_SEC;
            }

            void Source::setLastFrameNow()
            {
                last_frame = clock();
            }
        }
    }
}
