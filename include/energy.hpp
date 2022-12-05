#pragma once
#include <string>
#include <deque>

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include "openvino/openvino.hpp"

#include "high_num.hpp"
#include "json.hpp"
#include "debug.hpp"

namespace swq
{
    //存储目标的相关参数
    struct buffer_para
    {
        double delta_angle;
        double f_time;
        //这里的待击打点是一个三个点的空间坐标
        std::vector<double> armor_point;
        //这里的center是一个网络输出的完整列向量
        std::vector<double> center;
    };
    //能量机关需要的筛选参数
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
    //向量的形状
    struct model_shape
    {
        int n;
        int c;
        int w;
        int h;
    };
    //模型相关的参数
    struct model_para
    {
        model_shape input;
        model_shape stride8;
        model_shape stride16;
        model_shape stride32;
        std::string type_str;
    };

    class GetEnergyMac final
    {
    public:
        GetEnergyMac();
        GetEnergyMac(int input_mode);
        ~GetEnergyMac() = default;

        void set(int input_mode);
        std::vector<int> process(cv::Mat &input_frame, double f_time);

#ifdef COMPILE_DEBUG
        // debug下用于返回图像
        std::list<cv::Mat> debug_frame(cv::Mat &input_frame);
        // debug下用于获取参数
        energy_para get_argument();
        // debug下更新筛选参数
        void updata_argument(const energy_para &input);
        // debug下json文件里的参数
        void update_json(const std::string &filename);
#endif

    private:
        //读取参数
        void load_json();
        //openvino的初始化
        void openvino_init();
        //获取模型参数
        void model_para_init();
        //卡尔曼滤波器初始化
        void kalmanfilter_init();
        //将cv::Mat转换为ov::Tensor,包括图像相关的前处理,仅支持FP32精度
        void trans_mat_to_tensor();
        //将模型输出的tensor转换为Matrix,并按照对应形状组织起来,仅支持FP32精度
        void trans_tansor_to_matrix(std::vector<ov::Tensor> out_tenosr);
        // sigmoid函数
        double sigmoid(double input_num);
        //筛选中心
        void center_filter(buffer_para &buffer);
        //筛选被击打的装甲板
        void energy_filter(buffer_para &buffer);
        //预测装甲板位置
        std::vector<int> angle_predicted();
        //判断符的旋转方向
        void judge_rotate_direct();
        //笛卡尔坐标与极坐标转换
        double cartesian_to_polar(buffer_para &buffer);
        //小符预测
        double energymac_forecast_small(double angle);
        //大符预测
        double energymac_forecast_big(double angle);
        //检查维护目标历史记录
        void vector_protect_process();

        //阻止构造一些常用的特定重载函数
        void  operator <<(const GetEnergyMac&) = delete;
        void  operator >>(const GetEnergyMac&) = delete;
        void  operator =(const GetEnergyMac&) = delete;
        void  operator +(const GetEnergyMac&) = delete;
        void  operator -(const GetEnergyMac&) = delete;
        void  operator *(const GetEnergyMac&) = delete;
        void  operator /(const GetEnergyMac&) = delete;
        void  operator ++() = delete;
        void  operator --() = delete;
        void  operator &(const GetEnergyMac&) = delete;

        //检测的开始时间
        double begin_time = 0;
        //能量机关的旋转模式
        int mode = SMALL_ENERGY_BUFFER;
        //能量机关的旋转方向,-1顺时针,1逆时针
        int detect = 0;
        //能量机关的半径
        double hitDis = 0;
        //处理的图像
        cv::Mat frame;
        //指向处理图像的指针
        std::shared_ptr<float> _data;
        //读取的模型
        std::shared_ptr<ov::Model> model;
        //编译好,已加载到设备的模型
        ov::CompiledModel compiled_model;
        //模型推演请求
        ov::InferRequest infer_request;
        //根据模型结构定义的输入输出管道,
        ov::Output<const ov::Node> input_port;
        ov::Output<const ov::Node> output_port_stride8;
        ov::Output<const ov::Node> output_port_stride16;
        ov::Output<const ov::Node> output_port_stride32;
        //输入模型的向量
        ov::Tensor input_tensor;
        //模型输出
        std::vector<std::vector<double>> output_res;
        //历史目标记录,使用队列存储
        std::deque<buffer_para> armor;
        //存储的筛选参数
        energy_para energy_par;
        model_para model_par;
        //opencv卡尔曼滤波器
        cv::KalmanFilter KF;
        //初始测量值x'(0)
        cv::Mat measurement;
        
        //常用高精度整数
        high_float nms_limit;
        high_float armor_R_limit_min;
        high_float armor_R_limit_max;
        high_float fan_armor_limit_min;
        high_float fan_armor_limit_max;


        //模型的anchor直接写死在这里了,如果需要更改训练程序中的anchor，请配合更改这里
        const float anchors[3][6] = {{4, 5, 8, 10, 13, 16}, {23, 29, 43, 55, 73, 105}, {146, 217, 231, 300, 335, 433}};
        //特征图尺寸,顺序是根据模型来的，更改这里需要配合更改训练程序
        const float stride[3] = {8.0, 16.0, 32.0};
        //模型类别,同样需要同步更改训练程序
        const int classes = 3;
    };

}