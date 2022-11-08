#include <iostream>
#include <thread>

#include "get_frame.hpp"
#include "aimbot.hpp"
#include "energy.hpp"
#include "utils.hpp"
#include "json.hpp"
#include "hint.hpp"
#include "logger.hpp"
#include "xml.hpp"
#include "debug.hpp"


std::shared_ptr<float> getData(cv::Mat& img) 
{
int width = img.cols;

int height = img.rows;

std::shared_ptr<float> _data;
size_t size = width * height * img.channels();

_data.reset(new float[size], std::default_delete<float[]>());
cv::Mat resized(cv::Size(width, height), img.type(), _data.get());

cv::resize(img, resized, cv::Size(width, height));
return _data;
}

int main()
{
    log_debug("程序开始");
    std::cout << ov::get_openvino_version() << std::endl;

    cv::Mat img;
    img = cv::imread("../asset/temp.bmp");
    // int width = img.cols;
    // int height = img.rows;
    // std::shared_ptr<unsigned char> _data;
    // size_t size = width * height * img.channels();
    // _data.reset(new unsigned char[size], std::default_delete<unsigned char[]>());
    // cv::Mat resized(cv::Size(width, height), img.type(), _data.get());
    // cv::resize(img, resized, cv::Size(width, height));

    // float* _data = (float*)img.data;

    // ov::element::Type input_tensor_type = ov::element::u8;
    const ov::Layout input_tensor_layout{ "NHWC" };
    auto input_data = getData(img);

    ov::Core core;
    auto model = core.read_model("../asset/model/bestyao_13_416.xml");
    ov::CompiledModel compiled_model = core.compile_model(model, "CPU");
    ov::InferRequest infer_request = compiled_model.create_infer_request();


    auto input_port = compiled_model.input();
    std::cout << input_port.get_element_type().get_type_name() << std::endl;
    // std::cout << input_port.get_shape() << std::endl;
    
    ov::Tensor input_tensor(input_port.get_element_type(), input_port.get_shape(), input_data.get());
    //std::cout << input_tensor.get_shape() << std::endl;

    infer_request.set_input_tensor(input_tensor);
    infer_request.infer();

    log_debug("程序结束");
    return 0;
}


