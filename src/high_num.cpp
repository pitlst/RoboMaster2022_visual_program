#include <string>
#include <sstream>

#include "high_num.hpp"

using namespace swq;

high_float::high_float()
{
    sign = true;
    front_point->push_back(0);
    back_point->push_back(0);
}

high_float::high_float(int input_num)
{
    if (input_num < 0)
        sign = false;
    else
        sign = true;
    input_num = std::abs(input_num);
    do
    {
        front_point->push_back((char)(input_num % 10)); // 按位倒序写入整数部分
        input_num /= 10;
    } while (input_num != 0);
}

high_float::high_float(double input_num)
{
    *this = high_float(std::to_string(input_num));
}

high_float::high_float(const std::string &input_num)
{
    // 用于判断小数与整数部分交界
    bool type = input_num.find('.') == std::string::npos ? false : true;

    // 默认为正数，读到'-'再变为负数
    sign = true;

    // 逆向迭代
    for (auto iter = input_num.crbegin(); iter < input_num.crend(); iter++)
    {
        char ch = (*iter);
        if (ch == '.') // 遇到小数点则开始向整数部分写入
        {
            type = false;
            iter++;
        }
        if (iter == input_num.rend() - 1) // 读取正负号
        {
            if (ch == '+')
            {
                break;
            }
            if (ch == '-')
            {
                sign = false;
                break;
            }
        }
        // 利用逆向迭代器，将整个数据倒序存入
        if (type)
            back_point->push_back((char)((*iter) - '0'));
        else
            back_point->push_back((char)((*iter) - '0'));
    }
}

high_float::high_float(const char *input_num)
{
    *this = high_float(std::string(input_num));
}

high_float::high_float(const high_float &input_num)
{
    front_point = input_num.front_point;
    back_point = input_num.back_point;
    sign = input_num.sign;
}

high_float::high_float(high_float &&input_num) noexcept
{
    front_point->swap(*(input_num.front_point));
    back_point->swap(*(input_num.back_point));
    sign = input_num.sign;
}

high_float::operator double()
{
    return std::stod(std::string(*this)); 
}

high_float::operator std::string()
{
    std::stringstream ss;
    if (!sign)
    {
        ss << "-";
    }

    for (auto iter = front_point->crbegin(); iter < front_point->crend(); iter++)
    {
        ss << *iter;
    }
    ss << ".";
    for (auto iter = back_point->crbegin(); iter < back_point->crend(); iter++)
    {
        ss << *iter;
    }  
    return ss.str();
}

high_float high_float::operator=(const high_float &input_num)
{
    front_point = input_num.front_point;
    back_point = input_num.back_point;
    sign = input_num.sign;
    return (*this);
}

high_float high_float::operator=(high_float &&input_num) noexcept
{
    front_point->swap(*(input_num.front_point));
    back_point->swap(*(input_num.back_point));
    sign = input_num.sign;
    return (*this);
}

void high_float::trim()
{
    // 因为我们是逆向存储的，所以整数的尾部和小数的首部可能会有多余的0
    auto iter = front_point->rbegin();

    // 对整数部分
    while (!front_point->empty() and (*iter) == 0)
    {
        front_point->pop_back();      // 指向不为空且尾部为0，删去
        iter = front_point->rbegin(); // 再次指向尾部
                                      // 整数部分的“尾部”就是最高位，如00515.424900的左两个0
    }

    if (front_point->size() == 0 and back_point->size() == 0) // 如果整数、小数全为空
    {
        sign = true;
    }

    if (front_point->size() == 0) // 如果整数部分是0
    {
        front_point->push_back(0);
    }

    auto it = back_point->begin();

    // 对小数部分
    while (!back_point->empty() and (*it) == 0)
    {
        it = back_point->erase(it); // 指向不为空且首部为0，删去
                                    // 小数部分的“首部”就是最低位，上例中的右两个0
    }

    if (back_point->size() == 0) // 如果小数部分是0
    {
        back_point->push_back(0);
    }
}

high_float high_float::abs() const
{
    if (sign)
        return (*this);
    else
        return -(*this);
}