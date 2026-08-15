#include "analizer-dnn.hh"
#include <opencv2/dnn/dnn.hpp>
#include <stdexcept>
#include <string>

#include "utils/cache/cache-manager.hh"
#include "utils/logger/logger.hh"

#define DNN_CATEGORY_NAME "DDNManager"

#define DNN_NOT_FOUND_ERROR_MSG "Unable to start deep neural network: config files not found and auto pull is disabled!"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            namespace dnn
            {
                const std::vector<std::string> AnalizerDNN::coco_class_names = {
                      "person",
                      "bicycle",
                      "car",
                      "motorcycle",
                      "airplane",
                      "bus",
                      "train",
                      "truck",
                      "boat",
                      "traffic light",
                      "fire hydrant",
                      "stop sign",
                      "parking meter",
                      "bench",
                      "bird",
                      "cat",
                      "dog",
                      "horse",
                      "sheep",
                      "cow",
                      "elephant",
                      "bear",
                      "zebra",
                      "giraffe",
                      "backpack",
                      "umbrella",
                      "handbag",
                      "tie",
                      "suitcase",
                      "frisbee",
                      "skis",
                      "snowboard",
                      "sports ball",
                      "kite",
                      "baseball bat",
                      "baseball glove",
                      "skateboard",
                      "surfboard",
                      "tennis racket",
                      "bottle",
                      "wine glass",
                      "cup",
                      "fork",
                      "knife",
                      "spoon",
                      "bowl",
                      "banana",
                      "apple",
                      "sandwich",
                      "orange",
                      "broccoli",
                      "carrot",
                      "hot dog",
                      "pizza",
                      "donut",
                      "cake",
                      "chair",
                      "couch",
                      "potted plant",
                      "bed",
                      "dining table",
                      "toilet",
                      "tv",
                      "laptop",
                      "mouse",
                      "remote",
                      "keyboard",
                      "cell phone",
                      "microwave",
                      "oven",
                      "toaster",
                      "sink",
                      "refrigerator",
                      "book",
                      "clock",
                      "vase",
                      "scissors",
                      "teddy bear",
                      "hair drier",
                      "toothbrush"
                };

                const std::string AnalizerDNN::DNN_CACHE_NAME = "dnn";

                AnalizerDNN::AnalizerDNN(std::string name, std::string net_name, std::string net_url, bool pull_net, NetStoreType net_store)
                    : Analizer(name),
                    loaded(false),
                    net_name(net_name),
                    net_url(net_url),
                    net_store(net_store),
                    pull_net(pull_net)
                {
                }

                void AnalizerDNN::loadNet()
                {
                    switch (net_store)
                    {
                        case NetStoreType::TENSOR_FLOW:
                        {
                            net = loadFromTensor(net_name, net_url, pull_net);

                            break;
                        }
                        case NetStoreType::TORCH:
                        {
                            net = loadFromTorch(net_name, net_url, pull_net);

                            break;
                        }
                        case NetStoreType::ONNX:
                        {
                            net = loadFromONNX(net_name, net_url, pull_net);

                            break;
                        }
                    }

                    loaded = true;
                }

                cv::dnn::dnn4_v20260709::Net AnalizerDNN::loadFromTensor(std::string net_name, std::string, bool pull_net)
                {
                    std::string pb_name = net_name + ".pb";
                    std::string pbtxt_name = net_name + ".pbtxt";

                    std::string pb_path = utils::cache::CacheManager::instance().getPath(DNN_CACHE_NAME, pb_name);
                    std::string pbtxt_path = utils::cache::CacheManager::instance().getPath(DNN_CACHE_NAME, pbtxt_name);

                    if (!utils::cache::CacheManager::instance().hasFile(DNN_CACHE_NAME, pb_name) ||
                        !utils::cache::CacheManager::instance().hasFile(DNN_CACHE_NAME, pbtxt_name))
                    {
                        std::string cat_name = DNN_CATEGORY_NAME;

                        if (!pull_net)
                        {
                            utils::logger::Logger::instance().Log(DNN_CATEGORY_NAME, DNN_NOT_FOUND_ERROR_MSG, utils::logger::Logger::LogLevel::CRITICAL);
                            throw std::runtime_error(cat_name + ": " + DNN_NOT_FOUND_ERROR_MSG);
                        }

                        throw std::runtime_error(cat_name + ": Auto pull is not implemented yet!");
                    }

                    return cv::dnn::dnn4_v20260709::readNetFromTensorflow(pb_path, pbtxt_path);
                }

                cv::dnn::dnn4_v20260709::Net AnalizerDNN::loadFromTorch(std::string net_name, std::string, bool pull_net)
                {
                    std::string file_name = net_name + ".dump";

                    std::string pb_path = utils::cache::CacheManager::instance().getPath(DNN_CACHE_NAME, file_name);

                    if (!utils::cache::CacheManager::instance().hasFile(DNN_CACHE_NAME, file_name))
                    {
                        std::string cat_name = DNN_CATEGORY_NAME;

                        if (!pull_net)
                        {
                            utils::logger::Logger::instance().Log(DNN_CATEGORY_NAME, DNN_NOT_FOUND_ERROR_MSG, utils::logger::Logger::LogLevel::CRITICAL);
                            throw std::runtime_error(cat_name + ": " + DNN_NOT_FOUND_ERROR_MSG);
                        }

                        throw std::runtime_error(cat_name + ": Auto pull is not implemented yet!");
                    }

                    return cv::dnn::dnn4_v20260709::readNetFromTorch(pb_path, true, false);
                }

                cv::dnn::dnn4_v20260709::Net AnalizerDNN::loadFromONNX(std::string net_name, std::string, bool pull_net)
                {
                    std::string file_name = net_name + ".onnx";

                    std::string file_path = utils::cache::CacheManager::instance().getPath(DNN_CACHE_NAME, file_name);

                    if (!utils::cache::CacheManager::instance().hasFile(DNN_CACHE_NAME, file_name))
                    {
                        std::string cat_name = DNN_CATEGORY_NAME;

                        if (!pull_net)
                        {
                            utils::logger::Logger::instance().Log(DNN_CATEGORY_NAME, DNN_NOT_FOUND_ERROR_MSG, utils::logger::Logger::LogLevel::CRITICAL);
                            throw std::runtime_error(cat_name + ": " + DNN_NOT_FOUND_ERROR_MSG);
                        }

                        throw std::runtime_error(cat_name + ": Auto pull is not implemented yet!");
                    }

                    return cv::dnn::dnn4_v20260709::readNetFromONNX(file_path);
                }

                cv::dnn::dnn4_v20260709::Net& AnalizerDNN::getNet()
                {
                    if (!isLoaded())
                    {
                        loadNet();
                    }

                    return net;
                }

                bool AnalizerDNN::setCUDA()
                {
                    net.setPreferableBackend(cv::dnn::dnn4_v20260709::DNN_BACKEND_CUDA);
                    net.setPreferableTarget(cv::dnn::dnn4_v20260709::DNN_TARGET_CUDA);

                    return false;
                }

                std::string AnalizerDNN::getNetName()
                {
                    return net_name;
                }

                bool AnalizerDNN::isLoaded()
                {
                    return loaded;
                }
            }
        }
    }
}
