#pragma once
#include <string>

#include "debug.hpp"

namespace swq
{
    //读取文件转换成字符串，支持string格式路径和char格式路径
    std::string get_file_str(const std::string & load_path);
    std::string get_file_str(const char * load_path);

    //保存文件，支持字符串和构建完成的数据直接传入
    void save_file(const std::string & load_path,const std::string & file_str);
    void save_file(const char * load_path,const std::string & file_str);

    //判断字符串是否为纯数字组成
    bool is_Numeric(std::string str);

    //计算欧式距离
    float EuclideanDistance(float x0, float y0, float x1, float y1);
}
