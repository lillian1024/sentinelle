#include "analizer-dnn-generic-ident.hh"
#include "core/components/analizers/modules/dnn/analizer-dnn.hh"
#include "utils/io_data/types/io_data_mat.hh"
#include "utils/logger/logger.hh"
#include "utils/cache/cache-manager.hh"
#include <iostream>
#include <map>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

#define NN_NAME "ssd_mobilenet_v2_coco_2018_03_29"
#define NN_URL ""

#define INPUT_IMAGE_NAME "image"

#define OUTPUT_DEBUG_IMAGE_NAME "debug-image"

#define IDENT_CATEGORY_FILE_NAME "ssd_mobilenet_v2_coc_categories.txt"
#define DNN_CATEGORY_NAME "DDNManager"

#define MISSING_CLASS_FILE_ERROR_MSG "Unable to start analizer: unable to locate class data file in cache!"

// TODO: Change to config from yaml
#define MINIMUM_CONFIDENCE_SCORE 0.6

namespace core
{
    namespace components
    {
        namespace analizer
        {
            namespace dnn
            {
                const std::vector<std::string> AnalizerDNNGenericIdent::class_names = {
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
                    "street sign",
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
                    "hat",
                    "backpack",
                    "umbrella",
                    "shoe",
                    "eye glasses",
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
                    "plate",
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
                    "mirror",
                    "dining table",
                    "window",
                    "desk",
                    "toilet",
                    "door",
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
                    "blender",
                    "book",
                    "clock",
                    "vase",
                    "scissors",
                    "teddy bear",
                    "hair drier",
                    "toothbrush"
                };

                AnalizerDNNGenericIdent::AnalizerDNNGenericIdent(YAML::Node _, std::string name)
                    : AnalizerDNN(name, NN_NAME, NN_URL, true, NetStoreType::TENSOR_FLOW)
                {

                }

                std::map<std::string, utils::io_data::IODataType> AnalizerDNNGenericIdent::getInputs()
                {
                    std::map<std::string, utils::io_data::IODataType> res;

                    res.insert({INPUT_IMAGE_NAME, utils::io_data::IODataType::MAT});

                    return res;
                }

                std::map<std::string, utils::io_data::IODataType> AnalizerDNNGenericIdent::getOutputs()
                {
                    std::map<std::string, utils::io_data::IODataType> res;

                    res.insert({OUTPUT_DEBUG_IMAGE_NAME, utils::io_data::IODataType::MAT});

                    return res;
                }

                std::map<std::string, std::unique_ptr<utils::io_data::IOData>> AnalizerDNNGenericIdent::process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source&)
                {
                    //Check input types
                    if (!validateInputs(inputs))
                    {
                        utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to analize: missmatched arguments!", utils::logger::Logger::LogLevel::ERROR);

                        return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                    }

                    //Retrieve image
                    utils::io_data::IOData* image_data = inputs[INPUT_IMAGE_NAME];

                    utils::io_data::IODataMat* image_mat_data = dynamic_cast<utils::io_data::IODataMat*>(image_data);

                    if (image_mat_data == nullptr)
                    {
                        utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to analize: missmatched arguments!", utils::logger::Logger::LogLevel::ERROR);

                        return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                    }

                    cv::Mat image = image_mat_data->getData();

                    cv::Mat blob = cv::dnn::dnn4_v20260709::blobFromImage(image, 1.0, cv::Size(300, 300), cv::Scalar(127.5, 127.5, 127.5),
                                            true, false);

                    cv::dnn::dnn4_v20260709::Net& net = getNet();

                    net.setInput(blob);

                    cv::Mat output = net.forward();

                    cv::Mat results(output.size[2], output.size[3], CV_32F, output.ptr<float>());

                    for (int i = 0; i < results.rows; i++){
                        int class_id = int(results.at<float>(i, 1));
                        float confidence = results.at<float>(i, 2);

                        // Check if the detection is over the min threshold and then draw bbox
                        if (confidence > MINIMUM_CONFIDENCE_SCORE){
                            int bboxX = int(results.at<float>(i, 3) * image.cols);
                            int bboxY = int(results.at<float>(i, 4) * image.rows);
                            int bboxWidth = int(results.at<float>(i, 5) * image.cols - bboxX);
                            int bboxHeight = int(results.at<float>(i, 6) * image.rows - bboxY);

                            std::string class_name = class_names[class_id-1];

                            std::ostringstream sb;

                            sb << "Detected ";
                            sb << class_name;
                            sb << ", confidence: ";
                            sb << confidence * 100;
                            sb << "%";

                            // TODO: Change to event system
                            utils::logger::Logger::instance().Log("Event", sb.str(), utils::logger::Logger::LogLevel::WARNING);

                            cv::rectangle(image, cv::Point(bboxX, bboxY), cv::Point(bboxX+bboxWidth, bboxY+bboxHeight), cv::Scalar(255,255,255), 2);

                            std::ostringstream box_string_builder;

                            box_string_builder << class_name;
                            box_string_builder << " ";
                            box_string_builder << (confidence*100);
                            box_string_builder << "%";

                            cv::putText(image, box_string_builder.str(), cv::Point(bboxX, bboxY-5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,255), 1);
                        }
                    }

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> res;

                    auto debug_image_data = std::make_unique<utils::io_data::IODataMat>(image);

                    res.insert({OUTPUT_DEBUG_IMAGE_NAME, std::move(debug_image_data)});

                    return res;
                }
            }
        }
    }
}
