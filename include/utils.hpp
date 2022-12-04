#pragma once
#include <string>

#include "aimbot.hpp"
#include "energy.hpp"
#include "debug.hpp"
#include "high_num.hpp"

//用于获取变量名称
#define NAME(variable) (#variable)

namespace swq
{
    //能量机关的相关参数-滑动条的回调格式
    struct m_energy_para
    {
        int center_dis_x;
        int center_dis_y;
        int pass_number_max;
        int delta_angle_distance;
        int frame_size;
        int predict_small;
        int predict_big;
        int R_noise;
        int Q_noise;
        int fan_armor_distence_max;
        int fan_armor_distence_min;
        int armor_R_distance_max;
        int armor_R_distance_min;
        int nms_distence_max;
    };

    //自喵的相关参数-滑动条的回调格式
    struct m_fiter_para
    {
        // hsv阈值
        int lowHue;
        int lowSat;
        int lowVal;
        int highHue;
        int highSat;
        int highVal;
        //灯条筛选
        int minlighterarea;
        int maxlighterarea;
        int minlighterProp;
        int maxlighterProp;
        int minAngleError;
        int maxAngleError;
        //装甲板筛选
        int minarealongRatio;
        int maxarealongRatio;
        int lightBarAreaDiff;
        int armorAngleMin;
        int minarmorArea;
        int maxarmorArea;
        int minarmorProp;
        int maxarmorProp;
        int minBigarmorProp;
        int maxBigarmorProp;
        int angleDiff_near;
        int angleDiff_far;
        int minareawidthRatio;
        int maxareawidthRatio;
        int minareaRatio;
        int maxareaRatio;
        int area_limit;
        int xcenterdismax;
        int ylengthmin;
        int ylengcenterRatio;
        int yixaingangleDiff_near;
        int yixaingangleDiff_far;
        //测距
        int kh;
    };

    //读取文件转换成字符串，支持string格式路径和char格式路径
    std::string get_file_str(const std::string &load_path);
    std::string get_file_str(const char *load_path);

    //保存文件，支持字符串和构建完成的数据直接传入
    void save_file(const std::string &load_path, const std::string &file_str);
    void save_file(const char *load_path, const std::string &file_str);

    //判断字符串是否为纯数字组成
    bool is_Numeric(std::string str);

    //计算欧式距离的平方
    high_float EuclideanDistance(float x0, float y0, float x1, float y1);

    //创建自瞄的滑动条
    void bar_creat_aimbot(m_fiter_para &input_par);
    //创建能量机关的滑动条
    void bar_creat_bufferr(m_energy_para &input_par);
    //将滑动条的参数同步成筛选需要的格式
    fiter_para trans_bar_to_para(const m_fiter_para &input_aimbot);
    energy_para trans_bar_to_para(const m_energy_para &input_buffer);
    //将滑动条的参数同步成筛选需要的格式
    m_fiter_para trans_para_to_bar(const fiter_para &input_aimbot);
    m_energy_para trans_para_to_bar(const energy_para &input_buffer);
}

//滑动条回调函数,用于获取滑动条的值
void onchange();