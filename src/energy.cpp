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
    //由于模型的结构是16,32,8,所以这里把最后一个移到前面来,同时组织成容器，方便循环时调用
    std::vector<ov::Tensor> output_tensor;
    output_tensor.emplace_back(output_tensor_2);
    output_tensor.emplace_back(output_tensor_0);
    output_tensor.emplace_back(output_tensor_1);
    //把向量保存到矩阵里
    trans_tansor_to_matrix(output_tensor);
    res_label = 0;
    std::vector<long long int> temp = {0, 0, 0};
    return temp;
}

void GetEnergyMac::center_filter()
{
#ifdef CENTER_FILTER_MODE 0
    
#elif CENTER_FILTER_MODE 1
#elif CENTER_FILTER_MODE 2
#elif CENTER_FILTER_MODE 3
#endif
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
    output_res.resize(model_par.stride8.n * model_par.stride8.c * model_par.stride8.w +
                          model_par.stride16.n * model_par.stride16.c * model_par.stride16.w +
                          model_par.stride32.n * model_par.stride32.c * model_par.stride32.w,
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
    _data.reset(new float[size], std::default_delete<float[]>()); //按照图像大小初始化指向的位置
    cv::Mat resized(cv::Size(width, height), temp.type(), _data.get());
    cv::resize(temp, resized, cv::Size(width, height)); //这里借用opencv自己的构造，让数据填充到_data指向的的位置
    input_tensor = ov::Tensor(input_port.get_element_type(), input_port.get_shape(), _data.get());
}

void GetEnergyMac::trans_tansor_to_matrix(std::vector<ov::Tensor> out_tenosr)
{

    float ratioh = float(frame.rows) / model_par.input_n;
    float ratiow = float(frame.cols) / model_par.input_w;
    //遍历尺度
    for (auto n = 0; n < 3; n++)
    {
        const float *input = out_tenosr[n].data<const float>();
        int num_grid_x = model_par.input_n / stride[n];
        int num_grid_y = model_par.input_w / stride[n];
        int area = num_grid_x * num_grid_y;
        //遍历anchor
        for (auto q = 0; q < 3; q++)
        {
            const float anchor_w = anchors[n][q * 2];
            const float anchor_h = anchors[n][q * 2 + 1];
            input += q * area * (classes + 6);
            //遍历每一个生成框
            for (auto i = 0; i < num_grid_y; i++)
            {
                for (auto j = 0; j < num_grid_x; j++)
                {
                    input += 4;
                    auto score = sigmoid(*input);
                    input++;
                    //提取该图上有目标的概率,超过阈值再保存
                    if (score > MODEL_THRESHOLD)
                    {
                        float max_class_score = 0;
                        int class_id = 0;
                        //获取类型
                        for (auto c = 0; c < classes; c++)
                        {
                            auto class_socre = sigmoid(*input);
                            input++;
                            if (class_socre > max_class_score)
                            {
                                max_class_score = class_socre;
                                class_id = c;
                            }
                        }
                        //转换输出的坐标并保存
                        if (max_class_score > CLASSES_THRESHOLD)
                        {
                            auto angle = sigmoid(*input);
                            //指针跳回向量的开头
                            input -= (classes + 6);
                            auto temp_x = sigmoid(*input);
                            input++;
                            auto temp_y = sigmoid(*input);
                            input++;
                            auto temp_w = sigmoid(*input);
                            input++;
                            auto temp_h = sigmoid(*input);
                            input++;

                            float cx = (temp_x * 2.f - 0.5f + j) * stride[n];
                            float cy = (temp_y * 2.f - 0.5f + i) * stride[n];
                            float w = powf(temp_w * 2.f, 2.f) * anchor_w;
                            float h = powf(temp_h * 2.f, 2.f) * anchor_h;

                            //坐标还原到原图上
                            int rel_x = cx * ratiow;
                            int rel_y = cy * ratioh;
                            int rel_w = w * ratiow;
                            int rel_h = h * ratioh;
                            //置信度、类别、框的x、框的中心y、框的长w、框的宽h(这四个都相对于图像整体)、框的角度angle
                            output_res(res_label, 0) = score;
                            output_res(res_label, 1) = class_id;
                            output_res(res_label, 2) = rel_x;
                            output_res(res_label, 3) = rel_y;
                            output_res(res_label, 4) = rel_w;
                            output_res(res_label, 5) = rel_h;
                            output_res(res_label, 6) = angle;
                            res_label++;
                        }
                        //指针跳到下一个向量的开头
                        input += (classes + 2);
                    }
                    //指针跳到下一个向量的开头
                    input += (classes + 1);
                }
            }
        }
    }
}

float GetEnergyMac::sigmoid(float input_num)
{
    if (input_num < -10)
    {
        //防止数据溢出
        input_num = -10;
    }
    return 1.0 / (1 + exp(-input_num));
}
