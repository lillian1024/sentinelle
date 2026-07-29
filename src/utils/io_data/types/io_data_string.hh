#pragma once

#include "utils/io_data/io_data.hh"
#include <string>

namespace utils
{
    namespace io_data
    {
        class IODataString : public IOData
        {
            public:
                IODataString(std::string data)
                    : data(data) {}

                std::string getData() const
                {
                    return data;
                }

                IODataType getType() const
                {
                    return IODataType::STRING;
                }

                std::unique_ptr<IOData> clone()
                {
                    return std::make_unique<IODataString>(data);
                }
            protected:
                std::string data;
        };
    }
}
