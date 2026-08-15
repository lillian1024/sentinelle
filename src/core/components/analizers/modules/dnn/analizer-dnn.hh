#pragma once

#include "core/components/analizers/analizer.hh"
#include <opencv2/dnn/dnn.hpp>
#include <string>

namespace core
{
    namespace components
    {
        namespace analizer
        {
            namespace dnn
            {
                class AnalizerDNN : public Analizer
                {
                    public:
                        enum class NetStoreType
                        {
                            TENSOR_FLOW,
                            TORCH,
                            ONNX
                        };

                        AnalizerDNN(std::string name, std::string net_name, std::string net_url, bool pull_net, NetStoreType net_store);
                        virtual ~AnalizerDNN() = default;

                        std::string getNetName();
                    protected:
                        static const std::string DNN_CACHE_NAME;

                        cv::dnn::dnn4_v20260709::Net& getNet();

                        void loadNet();
                        bool isLoaded();

                        bool setCUDA();

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
                };
            }
        }
    }
}
