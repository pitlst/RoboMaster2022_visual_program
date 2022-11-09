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
    swq::GetEnergyMac temp;

    cv::Mat img;
    img = cv::imread("../asset/temp.bmp");

    temp.process(img);

    log_debug("程序结束");
    return 0;
}


