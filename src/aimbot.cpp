#include "aimbot.hpp"
#include "logger.hpp"
#include "debug.hpp"
#include "utils.hpp"
#include "high_num.hpp"

using namespace swq;

GetArmor::GetArmor()
{
    lightBarList = new std::vector<cv::RotatedRect>();
    realCenter_list = new std::vector<std::vector<float>>();
}

GetArmor::~GetArmor()
{
    delete lightBarList;
    delete realCenter_list;
}

void GetArmor::set(bool input_color)
{
    color = input_color;
    load_json();
}

void GetArmor::load_json()
{
    json load_armor;
    json load_camera;

    load_armor.parse(get_file_str(PATH_ARMOR_JSON));
    load_camera.parse(get_file_str(PATH_CAMERA_JSON));
    log_debug("color is :", color);
    if (color == false)
    {
        load_par.lowHue = load_armor["ImageProcess_red"]["hsvPara_low"][0];
        load_par.lowSat = load_armor["ImageProcess_red"]["hsvPara_low"][1];
        load_par.lowVal = load_armor["ImageProcess_red"]["hsvPara_low"][2];
        load_par.highHue = load_armor["ImageProcess_red"]["hsvPara_high"][0];
        load_par.highSat = load_armor["ImageProcess_red"]["hsvPara_high"][1];
        load_par.highVal = load_armor["ImageProcess_red"]["hsvPara_high"][2];
    }
    else if (color == true)
    {
        load_par.lowHue = load_armor["ImageProcess_blue"]["hsvPara_low"][0];
        load_par.lowSat = load_armor["ImageProcess_blue"]["hsvPara_low"][1];
        load_par.lowVal = load_armor["ImageProcess_blue"]["hsvPara_low"][2];
        load_par.highHue = load_armor["ImageProcess_blue"]["hsvPara_high"][0];
        load_par.highSat = load_armor["ImageProcess_blue"]["hsvPara_high"][1];
        load_par.highVal = load_armor["ImageProcess_blue"]["hsvPara_high"][2];
    }
    else
    {
        log_error("unknow color num");
        load_par.lowHue = -1;
        load_par.lowSat = -1;
        load_par.lowVal = -1;
        load_par.highHue = -1;
        load_par.highSat = -1;
        load_par.highVal = -1;
    }

    load_par.minlighterarea = load_armor["ArmorFind"]["minlighterarea"];
    load_par.maxlighterarea = load_armor["ArmorFind"]["maxlighterarea"];
    load_par.minlighterProp = load_armor["ArmorFind"]["minlighterProp"];
    load_par.minAngleError = load_armor["ArmorFind"]["minAngleError"];
    load_par.maxAngleError = load_armor["ArmorFind"]["maxAngleError"];

    load_par.minarealongRatio = load_armor["ArmorFind"]["minarealongRatio"];
    load_par.maxarealongRatio = load_armor["ArmorFind"]["maxarealongRatio"];
    load_par.lightBarAreaDiff = load_armor["ArmorFind"]["lightBarAreaDiff"];
    load_par.armorAngleMin = load_armor["ArmorFind"]["armorAngleMin"];
    load_par.minarmorArea = load_armor["ArmorFind"]["minarmorArea"];
    load_par.maxarmorArea = load_armor["ArmorFind"]["maxarmorArea"];
    load_par.minarmorProp = load_armor["ArmorFind"]["minarmorProp"];
    load_par.maxarmorProp = load_armor["ArmorFind"]["maxarmorProp"];
    load_par.minBigarmorProp = load_armor["ArmorFind"]["minBigarmorProp"];
    load_par.maxBigarmorProp = load_armor["ArmorFind"]["maxBigarmorProp"];
    load_par.angleDiff_near = load_armor["ArmorFind"]["angleDiff_near"];
    load_par.angleDiff_far = load_armor["ArmorFind"]["angleDiff_far"];
    load_par.minareawidthRatio = load_armor["ArmorFind"]["minareawidthRatio"];
    load_par.maxareawidthRatio = load_armor["ArmorFind"]["maxareawidthRatio"];
    load_par.minareaRatio = load_armor["ArmorFind"]["minareaRatio"];
    load_par.maxareaRatio = load_armor["ArmorFind"]["maxareaRatio"];
    load_par.area_limit = load_armor["ArmorFind"]["area_limit"];
    load_par.xcenterdismax = load_armor["ArmorFind"]["xcenterdismax"];
    load_par.ylengthmin = load_armor["ArmorFind"]["ylengthmin"];
    load_par.ylengcenterRatio = load_armor["ArmorFind"]["ylengcenterRatio"];
    load_par.yixaingangleDiff_near = load_armor["ArmorFind"]["yixaingangleDiff_near"];
    load_par.yixaingangleDiff_far = load_armor["ArmorFind"]["yixaingangleDiff_far"];

    load_par.kh = load_armor["ArmorFind"]["kh"];

    img_xCenter = load_camera["Aimbot"]["width"].Int() / 2;
    img_yCenter = load_camera["Aimbot"]["height"].Int() / 2;
    load_armor.clear();
    load_camera.clear();
}

