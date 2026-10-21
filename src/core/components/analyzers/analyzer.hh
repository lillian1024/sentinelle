#pragma once

#include "core/components/sources/source.hh"
#include "utils/io_data/io_data.hh"
#include "yaml-cpp/node/node.h"
#include <map>
#include <memory>
#include <string>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            enum class Type
            {
                DDN_GENERAL_IDENT,
                YOLOV9M,
                YOLOV9P,
                SHOW_IMAGE,
                SAVE_VIDEO,
                VIDEO_RECORDER,
                STREAM_VIDEO,
                TRIGGER_IF_TRUE,
                BOOL_DELAY,
            };

            /**
             * \brief The parent class for all analyzers. This class is used to implements analyzing functions.
             */
            class Analyzer
            {
                public:
                    /**
                     * \brief Category name for logging purposes.
                     */
                    static const std::string ANALIZE_CATEGORY_NAME;

                    /**
                     * \brief Construct an analyzer from its name.
                     */
                    Analyzer(std::string name);
                    virtual ~Analyzer() = default;

                    /**
                     * \brief Create an analyzer from a configuration.
                     * \details This function will select the correct analyzer class to use depending on the type specified in the configuration.
                     * \param node The configuration node
                     * \param name The name of the analyzer
                     */
                    static std::unique_ptr<Analyzer> getAnalyzerFromNode(YAML::Node node, std::string name);

                    /**
                     * \brief A function returning all the expected input variable types.
                     * \return A map of input variable types mapped by variable's name
                     */
                    virtual std::map<std::string, utils::io_data::IODataType> getInputs() = 0;
                    /**
                     * \brief A function returning all the provided output variable types.
                     * \return A map of output variable types mapped by variable's name
                     */
                    virtual std::map<std::string, utils::io_data::IODataType> getOutputs() = 0;

                    /**
                     * \brief Validate a list of input variables.
                     * \param inputs A map of inputs mapped by variable name
                     * \return If the list of input variables is valid for this anayzer
                     */
                    virtual bool validateInputs(std::map<std::string, utils::io_data::IOData*> inputs);
                    /**
                     * \brief Validate a list of input variable types.
                     * \param inputs A map of input types mapped by variable name
                     * \return If the list of input variable types is valid for this anayzer
                     */
                    virtual bool validateInputs(std::map<std::string, utils::io_data::IODataType> types);

                    /**
                     * \brief Get the input type of the requested variable.
                     * \param inputs The list of input data
                     * \return The DataType of the requested variable in the inputs. Or nullptr if the variable is not in the inputs.
                     */
                    template<class T>
                    static T* getInputTypeFromInputs(std::map<std::string, utils::io_data::IOData*> inputs, std::string name);

                    /**
                     * \brief Process this analyzer.
                     * \param inputs The input variables that must correspond to the list of types provided by getInputs()
                     * \param trigger The source trigger variable for this iteration of the source processing
                     * \return The list of output variables. This list must match the variables type provided by getOutputs()
                     */
                    virtual std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source&, bool& trigger) = 0;

                    /**
                     * \brief Return the name of the analyzer.
                     */
                    virtual inline std::string getName() const { return name; }

                    /**
                     * \brief Clone this analyzer using the Clone Design Patern.
                     * \details Memory intensive operations should be executed as late as possible to prevent cloning of useless data
                     * \return unique_ptr to the new Analyzer instance
                     */
                    virtual std::unique_ptr<Analyzer> clone() = 0;
                protected:
                    std::string name;
            };
        }
    }
}

#include "analyzer.hxx"
