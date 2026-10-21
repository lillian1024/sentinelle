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
                    PathData(int stage, std::string analyzer_name, std::string output_name)
                        : stage(stage), analyzer_name(analyzer_name), output_name(output_name) { }

                    inline int getStage() { return stage; }
                    inline std::string getAnalyzerName() { return analyzer_name; }
                    inline std::string getOutputName() { return output_name; }
                private:
                    int stage;
                    std::string analyzer_name;
                    std::string output_name;
            };
        }
    }
}
