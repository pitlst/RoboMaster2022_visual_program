#include "utils.hpp"
#include <cmath>
#include <fstream>
#include <sstream>

std::string swq::get_file_str(const std::string & load_path)
{
    std::ifstream ifs;
    ifs.open(load_path);
    if (!ifs.is_open())
    {
        throw std::logic_error("read filure");
    }
    std::istreambuf_iterator<char> beg(ifs), end;
    std::string strdata(beg, end);
    ifs.close();
    return strdata;
}

std::string swq::get_file_str(const char * load_path)
{
    std::string temp(load_path);
    return swq::get_file_str(temp);
}

void swq::save_file(const std::string & load_path, const std::string & file_str)
{
    std::ofstream ofs;
    ofs.open(load_path, std::ios_base::out | std::ios_base::trunc);  //删除文件重写
    if (!ofs.is_open())
    {
        throw std::logic_error("write filure");
    }
    ofs << file_str;
    ofs.close();
}

void swq::save_file(const char * load_path, const std::string & file_str)
{
    std::string temp(load_path);
    swq::save_file(temp,file_str);
}

bool swq::is_Numeric(std::string str)
{
    for (auto &ch : str)
    {
        if (ch < '0' || ch > '9')
        {
            return false;
        }
    }
    return true;
}

inline float swq::EuclideanDistance(float x0, float y0, float x1, float y1)
{
    return pow(pow(x0 - x1, 2) + pow(y0 - y1, 2), 0.5);
}