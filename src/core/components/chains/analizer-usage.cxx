#include "analizer-usage.hh"
#include "core/components/chains/context/context.hh"

namespace core
{
    namespace components
    {
        namespace chains
        {
            AnalizerUsage::AnalizerUsage(analizer::Analizer& analizer, std::map<std::string, PathData> inputPath)
                : analizer(analizer),
                inputPath(inputPath)
            {

            }

            void AnalizerUsage::process(Context& context)
            {
                //analizer.process(std::map<std::string, utils::io_data::IOData *>, source::Source &);
            }
        }
    }
}
