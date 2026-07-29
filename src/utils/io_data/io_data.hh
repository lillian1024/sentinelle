#pragma once

#include <memory>
namespace utils
{
    namespace io_data
    {
        enum class IODataType
        {
            BOOL,
            INT,
            FLOAT,
            LONG,
            DOUBLE,
            STRING,
            MAT
        };

        class IOData
        {
            public:
                virtual ~IOData() = default;

                virtual IODataType getType() const = 0;

                virtual std::unique_ptr<IOData> clone() = 0;
        };
    }
}

//#include "io_data.hxx"
