#pragma once

#include <opencv2/opencv.hpp>

namespace core
{
    namespace components
    {
        namespace source
        {
            class source
            {
            protected:
                cv::VideoCapture video_stream;
            };
        }
    }
}
