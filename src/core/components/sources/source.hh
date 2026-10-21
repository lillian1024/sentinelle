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

            /**
             * \brief A class representing a video source.
             * \details This class handles abstracts the proccess of retrieving the image from a video source (stream, file...) and allows high level manipulation.
             */
            class Source
            {
            public:
                static std::unique_ptr<Source> getSourceFromConfig(const utils::config::data::sources::SourceSettings& config);

                /**
                 * \brief Loads a source from a configuration.
                 * \details The source will not be started by this function.
                 */
                Source(const utils::config::data::sources::SourceSettings& config);
                virtual ~Source();

                /**
                 * \brief Retrieves the most recent image availiable from this source.
                 */
                virtual std::optional<cv::Mat> getImage();

                /**
                 * \brief Start the resource.
                 * \details Allocate the resources to access the video data.
                 */
                virtual void startResource();
                /**
                 * \brief Stop the resource.
                 * \details Release the video resource.
                 */
                virtual void releaseSource();

                /**
                 * \brief Return the current fps value depending on isActive() value. This function is thread-safe.
                 * \details Switch between active_fps and passive_fps depending on value of isActive().
                 * \return The current fps
                 */
                float getCurrentFPS() const;
                /**
                 * \brief Return the current fps value depending on provided value. This function is thread-safe.
                 * \details Switch between active_fps and passive_fps depending on provided value.
                 * \return The requested fps value
                 */
                float getFPS(bool is_active) const;

                /**
                 * \brief Return the name of the source. This function is thread-safe.
                 */
                inline std::string getName() const { return name; }
                /**
                 * \brief Return if the video source is openned.
                 */
                inline bool isSourceOpen() const { return video_stream->isOpened(); }

                /**
                 * \brief Get max fail retries config value. This function is thread-safe.
                 */
                int getMaxFailRetries() const;
                /**
                 * \brief Get restart config value. This function is thread-safe.
                 */
                bool getRestart() const;
                /**
                 * \brief Get last frame time. This function is NOT thread-safe!
                 */
                time_type getLastFrameTime() const;
                /**
                 * \brief Return if the source is in cooldown depending on current fps value. This function is NOT thread-safe!
                 */
                bool isInCooldown() const;
                /**
                 * \brief Get the delay until the source is not in cooldown. This function is NOT thread-safe!
                 */
                time_type getCooldownStopTime() const;
                /**
                 * \brief Get the delay until the source is not in cooldown in seconds. This function is NOT thread-safe!
                 */
                float getRemainingCoolSecs() const;

                /**
                 * \brief Return if the source is enabled or in pause. This function is thread-safe.
                 */
                bool isEnabled() const;
                /**
                 * \brief Return if the source is active. This function is thread-safe.
                 * \details The source is active if it has been triggered or a source of its group has been triggered (wip).
                 */
                bool isActive() const;
                /**
                 * \brief Return if the source is triggered. This function is thread-safe.
                 */
                bool isTriggered() const;

                /**
                 * \brief Set the enable state. This function is thread-safe.
                 */
                void setEnabled(bool status);

                /**
                 * \brief Set the active state. This function is thread-safe.
                 */
                void setActive(bool value);
                /**
                 * \brief Set the trigger state. This function is thread-safe.
                 */
                void setTriggered(bool value);

                /**
                 * \brief Return if the source is stopping or stopped. This function is thread-safe.
                 */
                bool isStopping() const;

                /**
                 * \brief Request the source to stop. This function is thread-safe.
                 */
                void Stop();
                /**
                 * \brief Prepare the source to start after a call to Stop(). This function does not resume the processing. This function is thread-safe.
                 */
                void ResetStopState();

                /**
                 * \brief Set the last frame time to now. This function is NOT thread-safe.
                 */
                void setLastFrameNow();

                /**
                 * \brief Return the config of the source.
                 */
                inline const utils::config::data::sources::SourceSettings& getSourceConfig() const { return *source_config.get(); }
            protected:
                static size_t secsToMillisRound(float seconds);
                void UpdateGroupActivation(components::source::Source& source);

                std::string name;

                std::unique_ptr<VideoStream> video_stream;
                float active_fps;
                float passive_fps;

                int max_fail_retries;
                bool restart;

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
