#pragma once
#include <vector>
#include <string>
#include <list>

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"

#include "json.hpp"
#include "utils.hpp"

namespace swq
{
    class GetArmor
    {
    public:
        GetArmor();
        GetArmor(int input_debug, int input_color, int input_mode);
        ~GetArmor();

        void set(int input_debug, int input_color, int input_mode);
        void load_json();
        std::vector<long long int> process(cv::Mat &input_frame);

    private:
        //将输入图像处理成hsv格式，然后通过阈值转换成二值图
        void HSV_Process();
        //处理提取完的二值图，生成存储灯条特征的二维数组
        void GetLightBar();
        //匹配装甲板，生成拟合完成的装甲板列表
        void CombineLightBar_ground();
        //单目测距
        float GetArmorDistance(float s0, float s1);
        //计算欧式距离
        float EuclideanDistance(float x0, float y0, float x1, float y1);

        int debug = 0;
        int mode = 0;
        int color = 0;
        //图像中心像素数
        int img_xCenter;
        int img_yCenter;
        //处理的图像
        cv::Mat frame;
        cv::Mat mask;
        //存储灯条
        std::vector<cv::RotatedRect> *lightBarList;
        //存储装甲板
        std::vector<std::vector<float>> *realCenter_list;
        //存储最后的空间坐标
        struct armor_final
        {
            long long int x;
            long long int y;
            long long int z;
        };
        //筛选需要的参数
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
            double minlighterarea;
            double maxlighterarea;
            double minlighterProp;
            double maxlighterProp;
            double minAngleError;
            double maxAngleError;
            //装甲板筛选
            double minarealongRatio;
            double maxarealongRatio;
            double lightBarAreaDiff;
            double armorAngleMin;
            double minarmorArea;
            double maxarmorArea;
            double minarmorProp;
            double maxarmorProp;
            double minBigarmorProp;
            double maxBigarmorProp;
            double angleDiff_near;
            double angleDiff_far;
            double minareawidthRatio;
            double maxareawidthRatio;
            double minareaRatio;
            double maxareaRatio;
            double area_limit;
            double xcenterdismax;
            double ylengthmin;
            double ylengcenterRatio;
            double yixaingangleDiff_near;
            double yixaingangleDiff_far;
            //测距
            json kh;
        };
        armor_final armor;
        fiter_para load_par;

#ifdef COMPILE_DEBUG
    public:
        // debug下用于返回图像
        std::list<cv::Mat> debug_frame();

    private:
        // debug下更新筛选参数
        void updata_argument();
        // debug下json文件里的参数
        void update_json();

        //加载的参数
        json load_armor;
        json load_sentry;
        json load_camera;
        //参数文件路径
        std::string file_name;
#endif
    };
}
