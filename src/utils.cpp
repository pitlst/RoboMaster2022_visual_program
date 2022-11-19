#include "utils.hpp"
#include <cmath>
#include <fstream>
#include <sstream>

using namespace swq;

std::string swq::get_file_str(const std::string &load_path)
{
    std::ifstream ifs;
    ifs.open(load_path);
    if (!ifs.is_open())
    {
        throw std::logic_error("read filure");
    }
    std::istreambuf_iterator<char> beg(ifs), end;
    std::string strdata(beg, end);
    ifs.close();
    return strdata;
}

std::string swq::get_file_str(const char *load_path)
{
    std::string temp(load_path);
    return swq::get_file_str(temp);
}

void swq::save_file(const std::string &load_path, const std::string &file_str)
{
    std::ofstream ofs;
    ofs.open(load_path, std::ios_base::out | std::ios_base::trunc); //删除文件重写
    if (!ofs.is_open())
    {
        throw std::logic_error("write filure");
    }
    ofs << file_str;
    ofs.close();
}

void swq::save_file(const char *load_path, const std::string &file_str)
{
    std::string temp(load_path);
    swq::save_file(temp, file_str);
}

bool swq::is_Numeric(std::string str)
{
    for (auto &ch : str)
    {
        if (ch < '0' || ch > '9')
        {
            return false;
        }
    }
    return true;
}

float swq::EuclideanDistance(float x0, float y0, float x1, float y1)
{
    return pow(pow(x0 - x1, 2) + pow(y0 - y1, 2), 0.5);
}



tarch_bar::tarch_bar()
{
    cv::namedWindow("调参窗口aimbot_1");
    cv::namedWindow("调参窗口aimbot_2");
    cv::namedWindow("调参窗口buffer_1");
    cv::namedWindow("调参窗口buffer_2");
    cv::namedWindow("frame_debug");
    cv::namedWindow("mask_debug");
}

void tarch_bar::trans_bar_to_par()
{


}


void tarch_bar::bar_creat_aimbot(const GetArmor::fiter_para &input_par)
{
    intput_aimbot = input_par;

    bar_aimbot.lowHue = intput_aimbot.lowHue;
    bar_aimbot.lowSat = intput_aimbot.lowSat;
    bar_aimbot.lowVal = intput_aimbot.lowVal;
    bar_aimbot.highHue = intput_aimbot.highHue;
    bar_aimbot.highSat = intput_aimbot.highSat;
    bar_aimbot.highVal = intput_aimbot.highVal;

    bar_aimbot.minlighterarea = intput_aimbot.minlighterarea*10;
    bar_aimbot.maxlighterarea = intput_aimbot.maxlighterarea*10;
    bar_aimbot.minlighterProp = intput_aimbot.minlighterProp;
    bar_aimbot.maxlighterProp = intput_aimbot.maxlighterProp;
    bar_aimbot.minAngleError = intput_aimbot.minAngleError*100;
    bar_aimbot.maxAngleError = intput_aimbot.maxAngleError*100

    bar_aimbot.highVal
    bar_aimbot.highVal
    bar_aimbot.highVal
    bar_aimbot.highVal


    cv::createTrackbar(NAME(bar_aimbot.lowHue), "调参窗口aimbot_1", &(bar_aimbot.lowHue), 255, nullptr);
    cv::createTrackbar(NAME(bar_aimbot.lowSat), "调参窗口aimbot_1", &(bar_aimbot.lowSat), 255, nullptr);
    cv::createTrackbar(NAME(bar_aimbot.lowVal), "调参窗口aimbot_1", &(bar_aimbot.lowVal), 255, nullptr);
    cv::createTrackbar(NAME(bar_aimbot.highHue), "调参窗口aimbot_1", &(bar_aimbot.highHue), 255, nullptr);
    cv::createTrackbar(NAME(bar_aimbot.highSat), "调参窗口aimbot_1", &(bar_aimbot.highSat), 255, nullptr);
    cv::createTrackbar(NAME(bar_aimbot.highVal), "调参窗口aimbot_1", &(bar_aimbot.highVal), 255, nullptr);

    cv::createTrackbar(NAME(bar_aimbot.minlighterarea), "调参窗口aimbot_2", &(bar_aimbot.minlighterarea), 255, nullptr);
    cv::createTrackbar(NAME(bar_aimbot.maxlighterarea), "调参窗口aimbot_2", &(bar_aimbot.maxlighterarea), 255, nullptr);
    cv::createTrackbar(NAME(bar_aimbot.minlighterProp), "调参窗口aimbot_2", &(bar_aimbot.minlighterProp), 255, nullptr);
    cv::createTrackbar(NAME(bar_aimbot.maxlighterProp), "调参窗口aimbot_2", &(bar_aimbot.maxlighterProp), 255, nullptr);
}

void tarch_bar::bar_creat_bufferr(const GetEnergyMac::energy_para &input_par)
{
}

