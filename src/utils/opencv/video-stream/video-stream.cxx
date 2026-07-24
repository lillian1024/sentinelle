#include "video-stream.hh"
#include <opencv2/videoio.hpp>

namespace utils
{
    namespace opencv
    {
        namespace video_stream
        {
            VideoStream::VideoStream(cv::VideoCapture video_stream)
                : video_stream(video_stream)
            { }

            bool VideoStream::isOpened() const
            {
                return video_stream.isOpened();
            }
        }
    }
}
