#pragma once

#include "utils/io_data/io_data.hh"

namespace utils
{
    namespace io_data
    {
        class IODataBool : public IOData
        {
            public:
                IODataBool(bool data)
                    : data(data) {}

                bool getData() const
                {
                    return data;
                }

                IODataType getType() const
                {
                    return IODataType::BOOL;
                }
            protected:
                bool data;
        };
    }
}