std::vector<int> GetArmor::process(cv::Mat &input_frame)
{
    frame = input_frame;
    start_label = 1;
    HSV_Process();
    GetLightBar();
    CombineLightBar_ground();
    std::vector<int> xyz = {armor.x, armor.y, armor.z};
    return xyz;
}

void GetArmor::HSV_Process()
{
    // log_debug("?????????HSV_Process");
    cv::Mat temp;
    cv::cvtColor(frame, temp, cv::COLOR_RGB2HSV);
    cv::inRange(temp, cv::Scalar(load_par.lowHue, load_par.lowSat, load_par.lowVal), cv::Scalar(load_par.highHue, load_par.highSat, load_par.highVal), mask);
}

void GetArmor::GetLightBar()
{
    // log_debug("?????????GetLightBar");
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_L1);
    if (contours.size())
    {
        lightBarList->clear();
    }
    else
    {
        for (auto &ch : contours)
        {
            if (ch.size() > 5)
            {
                cv::RotatedRect rotate = cv::fitEllipse(ch);
                if (rotate.size.height <= 0 OR rotate.size.width <= 0)
                {
                    continue;
                }
                auto rectProp = rotate.size.width / rotate.size.height;
                auto rectArea = rotate.size.width * rotate.size.height;
                auto angleHori = abs(int(rotate.angle) % 180);
                if (angleHori < load_par.minAngleError OR angleHori > load_par.maxAngleError)
                {
                    continue;
                }
                else if (rectProp < load_par.minlighterProp OR rectProp > load_par.maxlighterProp)
                {
                    continue;
                }
                else if (rectArea < load_par.minlighterarea OR rectArea > load_par.maxlighterarea)
                {
                    continue;
                }
                lightBarList->emplace_back(rotate);
            }
        }
    }
}

