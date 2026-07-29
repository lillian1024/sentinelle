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

                std::unique_ptr<IOData> clone()
                {
                    return std::make_unique<IODataFloat>(data);
                }
            protected:
                float data;
        };
    }
}
