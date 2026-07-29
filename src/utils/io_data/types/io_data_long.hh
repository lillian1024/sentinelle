#pragma once

#include "utils/io_data/io_data.hh"

namespace utils
{
    namespace io_data
    {
        class IODataLong : public IOData
        {
            public:
                IODataLong(long data)
                    : data(data) {}

                long getData() const
                {
                    return data;
                }

                IODataType getType() const
                {
                    return IODataType::LONG;
                }

                std::unique_ptr<IOData> clone()
                {
                    return std::make_unique<IODataLong>(data);
                }
            protected:
                long data;
        };
    }
}
