#pragma once
#include <vector>
#include <fstream>

#include "debug.hpp"

namespace swq
{
    class Serial final
    {
    public:
        Serial();
        ~Serial();
        //返回封装好的模式和颜色信息
        std::vector<int> get_msg();
        //发送封装好的坐标信息
        void send_msg(const std::vector<int> &msg);

    private:
        //阻止构造一些常用的特定重载函数
        Serial operator <<(const Serial&) = delete;
        Serial operator >>(const Serial&) = delete;
        Serial operator =(const Serial&) = delete;
        Serial operator +(const Serial&) = delete;
        Serial operator -(const Serial&) = delete;
        Serial operator *(const Serial&) = delete;
        Serial operator /(const Serial&) = delete;
        Serial operator ++() = delete;
        Serial operator --() = delete;
        Serial operator &(const Serial&) = delete;

        //串口设备描述符
        int fd;
        //串口接受数据的缓冲区
        unsigned char rbuff[10];
        //串口发送数据的缓冲区
        unsigned char sbuff[3];
        //记录的模式信息
        int mode = DEFALUTE_MODE;
        //记录的颜色信息
        bool color = DEFALUTE_COLOR;
#ifdef SAVE_SERIAL_INPUT
        //用于记录串口文件的文件流
        std::fstream m_file;
#endif
    };
}