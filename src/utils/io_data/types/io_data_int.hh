#pragma once

#include "utils/io_data/io_data.hh"

namespace utils
{
    namespace io_data
    {
        class IODataInt : public IOData
        {
            public:
                IODataInt(int data)
                    : data(data) {}

                int getData() const
                {
                    return data;
                }

                IODataType getType() const
                {
                    return IODataType::INT;
                }
            protected:
                int data;
        };
    }
}
