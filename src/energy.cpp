#include "energy.hpp"
#include <memory>
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
    load_energy.parse("../asset/energy_find.json");
    load_camera.parse("../asset/energy_find.json");

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
    energy_par.model_path = load_energy["EnergyFind"]["model_path"].str();

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
    auto input_layer = model->input(0);
}