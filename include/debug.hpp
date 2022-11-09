//本头文件用于定义常用宏,方便功能测试

//配置参数文件路径
#define PATH_ENERGY_FIND "../asset/json/energy_find.json"
#define PATH_ARMOR_FIND "../asset/json/armor_find.json"
#define PATH_SENTRY_FIND "../asset/json/sentry_find.json"
#define PATH_ECAMERA "../asset/json/common.json"

//日志文件路径
#define PATH_LOG_FILE "../asset/log.txt"

//模型文件路径
#define PATH_MODEL_FILE "../asset/model/bestyao_13_416.xml"

//这是模型的置信度阈值
#define MODEL_THRESHOLD 0.5

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