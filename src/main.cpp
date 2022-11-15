/*

               _____                       _____                       ____
              /\    \                     /\    \                     /\    \
             /::\    \                   /::\    \                   /::\    \
             \:::\    \                 /::::\    \                 /::::\    \
              \:::\    \               /::::::\    \               /::::::\    \
               \:::\    \             /:::/\:::\    \             /:::/\:::\    \
                \:::\    \           /:::/  \:::\    \           /:::/__\:::\    \
                /::::\    \         /:::/    \:::\    \         /::::\   \:::\    \
               /::::::\    \       /:::/      \:::\    \       /::::::\   \:::\    \
              /:::/\:::\    \     /:::/        \:::\    \     /:::/\:::\   \:::\    \
             /:::/  \:::\____\   /:::/          \:::\____\   /:::/__\:::\   \:::\____\
            /:::/   /\::/    /   \:::\          /:::/    /   \:::\   \:::\   \::/    /
           /:::/   /  \/____/     \:::\        /:::/    /     \:::\   \:::\   \/____/
          /:::/   /                \:::\      /:::/    /       \:::\   \:::\    \
         /:::/   /                  \:::\    /:::/    /         \:::\   \:::\____\
        /:::/   /                    \:::\  /:::/    /           \:::\   \::/    /
       /:::/   /                      \:::\/:::/    /             \:::\   \/____/
      /:::/   /                        \::::::/    /               \:::\    \
     /:::/   /                          \::::/    /                 \:::\____\
     \::/   /                            \::/    /                   \::/    /
      \/___/                              \/____/                     \/____/

TOE实验室算法组---打符/自瞄程序C++版
@作者：孙墨明
*/

// c++库函数
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>



//多线程
#include <queue>
#include <mutex>
#include <condition_variable>

#include "get_frame.hpp"
#include "aimbot.hpp"
#include "energy.hpp"
#include "utils.hpp"
#include "json.hpp"
#include "hint.hpp"
#include "logger.hpp"
#include "xml.hpp"
#include "debug.hpp"
#include "serial.hpp"

// //全局变量声明

// //图像队列
// static std::queue<cv::Mat> produced_frame;
// //图像时间戳
// static std::queue<double> produced_ftime;
// //目标坐标队列
// static std::queue<std::vector<int>> produced_msg;
// //全局互斥锁
// static std::mutex mtx;
// //全局条件变量
// static std::condition_variable cv;
// //全局通知标志位
// static bool notified = false;
// //全局模式标志位
// static int mode = 0;
// //全局颜色标志位
// static bool color = 0;


int main()
{
    log_debug("程序开始");
    swq::Serial acm0;
    std::vector<int> temp_v = {0,0,0};
    acm0.send_msg(temp_v);
    log_debug("-----");
    auto temp_a = acm0.get_msg();
    std::cout << temp_a[0] << temp_a[1] << temp_a[2] << std::endl;

    log_debug("程序结束");
    return 0;
}
