#pragma once

#include <memory>
#include <yaml-cpp/node/node.h>
namespace middle_end
{
    class MiddleEnd
    {
        public:
            MiddleEnd() = default;
            virtual ~MiddleEnd() = default;

            static std::unique_ptr<MiddleEnd> createNewMiddleEnd(std::string name, YAML::Node node);

            virtual void Init() = 0;
            virtual void Start() = 0;
            virtual void Stop() = 0;
    };
}
