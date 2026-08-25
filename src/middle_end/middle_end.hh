#pragma once

#include <yaml-cpp/node/node.h>
namespace middle_end
{
    class MiddleEnd
    {
        public:
            MiddleEnd() = default;
            virtual ~MiddleEnd() = default;

            virtual void Init() = 0;
            virtual void Start() = 0;
            virtual void Stop() = 0;
    };
}
