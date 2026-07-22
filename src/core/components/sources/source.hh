#pragma once

#include "utils/config/data/sources/source-settings.hh"
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <optional>

namespace core
{
    namespace components
    {
        namespace source
        {
            class Source
            {
            public:
                static Source getSourceFromConfig(utils::config::data::sources::SourceSettings& config);

                Source(cv::VideoCapture video, float active_fps, float passive_fps);
                ~Source();

                virtual std::optional<cv::Mat> getImage();

                float getCurrentFPS();

                std::clock_t getLastFrameClocks();
                bool isInCooldown();
                std::clock_t getCooldownStopClocks();
                float getRemainingCoolSecs();

                void setLastFrameNow();
            protected:
                cv::VideoCapture video_stream;
                float active_fps;
                float passive_fps;

                bool is_active;
                bool is_triggered;

                std::clock_t last_frame;
            };
        }
    }
}
