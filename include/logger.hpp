#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <initializer_list>

#include "debug.hpp"

//定义一下日志文件所在地

#define log_debug(...) \
    swq::logger::instance().log(swq::logger::DEBUG, swq::logger::instance().format(__VA_ARGS__))

#define log_info(...) \
    swq::logger::instance().log(swq::logger::INFO, swq::logger::instance().format(__VA_ARGS__))

#define log_warn(...) \
    swq::logger::instance().log(swq::logger::WARN, swq::logger::instance().format(__VA_ARGS__))

#define log_error(...) \
    swq::logger::instance().log(swq::logger::ERROR, swq::logger::instance().format(__VA_ARGS__))

#define log_fatal(...) \
    swq::logger::instance().log(swq::logger::FATAL, swq::logger::instance().format(__VA_ARGS__))

namespace swq
{

    class logger
    {
    public:
        enum Level
        {
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NONE
        };

        std::vector<std::string> label =
            {
                "DEBUG",
                "INFO",
                "WARN",
                "ERROR",
                "FATAL",
        };

        static logger &instance();
        void set_level(Level input_level);
        void open(const std::string &filename);
        void log(Level level, const std::string &input);
        void close();

        //模板推导，直接在头文件实现，格式化输入的变量
        template <typename T, typename... args>
        std::string format(const T &input, const args &...rest)
        {
            ss << input;
            return format(rest...);
        }
        //包中最后一个元素
        template <typename T>
        std::string format(const T &input)
        {
            ss << input;
            auto temp = ss.str();
            //清空保存的字符串
            ss.str(std::string());
            //字符串流复位
            ss.clear();
            return temp; 
        }

    private:
        logger();
        logger(const std::string &filepath);
        ~logger();

        Level m_level;
        std::string m_filename;
        std::fstream m_file;
        std::stringstream ss;
    };
}
