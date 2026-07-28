#pragma once

#include "utils/opencv/video-stream/video-stream.hh"
#include <memory>
#include <mutex>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <optional>
#include <thread>

#define LIVE_VIDEO_STREAM_CATEGORY_NAME "LiveVideoStream"

namespace utils
{
    namespace opencv
    {
        namespace video_stream
        {
            class LiveVideoStream : public VideoStream
            {
                public:
                    LiveVideoStream(const cv::VideoCapture& video_stream);
                    virtual ~LiveVideoStream();

                    std::optional<cv::Mat> getLastImage();
                    std::optional<cv::Mat> getImage();

                    void release();
                    void releaseThread(bool join_release);
                protected:
                    static void handleVideoStream(LiveVideoStream& current);

                    std::atomic<bool> stop;

                    std::unique_ptr<cv::Mat> last_image;

                    std::mutex last_image_lock;
                    std::thread handle_thread;
            };
        }
    }
}
