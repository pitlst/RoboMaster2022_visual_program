#pragma once
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

//定义一下日志文件所在地

#define log_debug(input) \
    swq::logger::instance().log(swq::logger::DEBUG, swq::logger::instance().format(input))

#define log_info(input) \
    swq::logger::instance().log(swq::logger::INFO, swq::logger::instance().format(input))

#define log_warn(input) \
    swq::logger::instance().log(swq::logger::WARN, swq::logger::instance().format(input))

#define log_error(input) \
    swq::logger::instance().log(swq::logger::ERROR, swq::logger::instance().format(input))

#define log_fatal(input) \
    swq::logger::instance().log(swq::logger::FATAL, swq::logger::instance().format(input))


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

        static logger & instance();
        void set_level(Level input_level);
        void open(const std::string &filename);
        void log(Level level, const std::string & input);
        void close();

        //模板推导，直接在头文件实现，格式化输入的变量
        template<typename T>
        std::string format(T input){
            std::stringstream ss;
            ss << input;
            return ss.str();
        }

    private:
        logger();
        logger(const std::string & filepath);
        ~logger();

        Level m_level;
        std::string m_filename;
        std::fstream m_file;
    };  
}


