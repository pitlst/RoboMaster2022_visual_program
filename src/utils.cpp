#include "utils.hpp"

#include <cmath>
#include <fstream>
#include <sstream>

extern swq::m_fiter_para bar_aimbot_global;
extern swq::m_energy_para bar_buffer_global;

void onchange(int pos, void *userdata)
{
    auto &temp = *((int *)userdata);
    temp = pos;
}

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
        if (ch < '0' or ch > '9')
        {
            return false;
        }
    }
    return true;
}

float swq::EuclideanDistance(float x0, float y0, float x1, float y1)
{
    auto temp = pow(pow(x0 - x1, 2) + pow(y0 - y1, 2), 0.5);
    //防止溢出
    if (temp > 100000)
    {
        temp = 100000;
    }
    return temp;
}

swq::fiter_para swq::trans_bar_to_para(const swq::m_fiter_para &input_aimbot)
{
    swq::fiter_para out_aimbot;

    out_aimbot.lowHue = input_aimbot.lowHue;
    out_aimbot.lowSat = input_aimbot.lowSat;
    out_aimbot.lowVal = input_aimbot.lowVal;
    out_aimbot.highHue = input_aimbot.highHue;
    out_aimbot.highSat = input_aimbot.highSat;
    out_aimbot.highVal = input_aimbot.highVal;

    out_aimbot.minlighterarea = input_aimbot.minlighterarea / 10;
    out_aimbot.maxlighterarea = input_aimbot.maxlighterarea / 10;
    out_aimbot.minlighterProp = input_aimbot.minlighterProp;
    out_aimbot.maxlighterProp = input_aimbot.maxlighterProp;
    out_aimbot.minAngleError = input_aimbot.minAngleError / 100;
    out_aimbot.maxAngleError = input_aimbot.maxAngleError / 100;

    out_aimbot.minarealongRatio = input_aimbot.minarealongRatio / 100;
    out_aimbot.maxarealongRatio = input_aimbot.maxarealongRatio / 100;
    out_aimbot.lightBarAreaDiff = input_aimbot.lightBarAreaDiff;
    out_aimbot.armorAngleMin = input_aimbot.armorAngleMin / 10;
    out_aimbot.minarmorArea = input_aimbot.minarmorArea;
    out_aimbot.maxarmorArea = input_aimbot.maxarmorArea;
    out_aimbot.minarmorProp = input_aimbot.minarmorProp / 100;
    out_aimbot.maxarmorProp = input_aimbot.maxarmorProp / 100;
    out_aimbot.minBigarmorProp = input_aimbot.minBigarmorProp / 100;
    out_aimbot.maxBigarmorProp = input_aimbot.maxBigarmorProp / 100;
    out_aimbot.angleDiff_near = input_aimbot.angleDiff_near / 10;
    out_aimbot.angleDiff_far = input_aimbot.angleDiff_far / 10;
    out_aimbot.minareawidthRatio = input_aimbot.minareawidthRatio / 100;
    out_aimbot.maxareawidthRatio = input_aimbot.maxareawidthRatio / 100;
    out_aimbot.minareaRatio = input_aimbot.minareaRatio / 100;
    out_aimbot.maxareaRatio = input_aimbot.maxareaRatio / 100;
    out_aimbot.area_limit = input_aimbot.area_limit;
    out_aimbot.xcenterdismax = input_aimbot.xcenterdismax / 10;
    out_aimbot.ylengthmin = input_aimbot.ylengthmin;
    out_aimbot.ylengcenterRatio = input_aimbot.ylengcenterRatio;
    out_aimbot.yixaingangleDiff_near = input_aimbot.yixaingangleDiff_near;
    out_aimbot.yixaingangleDiff_far = input_aimbot.yixaingangleDiff_far;
    out_aimbot.kh = input_aimbot.kh;

    return out_aimbot;
}

swq::energy_para swq::trans_bar_to_para(const swq::m_energy_para &input_buffer)
{
    swq::energy_para out_buffer;

    out_buffer.center_dis_x = input_buffer.center_dis_x;
    out_buffer.center_dis_y = input_buffer.center_dis_y;
    out_buffer.pass_number_max = input_buffer.pass_number_max;
    out_buffer.delta_angle_distance = input_buffer.delta_angle_distance;
    out_buffer.frame_size = input_buffer.frame_size;

    out_buffer.predict_small = input_buffer.predict_small;
    out_buffer.predict_big = input_buffer.predict_big;
    out_buffer.R_noise = input_buffer.R_noise;
    out_buffer.Q_noise = input_buffer.Q_noise;
    out_buffer.fan_armor_distence_max = input_buffer.fan_armor_distence_max / 100;
    out_buffer.fan_armor_distence_min = input_buffer.fan_armor_distence_min / 100;
    out_buffer.armor_R_distance_max = input_buffer.armor_R_distance_max / 100;
    out_buffer.armor_R_distance_min = input_buffer.armor_R_distance_max / 100;
    out_buffer.nms_distence_max = input_buffer.nms_distence_max / 100;

    return out_buffer;
}

