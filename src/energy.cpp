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
    model_para_init();
    kalmanfilter_init();
}

GetEnergyMac::GetEnergyMac(int input_mode)
{
    set(input_mode);
    load_json();
    openvino_init();
    model_para_init();
    kalmanfilter_init();
}

GetEnergyMac::~GetEnergyMac()
{
}

void GetEnergyMac::set(int input_mode)
{
    if (input_mode == 2)
    {
        mode = BIG_ENERGY_BUFFER;
    }
    else if (input_mode == 1)
    {
        mode = SMALL_ENERGY_BUFFER;
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
    log_debug("buffer_now.center: ", buffer_now.center[2], " ", buffer_now.center[3]);
    //寻找装甲板
    energy_filter(buffer_now);
    log_debug("buffer_now.armor: ", buffer_now.armor_point[0], " ", buffer_now.armor_point[1]);
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
    model = core.read_model(energy_par.model_path);
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

void GetEnergyMac::kalmanfilter_init()
{
    cv::RNG rng;
    KF.init(4, 2, 0);
    KF.transitionMatrix = (cv::Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1); //转移矩阵A
    cv::setIdentity(KF.measurementMatrix);                                                           //测量矩阵H
    cv::setIdentity(KF.processNoiseCov, cv::Scalar::all(energy_par.Q_noise));                        //系统噪声方差矩阵Q
    cv::setIdentity(KF.measurementNoiseCov, cv::Scalar::all(energy_par.R_noise));                    //测量噪声方差矩阵R
    cv::setIdentity(KF.errorCovPost, cv::Scalar::all(1));                                            //后验错误估计协方差矩阵P
    rng.fill(KF.statePost, cv::RNG::UNIFORM, 0, 0);                                                  //初始状态值x(0)
    measurement = cv::Mat::zeros(2, 1, CV_32F);                                                      //初始测量值x'(0)
}

void GetEnergyMac::center_filter(buffer_para &buffer)
{
    std::vector<std::vector<double>> temp_center;
    swq::buffer_para temp_buffer;
    bool label = false;
    if (armor.size())
    {
        temp_buffer = armor.back();
        if (temp_buffer.center[2] != -1)
        {
            label = true;
        }
    }
    //根据距离筛选出所有中心
    for (auto &ch : output_res)
    {
        if (ch[1] == 2)
        {
            if (label)
            {
                //距离上次坐标过长的删掉
                auto distance = EuclideanDistance(ch[2], ch[3], temp_buffer.center[2], temp_buffer.center[3]);
                if (distance > energy_par.nms_distence_max * energy_par.frame_size)
                {
                    continue;
                }
            }
            //距离本次坐标过长的删掉
            bool label_2 = true;
            for (auto &_ch : temp_center)
            {
                auto distance = EuclideanDistance(ch[2], ch[3], _ch[2], _ch[3]);
                if (distance > energy_par.nms_distence_max * energy_par.frame_size)
                {
                    label_2 = false;
                    break;
                }
            }
            if (label_2)
            {
                temp_center.emplace_back(ch);
            }
        }
    }
    //遍历所有可能的中心nms
    if (temp_center.size())
    {
#ifdef CENTER_FILTER_IOU
        //对于第一次没有上一次做参照，直接选择置信度最高的中心返回
        if (!label)
        {
            for (auto &ch : temp_center)
            {
                if (buffer.center.size() == 0)
                {
                    buffer.center = ch;
                    continue;
                }
                if (buffer.center[0] < ch[0])
                {
                    buffer.center = ch;
                    continue;
                }
            }
        }
        else
        {
            //计算上一次中心的框
            auto last_x00 = temp_buffer.center[2] - temp_buffer.center[4] / 2;
            auto last_y00 = temp_buffer.center[3] - temp_buffer.center[5] / 2;
            auto last_x11 = temp_buffer.center[2] + temp_buffer.center[4] / 2;
            auto last_y11 = temp_buffer.center[3] + temp_buffer.center[5] / 2;

            double last_iou;
            for (auto &ch : temp_center)
            {
                //计算中心的框
                auto m_x00 = ch[2] - ch[4] / 2;
                auto m_y00 = ch[3] - ch[5] / 2;
                auto m_x11 = ch[2] + ch[4] / 2;
                auto m_y11 = ch[3] + ch[5] / 2;

                //计算并集面积
                auto x00 = std::min(last_x00, m_x00);
                auto y00 = std::min(last_y00, m_y00);
                auto x11 = std::max(last_x11, m_x11);
                auto y11 = std::max(last_y11, m_y11);
                auto area_max = std::abs(x00 - x11) * std::abs(y00 - y11);

                if (area_max == 0)
                {
                    continue;
                }

                //计算交集面积
                x00 = std::max(last_x00, m_x00);
                y00 = std::max(last_y00, m_y00);
                x11 = std::min(last_x11, m_x11);
                y11 = std::min(last_y11, m_y11);
                auto area_min = std::abs(x00 - x11) * std::abs(y00 - y11);

                auto IOU = area_min / area_max;
                //选择交并比最大的中心
                if (last_iou < IOU)
                {
                    buffer.center = ch;
                }
                last_iou = IOU;
            }
        }

#endif
#ifdef CENTER_FILTER_WEIGHTED
        std::vector<double> average_center;
        if (label)
        {
            average_center = temp_buffer.center;
            //类别不变
            //长宽坐标计算为置信度加权平均
            average_center[2] = average_center[2] * average_center[0];
            average_center[3] = average_center[3] * average_center[0];
            average_center[4] = average_center[4] * average_center[0];
            average_center[5] = average_center[5] * average_center[0];
            //角度默认置为0
            average_center[6] = 0.0;
        }
        else
        {
            average_center = {0, 0, 0, 0, 0, 0, 0};
        }

        for (const auto &ch : temp_center)
        {
            //置信度计算和
            average_center[0] += ch[0];
            //类别不变
            //长宽坐标计算为置信度加权平均
            average_center[2] += ch[2] * ch[0];
            average_center[3] += ch[3] * ch[0];
            average_center[4] += ch[4] * ch[0];
            average_center[5] += ch[5] * ch[0];
        }
        average_center[2] = average_center[2] / average_center[0];
        average_center[3] = average_center[3] / average_center[0];
        average_center[4] = average_center[4] / average_center[0];
        average_center[5] = average_center[5] / average_center[0];

        average_center[0] = average_center[0] / temp_center.size();
        //如果可以这里的计算都应转换为大整数计算
        buffer.center = average_center;
#endif
    }
    //校验是否为空
    if (buffer.center.empty())
    {
        buffer.center = {-1, -1, -1, -1, -1, -1, -1};
    }
}

void GetEnergyMac::energy_filter(buffer_para &buffer)
{
    std::vector<std::vector<double>> nms_armor;
    std::vector<std::vector<double>> nms_full;
    //中心没找到直接返回负数
    if (buffer.center[2] == -1)
    {
        buffer.armor_point = {-1, -1, -1};
    }
    //装甲板和扇叶nms
    for (const auto &ch : output_res)
    {
        if (ch[1] == 0)
        {
            //第一次直接存进结果中
            if (nms_armor.size() == 0)
            {
                nms_armor.emplace_back(ch);
                continue;
            }
            //遍历历史结果
            auto label_out = true;
            for (auto &ch_armor : nms_armor)
            {
                auto distance = EuclideanDistance(ch[2], ch[3], ch_armor[2], ch_armor[3]);
                //在距离内并且置信度更大，替换
                if (distance > energy_par.nms_distence_max * energy_par.frame_size)
                {
                    if (ch[0] > ch_armor[0])
                    {
                        ch_armor = ch;
                    }
                    label_out = false;
                    break;
                }
            }
            //在距离外，添加
            if (label_out)
            {
                nms_armor.emplace_back(ch);
            }
        }
        else if (ch[1] == 1)
        {
            //第一次直接存进结果中
            if (nms_full.size() == 0)
            {
                nms_full.emplace_back(ch);
                continue;
            }
            //遍历历史结果
            auto label_out = true;
            for (auto &ch_full : nms_full)
            {
                auto distance = EuclideanDistance(ch[2], ch[3], ch_full[2], ch_full[3]);
                //在距离内并且置信度更大，替换
                if (distance > energy_par.nms_distence_max * energy_par.frame_size)
                {
                    if (ch[0] > ch_full[0])
                    {
                        ch_full = ch;
                    }
                    label_out = false;
                    break;
                }
            }
            //在距离外，添加
            if (label_out)
            {
                nms_full.emplace_back(ch);
            }
        }
    }

    if (nms_armor.size() == 0)
    {
        buffer.armor_point = {-1, -1, -1};
    }
    else
    {
        auto pos_true = true;
        for (const auto &ch : nms_armor)
        {
            auto distance = EuclideanDistance(buffer.center[2], buffer.center[3], ch[2], ch[3]);
            //装甲板与旋转中心距离超过阈值直接跳过
            if (distance < energy_par.armor_R_distance_min * energy_par.frame_size || distance > energy_par.armor_R_distance_max * energy_par.frame_size)
            {
                continue;
            }
            for (const auto &_ch : nms_full)
            {
                auto _distance = EuclideanDistance(_ch[2], _ch[3], ch[2], ch[3]);
                //判断装甲板与大符中心的距离，用于判断装甲板是否被击中
                if (_distance > energy_par.fan_armor_distence_min * energy_par.frame_size && _distance < energy_par.fan_armor_distence_max * energy_par.frame_size)
                {
                    //不符合要求
                    pos_true = false;
                    break;
                }
            }
            //符合的装甲版
            if (pos_true)
            {
                buffer.armor_point = {ch[2], ch[3], 1};
                break;
            }
        }
    }
    //校验是否为空
    if (buffer.armor_point.empty())
    {
        buffer.armor_point = {-1, -1, -1};
    }
}

std::vector<int> GetEnergyMac::angle_predicted()
{
    auto temp_buffer = armor.back();
    std::vector<int> armor_point;
    //检查开始时间
    if (begin_time == 0)
    {
        begin_time = temp_buffer.f_time;
    }
    //检查是否检测到了中心
    if (temp_buffer.center[0] == -1)
    {
        log_debug("没有找到中心");
        return armor_point;
    }
    //检测是否检测到了目标
    if (temp_buffer.armor_point[2] == -1)
    {
        log_debug("没有找到目标");
        return armor_point;
    }
    //判断旋转方向
    judge_rotate_direct();
    //计算能量机关半径
    hitDis = EuclideanDistance(temp_buffer.armor_point[0], temp_buffer.armor_point[1], temp_buffer.center[2], temp_buffer.center[3]);
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
    // log_debug(hitDis);
    // log_debug(angle);
    // log_debug(temp_buffer.center[2]);
    // log_debug(temp_buffer.center[3]);
    angle = angle / 180.0f * PI;
    armor_point[0] = temp_buffer.center[2] + hitDis * cos(angle);
    armor_point[1] = temp_buffer.center[3] + hitDis * sin(angle);
    armor_point[2] = temp_buffer.armor_point[2];
    return armor_point;
}

void GetEnergyMac::judge_rotate_direct()
{
    log_debug("判断旋转方向");
    static int count_n;
    count_n++;
    if (count_n > BUFFER_DETECT_COUNT)
    {
        return;
    }
    else if (armor.size() > 3)
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
}

double GetEnergyMac::cartesian_to_polar(buffer_para &buffer)
{
    auto vectorx = buffer.armor_point[0] - buffer.center[2];
    auto vectory = buffer.armor_point[1] - buffer.center[3];
    double angle = -1;
    if (vectorx > 0 && vectory > 0)
    {
        angle = std::atan(std::abs(vectory / vectorx)) * 180 / PI;
    }
    else if (vectorx < 0 && vectory > 0)
    {
        angle = 180.0f - std::atan(std::abs(vectory / vectorx)) * 180 / PI;
    }
    else if (vectorx < 0 && vectory < 0)
    {
        angle = 180.0f + std::atan(std::abs(vectory / vectorx)) * 180 / PI;
    }
    else if (vectorx > 0 && vectory < 0)
    {
        angle = 360.0f - std::atan(std::abs(vectory / vectorx)) * 180 / PI;
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
    else if (vectory == 0 && vectorx <= 0)
    {
        angle = 180.0f;
    }
    return angle;
}

double GetEnergyMac::energymac_forecast_small(double angle)
{
    // log_debug("执行小幅预测，角度： ", angle);
    return detect * energy_par.predict_small + angle;
}

double GetEnergyMac::energymac_forecast_big(double angle)
{
    auto angle_1 = atan2(armor[-1].armor_point[0], armor[-1].armor_point[1]);
    auto angle_2 = atan2(armor[-2].armor_point[0], armor[-2].armor_point[1]);
    auto delta_angle = abs((angle_1 - angle_2) * 180.0f / PI);
    auto delta_time = armor[-1].f_time - armor[-2].f_time;
    auto temp_angel_speed = delta_angle / delta_time;
    auto f_time = armor[-1].f_time;

    measurement.at<double>(0) = temp_angel_speed;
    measurement.at<double>(0) = f_time;
    KF.correct(measurement);

    cv::Mat prediction = KF.predict();
    angle = angle + detect * prediction.at<double>(0) * prediction.at<double>(1);
    return angle;
}

void GetEnergyMac::trans_mat_to_tensor()
{
    //图像归一化
    cv::normalize(frame, frame, 1.0, 0.0, cv::NORM_MINMAX);
    int width = frame.cols;
    int height = frame.rows;
    size_t size = width * height * frame.channels();
    _data.reset(new float[size], std::default_delete<float[]>()); //按照图像大小初始化指向的位置
    cv::Mat resized(cv::Size(width, height), frame.type(), _data.get());
    cv::resize(frame, resized, cv::Size(width, height)); //这里借用opencv自己的构造，让数据填充到_data指向的的位置
    input_tensor = ov::Tensor(input_port.get_element_type(), input_port.get_shape(), _data.get());
}

void GetEnergyMac::trans_tansor_to_matrix(std::vector<ov::Tensor> out_tenosr)
{
    auto index = 0;
    auto ratioh = double(frame.rows) / model_par.input.w;
    auto ratiow = double(frame.cols) / model_par.input.h;
    //遍历尺度
    for (auto n = 0; n < 3; n++)
    {
        const float *input = out_tenosr[n].data<const float>();
        int num_grid_x = model_par.input.w / stride[n];
        int num_grid_y = model_par.input.h / stride[n];
        auto area = num_grid_x * num_grid_y;
        //遍历anchor
        for (auto q = 0; q < 3; q++)
        {
            const double anchor_w = anchors[n][q * 2];
            const double anchor_h = anchors[n][q * 2 + 1];
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
                        double max_class_score = 0;
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

                            double cx = (temp_x * 2.f - 0.5f + j) * stride[n];
                            double cy = (temp_y * 2.f - 0.5f + i) * stride[n];
                            double w = powf(temp_w * 2.f, 2.f) * anchor_w;
                            double h = powf(temp_h * 2.f, 2.f) * anchor_h;

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
                            // log_debug(score, ",", class_id, ",", rel_x, ",", rel_y, ",", rel_w, ",", rel_h, ",", angle);
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
    // log_debug("--------------------------------------");
}

double GetEnergyMac::sigmoid(double input_num)
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

#ifdef COMPILE_DEBUG
std::list<cv::Mat> GetEnergyMac::debug_frame(cv::Mat &input_frame)
{
    std::list<cv::Mat> temp;
    auto temp_armor = armor.back();
    if (!temp_armor.center.empty())
    {
        cv::circle(input_frame, cv::Point(temp_armor.center[2], temp_armor.center[3]), energy_par.armor_R_distance_max, cv::Scalar(BUFFER_CENTER_COLOR));
        cv::circle(input_frame, cv::Point(temp_armor.center[2], temp_armor.center[3]), energy_par.armor_R_distance_min, cv::Scalar(BUFFER_CENTER_COLOR));
    }
    if (!temp_armor.armor_point.empty())
    {
        cv::circle(input_frame, cv::Point(temp_armor.armor_point[0], temp_armor.armor_point[1]), energy_par.fan_armor_distence_max, cv::Scalar(BUFFER_ARMOR_COLOR));
        cv::circle(input_frame, cv::Point(temp_armor.armor_point[0], temp_armor.armor_point[1]), energy_par.fan_armor_distence_min, cv::Scalar(BUFFER_ARMOR_COLOR));
        cv::circle(input_frame, cv::Point(temp_armor.armor_point[0], temp_armor.armor_point[1]), energy_par.nms_distence_max, cv::Scalar(0, 0, 255));
    }
    temp.emplace_back(input_frame);
    return temp;
}

swq::energy_para GetEnergyMac::get_argument()
{
    return energy_par;
}

void GetEnergyMac::updata_argument(const swq::energy_para &input)
{
    energy_par = input;
}

void GetEnergyMac::update_json(const std::string &filename)
{
    json temp_load;
    temp_load.parse(get_file_str(filename));

    temp_load["EnergyFind"]["center_dis_y"] = energy_par.center_dis_y;
    temp_load["EnergyFind"]["center_dis_x"] = energy_par.center_dis_x;
    temp_load["EnergyFind"]["pass_number_max"] = energy_par.pass_number_max;
    temp_load["EnergyFind"]["delta_angle_distance"] = energy_par.delta_angle_distance;
    temp_load["EnergyFind"]["predict_small"] = energy_par.predict_small;
    temp_load["EnergyFind"]["predict_big"] = energy_par.predict_big;
    temp_load["EnergyFind"]["R_noise"] = energy_par.R_noise;
    temp_load["EnergyFind"]["Q_noise"] = energy_par.Q_noise;
    temp_load["EnergyFind"]["fan_armor_distence_max"] = energy_par.fan_armor_distence_max;
    temp_load["EnergyFind"]["fan_armor_distence_min"] = energy_par.fan_armor_distence_min;
    temp_load["EnergyFind"]["armor_R_distance_max"] = energy_par.armor_R_distance_max;
    temp_load["EnergyFind"]["armor_R_distance_min"] = energy_par.armor_R_distance_min;
    temp_load["EnergyFind"]["nms_distence_max"] = energy_par.nms_distence_max;

    temp_load["Energy_mac"]["width"] = energy_par.frame_size;

    save_file(filename, temp_load.str());
    temp_load.clear();
}
#endif