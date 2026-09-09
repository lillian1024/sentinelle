#include "analizer-dnn-yolov9.hh"
#include "core/components/analizers/modules/dnn/analizer-dnn.hh"
#include "utils/config/data-module.hh"
#include "utils/io_data/types/io_data_bool.hh"
#include "utils/io_data/types/io_data_mat.hh"
#include "utils/logger/logger.hh"
#include "utils/cache/cache-manager.hh"
#include <iostream>
#include <map>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define CATEGORY_NAME "AnalizerDNNYoloV9"

#define CATEGORY_PROPERTY_NAME "classes"
#define CONFIDENCE_PROPERTY_NAME "min_confidence"
#define NMS_PROPERTY_NAME "nms"

#define CONFIDENCE_PROPERTY_DEFAULT2 "60"
#define NMS_PROPERTY_DEFAULT "45"

#define PERCENT_MIN_VAL 0
#define PERCENT_MAX_VAL 100

#define NN_NAME "yolov9-m"
#define NN_URL ""

#define INPUT_IMAGE_NAME "image"

#define OUTPUT_DEBUG_IMAGE_NAME "debug_image"
#define OUTPUT_DETECTED_NAME "detected"

#define DNN_CATEGORY_NAME "DDNManager"

#define MISSING_CLASS_FILE_ERROR_MSG "Unable to start analizer: unable to locate class data file in cache!"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            namespace dnn
            {
                AnalizerDNNYoloV9::AnalizerDNNYoloV9(YAML::Node node, std::string name)
                    : AnalizerDNN(name, NN_NAME, NN_URL, true, NetStoreType::ONNX)
                {
                    auto cat_map = utils::config::DataModule::readSequenceOrError(node, CATEGORY_PROPERTY_NAME, name + " analizer");
                    auto min_confidence_str = utils::config::DataModule::readScalarOptional(node, CONFIDENCE_PROPERTY_NAME).value_or(CONFIDENCE_PROPERTY_DEFAULT2);
                    auto nms_str = utils::config::DataModule::readScalarOptional(node, NMS_PROPERTY_NAME).value_or(NMS_PROPERTY_DEFAULT);

                    std::string prefix = "[";

                    for (size_t i = 0; i < cat_map.size(); i++)
                    {
                        if (!cat_map[i].IsScalar())
                        {
                            throw std::runtime_error(prefix + CATEGORY_NAME + "]: " + CATEGORY_PROPERTY_NAME + " must only contain strings!");
                        }

                        searching_category.push_back(cat_map[i].Scalar());
                    }

                    try
                    {
                        unsigned int percent_min_confidence = std::stoi(min_confidence_str);
                        unsigned int percent_nms = std::stoi(nms_str);

                        if (percent_min_confidence < PERCENT_MIN_VAL || percent_min_confidence > PERCENT_MAX_VAL)
                        {
                            throw std::runtime_error(prefix + CATEGORY_NAME + "]: " + CONFIDENCE_PROPERTY_NAME + " must be an integer between 1 and 100 included!");
                        }

                        if (percent_nms < PERCENT_MIN_VAL || percent_nms > PERCENT_MAX_VAL)
                        {
                            throw std::runtime_error(prefix + CATEGORY_NAME + "]: " + NMS_PROPERTY_NAME + " must be an integer between 1 and 100 included!");
                        }

                        min_confidence = ((float)percent_min_confidence) / 100.0;
                        nms_threshold = ((float)percent_nms) / 100.0;

                        std::ostringstream sb;

                        sb << "confid: ";
                        sb << min_confidence;
                        sb << ", nms: ";
                        sb << nms_threshold;

                        utils::logger::Logger::instance().Log("Test----------", sb.str(), utils::logger::Logger::LogLevel::DEBUG);
                    }
                    catch (const std::invalid_argument&)
                    {
                        throw std::runtime_error(prefix + CATEGORY_NAME + "]: " + CONFIDENCE_PROPERTY_NAME + " (optional) and " + NMS_PROPERTY_NAME + " (optional) must be an integers between 1 and 100 included!");
                    }
                    catch (const std::out_of_range&)
                    {
                        throw std::runtime_error(prefix + CATEGORY_NAME + "]: " + CONFIDENCE_PROPERTY_NAME + " (optional) and " + NMS_PROPERTY_NAME + " (optional) must be an integers between 1 and 100 included!");
                    }
                }

                AnalizerDNNYoloV9::AnalizerDNNYoloV9(const AnalizerDNNYoloV9& from)
                    : AnalizerDNN(from.getName(), NN_NAME, NN_URL, true, NetStoreType::ONNX),
                    min_confidence(from.min_confidence),
                    nms_threshold(from.nms_threshold)
                {
                    for (auto s : from.searching_category)
                    {
                        searching_category.push_back(s);
                    }
                }

                std::map<std::string, utils::io_data::IODataType> AnalizerDNNYoloV9::getInputs()
                {
                    std::map<std::string, utils::io_data::IODataType> res;

                    res.insert({INPUT_IMAGE_NAME, utils::io_data::IODataType::MAT});

                    return res;
                }

                std::map<std::string, utils::io_data::IODataType> AnalizerDNNYoloV9::getOutputs()
                {
                    std::map<std::string, utils::io_data::IODataType> res;

                    res.insert({OUTPUT_DEBUG_IMAGE_NAME, utils::io_data::IODataType::MAT});
                    res.insert({OUTPUT_DETECTED_NAME, utils::io_data::IODataType::BOOL});

                    return res;
                }

                std::map<std::string, std::unique_ptr<utils::io_data::IOData>> AnalizerDNNYoloV9::process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source&, bool&)
                {
                    //Check input types
                    if (!validateInputs(inputs))
                    {
                        utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to analize: missmatched arguments!", utils::logger::Logger::LogLevel::ERROR);

                        return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                    }

                    const int INPUT_W = 640;
                    const int INPUT_H = 640;

                    //Retrieve image
                    utils::io_data::IOData* image_data = inputs[INPUT_IMAGE_NAME];

                    utils::io_data::IODataMat* image_mat_data = dynamic_cast<utils::io_data::IODataMat*>(image_data);

                    if (image_mat_data == nullptr)
                    {
                        utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to analize: missmatched arguments!", utils::logger::Logger::LogLevel::ERROR);

                        return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                    }

                    cv::Mat image = image_mat_data->getData();

                    //Preprocessing

                    float scale;
                    cv::Mat letterboxed = letterbox(image, INPUT_W, INPUT_H, scale);

                    cv::Mat blob = cv::dnn::dnn4_v20260709::blobFromImage(letterboxed, 1.0/255.0, cv::Size(INPUT_W, INPUT_H), cv::Scalar(),
                                            true, false);

                    //Processing

                    cv::dnn::dnn4_v20260709::Net& net = getNet();

                    net.setInput(blob);

                    std::vector<cv::Mat> outputs;
                    net.forward(outputs, net.getUnconnectedOutLayersNames());

                    //Postprocessing

                    cv::Mat output = outputs[0];
                    int dimensions = output.size[1];   // 4 + num_classes

                    cv::Mat reshaped = output.reshape(1, dimensions); // dimensions x numAnchors
                    cv::Mat detOutput;
                    transpose(reshaped, detOutput);

                    bool detected = false;

                    std::vector<int> classIds;
                    std::vector<float> confidences;
                    std::vector<cv::Rect> boxes;

                    for (int i = 0; i < detOutput.rows; i++) {
                        cv::Mat classScores = detOutput.row(i).colRange(4, dimensions);
                        cv::Point classIdPoint;
                        double maxScore;
                        minMaxLoc(classScores, nullptr, &maxScore, nullptr, &classIdPoint);

                        if (maxScore < min_confidence) continue;

                        float cx = detOutput.at<float>(i, 0);
                        float cy = detOutput.at<float>(i, 1);
                        float w  = detOutput.at<float>(i, 2);
                        float h  = detOutput.at<float>(i, 3);

                        // Undo letterbox scaling to map back to original image coordinates.
                        int left   = (int)((cx - w / 2.0f) / scale);
                        int top    = (int)((cy - h / 2.0f) / scale);
                        int width  = (int)(w / scale);
                        int height = (int)(h / scale);

                        boxes.emplace_back(left, top, width, height);
                        confidences.push_back((float)maxScore);
                        classIds.push_back(classIdPoint.x);
                    }

                    std::vector<int> keep;
                    cv::dnn::dnn4_v20260709::NMSBoxes(boxes, confidences, min_confidence, nms_threshold, keep);

                    std::vector<Detection> detections;

                    for (int idx : keep)
                    {
                        int class_id = classIds[idx];
                        double confidence = confidences[idx];

                        cv::Rect rect = boxes[idx];

                        detections.push_back(Detection(class_id, confidence, rect));
                    }

                    processDetection(image, detections, detected);

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> res;

                    auto debug_image_data = std::make_unique<utils::io_data::IODataMat>(image);
                    auto debug_detected_data = std::make_unique<utils::io_data::IODataBool>(detected);

                    res.insert({OUTPUT_DEBUG_IMAGE_NAME, std::move(debug_image_data)});
                    res.insert({OUTPUT_DETECTED_NAME, std::move(debug_detected_data)});

                    return res;
                }

                void AnalizerDNNYoloV9::processDetection(cv::Mat& image, std::vector<Detection> detections, bool& detected)
                {
                    for (auto det: detections)
                    {
                        if (det.class_id < 0 || size_t(det.class_id) >= coco_class_names.size())
                        {
                            std::ostringstream sb;

                            sb << "detected unkown class: ";
                            sb << det.class_id;
                            sb << "/";
                            sb << coco_class_names.size();
                            sb << " !";

                            utils::logger::Logger::instance().Log(CATEGORY_NAME, sb.str(), utils::logger::Logger::LogLevel::WARNING);

                            continue;
                        }

                        std::string class_name = coco_class_names[det.class_id];

                        bool is_searched_class = std::ranges::contains(searching_category, class_name);

                        if (!is_searched_class)
                        {
                            continue;
                        }

                        detected = true;

                        std::ostringstream sb;

                        sb << "Detected ";
                        sb << class_name;
                        sb << ", confidence: ";
                        sb << det.confidence * 100;
                        sb << "%";

                        // TODO: Change to event system
                        utils::logger::Logger::instance().Log("Event", sb.str(), utils::logger::Logger::LogLevel::WARNING);

                        cv::rectangle(image, cv::Point(det.box.x, det.box.y), cv::Point(det.box.x+det.box.width, det.box.y+det.box.height), cv::Scalar(255,255,255), 2);

                        std::ostringstream box_string_builder;

                        box_string_builder << class_name;
                        box_string_builder << " ";
                        box_string_builder << (det.confidence*100);
                        box_string_builder << "%";

                        cv::putText(image, box_string_builder.str(), cv::Point(det.box.x, det.box.y-5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,255), 1);
                    }

                    std::ostringstream sb;

                    sb << "Detected: ";
                    sb << (detected ? "true" : "false");

                    cv::putText(image, sb.str(), cv::Point(5, 90), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,255), 1);
                }

                cv::Mat AnalizerDNNYoloV9::letterbox(const cv::Mat& src, int targetW, int targetH, float& scale)
                {
                    int srcW = src.cols, srcH = src.rows;
                    scale = cv::min((float)targetW / srcW, (float)targetH / srcH);
                    int newW = (int)round(srcW * scale);
                    int newH = (int)round(srcH * scale);

                    cv::Mat resized;
                    resize(src, resized, cv::Size(newW, newH));

                    cv::Mat canvas = cv::Mat::zeros(targetH, targetW, CV_8UC3);
                    resized.copyTo(canvas(cv::Rect(0, 0, newW, newH)));

                    return canvas;
                }

                std::unique_ptr<Analizer> AnalizerDNNYoloV9::clone()
                {
                    return std::make_unique<AnalizerDNNYoloV9>(*this);
                }
            }
        }
    }
}
