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
        // load_json();
        // openvino_init();
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

std::vector<int> GetEnergyMac::process(cv::Mat &input_frame, double f_time)
{
    buffer_para buffer_now;
    buffer_now.f_time = f_time;
    frame = input_frame;
    //输入图像转换成向量
    trans_mat_to_tensor();
    //输入图像
    infer_request.set_input_tensor(input_tensor);
    //推理
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
    //转换向量的格式
    trans_tansor_to_matrix(output_tensor);
    //寻找中心
    center_filter(buffer_now);
    //寻找装甲板
    energy_filter(buffer_now);
    //保存该结果
    armor.emplace_back(buffer_now);
    //预测装甲板的位置
    auto hit_pos = angle_predicted();
    return hit_pos;
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
    model_par.input.n = input_port.get_shape()[0];
    model_par.input.c = input_port.get_shape()[1];
    model_par.input.w = input_port.get_shape()[2];
    model_par.input.h = input_port.get_shape()[3];
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

    //这里提前为输出矩阵申请最大可能长度的内存
    output_res.resize(model_par.stride8.n * model_par.stride8.c * model_par.stride8.w +
                      model_par.stride16.n * model_par.stride16.c * model_par.stride16.w +
                      model_par.stride32.n * model_par.stride32.c * model_par.stride32.w);
    for (auto &ch : output_res)
    {
        ch.resize(7);
    }
}

void GetEnergyMac::center_filter(buffer_para &buffer)
{
    std::vector<float> max_center;
    for (auto &ch : output_res)
    {
        if (ch[1] == 2)
        {
            if (buffer.center.size() == 0)
            {
                buffer.center = ch;
            }
            else if (max_center[0] < ch[0])
            {
                buffer.center = ch;
            }
        }
    }
}

void GetEnergyMac::energy_filter(buffer_para &buffer)
{
    std::vector<float> temp = {-1, -1, -1};
    //中心没找到直接返回负数
    if (buffer.center.size() == 0)
    {
        buffer.armor_point = temp;
    }
    for (const auto &ch : output_res)
    {
        auto pos_true = false;
        if (ch[1] == 0)
        {
            auto distance = pow((buffer.center[2] - ch[2]), 2) + pow((buffer.center[3] - ch[3]), 2);
            if (distance > energy_par.armor_R_distance_min && distance < energy_par.armor_R_distance_max)
            {
                for (const auto &_ch : output_res)
                {
                    if (_ch[1] == 1)
                    {
                        auto _distance = pow((_ch[2] - ch[2]), 2) + pow((_ch[3] - ch[3]), 2);
                        if (_distance > energy_par.fan_armor_distence_max && _distance < energy_par.fan_armor_distence_min)
                        {
                            break;
                        }
                    }
                }
                pos_true = true;
            }
        }
        if (pos_true)
        {
            if (temp[2] != -1)
            {
                log_error("存在多个待击打目标,随机选择一个，请注意");
            }
            temp = {ch[2], ch[3], 1};
        }
    }
    buffer.armor_point = temp;
}

std::vector<int> GetEnergyMac::angle_predicted()
{
    auto temp_buffer = armor.back();
    std::vector<int> armor_point = {-1, -1, -1};
    //检查开始时间
    if (begin_time == 0)
    {
        begin_time = temp_buffer.f_time;
    }
    //检查是否检测到了中心
    if (temp_buffer.center.size() == 0)
    {
        return armor_point;
    }
    //检测是否检测到了目标
    if (temp_buffer.armor_point[2] == -1)
    {
        return armor_point;
    }
    //判断旋转方向
    detect = judge_rotate_direct();
    //计算能量机关半径
    hitDis = EuclideanDistance(temp_buffer.armor_point[2], temp_buffer.armor_point[3], temp_buffer.center[2], temp_buffer.center[3]);
    //计算旋转角度,通过角度进行预测
    auto angle = cartesian_to_polar(temp_buffer);
    if (mode == SMALL_ENERGY_BUFFER)
    {
        angle = energymac_forecast_small(angle);
    }
    else if (mode == BIG_ENERGY_BUFFER)
    {
        angle = energymac_forecast_big(angle);
    }
    else
    {
        log_error("未知的大符旋转标志位,输出未预测的原坐标");
    }

    angle = angle /180.0f*PI;
    armor_point[0] = temp_buffer.center[2] + hitDis*cos(angle);
    armor_point[1] = temp_buffer.center[3] + hitDis*sin(angle);
    armor_point[2] = temp_buffer.armor_point[2];

    return armor_point;
}

