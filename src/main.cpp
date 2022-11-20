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
//滑动条用的全局回调数据
swq::m_fiter_para bar_aimbot_global;
swq::m_energy_para bar_buffer_global;

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
        //线程退出
        if (return_flag)
        {
            return;
        }
        
        auto time = std::chrono::high_resolution_clock::now() - start_t;

        img_mtx.lock();
        //限制队列最大大小，防止消费者抢不到锁
        if (img_queue.size() > FRAME_QUEUE_SIZE_MAX)
        {
            img_mtx.unlock();
            continue;
        }
        auto frame = capture.GetOneFrame();
        img_queue.push(frame);
        time_queue.push(time.count() / TIME_TRANSFORMER);
        img_mtx.unlock();

        
        count++;
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
        //保存录像
#ifdef SAVE_VIDEO

#endif

    }
}

#ifdef COMPILE_DEBUG
void process_th()
{
    cv::Mat frame;
    double time;
    int k;
    std::vector<int> msg = {-1, -1, -1};
    cv::namedWindow("调参窗口aimbot_1");
    cv::namedWindow("调参窗口aimbot_2");
    cv::namedWindow("调参窗口aimbot_3");
    cv::namedWindow("调参窗口buffer");
    cv::namedWindow("frame_debug");
    cv::namedWindow("mask_debug");
    //获取相对应的数据
    ::bar_aimbot_global = swq::trans_para_to_bar(aimbot.get_argument());
    ::bar_buffer_global = swq::trans_para_to_bar(buffer.get_argument());
    //创建滑动条
    swq::bar_creat_aimbot(bar_aimbot_global);
    swq::bar_creat_bufferr(bar_buffer_global);
    while (1)
    {
        //线程退出
        if (k == 27 || flag == 1)
        {
            cv::destroyAllWindows();
            flag = 1;
            break;
        }

        img_mtx.lock();
        //队列为空时等待生产者
        if (img_queue.empty())
        {
            img_mtx.unlock();
            continue;
        }
        frame = img_queue.front();
        time = time_queue.front();
        img_queue.pop();
        time_queue.pop();
        
        if (mode == 0)
        {
            msg = aimbot.process(frame);
            auto debug_frame = aimbot.debug_frame(frame);
            aimbot.updata_argument(trans_bar_to_para(bar_aimbot_global));
            aimbot.update_json(PATH_ARMOR_JSON);
            cv::imshow("frame_debug", debug_frame.front());
            cv::imshow("mask_debug", debug_frame.back());
        }
        else if (mode == 1 || mode == 2)
        {
            buffer.set(mode);
            auto frame_temp = frame.clone();
            msg = buffer.process(frame, time);
            log_debug("msg is :", msg[0], " ", msg[1], " ", msg[2]);
            buffer.updata_argument(trans_bar_to_para(bar_buffer_global));
            auto debug_frame = buffer.debug_frame(frame_temp);
            // buffer.update_json(PATH_ENERGY_JSON);
            cv::imshow("frame_debug", debug_frame.front());
            // cv::imshow("mask_debug", debug_frame.back());
        }
        else
        {
            msg = std::vector<int>{-1, -1, -1};
        }
        img_mtx.unlock();

        msg_mtx.lock();
        msg_queue.push(msg);
        msg_mtx.unlock();
        
        //获取按键
        k = cv::waitKey(1);
    }
}
#endif
#ifndef COMPILE_DEBUG
void process_th()
{
    cv::Mat frame;
    double time;
    std::vector<int> msg = {-1, -1, -1};
    while (1)
    {
        //线程退出
        if (return_flag)
        {
            return;
        }

        img_mtx.lock();
        //队列为空时等待生产者
        if (img_queue.empty())
        {
            img_mtx.unlock();
            continue;
        }
        frame = img_queue.front();
        time = time_queue.front();
        img_queue.pop();
        time_queue.pop();

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
        img_mtx.unlock();

        msg_mtx.lock();
        msg_queue.push(msg);
        msg_mtx.unlock();
    }
}
#endif

void serial_th()
{
    std::vector<int> msg = {-1, -1, -1};
    int count = 0;
    while (1)
    {
        //线程退出
        if (return_flag)
        {
            return;
        }

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
    }
}
#endif

//重载控制台输入强制终止回调函数
static void my_handler(int sig)
{
    flag = 1;
}

int main()
{
    log_debug("主线程开始");
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
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //检测到强行终止进行资源释放
        if (flag)
        {
            return_flag = true;
            log_info("正在等待线程停止...");
            frame_thread.join();
            process_thread.join();
            serial_thread.join();
            log_info("正在等待资源释放...");
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
    log_debug("主线程结束");
    return 0;
}
