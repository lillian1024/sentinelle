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
            protected:
                double data;
        };
    }
}
