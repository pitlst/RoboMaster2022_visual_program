#include "energy.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "debug.hpp"

#include <cmath>

using namespace swq;

GetEnergyMac::GetEnergyMac()
{
    load_json();
    openvino_init();
}

GetEnergyMac::GetEnergyMac(int input_debug, int input_color, int input_mode)
{
    debug = input_debug;
    color = input_color;
    mode = input_mode;
    load_json();
    openvino_init();
}

GetEnergyMac::~GetEnergyMac()
{
}

void GetEnergyMac::set(int input_debug, int input_color, int input_mode)
{
    //检查是否有更改
    bool label = false;
    if (debug != input_debug)
    {
        debug = input_debug;
        label = true;
    }
    if (color != input_color)
    {
        color = input_color;
        label = true;
    }
    if (mode != input_mode)
    {
        mode = input_mode;
        label = true;
    }
    //如果更改重新配置
    if (label)
    {
        load_json();
        openvino_init();
    }
}

void GetEnergyMac::load_json()
{
    json load_energy;
    json load_camera;
    load_energy.parse(get_file_str(PATH_ENERGY_JSON));
    load_camera.parse(get_file_str(PATH_CAMERA_JSON));

    energy_par.center_dis_y = load_energy["EnergyFind"]["center_dis_y"];
    energy_par.center_dis_x = load_energy["EnergyFind"]["center_dis_x"];
    energy_par.pass_number_max = load_energy["EnergyFind"]["pass_number_max"];
    energy_par.delta_angle_distance = load_energy["EnergyFind"]["delta_angle_distance"];
    energy_par.predict_small = load_energy["EnergyFind"]["predict_small"];
    energy_par.predict_big = load_energy["EnergyFind"]["predict_big"];
    energy_par.R_noise = load_energy["EnergyFind"]["R_noise"];
    energy_par.Q_noise = load_energy["EnergyFind"]["Q_noise"];
    energy_par.fan_armor_distence_max = load_energy["EnergyFind"]["fan_armor_distence_max"];
    energy_par.fan_armor_distence_min = load_energy["EnergyFind"]["fan_armor_distence_min"];
    energy_par.armor_R_distance_max = load_energy["EnergyFind"]["armor_R_distance_max"];
    energy_par.armor_R_distance_min = load_energy["EnergyFind"]["armor_R_distance_min"];
    energy_par.nms_distence_max = load_energy["EnergyFind"]["nms_distence_max"];
    energy_par.model_path = PATH_MODEL_FILE;

    energy_par.frame_size = load_camera["Energy_mac"]["width"];

    load_energy.clear();
    load_camera.clear();
}

std::vector<long long int> GetEnergyMac::process(cv::Mat &input_frame)
{
    frame = input_frame;
    trans_mat_to_tensor();
    infer_request.set_input_tensor(input_tensor);
    infer_request.infer();
    //提取模型处理完的向量
    auto output_tensor_0 = infer_request.get_tensor(output_port_stride8);
    auto output_tensor_1 = infer_request.get_tensor(output_port_stride16);
    auto output_tensor_2 = infer_request.get_tensor(output_port_stride32);
    //把向量保存到矩阵里
    trans_tansor_to_matrix(output_tensor_0, output_tensor_1, output_tensor_2);
    res_label = 0;
    std::vector<long long int> temp = {0,0,0};
    return temp;
}

void GetEnergyMac::openvino_init()
{
    //初始化openvino核心
    ov::Core core;
    //读取模型
    auto model = core.read_model(energy_par.model_path);
    //将模型加载到设备
#ifdef GPU_INFER
    compiled_model = core.compile_model(model, "GPU");
#else
    compiled_model = core.compile_model(model, "CPU");
#endif
    //获取推理请求
    infer_request = compiled_model.create_infer_request();
    //获取网络的输入输出形状
    input_port = compiled_model.input(0);
    output_port_stride8 = compiled_model.output(0);
    output_port_stride16 = compiled_model.output(1);
    output_port_stride32 = compiled_model.output(2);
    //获取网络精度
    model_par.type_str = input_port.get_element_type().get_type_name();
    //暂时只支持fp32精度,进行一下检查
    if (model_par.type_str != "f32")
    {
        throw std::logic_error("抱歉，暂时不支持该精度");
    }
    //初始化模型参数
    model_para_init();
}

