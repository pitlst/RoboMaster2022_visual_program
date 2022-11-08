#include "energy.hpp"
#include "utils.hpp"
#include "debug.hpp"

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

void GetEnergyMac::set(int input_debug, int input_color, int input_mode)
{
    debug = input_debug;
    color = input_color;
    mode = input_mode;
    load_json();
    openvino_init();
}

void GetEnergyMac::load_json()
{
    json load_energy;
    json load_camera;
    load_energy.parse(get_file_str(PATH_ENERGY_FIND));
    load_camera.parse(get_file_str(PATH_ECAMERA));

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

void GetEnergyMac::openvino_init()
{
    //读取模型
    model = core.read_model(energy_par.model_path);
    //将模型加载到设备
    auto compiled_model = core.compile_model(model, "GPU");
    //创建推理请求
    infer_request = compiled_model.create_infer_request();
    //获取网络的输入输出形状
    input_port = compiled_model.input();
    //保存模型输入大小
    model_par.input_n = input_port.get_shape()[0];
    model_par.input_c = input_port.get_shape()[1];
    model_par.input_h = input_port.get_shape()[2];
    model_par.input_w = input_port.get_shape()[3];
    //创建指向输入数据推理向量
    float* frame_data = (float*)frame.data;
    ov::Tensor input_tensor(input_port.get_element_type(), input_port.get_shape(), frame_data);
}

ov::Tensor GetEnergyMac::trans_mat_to_tensor(cv::Mat & img)
{

    int width = img.cols;
    int height = img.rows;
    size_t size = width * height * img.channels();
    std::string input_type_str = input_port.get_element_type().get_type_name();
    //注意，这里暂时只支持fp32格式
    if (input_type_str == "f32" || input_type_str == "f16")
    {
        std::shared_ptr<float> _data;
        _data.reset(new float[size], std::default_delete<float[]>());
    }
    else if (input_type_str == "")
    {
        std::shared_ptr<float> _data;
        _data.reset(new float[size], std::default_delete<float[]>());
    }
    
    
    
    cv::Mat resized(cv::Size(width, height), img.type(), _data.get());
    cv::resize(img, resized, cv::Size(width, height));
}