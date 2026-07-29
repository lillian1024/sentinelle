#pragma once

#include <string>
namespace core
{
    namespace components
    {
        namespace chains
        {
            class PathData
            {
                public:
                    PathData(int stage, std::string analizer_name, std::string output_name)
                        : stage(stage), analizer_name(analizer_name), output_name(output_name) { }

                    PathData(std::string path);

                    inline int getStage() { return stage; }
                    inline std::string getAnalizerName() { return analizer_name; }
                    inline std::string getOutputName() { return output_name; }
                private:
                    int stage;
                    std::string analizer_name;
                    std::string output_name;
            };
        }
    }
}
