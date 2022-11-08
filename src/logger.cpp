#include <iostream>
#include "logger.hpp"
using namespace swq;

logger::logger()
{
    #ifdef COMPILE_DEBUG
    m_level = DEBUG;
    #endif
    #ifdef COMPILE_RELEASE
    m_level = INFO;
    #endif
    #ifdef COMPILE_MINSIZEREL
    m_level = FATAL;
    #endif
    open("");
}

logger::logger(const std::string & filepath)
{
    #ifdef COMPILE_DEBUG
    m_level = DEBUG;
    #endif
    #ifdef COMPILE_RELEASE
    m_level = INFO;
    #endif
    #ifdef COMPILE_MINSIZEREL
    m_level = FATAL;
    #endif
    open(filepath);
}

logger::~logger()
{
    close();
}

logger & logger::instance()
{
	static logger m_instance(LOG_FILE_PATH);
    return m_instance;
}

void logger::set_level(Level input_level)
{
    m_level = input_level;
}

void logger::open(const std::string &filename)
{
    m_filename = filename;
    if(m_filename.empty())
    {
        return;
    }
    //以追加模式打开文件
    m_file.open(filename, std::ios::app);
    if (m_file.fail())
    {
        throw std::logic_error("open log file failed: " + filename);
    }
    //将文件指针定位到文件的末尾
    m_file.seekg(0, std::ios::end);
    //获取系统时间
    time_t rawtime = time(nullptr);
    struct tm *ptminfo = localtime(&rawtime);
    std::stringstream ss;
    ss << ptminfo->tm_year + 1900;
    ss << "-";
    ss << ptminfo->tm_mon + 1;
    ss << "-";
    ss << ptminfo->tm_mday;
    ss << " ";
    ss << ptminfo->tm_hour;
    ss << ":";
    ss << ptminfo->tm_min;
    ss << ":";
    ss << ptminfo->tm_sec;
    std::cout << "----------" << ss.str() << "----------" << std::endl;
    //在日志中表示开始下一次写入文件
    m_file << "----------" << ss.str() << "----------" << std::endl;
}

void logger::log(Level level, const std::string & input)
{
    if (m_level > level)
    {
        return;
    }
    if (m_filename.empty() && !m_file.good())
    {
        throw std::logic_error("open log file failed: " + m_filename);
    }
    //获取系统时间
    time_t rawtime = time(nullptr);
    struct tm *ptminfo = localtime(&rawtime);
    std::stringstream ss;
    //写入系统时间
    ss << ptminfo->tm_year + 1900;
    ss << "-";
    ss << ptminfo->tm_mon + 1;
    ss << "-";
    ss << ptminfo->tm_mday;
    ss << " ";
    ss << ptminfo->tm_hour;
    ss << ":";
    ss << ptminfo->tm_min;
    ss << ":";
    ss << ptminfo->tm_sec;
    ss << ": ";
    ss << label[level];
    ss << " :: ";
    ss << input;
    //写入命令行
    std::cout << ss.str() << std::endl;
    //写入文件
    if(!m_filename.empty())
    {
        m_file << ss.str() << std::endl;
    }
}
    

void logger::close()
{
    if(m_filename.empty())
    {
        return;
    }
    else
    {
        m_file.close();
    }
}