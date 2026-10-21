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
                class AnalyzerDNN : public Analyzer
                {
                    public:
                        enum class NetStoreType
                        {
                            TENSOR_FLOW,
                            TORCH,
                            ONNX
                        };

                        AnalyzerDNN(std::string name, std::string net_name, std::string net_url, bool pull_net, NetStoreType net_store);
                        virtual ~AnalyzerDNN() = default;

                        std::string getNetName();
                    protected:
                        static const std::string DNN_CACHE_NAME;

                        cv::dnn::dnn4_v20260709::Net& getNet();

                        void loadNet();
                        bool isLoaded();

                        bool setNetToCUDA();

                        void setUseCUDA(bool useCUDA);
                        bool getUseCUDA() const;

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
