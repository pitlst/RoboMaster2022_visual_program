#pragma once
#include <string>
#include <sstream>

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"

#include "MvCameraControl.h"

#include "json.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include "debug.hpp"



namespace swq
{

    class GetFrame final
    {
    public:
        GetFrame();
        GetFrame(const std::string & source_path, int input_mode);
        ~GetFrame();

        void set(const std::string &source_path, int input_mode);
        void restart_camera(int input_mode);
        cv::Mat & GetOneFrame();
        void EndCamera();

        struct camera_data
        {
            int width;
            int height;
            int exposure_time;
            int offsetX;
            int offsetY;
        };
        camera_data m_camera;

    private:
        void read_json(const std::string & input_filename);  
        void StartCamera();

        //阻止构造一些常用的特定重载函数
        GetFrame operator <<(const GetFrame&) = delete;
        GetFrame operator >>(const GetFrame&) = delete;
        GetFrame operator =(const GetFrame&) = delete;
        GetFrame operator +(const GetFrame&) = delete;
        GetFrame operator -(const GetFrame&) = delete;
        GetFrame operator *(const GetFrame&) = delete;
        GetFrame operator /(const GetFrame&) = delete;
        GetFrame operator ++() = delete;
        GetFrame operator --() = delete;
        GetFrame operator &(const GetFrame&) = delete;

        int mode;
        int video_debug_set; 
        //标志位，相机是否正常开启
        bool open_label = false;
        std::string source;
        std::string filename;
        json file;
        cv::VideoCapture capture;
        cv::Mat frame;


        //海康相机的相关函数

        //打印相机相关信息
        bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);
        //帧数据转换为Mat格式图片并保存
        void Convert2Mat();
        //帧数据像素排列由RGB转为BGR
        void RGB2BGR( unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight);

        //海康相机需要的相关参数

        //海康相机指针
        void* handle = nullptr;
        //帧数据指针
        unsigned char * pData;
        //帧数据大小
        unsigned int nDataSize;
        //输出帧的信息
        MV_FRAME_OUT_INFO_EX stImageInfo = {0};
        //设备信息
        MV_CC_DEVICE_INFO *pDeviceInfo;

    #ifdef COMPILE_DEBUG
    public:
        int get_video_debug();
    #endif
    };
}

