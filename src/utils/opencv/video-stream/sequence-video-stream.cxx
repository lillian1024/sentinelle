#include "sequence-video-stream.hh"
#include "utils/opencv/video-stream/video-stream.hh"

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <optional>

namespace utils
{
    namespace opencv
    {
        namespace video_stream
        {
            SequenceVideoStream::SequenceVideoStream(const cv::VideoCapture& video_stream)
                : VideoStream(video_stream)
            {

            }

            SequenceVideoStream::~SequenceVideoStream()
            {
                release();
            }

            void SequenceVideoStream::release()
            {
                if (video_stream.isOpened())
                {
                    video_stream.release();
                }
            }

            std::optional<cv::Mat> SequenceVideoStream::getImage()
            {
                cv::Mat res;

                bool success = video_stream.read(res);

                if (!success)
                {
                    return std::nullopt;
                }

                return res;
            }
        }
    }
}
