#include "source.hh"
#include "utils/config/data/sources/source-settings.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
#include "utils/opencv/video-stream/live-video-stream.hh"
#include "utils/opencv/video-stream/sequence-video-stream.hh"
#include <chrono>
#include <cstddef>
#include <ctime>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <optional>
#include <stdexcept>

#define SECS_TO_MILLIS 1000

namespace core
{
    namespace components
    {
        namespace source
        {
            std::unique_ptr<Source> Source::getSourceFromConfig(const utils::config::data::sources::SourceSettings& config)
            {
                switch (config.getType()) {
                    case utils::config::data::sources::SourceSettings::SourceType::URL:
                    {
                        auto* url_config = dynamic_cast<const utils::config::data::sources::UrlSourceSettings*>(&config);

                        if (url_config == nullptr)
                        {
                            throw std::runtime_error("[ConfigManager]: Unable to cast source config to the correct type! Please report this error.");
                        }

                        cv::VideoCapture video_stream(url_config->getUrl());

                        return std::make_unique<Source>(*url_config);
                    }
                }

                throw std::runtime_error("[ConfigManager]: Unkown source config type! Please report this error.");
            }

            Source::Source(const utils::config::data::sources::SourceSettings& config)
                : active_fps(config.getActiveFps()),
                  passive_fps(config.getPassiveFps()),
                  show_debug_view(config.getShowDebugView()),
                  is_active(false),
                  is_triggered(false),
                  last_frame(std::chrono::steady_clock::now())
            {
                source_config = config.clone();
            }

            Source::~Source()
            {
                releaseSource();
            }

            std::optional<cv::Mat> Source::getImage()
            {
                if (!video_stream->isOpened())
                {
                    return std::nullopt;
                }

                std::optional<cv::Mat> image = video_stream->getImage();

                return image;
            }

            void Source::startResource()
            {
                if (source_config->getIsLive())
                {
                    video_stream = std::make_unique<LiveVideoStream>(source_config->getVideoCapture());
                }
                else
                {
                    video_stream = std::make_unique<SequenceVideoStream>(source_config->getVideoCapture());
                }
            }

            void Source::releaseSource()
            {
                if (video_stream->isOpened())
                {
                    video_stream->release();
                }
            }

            float Source::getCurrentFPS()
            {
                return is_active ? active_fps : passive_fps;
            }

            time_type Source::getLastFrameTime()
            {
                return last_frame;
            }

            bool Source::isInCooldown()
            {
                time_type c_stop = getCooldownStopTime();

                time_type now = std::chrono::steady_clock::now();

                return now < c_stop;
            }

            time_type Source::getCooldownStopTime()
            {
                float c_fps = getCurrentFPS();

                if (c_fps == 0.0)
                {
                    return last_frame;
                }

                float f_interval_sec = 1.0/c_fps;

                return last_frame + std::chrono::milliseconds(secsToMillisRound(f_interval_sec));
            }

            float Source::getRemainingCoolSecs()
            {
                typedef std::chrono::duration<float> fsec;

                time_type c_stop = getCooldownStopTime();

                time_type now = std::chrono::steady_clock::now();

                fsec diffSecs = c_stop - now;

                return diffSecs.count();
            }

            void Source::setLastFrameNow()
            {
                last_frame = std::chrono::steady_clock::now();
            }

            size_t Source::secsToMillisRound(float seconds)
            {
                return size_t(seconds * SECS_TO_MILLIS);
            }
        }
    }
}
