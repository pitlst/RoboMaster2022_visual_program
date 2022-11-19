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
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
//相关声明
#include "get_frame.hpp"
#include "aimbot.hpp"
#include "energy.hpp"
#include "utils.hpp"
#include "json.hpp"
#include "hint.hpp"
#include "logger.hpp"
#include "xml.hpp"
#include "serial.hpp"
#include "debug.hpp"
//系统库
#include <signal.h>


//全局标志位:模式与颜色
int mode;
bool color;
//类初始化时间
std::chrono::_V2::system_clock::time_point start_t;
//使用的类初始化
swq::Serial serial_com;
swq::GetFrame capture;
swq::GetArmor aimbot;
swq::GetEnergyMac buffer;
//用于处理控制台回调的标志位
volatile sig_atomic_t flag = 0;

#ifndef THREADING_DEBUG
//图像队列
std::queue<cv::Mat> img_queue;
//图像的时间戳队列
std::queue<double> time_queue;
//图像队列的互斥锁
std::mutex img_mtx;
//串口消息队列
std::queue<std::vector<int>> msg_queue;
//串口队列的互斥锁
std::mutex msg_mtx;
//用于通知线程终止的标志位
volatile bool return_flag = false;

void frame_th()
{
    auto mode_temp = mode;
    auto count = 0;
    while (1)
    {
        auto frame = capture.GetOneFrame();
        auto time = std::chrono::high_resolution_clock::now() - start_t;
        count++;

        img_mtx.lock();
        img_queue.push(frame);
        time_queue.push(time.count() / TIME_TRANSFORMER);
        img_mtx.unlock();
        //每发送10次校验一下输入的模式和颜色
        if (count > 10)
        {
            count = 0;
            if (mode_temp != mode)
            {
                capture.restart_camera(mode);
                mode_temp = mode;
            }
        }
        //线程退出
        if (return_flag)
        {
            return;
        }
    }
}

void process_th()
{
    cv::Mat frame;
    double time;
    std::vector<int> msg = {-1, -1, -1};
    while (1)
    {
        img_mtx.lock();
        while (img_queue.empty() || time_queue.empty())
        {
            //等待20毫秒再去获取图像
            img_mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            img_mtx.lock();
        }
        frame = img_queue.front();
        time = time_queue.front();
        img_queue.pop();
        time_queue.pop();
        img_mtx.unlock();
        if (mode == 0)
        {
            msg = aimbot.process(frame);
        }
        else if (mode == 1 || mode == 2)
        {
            buffer.set(mode);
            msg = buffer.process(frame, time);
        }
        else
        {
            msg = std::vector<int>{-1, -1, -1};
        }
        msg_mtx.lock();
        msg_queue.push(msg);
        msg_mtx.unlock();
        //线程退出
        if (return_flag)
        {
            return;
        }
    }
}

void serial_th()
{
    std::vector<int> msg = {-1, -1, -1};
    int count = 0;
    while (1)
    {
        msg_mtx.lock();
        if (!msg_queue.empty())
        {
            msg = msg_queue.front();
            msg_queue.pop();
        }
        msg_mtx.unlock();
        serial_com.send_msg(msg);
        count++;
        //每发送10次校验一下输入的模式和颜色
        if (count > 10)
        {
            count = 0;
            auto msg_ = serial_com.get_msg();
            color = msg_[0];
            mode = msg_[1];
        }
        //线程退出
        if (return_flag)
        {
            return;
        }
    }
}


void debug_th()
{
//如果不是debug模式关闭debug线程
#ifdef COMPILE_DEBUG
    //等一等正常的处理线程
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    //初始化需要的数据结构,并创建窗口
    swq::tarch_bar bar;
    //再获取相对应的数据创见滑动条
    bar.bar_creat_aimbot(aimbot.get_argument());
    bar.bar_creat_bufferr(buffer.get_argument());
    while (1)
    {
        if (mode == 0)
        {
            auto debug_frame = aimbot.debug_frame();
            aimbot.updata_argument(bar.get_bar_value_aimbot());
            aimbot.update_json(PATH_ARMOR_JSON);
            cv::imshow("frame_debug", debug_frame.front());
            cv::imshow("mask_debug", debug_frame.back());
        }
        else
        {
            buffer.updata_argument(bar.get_bar_value_buffer());
            auto debug_frame = buffer.debug_frame();
            buffer.update_json(PATH_ENERGY_JSON);
            cv::imshow("frame_debug", debug_frame.front());
            // cv::imshow("mask_debug", debug_frame.back());
        }

        auto k = cv::waitKey(1);
        if (k == 27)
        {
            cv::destroyAllWindows();
            flag = 1;
            break;
        }
    }
#endif
#ifndef COMPILE_DEBUG
    log_info("debug线程空置,已执行完成");
#endif
}

#endif

//重载控制台输入强制终止回调函数
static void my_handler(int sig)
{
    flag = 1;
}

//滑动条响应时的回调函数
void on(int, void*){
 
}

int main()
{
    log_debug("程序开始");
    //接收并处理控制台输入
    signal(SIGINT, my_handler);

    std::vector<int> msg_first = serial_com.get_msg();
    color = msg_first[0];
    mode = msg_first[1];
    capture.set("HIVISION", mode);
    aimbot.set(color);
    buffer.set(mode);
    //开始记录初始化时间
    start_t = std::chrono::high_resolution_clock::now();
#ifndef THREADING_DEBUG
    std::thread frame_thread(frame_th);
    std::thread process_thread(process_th);
    std::thread serial_thread(serial_th);
    std::thread debug_thread(debug_th);
    while (1)
    {
        //检测到强行终止进行资源释放
        if (flag)
        {
            return_flag = true;
            log_debug("正在等待线程停止...");
            frame_thread.join();
            process_thread.join();
            serial_thread.join();
            debug_thread.join();

            log_debug("正在等待资源释放...");
            //清空各自的队列
            std::queue<cv::Mat> temp0;
            std::queue<double> temp1;
            std::queue<std::vector<int>> temp2;
            img_queue.swap(temp0);
            time_queue.swap(temp1);
            msg_queue.swap(temp2);
            break;
        }
    }
#endif
#ifdef THREADING_DEBUG
    std::vector<int> msg = {-1, -1, -1};
    while (1)
    {
        auto frame = capture.GetOneFrame();
        auto time = (std::chrono::high_resolution_clock::now() - start_t).count() / TIME_TRANSFORMER;
        if (mode == 0)
        {
            msg = aimbot.process(frame);
        }
        else if (mode == 1 || mode == 2)
        {
            msg = buffer.process(frame, time);
        }
        else
        {
            msg = std::vector<int>{-1, -1, -1};
        }
        serial_com.send_msg(msg);
    }
#endif
    log_debug("程序结束");
    return 0;
}
