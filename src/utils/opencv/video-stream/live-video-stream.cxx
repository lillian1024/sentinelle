#include "live-video-stream.hh"
#include "utils/logger/logger.hh"
#include "utils/opencv/video-stream/video-stream.hh"
#include "utils/thread/thread-manager.hh"
#include <functional>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <optional>

namespace utils
{
    namespace opencv
    {
        namespace video_stream
        {
            LiveVideoStream::LiveVideoStream(const cv::VideoCapture& video_stream)
                : VideoStream(video_stream),
                  stop(false)
            {
                auto t = thread::ThreadManager::instance().CreateThread(false, handleVideoStream, std::ref(*this));

                if (!t.has_value())
                {
                    logger::Logger::instance().Log(LIVE_VIDEO_STREAM_CATEGORY_NAME, "Unable to start the live video stream: thread limit reached!", logger::Logger::LogLevel::ERROR);

                    return;
                }

                handle_thread = std::move(t.value());
            }

            LiveVideoStream::~LiveVideoStream()
            {
                releaseThread(true);

                if (video_stream.isOpened())
                {
                    video_stream.release();
                }
            }

            void LiveVideoStream::release()
            {
                releaseThread(true);
            }

            void LiveVideoStream::releaseThread(bool join_release)
            {
                stop = true;

                if (join_release)
                {
                    handle_thread.join();
                }
            }

            std::optional<cv::Mat> LiveVideoStream::getLastImage()
            {
                std::lock_guard<std::mutex> lock(last_image_lock);

                if (last_image.get() == nullptr)
                {
                    return std::nullopt;
                }

                return *last_image;
            }

            std::optional<cv::Mat> LiveVideoStream::getImage()
            {
                auto img = getLastImage();

                if (!img.has_value() || img.value().empty())
                {
                    return std::nullopt;
                }

                return img;
            }

            void LiveVideoStream::handleVideoStream(LiveVideoStream& current)
            {
                while (!current.stop && current.video_stream.isOpened())
                {
                    auto frame = std::make_unique<cv::Mat>();
                    current.video_stream.read(*frame);

                    {
                        std::lock_guard<std::mutex> lock(current.last_image_lock);

                        current.last_image = std::move(frame);
                    }

                    //Gives more chances for other threads to lock the mutex
                    std::this_thread::yield();
                }

                if (current.video_stream.isOpened())
                {
                    current.video_stream.release();
                }
            }
        }
    }
}