void swq::bar_creat_aimbot(swq::m_fiter_para &input_par)
{
    cv::createTrackbar(NAME(input_par.lowHue), "调参窗口aimbot_1", nullptr, 255, onchange, (void *)&(input_par.lowHue));
    cv::createTrackbar(NAME(input_par.lowSat), "调参窗口aimbot_1", nullptr, 255, onchange, (void *)&(input_par.lowSat));
    cv::createTrackbar(NAME(input_par.lowVal), "调参窗口aimbot_1", nullptr, 255, onchange, (void *)&(input_par.lowVal));
    cv::createTrackbar(NAME(input_par.highHue), "调参窗口aimbot_1", nullptr, 255, onchange, (void *)&(input_par.highHue));
    cv::createTrackbar(NAME(input_par.highSat), "调参窗口aimbot_1", nullptr, 255, onchange, (void *)&(input_par.highSat));
    cv::createTrackbar(NAME(input_par.highVal), "调参窗口aimbot_1", nullptr, 255, onchange, (void *)&(input_par.highVal));
    cv::createTrackbar(NAME(input_par.minlighterarea), "调参窗口aimbot_1", nullptr, 255, onchange, (void *)&(input_par.minlighterarea));
    cv::createTrackbar(NAME(input_par.maxlighterarea), "调参窗口aimbot_1", nullptr, 10000, onchange, (void *)&(input_par.maxlighterarea));
    cv::createTrackbar(NAME(input_par.minlighterProp), "调参窗口aimbot_1", nullptr, 500, onchange, (void *)&(input_par.minlighterProp));
    cv::createTrackbar(NAME(input_par.maxlighterProp), "调参窗口aimbot_1", nullptr, 3000, onchange, (void *)&(input_par.maxlighterProp));
    cv::createTrackbar(NAME(input_par.minAngleError), "调参窗口aimbot_1", nullptr, 3600, onchange, (void *)&(input_par.minAngleError));
    cv::createTrackbar(NAME(input_par.maxAngleError), "调参窗口aimbot_1", nullptr, 3600, onchange, (void *)&(input_par.maxAngleError));

    cv::createTrackbar(NAME(input_par.maxarealongRatio), "调参窗口aimbot_2", nullptr, 300, onchange, (void *)&(input_par.maxarealongRatio));
    cv::createTrackbar(NAME(input_par.minarealongRatio), "调参窗口aimbot_2", nullptr, 100, onchange, (void *)&(input_par.minarealongRatio));
    cv::createTrackbar(NAME(input_par.lightBarAreaDiff), "调参窗口aimbot_2", nullptr, 10000, onchange, (void *)&(input_par.lightBarAreaDiff));
    cv::createTrackbar(NAME(input_par.armorAngleMin), "调参窗口aimbot_2", nullptr, 3600, onchange, (void *)&(input_par.armorAngleMin));
    cv::createTrackbar(NAME(input_par.minarmorArea), "调参窗口aimbot_2", nullptr, 5000, onchange, (void *)&(input_par.minarmorArea));
    cv::createTrackbar(NAME(input_par.maxarmorArea), "调参窗口aimbot_2", nullptr, 100000, onchange, (void *)&(input_par.maxarmorArea));
    cv::createTrackbar(NAME(input_par.minarmorProp), "调参窗口aimbot_2", nullptr, 255, onchange, (void *)&(input_par.minarmorProp));
    cv::createTrackbar(NAME(input_par.maxarmorProp), "调参窗口aimbot_2", nullptr, 600, onchange, (void *)&(input_par.maxarmorProp));
    cv::createTrackbar(NAME(input_par.minBigarmorProp), "调参窗口aimbot_2", nullptr, 300, onchange, (void *)&(input_par.minBigarmorProp));
    cv::createTrackbar(NAME(input_par.maxBigarmorProp), "调参窗口aimbot_2", nullptr, 600, onchange, (void *)&(input_par.maxBigarmorProp));

    cv::createTrackbar(NAME(input_par.angleDiff_near), "调参窗口aimbot_3", nullptr, 100, onchange, (void *)&(input_par.angleDiff_near));
    cv::createTrackbar(NAME(input_par.angleDiff_far), "调参窗口aimbot_3", nullptr, 100, onchange, (void *)&(input_par.angleDiff_far));
    cv::createTrackbar(NAME(input_par.minareawidthRatio), "调参窗口aimbot_3", nullptr, 600, onchange, (void *)&(input_par.minareawidthRatio));
    cv::createTrackbar(NAME(input_par.maxareawidthRatio), "调参窗口aimbot_3", nullptr, 600, onchange, (void *)&(input_par.maxareawidthRatio));
    cv::createTrackbar(NAME(input_par.minareaRatio), "调参窗口aimbot_3", nullptr, 600, onchange, (void *)&(input_par.minareaRatio));
    cv::createTrackbar(NAME(input_par.maxareaRatio), "调参窗口aimbot_3", nullptr, 600, onchange, (void *)&(input_par.maxareaRatio));
    cv::createTrackbar(NAME(input_par.area_limit), "调参窗口aimbot_3", nullptr, 600, onchange, (void *)&(input_par.area_limit));
    cv::createTrackbar(NAME(input_par.xcenterdismax), "调参窗口aimbot_3", nullptr, 600, onchange, (void *)&(input_par.xcenterdismax));
    cv::createTrackbar(NAME(input_par.ylengthmin), "调参窗口aimbot_3", nullptr, 10, onchange, (void *)&(input_par.ylengthmin));
    cv::createTrackbar(NAME(input_par.ylengcenterRatio), "调参窗口aimbot_3", nullptr, 10, onchange, (void *)&(input_par.ylengcenterRatio));
    cv::createTrackbar(NAME(input_par.yixaingangleDiff_near), "调参窗口aimbot_3", nullptr, 10, onchange, (void *)&(input_par.yixaingangleDiff_near));
    cv::createTrackbar(NAME(input_par.yixaingangleDiff_far), "调参窗口aimbot_3", nullptr, 10, onchange, (void *)&(input_par.yixaingangleDiff_far));
    cv::createTrackbar(NAME(input_par.kh), "调参窗口aimbot_3", nullptr, 40000, onchange, (void *)&(input_par.kh));

    //设置初值
    cv::setTrackbarPos(NAME(input_par.lowHue), "调参窗口aimbot_1", input_par.lowHue);
    cv::setTrackbarPos(NAME(input_par.lowSat), "调参窗口aimbot_1", input_par.lowSat);
    cv::setTrackbarPos(NAME(input_par.lowVal), "调参窗口aimbot_1", input_par.lowVal);
    cv::setTrackbarPos(NAME(input_par.highHue), "调参窗口aimbot_1", input_par.highHue);
    cv::setTrackbarPos(NAME(input_par.highSat), "调参窗口aimbot_1", input_par.highSat);
    cv::setTrackbarPos(NAME(input_par.highVal), "调参窗口aimbot_1", input_par.highVal);
    cv::setTrackbarPos(NAME(input_par.minlighterarea), "调参窗口aimbot_1", input_par.minlighterarea);
    cv::setTrackbarPos(NAME(input_par.maxlighterarea), "调参窗口aimbot_1", input_par.maxlighterarea);
    cv::setTrackbarPos(NAME(input_par.minlighterProp), "调参窗口aimbot_1", input_par.minlighterProp);
    cv::setTrackbarPos(NAME(input_par.maxlighterProp), "调参窗口aimbot_1", input_par.maxlighterProp);
    cv::setTrackbarPos(NAME(input_par.minAngleError), "调参窗口aimbot_1", input_par.minAngleError);
    cv::setTrackbarPos(NAME(input_par.maxAngleError), "调参窗口aimbot_1", input_par.maxAngleError);

    cv::setTrackbarPos(NAME(input_par.maxarealongRatio), "调参窗口aimbot_2", input_par.maxarealongRatio);
    cv::setTrackbarPos(NAME(input_par.minarealongRatio), "调参窗口aimbot_2", input_par.minarealongRatio);
    cv::setTrackbarPos(NAME(input_par.lightBarAreaDiff), "调参窗口aimbot_2", input_par.lightBarAreaDiff);
    cv::setTrackbarPos(NAME(input_par.armorAngleMin), "调参窗口aimbot_2", input_par.armorAngleMin);
    cv::setTrackbarPos(NAME(input_par.minarmorArea), "调参窗口aimbot_2", input_par.minarmorArea);
    cv::setTrackbarPos(NAME(input_par.maxarmorArea), "调参窗口aimbot_2", input_par.maxarmorArea);
    cv::setTrackbarPos(NAME(input_par.minarmorProp), "调参窗口aimbot_2", input_par.minarmorProp);
    cv::setTrackbarPos(NAME(input_par.maxarmorProp), "调参窗口aimbot_2", input_par.maxarmorProp);
    cv::setTrackbarPos(NAME(input_par.minBigarmorProp), "调参窗口aimbot_2", input_par.minBigarmorProp);
    cv::setTrackbarPos(NAME(input_par.maxBigarmorProp), "调参窗口aimbot_2", input_par.maxBigarmorProp);

    cv::setTrackbarPos(NAME(input_par.angleDiff_near), "调参窗口aimbot_3", input_par.angleDiff_near);
    cv::setTrackbarPos(NAME(input_par.angleDiff_far), "调参窗口aimbot_3", input_par.angleDiff_far);
    cv::setTrackbarPos(NAME(input_par.minareawidthRatio), "调参窗口aimbot_3", input_par.minareawidthRatio);
    cv::setTrackbarPos(NAME(input_par.maxareawidthRatio), "调参窗口aimbot_3", input_par.maxareawidthRatio);
    cv::setTrackbarPos(NAME(input_par.minareaRatio), "调参窗口aimbot_3", input_par.minareaRatio);
    cv::setTrackbarPos(NAME(input_par.maxareaRatio), "调参窗口aimbot_3", input_par.maxareaRatio);
    cv::setTrackbarPos(NAME(input_par.area_limit), "调参窗口aimbot_3", input_par.area_limit);
    cv::setTrackbarPos(NAME(input_par.xcenterdismax), "调参窗口aimbot_3", input_par.xcenterdismax);
    cv::setTrackbarPos(NAME(input_par.ylengthmin), "调参窗口aimbot_3", input_par.ylengthmin);
    cv::setTrackbarPos(NAME(input_par.ylengcenterRatio), "调参窗口aimbot_3", input_par.ylengcenterRatio);
    cv::setTrackbarPos(NAME(input_par.yixaingangleDiff_near), "调参窗口aimbot_3", input_par.yixaingangleDiff_near);
    cv::setTrackbarPos(NAME(input_par.yixaingangleDiff_far), "调参窗口aimbot_3", input_par.yixaingangleDiff_far);
    cv::setTrackbarPos(NAME(input_par.kh), "调参窗口aimbot_3", input_par.kh);

    //onchange(input_par.lowHue, &(input_par.lowHue));
}

