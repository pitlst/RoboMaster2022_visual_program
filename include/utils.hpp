#pragma once
#include <string>

#include "aimbot.hpp"
#include "energy.hpp"
#include "debug.hpp"

//用于获取变量名称
#define NAME(variable) (#variable)

namespace swq
{
    //读取文件转换成字符串，支持string格式路径和char格式路径
    std::string get_file_str(const std::string & load_path);
    std::string get_file_str(const char * load_path);

    //保存文件，支持字符串和构建完成的数据直接传入
    void save_file(const std::string & load_path,const std::string & file_str);
    void save_file(const char * load_path,const std::string & file_str);

    //判断字符串是否为纯数字组成
    bool is_Numeric(std::string str);

    //计算欧式距离
    float EuclideanDistance(float x0, float y0, float x1, float y1);

    class tarch_bar
    {
    public:
        tarch_bar();
        ~tarch_bar();

        //创建自瞄的滑动条
        void bar_creat_aimbot(const swq::GetArmor::fiter_para &input_par);
        //创建能量机关的滑动条
        void bar_creat_bufferr(const swq::GetEnergyMac::energy_para &input_par);
        //获取自瞄滑动条的值
        swq::GetArmor::fiter_para get_bar_value_aimbot();
        //获取能量机关滑动条的值
        swq::GetEnergyMac::energy_para get_bar_value_buffer();
    
        struct energy_para
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
        struct fiter_para
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

    private:
        //将滑动条的参数同步成筛选需要的格式
        void trans_bar_to_par();
        //自喵的相关参数
        swq::GetArmor::fiter_para intput_aimbot;
        //能量机关的相关参数
        swq::GetEnergyMac::buffer_para input_buffer;
        //自喵的相关参数-滑动条的回调格式
        swq::tarch_bar::fiter_para bar_aimbot;
        //能量机关的相关参数-滑动条的回调格式
        swq::tarch_bar::energy_para bar_buffer;
    };
}
