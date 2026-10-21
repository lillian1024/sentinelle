#pragma once

#include "core/components/analyzers/analyzer.hh"
#include <opencv2/dnn/dnn.hpp>
#include <string>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            namespace dnn
            {
                /**
                 * \brief A helper parent class for Deep Neural Network analyzers. Provide functions to load and manage DNN using OpenCV.
                 */
                class AnalyzerDNN : public Analyzer
                {
                    public:
                        /**
                         * \brief Type of loading method to pass to OpenCV for loading instruction.
                         */
                        enum class NetStoreType
                        {
                            TENSOR_FLOW,
                            TORCH,
                            ONNX
                        };

                        /**
                         * \brief Create a DNN Analyzer from provided data.
                         * \details The constructor does not load the analyzer directly to prevent useless DNN loading. To load the DNN, call getNet() or loadNet().
                         * \param name The name of the analyzer
                         * \param net_name The name of the DNN file
                         * \param net_url The url of the DNN to download from
                         * \param pull_net Indicates if the DNN should be downloaded if missing
                         * \param net_store The type of storage format of the DNN file
                         */
                        AnalyzerDNN(std::string name, std::string net_name, std::string net_url, bool pull_net, NetStoreType net_store);
                        virtual ~AnalyzerDNN() = default;

                        /**
                         * \brief The name of the analyzer.
                         */
                        std::string getNetName();
                    protected:
                        static const std::string DNN_CACHE_NAME;

                        /**
                         * \brief Return the Net OpenCV object of the analyzer.
                         * \details This function will load the DNN if it is not loaded.
                         * \return The OpenCV's Net object
                         */
                        cv::dnn::dnn4_v20260709::Net& getNet();

                        /**
                         * \brief Load the DNN.
                         */
                        void loadNet();
                        /**
                         * \brief If the DNN has been loaded before.
                         */
                        bool isLoaded();

                        /**
                         * \brief Set the DNN to use CUDA backend immediatly.
                         * \details This function cannot be use if the DNN is not loaded.
                         */
                        bool setNetToCUDA();

                        /**
                         * \brief Set the analyzer to use or not use CUDA backend for the next loading of the DNN.
                         */
                        void setUseCUDA(bool useCUDA);
                        /**
                         * \brief Get if the next load of the DNN will use CUDA.
                         */
                        bool getUseCUDA() const;

                        /**
                         * \brief The list of all coco class names.
                         */
                        static const std::vector<std::string> coco_class_names;
                    private:
                        cv::dnn::dnn4_v20260709::Net loadFromTensor(std::string net_name, std::string net_url, bool pull_net);
                        cv::dnn::dnn4_v20260709::Net loadFromTorch(std::string net_name, std::string net_url, bool pull_net);
                        cv::dnn::dnn4_v20260709::Net loadFromONNX(std::string net_name, std::string net_url, bool pull_net);

                        cv::dnn::dnn4_v20260709::Net net;

                        bool loaded;

                        std::string net_name;
                        std::string net_url;

                        NetStoreType net_store;

                        bool pull_net;

                        bool useCUDA;
                };
            }
        }
    }
}