int GetEnergyMac::judge_rotate_direct()
{
    static int count_n = 0;
    count_n++;
    if (count_n > BUFFER_DETECT_COUNT)
    {
        return detect;
    }
    else if (armor.size() > 0)
    {
        auto angle_1 = atan2(armor[-1].armor_point[0], armor[-1].armor_point[1]);
        auto angle_2 = atan2(armor[-2].armor_point[0], armor[-2].armor_point[1]);
        auto delta_angle = abs((angle_1 - angle_2) * 180.0f / PI);
        //如果角度差不准，再添加最大值过滤
        armor[-1].delta_angle = delta_angle;

        decltype(delta_angle) temp_angle;
        for (const auto &ch : armor)
        {
            temp_angle += ch.delta_angle;
        }
        temp_angle = temp_angle / armor.size();
        if (temp_angle > 0)
        {
            detect = 1;
        }
        else if (temp_angle < 0)
        {
            detect = -1;
        }
    }
    return detect;
}

double GetEnergyMac::cartesian_to_polar(buffer_para &buffer)
{
    auto vectorx = buffer.armor_point[0] - buffer.center[2];
    auto vectory = buffer.armor_point[1] - buffer.center[3];
    float angle = -1;
    if (vectorx > 0 && vectory > 0)
    {
        angle = atan(abs(vectory / vectorx)) * 180 / PI;
    }
    else if (vectorx < 0 && vectory > 0)
    {
        angle = 180.0f - atan(abs(vectory / vectorx)) * 180 / PI;
    }
    else if (vectorx < 0 && vectory < 0)
    {
        angle = 180.0f + atan(abs(vectory / vectorx)) * 180 / PI;
    }
    else if (vectorx < 0 && vectory < 0)
    {
        angle = 360.0f - atan(abs(vectory / vectorx)) * 180 / PI;
    }
    else if (vectorx == 0 && vectory > 0)
    {
        angle = 270.0f;
    }
    else if (vectorx == 0 && vectory <= 0)
    {
        angle = 90.0f;
    }
    else if (vectory == 0 && vectorx > 0)
    {
        angle = 0.0f;
    }
    else if (vectorx == 0 && vectory <= 0)
    {
        angle = 180.0f;
    }
    return angle;
}

inline double GetEnergyMac::energymac_forecast_small(double angle)
{
    return detect*energy_par.predict_small + angle;
}

double GetEnergyMac::energymac_forecast_big(double angle)
{
    //这里可以采取两种预测方式：
    //一种是一维卡尔曼滤波,另一种是分段映射,手动绘制函数

}

void GetEnergyMac::trans_mat_to_tensor()
{
    cv::Mat temp;
    //图像归一化
    cv::normalize(frame, temp, 1.0, 0.0, cv::NORM_MINMAX);
    int width = temp.cols;
    int height = temp.rows;
    //图像不符合大小则进行缩放
    if (width != model_par.input.h || height != model_par.input.w)
    {
        cv::resize(temp, temp, cv::Size(model_par.input.h, model_par.input.w));
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
    auto index = 0;
    auto ratioh = float(frame.rows) / model_par.input.w;
    auto ratiow = float(frame.cols) / model_par.input.h;
    //遍历尺度
    for (auto n = 0; n < 3; n++)
    {
        const float *input = out_tenosr[n].data<const float>();
        int num_grid_x = model_par.input.n / stride[n];
        int num_grid_y = model_par.input.h / stride[n];
        auto area = num_grid_x * num_grid_y;
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
                            auto rel_x = cx * ratiow;
                            auto rel_y = cy * ratioh;
                            auto rel_w = w * ratiow;
                            auto rel_h = h * ratioh;

                            //置信度、类别、框的x、框的中心y、框的长w、框的宽h(这四个都相对于图像整体)、框的角度angle
                            auto index_temp = 0;
                            output_res[index][index_temp] = score;
                            index_temp++;
                            output_res[index][index_temp] = class_id;
                            index_temp++;
                            output_res[index][index_temp] = rel_x;
                            index_temp++;
                            output_res[index][index_temp] = rel_y;
                            index_temp++;
                            output_res[index][index_temp] = rel_w;
                            index_temp++;
                            output_res[index][index_temp] = rel_h;
                            index_temp++;
                            output_res[index][index_temp] = angle;
                            index++;
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

void GetEnergyMac::vector_protect_process()
{
    while (armor.size() > BUFFER_HISTORY_LEN_MAX)
    {
        armor.pop_front();
    }
}