void GetEnergyMac::model_para_init()
{
    model_par.input_n = input_port.get_shape()[0];
    model_par.input_c = input_port.get_shape()[1];
    model_par.input_h = input_port.get_shape()[2];
    model_par.input_w = input_port.get_shape()[3];
    model_par.stride8.n = output_port_stride8.get_shape()[1];
    model_par.stride8.c = output_port_stride8.get_shape()[2];
    model_par.stride8.w = output_port_stride8.get_shape()[3];
    model_par.stride8.h = output_port_stride8.get_shape()[4];
    model_par.stride16.n = output_port_stride16.get_shape()[1];
    model_par.stride16.c = output_port_stride16.get_shape()[2];
    model_par.stride16.w = output_port_stride16.get_shape()[3];
    model_par.stride16.h = output_port_stride16.get_shape()[4];
    model_par.stride32.n = output_port_stride32.get_shape()[1];
    model_par.stride32.c = output_port_stride32.get_shape()[2];
    model_par.stride32.w = output_port_stride32.get_shape()[3];
    model_par.stride32.h = output_port_stride32.get_shape()[4];

    //如果模型不变，这个矩阵的形状可以写死
    //矩阵的纵向长度7的含义为：
    //置信度(图像目标*类别置信度)、类别、框的中心x、框的中心y、框的长w、框的宽h(这四个都相对于图像整体)、框的角度angle
    output_res.resize(model_par.stride8.n*model_par.stride8.c*model_par.stride8.w +\
                    model_par.stride16.n*model_par.stride16.c*model_par.stride16.w + \
                    model_par.stride32.n*model_par.stride32.c*model_par.stride32.w ,\

                    7);

    
}

void GetEnergyMac::trans_mat_to_tensor()
{
    cv::Mat temp;
    //图像归一化
    cv::normalize(frame, temp, 1.0, 0.0, cv::NORM_MINMAX);
    int width = temp.cols;
    int height = temp.rows;
    //图像不符合大小则进行缩放
    if (width != model_par.input_w || height != model_par.input_h)
    {
        cv::resize(temp, temp, cv::Size(model_par.input_w, model_par.input_h));
        width = temp.cols;
        height = temp.rows;
    }
    size_t size = width * height * temp.channels();
    std::shared_ptr<float> _data;
    _data.reset(new float[size], std::default_delete<float[]>());//按照图像大小初始化指向的位置
    cv::Mat resized(cv::Size(width, height), temp.type(), _data.get());
    cv::resize(temp, resized, cv::Size(width, height));          //这里借用opencv自己的构造，让数据填充到_data指向的的位置
    input_tensor = ov::Tensor(input_port.get_element_type(), input_port.get_shape(), _data.get());
}

void GetEnergyMac::trans_tansor_to_matrix(ov::Tensor & out_tensor_8, ov::Tensor & out_tensor_16, ov::Tensor & out_tensor_32)
{
    const float *input = out_tensor_8.data<const float>();
    float ratioh = (float)frame.rows / this->inpHeight, ratiow = (float)frame.cols / this->inpWidth;
    // for (size_t i = 0; i < out_tensor_8.get_shape()[1]*out_tensor_8.get_shape()[2]*out_tensor_8.get_shape()[3]; i++)
    // {
    //     input += 4;
    //     //提取该图上有目标的概率,超过阈值再保存
    //     if (sigmoid(*input) > MODEL_THRESHOLD)
    //     {
    //         input -= 4;
    //         auto x = sigmoid(*input) * 2.0f -0.5f;
    //         input++;
    //         auto y = sigmoid(*input) * 2.0f -0.5f;
    //         input++;
    //         auto w = pow(sigmoid(*input) * 2.0f, 2.0f);
    //         input++;
    //         auto h = pow(sigmoid(*input) * 2.0f, 2.0f);
    //         input++;
    //         auto score = sigmoid(*input);
    //         input++;
    //         auto cls_0 = sigmoid(*input);
    //         input++;
    //         auto cls_1 = sigmoid(*input);
    //         input++;
    //         auto cls_2 = sigmoid(*input);
    //         input++;
    //         auto angle = sigmoid(*input);
    //         input++;
    //         res_label++;
    //     }
    // }
}

float GetEnergyMac::sigmoid(float input_num)
{
    if (input_num < -10)
    {
        //防止数据溢出
        input_num = -10;
    }
    return 1.0/(1+exp(-input_num));
}