void swq::bar_creat_bufferr(swq::m_energy_para &input_par)
{
    cv::createTrackbar(NAME(input_par.center_dis_x), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.center_dis_x));
    cv::createTrackbar(NAME(input_par.center_dis_x), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.center_dis_x));
    cv::createTrackbar(NAME(input_par.fan_armor_distence_max), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.fan_armor_distence_max));
    cv::createTrackbar(NAME(input_par.fan_armor_distence_min), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.fan_armor_distence_min));
    cv::createTrackbar(NAME(input_par.armor_R_distance_max), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.armor_R_distance_max));
    cv::createTrackbar(NAME(input_par.armor_R_distance_min), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.armor_R_distance_min));
    cv::createTrackbar(NAME(input_par.R_noise), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.R_noise));
    cv::createTrackbar(NAME(input_par.Q_noise), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.Q_noise));
    cv::createTrackbar(NAME(input_par.nms_distence_max), "调参窗口buffer", nullptr, 600, onchange, (void *)&(input_par.nms_distence_max));

    //设置初值
    cv::setTrackbarPos(NAME(input_par.center_dis_x), "调参窗口buffer", input_par.center_dis_x);
    cv::setTrackbarPos(NAME(input_par.center_dis_x), "调参窗口buffer", input_par.center_dis_x);
    cv::setTrackbarPos(NAME(input_par.fan_armor_distence_max), "调参窗口buffer", input_par.fan_armor_distence_max);
    cv::setTrackbarPos(NAME(input_par.fan_armor_distence_min), "调参窗口buffer", input_par.fan_armor_distence_min);
    cv::setTrackbarPos(NAME(input_par.armor_R_distance_max), "调参窗口buffer", input_par.armor_R_distance_max);
    cv::setTrackbarPos(NAME(input_par.armor_R_distance_min), "调参窗口buffer", input_par.armor_R_distance_min);
    cv::setTrackbarPos(NAME(input_par.R_noise), "调参窗口buffer", input_par.R_noise);
    cv::setTrackbarPos(NAME(input_par.Q_noise), "调参窗口buffer", input_par.Q_noise);
    cv::setTrackbarPos(NAME(input_par.nms_distence_max), "调参窗口buffer", input_par.nms_distence_max);

    //onchange(input_par.center_dis_x, &(input_par.center_dis_x));
}

