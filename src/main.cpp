#include <iostream>
#include <thread>
#include <ctime>

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
    swq::GetEnergyMac temp;

    clock_t start,end;

    cv::Mat img;
    img = cv::imread("../asset/temp.bmp");

    // //初始化openvino核心
    // ov::Core core;
    // //读取模型
    // auto model = core.read_model(PATH_MODEL_FILE);
    // auto compiled_model = core.compile_model(model, "CPU");

    // //获取推理请求
    // auto infer_request = compiled_model.create_infer_request();
    // //获取网络的输入输出形状
    // auto input_port = compiled_model.input(0);
    // auto output_port_stride8 = compiled_model.output(0);
    // auto output_port_stride16 = compiled_model.output(1);
    // auto output_port_stride32 = compiled_model.output(2);

    // cv::Mat temp;
    // cv::normalize(img, temp, 1.0, 0.0, cv::NORM_MINMAX);
    // int width = temp.cols;
    // int height = temp.rows;
    // size_t size = width * height * temp.channels();
    // std::shared_ptr<float> _data;
    // _data.reset(new float[size], std::default_delete<float[]>()); //按照图像大小初始化指向的位置
    // cv::Mat resized(cv::Size(width, height), img.type(), _data.get());
    // cv::resize(img, resized, cv::Size(width, height)); //这里借用opencv自己的构造，让数据填充到_data指向的的位置
    // auto input_tensor = ov::Tensor(input_port.get_element_type(), input_port.get_shape(), _data.get());

    // //输入图像
    // infer_request.set_input_tensor(input_tensor);
    // //推理
    // infer_request.infer();


    start = clock(); 
    auto s = 1000;
    for (size_t i = 0; i < s; i++)
    {
        temp.process(img, double(start));
    }
    end = clock();

    std::cout<<"平均帧 = "<<CLOCKS_PER_SEC/(double(end-start)/s)<<"fps"<<std::endl;


    log_debug("程序结束");
    return 0;
}


