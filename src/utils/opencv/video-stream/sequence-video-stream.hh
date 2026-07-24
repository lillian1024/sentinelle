#pragma once

#include "utils/opencv/video-stream/video-stream.hh"
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
            class SequenceVideoStream : public VideoStream
            {
                public:
                    SequenceVideoStream(const cv::VideoCapture& video_stream);
                    ~SequenceVideoStream();

                    std::optional<cv::Mat> getImage();

                    void release();
                protected:
            };
        }
    }
}