void GetArmor::CombineLightBar_ground()
{
    // log_debug("?????????CombineLightBar_ground");
    auto x = -1.0;
    auto y = -1.0;
    auto z = -1.0;
    if (lightBarList->size() < 2)
    {
        //log_debug("NO Armor");
        return;
    }
    for (auto it_x = lightBarList->begin(); it_x != lightBarList->end(); it_x++)
    {
        for (auto it_y = lightBarList->begin(); it_y != lightBarList->end(); it_y++)
        {
            auto xcenterdis = abs((*it_x).center.x - (*it_y).center.x);                                               //??????????????????
            auto ycenterdis = abs((*it_x).center.y - (*it_y).center.y);                                               //??????????????????
            auto angleDiff_caculate = abs((*it_x).angle - (*it_y).angle);                                             //???????????????
            auto areaDiff = abs((*it_x).size.area() - (*it_y).size.area());                                           //???????????????
            auto areaRatio = (*it_x).size.area() / (*it_y).size.area();                                               //???????????????
            auto arealongRatio = (*it_x).size.width / (*it_y).size.width;                                             //???????????????
            auto areawidthRatio = (*it_x).size.height / (*it_y).size.height;                                          //???????????????
            auto xCenter = ((*it_x).center.x + (*it_y).center.x) / 2;                                                 //???????????????x???
            auto yCenter = ((*it_x).center.y + (*it_y).center.y) / 2;                                                 //???????????????y???
            auto ylength = ((*it_x).size.width + (*it_y).size.width) / 2;                                             //?????????????????????
            auto xlength = (float)EuclideanDistance((*it_x).center.x, (*it_x).center.y, (*it_y).center.x, (*it_y).center.y); //?????????????????????
            auto armorProp = xlength / ylength;                                                                       //??????????????????
            auto armorArea = xlength * ylength;                                                                       //???????????????
            auto angle = abs(((*it_x).center.y - (*it_y).center.y) / ((*it_x).center.x - (*it_y).center.x));          //???????????????

            float angleDiff;
            float yixaingangleDiff;
            //??????????????????????????????
            if ((*it_x).size.area() + (*it_y).size.area() < load_par.area_limit)
            {
                angleDiff = load_par.angleDiff_near;
                yixaingangleDiff = load_par.yixaingangleDiff_near;
            }
            else
            {
                angleDiff = load_par.angleDiff_far;
                yixaingangleDiff = load_par.yixaingangleDiff_far;
            }
            //??????????????????????????????????????????
            if ((*it_x).center.x > (*it_y).center.y)
            {
                angle = angle * 180 / M_PI;
            }
            else
            {
                angle = -angle * 180 / M_PI;
            }
            //??????????????????????????????
            if (arealongRatio < load_par.minarealongRatio OR arealongRatio > load_par.maxarealongRatio)
            {
                continue;
            }
            //?????????????????????????????????
            if (areawidthRatio < load_par.minareawidthRatio OR areawidthRatio > load_par.maxareawidthRatio)
            {
                continue;
            }
            //???????????????????????????
            if ((angleDiff_caculate > angleDiff AND angleDiff_caculate < 180 - angleDiff) OR (angleDiff_caculate > 90 AND angleDiff_caculate < 180 - yixaingangleDiff))
            {
                continue;
            }
            //???????????????????????????
            if (areaRatio > load_par.maxareaRatio OR areaRatio < load_par.minareaRatio)
            {
                continue;
            }
            //????????????????????????
            if (xcenterdis < load_par.xcenterdismax)
            {
                continue;
            }
            //???????????????????????????
            if (areaDiff > load_par.lightBarAreaDiff)
            {
                continue;
            }
            //???????????????????????????
            if (abs(angle) > load_par.armorAngleMin)
            {
                continue;
            }
            //???????????????????????????
            if (ylength < load_par.ylengthmin)
            {
                continue;
            }
            //????????????????????????????????????
            if (armorArea < load_par.minarmorArea OR armorArea > load_par.maxarmorArea)
            {
                continue;
            }
            //????????????y????????????????????????
            if (ycenterdis > ylength * load_par.ylengcenterRatio)
            {
                continue;
            }
            //???????????????????????????????????????
            if ((armorProp < load_par.minarmorProp OR armorProp > load_par.maxarmorProp) AND (armorProp < load_par.minBigarmorProp OR armorProp > load_par.maxBigarmorProp))
            {
                continue;
            }
            auto z = GetArmorDistance((*it_x).size.width, (*it_y).size.width);
            std::vector<float> temp = {xCenter, yCenter, xlength, ylength, z, angle};
            realCenter_list->emplace_back(temp);
        }
        float temp_distence = -1;
        if (realCenter_list->size() > 0)
        {
            for (auto &realCenter : *realCenter_list)
            {
                auto armor_imgcenter_distence = (double)EuclideanDistance(realCenter[0], realCenter[1], img_xCenter, img_yCenter);
                if (temp_distence > armor_imgcenter_distence OR temp_distence == -1)
                    temp_distence = armor_imgcenter_distence;
                x = realCenter[0];
                y = realCenter[1];
                z = realCenter[5];
            }
        }
        if (temp_distence == -1)
        {
            x = -1.0;
            y = -1.0;
            z = -1.0;
        }
    }
    //?????????????????????
    armor.x = x;
    armor.y = y;
    armor.z = z;
}

float GetArmor::GetArmorDistance(float s0, float s1)
{
    return load_par.kh / (s0 + s1);
}

#ifdef COMPILE_DEBUG
std::list<cv::Mat> GetArmor::debug_frame(cv::Mat &input_frame)
{
    std::list<cv::Mat> temp;
    cv::Mat mask;
    //??????????????????????????????????????????
    cv::Mat frame_temp = input_frame.clone();
    cv::cvtColor(frame_temp, frame_temp, cv::COLOR_BGR2HSV);
    cv::inRange(frame_temp, cv::Scalar(load_par.lowHue, load_par.lowSat, load_par.lowVal), cv::Scalar(load_par.highHue, load_par.highSat, load_par.highVal), mask);
    //????????????
    for (auto &ch : (*lightBarList))
    {
        cv::ellipse(input_frame, ch.center, ch.size, ch.angle, 0, 360, cv::Scalar(LIGHTBAR_COLOR), FRAME_THICKNESS);
    }
    for (auto &ch : (*realCenter_list))
    {
        cv::ellipse(input_frame, cv::Point(ch[2], ch[3]), cv::Size(ch[0], ch[1]), ch[5], 0, 360, cv::Scalar(ARMOR_COLOR), FRAME_THICKNESS);
    }
    //????????????????????????
    std::string text = "distance = ";
    text += armor.z;
    cv::putText(input_frame, text, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(TEXT_COLOR), FRAME_THICKNESS);
    temp.emplace_back(input_frame);
    temp.emplace_back(mask);
    return temp;
}

