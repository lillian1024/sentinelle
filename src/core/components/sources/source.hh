#pragma once

#include "utils/config/data/sources/source-settings.hh"
#include "utils/opencv/video-stream/video-stream.hh"
#include <atomic>
#include <chrono>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <optional>
#include <ratio>
#include <string>

namespace core
{
    namespace components
    {
        namespace source
        {
            using namespace utils::opencv::video_stream;

            typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> time_type;

            class Source
            {
            public:
                static std::unique_ptr<Source> getSourceFromConfig(const utils::config::data::sources::SourceSettings& config);

                Source(const utils::config::data::sources::SourceSettings& config);
                virtual ~Source();

                virtual std::optional<cv::Mat> getImage();

                virtual void startResource();
                virtual void releaseSource();

                float getCurrentFPS() const;
                float getFPS(bool is_active) const;

                inline std::string getName() const { return name; }
                inline bool isSourceOpen() const { return video_stream->isOpened(); }

                time_type getLastFrameTime() const;
                bool isInCooldown() const;
                time_type getCooldownStopTime() const;
                float getRemainingCoolSecs() const;

                bool isEnabled() const;
                bool isActive() const;
                bool isTriggered() const;

                void setEnabled(bool status);

                void setActive(bool value);
                void setTriggered(bool value);

                bool isStopping() const;

                void Stop();
                void Resume();

                void setLastFrameNow();

                inline const utils::config::data::sources::SourceSettings& getSourceConfig() const { return *source_config.get(); }
            protected:
                static size_t secsToMillisRound(float seconds);
                void UpdateGroupActivation(components::source::Source& source);

                std::string name;

                std::unique_ptr<VideoStream> video_stream;
                float active_fps;
                float passive_fps;

                std::atomic<bool> enabled;

                std::atomic<bool> is_active;
                std::atomic<bool> is_triggered;

                std::atomic<bool> stop;

                time_type last_frame;

                std::unique_ptr<utils::config::data::sources::SourceSettings> source_config;
            };
        }
    }
}
