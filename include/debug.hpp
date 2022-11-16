//本头文件用于定义常用宏,方便功能测试
//注意，更改本文件后需要重新编译后运行
#pragma once
#include <cmath>
#define _USE_MATH_DEFINES 

//能量机关配置参数文件路径
#define PATH_ENERGY_JSON "../asset/json/energy.json"
//自瞄模式配置参数文件路径
#define PATH_ARMOR_JSON "../asset/json/armor.json"
//能量机关配置参数文件路径
#define PATH_CAMERA_JSON "../asset/json/camera.json"
//能量机关配置参数文件路径
#define PATH_MODEL_JSON "../asset/json/camera.json"
//日志文件路径
#define PATH_LOG_FILE "../asset/log.txt"
//串口文件记录路径
#define PATH_COM_FILE "../asset/com.txt"
//默认串口路径
#define PATH_SERIAL "/dev/ttyACM0"
//模型文件路径
#define PATH_MODEL_FILE "../asset/model/bestyao_13_416.xml"
//能量机关模式参数-小符
#define SMALL_ENERGY_BUFFER 8001
//能量机关模式参数-大符
#define BIG_ENERGY_BUFFER 8002
//圆周率常数
#define PI M_PI
//默认的识别颜色
#define DEFALUTE_COLOR 0
//默认的击打模式
#define DEFALUTE_MODE 0

// nms参数
//这是模型的目标置信度阈值
#define MODEL_THRESHOLD 0.
//这是模型的类别置信度阈值
#define CLASSES_THRESHOLD 0.

//大符预测的最大历史记录次数
#define BUFFER_HISTORY_LEN_MAX 300
//角度自动判断的迭代次数
#define BUFFER_DETECT_COUNT 500

//开启此宏定义关闭所有日志输出
//#define NO_LOG

//开启后在release模式下开始debug输出，用于测试编译器不同等级对程序的影响
#ifdef COMPILE_RELEASE
#undef COMPILE_RELEASE
#define COMPILE_DEBUG
#endif

//开启此宏定义记录视频
#define SAVE_VIDEO

//开启此宏定义保存串口接收数据
#define SAVE_SERIAL_INPUT

//开启此宏定义启用GPU推理,不启用默认使用CPU
#define GPU_INFER

//开启此宏定义关闭串口,使用默认数据
// #define SERIAL_CLOSE