void GetArmor::updata_argument(const fiter_para &input)
{
    load_par = input;
}

fiter_para GetArmor::get_argument()
{
    return load_par;
}

void GetArmor::update_json(const std::string &filename)
{
    json temp_load;
    temp_load.parse(get_file_str(filename));

    if (color == false)
    {
        temp_load["ImageProcess_red"]["hsvPara_low"][0] = load_par.lowHue;
        temp_load["ImageProcess_red"]["hsvPara_low"][1] = load_par.lowSat;
        temp_load["ImageProcess_red"]["hsvPara_low"][2] = load_par.lowVal;
        temp_load["ImageProcess_red"]["hsvPara_high"][0] = load_par.highHue;
        temp_load["ImageProcess_red"]["hsvPara_high"][1] = load_par.highSat;
        temp_load["ImageProcess_red"]["hsvPara_high"][2] = load_par.highVal;
    }
    else if (color == true)
    {
        temp_load["ImageProcess_blue"]["hsvPara_low"][0] = load_par.lowHue;
        temp_load["ImageProcess_blue"]["hsvPara_low"][1] = load_par.lowSat;
        temp_load["ImageProcess_blue"]["hsvPara_low"][2] = load_par.lowVal;
        temp_load["ImageProcess_blue"]["hsvPara_high"][0] = load_par.highHue;
        temp_load["ImageProcess_blue"]["hsvPara_high"][1] = load_par.highSat;
        temp_load["ImageProcess_blue"]["hsvPara_high"][2] = load_par.highVal;
    }

    temp_load["ArmorFind"]["minlighterarea"] = load_par.minlighterarea;
    temp_load["ArmorFind"]["maxlighterarea"] = load_par.maxlighterarea;
    temp_load["ArmorFind"]["minlighterProp"] = load_par.minlighterProp;
    temp_load["ArmorFind"]["minAngleError"] = load_par.minAngleError;
    temp_load["ArmorFind"]["maxAngleError"] = load_par.maxAngleError;

    temp_load["ArmorFind"]["minarealongRatio"] = load_par.minarealongRatio;
    temp_load["ArmorFind"]["maxarealongRatio"] = load_par.maxarealongRatio;
    temp_load["ArmorFind"]["lightBarAreaDiff"] = load_par.lightBarAreaDiff;
    temp_load["ArmorFind"]["armorAngleMin"] = load_par.armorAngleMin;
    temp_load["ArmorFind"]["minarmorArea"] = load_par.minarmorArea;
    temp_load["ArmorFind"]["maxarmorArea"] = load_par.maxarmorArea;
    temp_load["ArmorFind"]["minarmorProp"] = load_par.minarmorProp;
    temp_load["ArmorFind"]["maxarmorProp"] = load_par.maxarmorProp;
    temp_load["ArmorFind"]["minBigarmorProp"] = load_par.minBigarmorProp;
    temp_load["ArmorFind"]["maxBigarmorProp"] = load_par.maxBigarmorProp;
    temp_load["ArmorFind"]["angleDiff_near"] = load_par.angleDiff_near;
    temp_load["ArmorFind"]["angleDiff_far"] = load_par.angleDiff_far;
    temp_load["ArmorFind"]["minareawidthRatio"] = load_par.minareawidthRatio;
    temp_load["ArmorFind"]["maxareawidthRatio"] = load_par.maxareawidthRatio;
    temp_load["ArmorFind"]["minareaRatio"] = load_par.minareaRatio;
    temp_load["ArmorFind"]["maxareaRatio"] = load_par.maxareaRatio;
    temp_load["ArmorFind"]["area_limit"] = load_par.area_limit;
    temp_load["ArmorFind"]["xcenterdismax"] = load_par.xcenterdismax;
    temp_load["ArmorFind"]["ylengthmin"] = load_par.ylengthmin;
    temp_load["ArmorFind"]["ylengcenterRatio"] = load_par.ylengcenterRatio;
    temp_load["ArmorFind"]["yixaingangleDiff_near"] = load_par.yixaingangleDiff_near;
    temp_load["ArmorFind"]["yixaingangleDiff_far"] = load_par.yixaingangleDiff_far;

    temp_load["ArmorFind"]["kh"] = load_par.kh;

    save_file(filename, temp_load.str());
    temp_load.clear();
}


#endif