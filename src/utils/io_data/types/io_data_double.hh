#pragma once

#include "utils/io_data/io_data.hh"

namespace utils
{
    namespace io_data
    {
        class IODataDouble : public IOData
        {
            public:
                IODataDouble(double data)
                    : data(data) {}

                double getData() const
                {
                    return data;
                }

                IODataType getType() const
                {
                    return IODataType::DOUBLE;
                }

                std::unique_ptr<IOData> clone()
                {
                    return std::make_unique<IODataDouble>(data);
                }
            protected:
                double data;
        };
    }
}
