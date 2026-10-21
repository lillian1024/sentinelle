#pragma once

#include "core/components/analizers/analizer.hh"
#include "core/components/chains/analizer-usage.hh"
#include "core/components/sources/source.hh"
#include "yaml-cpp/node/node.h"
#include <map>
#include <string>
#include <vector>

namespace core
{
    namespace components
    {
        namespace chains
        {
            /**
             * \brief A class that represent a chain of analizer calls.
             * \details The objective of chains is to describe a single process that treats an image. For example: human detection or object tracking.
             * \details The analizers are stored directly in the chain. Therefore, all analizers have a unique instance for each usage in each chain.
             */
            class Chain
            {
                public:
                    /**
                     * \brief Create a Chain from the configuration described by the provided Yaml Node.
                     * \details The chains stores analizers in the chain. Thus the provided analizers will be copied into the created chain.
                     * \param node The node where the chain configuration is stored
                     * \param analizers The list of available analizers to use for this chain
                     */
                    Chain(YAML::Node node, std::map<std::string, std::reference_wrapper<analizer::Analizer>> analizers);

                    /**
                     * \brief Process this chain by executing all the analizers in order.
                     * \param input_image The image to process
                     * \param source The source that generated the image
                     * \param trigger The variable that holds source's trigger value for this source iteration
                     */
                    void process(cv::Mat& input_image, source::Source& source, bool& trigger);

                    /**
                     * \brief Dumps the chain configuration.
                     * \return The configuration in string form
                     */
                    std::string dumpConfig();
                private:
                    /**
                     * \brief Parse a single stage of the chain.
                     * \details Parse all analizer usages of a single stage.
                     * \return The map of all analizer usage of the parsed stage mapped by analizer's id
                     */
                    std::map<std::string, AnalizerUsage> parseStage(YAML::Node node, std::map<std::string, std::reference_wrapper<analizer::Analizer>> analizers);
                    /**
                     * \brief Parse all the inputs of a single analizer usage.
                     * \return The map of all inputs of the parsed analizer mapped by input's name
                     */
                    std::map<std::string, std::string> parseAnalizerinputs(YAML::Node node);

                    /**
                     * \brief A list of stages containing maps of analizers mapped by id.
                     */
                    std::vector<std::map<std::string, AnalizerUsage>> stages;
            };
        }
    }
}
