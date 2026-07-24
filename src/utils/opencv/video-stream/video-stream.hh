#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <optional>

#define LIVE_VIDEO_STREAM_CATEGORY_NAME "LiveVideoStream"

namespace utils
{
    namespace opencv
    {
        namespace video_stream
        {
            class VideoStream
            {
                public:
                    VideoStream(cv::VideoCapture video_stream);

                    bool isOpened() const;
                    virtual std::optional<cv::Mat> getImage() = 0;

                    virtual void release() = 0;
                protected:
                    cv::VideoCapture video_stream;
            };
        }
    }
}
