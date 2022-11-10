//本头文件用于定义常用宏,方便功能测试
//注意，更改本文件后需要重新编译后运行

//配置参数文件路径
#define PATH_ENERGY_JSON "../asset/json/energy.json"
#define PATH_ARMOR_JSON "../asset/json/armor.json"
#define PATH_SENTRY_JSON "../asset/json/sentry.json"
#define PATH_CAMERA_JSON "../asset/json/camera.json"
#define PATH_MODEL_JSON "../asset/json/camera.json"

//日志文件路径
#define PATH_LOG_FILE "../asset/log.txt"

//模型文件路径
#define PATH_MODEL_FILE "../asset/model/bestyao_13_416.xml"

//nms参数
//这是模型的目标置信度阈值
#define MODEL_THRESHOLD 0.
//这是模型的类别置信度阈值
#define CLASSES_THRESHOLD 0.

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

//开启此宏定义保存串口发送数据
#define SAVE_SERIAL_OUTPUT

//开启此宏定义启用GPU推理,不启用默认使用CPU
#define GPU_INFER