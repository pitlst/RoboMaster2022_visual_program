#pragma once
#include <string>
#include <memory>

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include "openvino/openvino.hpp"

#include "json.hpp"

namespace swq
{
    class GetEnergyMac
    {
    public:
        GetEnergyMac();
        GetEnergyMac(int input_debug, int input_color, int input_mode);
        ~GetEnergyMac();

        void set(int input_debug, int input_color, int input_mode);
        void load_json();
        std::vector<long long int> process(cv::Mat &input_frame);

    private:
        // openvino的初始化
        void openvino_init();
        // 将cv::Mat转换为ov::Tensor
        ov::Tensor trans_mat_to_tensor(cv::Mat & img);

        int debug = 0;
        int mode = 0;
        int color = 0;
        //图像中心像素数
        int img_xCenter;
        int img_yCenter;
        //处理的图像
        cv::Mat frame;
        cv::Mat mask;
        //用于推演的openvino 运行时核心
        ov::Core core;
        //模型推演请求
        ov::InferRequest infer_request;
        //加载进来的模型,使用智能指针管理
        std::shared_ptr<ov::Model> model;
        //创建的输入管道
        ov::Output<const ov::Node> input_port;
        //存储最后的空间坐标
        struct armor_final
        {
            long long int x;
            long long int y;
            long long int z;
        };
        //能量机关需要的参数
        struct energy_para
        {
            int center_dis_x;
            int center_dis_y;
            int pass_number_max;
            int delta_angle_distance;
            int frame_size;
            double predict_small;
            double predict_big;
            double R_noise;
            double Q_noise;
            double fan_armor_distence_max;
            double fan_armor_distence_min;
            double armor_R_distance_max;
            double armor_R_distance_min;
            double nms_distence_max;
            std::string model_path;
        };
        //模型相关的参数
        struct model_para
        {
            int input_n;
            int input_c;
            int input_h;
            int input_w;
        };

        armor_final armor;
        energy_para energy_par;
        model_para model_par;

#ifdef COMPILE_DEBUG
    public:
        // debug下用于返回图像
        std::list<cv::Mat> debug_frame();

    private:
        // debug下更新筛选参数
        void updata_argument();
        // debug下json文件里的参数
        void update_json();
#endif
    };
}