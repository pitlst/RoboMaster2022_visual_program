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
#define SMALL_ENERGY_BUFFER 1
//能量机关模式参数-大符
#define BIG_ENERGY_BUFFER 2
//chono系统时钟调用单位转换常数
#define TIME_TRANSFORMER 1000000000.0
//圆周率常数
#define PI M_PI
//默认的识别颜色
#define DEFALUTE_COLOR 0
//默认的击打模式
#define DEFALUTE_MODE 2
//debug下灯条的默认颜色
#define LIGHTBAR_COLOR (0,255,0)
//debug下装甲板的默认颜色
#define ARMOR_COLOR (0,255,255)
//debug下文字的默认颜色
#define TEXT_COLOR (255,255,255)
//debug下能量机关装甲筛选的默认颜色
#define BUFFER_ARMOR_COLOR (255, 0, 255)
//debug下能量机关中心筛选的默认颜色
#define BUFFER_CENTER_COLOR (255,255,255)
//debug下图像的默认线宽
#define FRAME_THICKNESS 2
//图像队列的最大大小
#define FRAME_QUEUE_SIZE_MAX 3

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

//此宏定义决定log等级和具体的调试,debug下会默认开启调试,正常比赛请开启release
// #define COMPILE_RELEASE
#define COMPILE_DEBUG

//开启此宏定义记录视频
#define SAVE_VIDEO

//开启此宏定义保存串口接收数据
// #define SAVE_SERIAL_INPUT

//开启此宏定义启用GPU推理,不启用默认使用CPU
#define GPU_INFER

//开启此宏定义关闭串口,使用默认数据
#define SERIAL_CLOSE

//开启此宏定义关闭多线程
// #define THREADING_DEBUG

//如果什么都没定义默认release
#ifndef COMPILE_RELEASE
#ifndef COMPILE_DEBUG
#define COMPILE_RELEASE
#endif
#endif

//如果定义了release就关闭debug的定义
#ifdef COMPILE_RELEASE
#undef COMPILE_DEBUG
#endif

