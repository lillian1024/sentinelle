#pragma once

#include "core/components/analizers/analizer.hh"
#include "core/components/chains/context/context.hh"
#include "core/components/chains/path-data.hh"
namespace core
{
    namespace components
    {
        namespace chains
        {
            class AnalizerUsage
            {
                public:
                    AnalizerUsage(analizer::Analizer& analizer, std::map<std::string, PathData> inputPath);

                    void process(Context& context);
                private:
                    analizer::Analizer& analizer;
                    std::map<std::string, PathData> inputPath;
            };
        }
    }
}
