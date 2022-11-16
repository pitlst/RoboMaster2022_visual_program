#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include "serial.hpp"
#include "logger.hpp"

using namespace swq;

Serial::Serial()
{
#ifndef SERIAL_CLOSE
    fd = open(PATH_SERIAL, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd)
    {
        throw std::logic_error("串口打开失败");
    }
    fcntl(fd, F_SETFL, 0);
#ifdef SAVE_SERIAL_INPUT
    m_file.open(PATH_COM_FILE, std::ios::app);
    if (m_file.fail())
    {
        throw std::logic_error("打开串口日志文件失败");
    }
    m_file.seekg(0, std::ios::end);
#endif
#endif
}

Serial::~Serial()
{
#ifndef SERIAL_CLOSE
    close(fd);
#ifdef SAVE_SERIAL_INPUT
    m_file.close();
#endif
#endif
}

std::vector<int> Serial::get_msg()
{
#ifndef SERIAL_CLOSE
    int nread = read(fd, rbuff, sizeof(rbuff));
    if (nread > 0)
    {
        //校验帧头
        if (int(rbuff[0]) == 166)
        {
            // color1为蓝色,0为红色
            if (int(rbuff[1]) > 100)
            {
                color = true;
            }
            else
            {
                color = false;
            }
            //模式信息:0为自瞄,1为小幅,2为大符
            mode = int(rbuff[2]);
        }
        else
        {
            log_error("未知的帧头");
        }
    }
    else
    {
        log_error("串口读取失败");
    }
#ifdef SAVE_SERIAL_INPUT
    for (auto ch : rbuff)
    {
        m_file << ch;
    }
    m_file << std::endl;
#endif
    std::vector<int> temp_return = {color, mode};
    return temp_return;
#endif
#ifdef SERIAL_CLOSE
    std::vector<int> temp_return = {color, mode};
    return temp_return;
#endif
}

void Serial::send_msg(const std::vector<int> &msg)
{
#ifndef SERIAL_CLOSE
    sbuff[0] = msg[0];
    sbuff[1] = msg[1];
    sbuff[2] = msg[2];
    auto temp = write(fd, sbuff, sizeof(sbuff));
#endif
}