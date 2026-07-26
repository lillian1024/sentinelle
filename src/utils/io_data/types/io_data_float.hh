#pragma once

#include "utils/io_data/io_data.hh"

namespace utils
{
    namespace io_data
    {
        class IODataFloat : public IOData
        {
            public:
                IODataFloat(float data)
                    : data(data) {}

                float getData() const
                {
                    return data;
                }

                IODataType getType() const
                {
                    return IODataType::FLOAT;
                }
            protected:
                float data;
        };
    }
}