swq::m_fiter_para swq::trans_para_to_bar(const swq::fiter_para &input_aimbot)
{
    swq::m_fiter_para bar_aimbot;

    bar_aimbot.lowHue = input_aimbot.lowHue;
    bar_aimbot.lowSat = input_aimbot.lowSat;
    bar_aimbot.lowVal = input_aimbot.lowVal;
    bar_aimbot.highHue = input_aimbot.highHue;
    bar_aimbot.highSat = input_aimbot.highSat;
    bar_aimbot.highVal = input_aimbot.highVal;

    bar_aimbot.minlighterarea = input_aimbot.minlighterarea * 10;
    bar_aimbot.maxlighterarea = input_aimbot.maxlighterarea * 10;
    bar_aimbot.minlighterProp = input_aimbot.minlighterProp;
    bar_aimbot.maxlighterProp = input_aimbot.maxlighterProp;
    bar_aimbot.minAngleError = input_aimbot.minAngleError * 100;
    bar_aimbot.maxAngleError = input_aimbot.maxAngleError * 100;

    bar_aimbot.minarealongRatio = input_aimbot.minarealongRatio * 100;
    bar_aimbot.maxarealongRatio = input_aimbot.maxarealongRatio * 100;
    bar_aimbot.lightBarAreaDiff = input_aimbot.lightBarAreaDiff;
    bar_aimbot.armorAngleMin = input_aimbot.armorAngleMin * 10;
    bar_aimbot.minarmorArea = input_aimbot.minarmorArea;
    bar_aimbot.maxarmorArea = input_aimbot.maxarmorArea;
    bar_aimbot.minarmorProp = input_aimbot.minarmorProp * 100;
    bar_aimbot.maxarmorProp = input_aimbot.maxarmorProp * 100;
    bar_aimbot.minBigarmorProp = input_aimbot.minBigarmorProp * 100;
    bar_aimbot.maxBigarmorProp = input_aimbot.maxBigarmorProp * 100;
    bar_aimbot.angleDiff_near = input_aimbot.angleDiff_near * 10;
    bar_aimbot.angleDiff_far = input_aimbot.angleDiff_far * 10;
    bar_aimbot.minareawidthRatio = input_aimbot.minareawidthRatio * 100;
    bar_aimbot.maxareawidthRatio = input_aimbot.maxareawidthRatio * 100;
    bar_aimbot.minareaRatio = input_aimbot.minareaRatio * 100;
    bar_aimbot.maxareaRatio = input_aimbot.maxareaRatio * 100;
    bar_aimbot.area_limit = input_aimbot.area_limit;
    bar_aimbot.xcenterdismax = input_aimbot.xcenterdismax * 10;
    bar_aimbot.ylengthmin = input_aimbot.ylengthmin;
    bar_aimbot.ylengcenterRatio = input_aimbot.ylengcenterRatio;
    bar_aimbot.yixaingangleDiff_near = input_aimbot.yixaingangleDiff_near;
    bar_aimbot.yixaingangleDiff_far = input_aimbot.yixaingangleDiff_far;
    bar_aimbot.kh = input_aimbot.kh;

    return bar_aimbot;
}

swq::m_energy_para swq::trans_para_to_bar(const swq::energy_para &input_buffer)
{
    swq::m_energy_para bar_buffer;

    bar_buffer.center_dis_x = input_buffer.center_dis_x;
    bar_buffer.center_dis_y = input_buffer.center_dis_y;
    bar_buffer.pass_number_max = input_buffer.pass_number_max;
    bar_buffer.delta_angle_distance = input_buffer.delta_angle_distance;
    bar_buffer.frame_size = input_buffer.frame_size;

    bar_buffer.predict_small = input_buffer.predict_small;
    bar_buffer.predict_big = input_buffer.predict_big;
    bar_buffer.R_noise = input_buffer.R_noise;
    bar_buffer.Q_noise = input_buffer.Q_noise;
    bar_buffer.fan_armor_distence_max = input_buffer.fan_armor_distence_max * 100;
    bar_buffer.fan_armor_distence_min = input_buffer.fan_armor_distence_min * 100;
    bar_buffer.armor_R_distance_max = input_buffer.armor_R_distance_max * 100;
    bar_buffer.armor_R_distance_min = input_buffer.armor_R_distance_max * 100;
    bar_buffer.nms_distence_max = input_buffer.nms_distence_max * 100;

    return bar_buffer;
}
