#pragma once

#include "utils/io_data/io_data.hh"
#include <opencv2/core/mat.hpp>

namespace utils
{
    namespace io_data
    {
        class IODataMat : public IOData
        {
            public:
                IODataMat(cv::Mat data)
                    : data(data) {}

                cv::Mat getData() const
                {
                    return data;
                }

                IODataType getType() const
                {
                    return IODataType::MAT;
                }
            protected:
                cv::Mat data;
        };
    }
}
