#include <string>
#include <sstream>
#include <deque>

#include "high_num.hpp"

//命名空间内的友元公共函数需要在同一命名空间下声明实现
namespace swq
{
    //基本运算符重载
    high_float operator+(const high_float &num1, const high_float &num2)
    {
        high_float temp(num1);
        temp += num2;
        return temp;
    }
    high_float operator-(const high_float &num1, const high_float &num2)
    {
        high_float temp(num1);
        temp -= num2;
        return temp;
    }

    high_float operator*(const high_float &num1, const high_float &num2)
    {
        high_float temp(num1);
        temp *= num2;
        return temp;
    }

    high_float operator/(const high_float &num1, const high_float &num2)
    {
        high_float temp(num1);
        temp /= num2;
        return temp;
    }

    high_float operator-(const high_float &num)
    {
        high_float temp(num);
        temp.sign = !temp.sign;
        return temp;
    }

    bool operator==(const high_float &num1, const high_float &num2)
    {
        //比较长度和符号
        if (num1.sign != num2.sign)
            return false;
        if (num1.front_point.size() != num2.front_point.size())
            return false;
        if (num1.back_point.size() != num2.back_point.size())
            return false;

        //如果长度和符号相同，那么下面逐位比较
        auto iter1 = num1.back_point.begin();
        auto iter2 = num2.back_point.begin();
        while (iter1 != num1.back_point.end())
        {
            if (*iter1 != *iter2)
                return false;
            iter1++;
            iter2++;
        }

        iter1 = num1.front_point.begin();
        iter2 = num2.front_point.begin();
        while (iter1 != num1.front_point.end())
        {
            if (*iter1 != *iter2)
                return false;
            iter1++;
            iter2++;
        }
        return true;
    }

    bool operator!=(const high_float &num1, const high_float &num2)
    {
        return !(num1 == num2);
    }

    bool operator<(const high_float &num1, const high_float &num2)
    {
        //返回值
        bool sign;
        //如果异号
        if (num1.sign != num2.sign)
        {
            //如果num1正，则不小于;反之，则小于
            sign = !num1.sign;
            return sign;
        }
        else
        {
            // 如果同号，先比较整数再比较小数
            if (num1.front_point.size() != num2.front_point.size()) // 如果整数部分不等长
            {
                if (num1.sign) // 如果同为正,则整数部分长的大
                {
                    sign = num1.front_point.size() < num2.front_point.size();
                    return sign;
                }
                else
                {
                    // 同为负，则整数部分长的小
                    sign = num1.front_point.size() > num2.front_point.size();
                    return sign;
                }
            }
            // 如果整数部分等长
            auto iter1 = num1.front_point.begin();
            auto iter2 = num2.front_point.begin();
            while (iter1 != num1.front_point.end())
            {
                if (num1.sign and *iter1 < *iter2)
                    return true;
                if (num1.sign and *iter1 > *iter2)
                    return false;
                if (!num1.sign and *iter1 > *iter2)
                    return true;
                if (!num1.sign and *iter1 < *iter2)
                    return false;
                iter1++;
                iter2++;
            }

            // 下面比较小数部分
            auto it1 = num1.back_point.rbegin();
            auto it2 = num2.back_point.rbegin();
            while (it1 != num1.back_point.rend() and it2 != num2.back_point.rend())
            {
                if (num1.sign and *it1 < *it2)
                    return true;
                if (num1.sign and *it1 > *it2)
                    return false;
                if (!num1.sign and *it1 > *it2)
                    return true;
                if (!num1.sign and *it1 < *it2)
                    return false;
                it1++;
                it2++;
            }
            // 如果整数部分，而小数部分停止前全部一样，那么看谁的小数位更多
            return (num1.sign and it2 != num2.back_point.rend()) or (!num1.sign and it1 != num1.back_point.rend());
        }
    }

    bool operator<=(const high_float &num1, const high_float &num2)
    {
        bool sign = (num1 < num2) or (num1 == num2);
        return sign;
    }

    bool operator>(const high_float &num1, const high_float &num2)
    {
        bool sign = !(num1 <= num2);
        return sign;
    }

    bool operator>=(const high_float &num1, const high_float &num2)
    {
        bool sign = (num1 > num2) or (num1 == num2);
        return sign;
    }

    //扩展运算符重载
    high_float operator+=(const high_float &num1, const high_float &num2)
    {
        // lambda表达式,用于小数部分相加
        auto back_point_add = [](const high_float &num_min, const high_float &num_max) -> high_float
        {
            //返回值
            high_float return_float_;
            //进位
            char carry = 0;
            //小数部分长度
            auto num_min_back_point_size = num_min.back_point.size();
            auto num_max_back_point_size = num_max.back_point.size();
            //校验长度
            if (num_min_back_point_size != num_max_back_point_size)
            {
                auto iter_ = num_max.back_point.rend();
                //指向长出部分
                iter_ = iter_ - num_min_back_point_size - 1;
                //将多余的小数直接交给返回值
                return_float_.back_point.assign(iter_, num_max.back_point.rend());
            }
            //提前声明最大大小,保证迭代器能正常索引
            return_float_.back_point.resize(num_max_back_point_size);
            //迭代器声明
            auto iter_min = num_min.back_point.rbegin();
            auto iter_max = num_max.back_point.rbegin();
            auto iter_return = return_float_.back_point.rbegin();
            //将指向调整到一一对应的位置
            iter_max = iter_max - (num_min_back_point_size - num_max_back_point_size);
            iter_return = iter_return - (num_min_back_point_size - num_max_back_point_size);
            //开始遍历
            while (iter_min != num_min.back_point.rend() and iter_max != num_max.back_point.rend())
            {
                (*iter_return) = (*iter_min) + (*iter_max) + carry;
                // 如果大于9则进位
                carry = ((*iter_return) > 9);
                (*iter_return) = (*iter_return) % 10;
                iter_min++;
                iter_max++;
                iter_return++;
                
            }
            //如果最后有进位,付给对应的整数位
            if (carry)
            {
                return_float_.front_point = {carry};
            }
            return return_float_;
        };

        //返回值
        high_float return_float;
        if (num1.sign == num2.sign) // 只处理同符号数，异号由-减法处理
        {
            //先处理小数部分
            //小数部分长度
            int num1_back_point_size = num1.back_point.size();
            int num2_back_point_size = num2.back_point.size();
            //如果num2小数部分更长
            if (num1_back_point_size < num2_back_point_size)
            {
                return_float = back_point_add(num1, num2);
            }
            //如果num1小数部分更长,同理
            else if (num1_back_point_size > num2_back_point_size)
            {
                return_float = back_point_add(num2, num1);
            }
            //如果二者等长
            else
            {
                return_float = back_point_add(num1, num2);
            }

            //再处理整数部分
            //进位
            char carry = 0;
            //如果有值,证明浮点部分有进位
            if (return_float.front_point.size())
            {
                carry = 1;
            }
            //整数部分长度
            int num1_front_point_size = num1.front_point.size();
            int num2_front_point_size = num2.front_point.size();
            //提前声明整数位的最大大小,保证迭代器正常索引
            return_float.front_point.resize(std::max(num1_front_point_size, num2_front_point_size)+1);
            //声明迭代器
            auto iter1 = num1.front_point.rbegin();
            auto iter2 = num2.front_point.rbegin();
            auto iter_return = return_float.front_point.rbegin();
            //从个位开始相加
            while (iter1 != num1.front_point.rend() and iter2 != num2.front_point.rend())
            {
                (*iter_return) = (*iter1) + (*iter2) + carry;
                //如果大于9则进位
                carry = ((*iter_return) > 9); 
                (*iter_return) = (*iter_return) % 10;
                iter1++;
                iter2++;
                iter_return++;
            }
            //总会有一个先到达end()
            while (iter1 != num1.front_point.rend())
            {
                (*iter_return) = (*iter1) + carry;
                carry = ((*iter_return) > 9);
                (*iter_return) = (*iter_return) % 10;
                iter1++;
                iter_return++;
            }
            while (iter2 != num2.front_point.rend())
            {
                (*iter_return) = (*iter2) + carry;
                carry = ((*iter_return) > 9);
                (*iter_return) = (*iter_return) % 10;
                iter2++;
                iter_return++;
            }
            //如果还有进位，则说明要添加一位
            if (carry != 0)
            {
                *iter_return = carry;
            }
            //处理多余的0
            return_float.trim();
        }
        //如果异号
        else
        {
            //如果被加数为正，加数为负，相当于减等于
            if (num1.sign)
            {
                return_float = num1 - (-num2);
            }
            else
            {
                return_float = num2 - (-num1);
            }
        }
        return return_float;
    }

    high_float operator-=(const high_float &num1, const high_float &num2)
    {       
        //返回值
        high_float return_float;
        //只处理同号，异号由+加法处理
        if (num1.sign == num2.sign) 
        {
            if (num1.sign) // 如果同为正
            {
                if (num1 < num2) // 且被减数小
                {
                    high_float temp(num2 - num1);
                    num1 = -temp;
                    num1.trim();
                    return num1;
                }
            }
            else
            {
                if (-num1 > -num2) // 如果同为负，且被减数绝对值大
                    return num1 = -((-num1) - (-num2));
                else
                    return num1 = (-num2) - (-num1);
            }

            // 下面是同为正，且减数小的情况
            // 小数部分
            char borrow = 0; // 借位
            int num1_back_point_size = num1.back_point.size();
            int num2_back_point_size = num2.back_point.size();
            auto it1 = num1.back_point.begin();
            auto it2 = num2.back_point.begin();

            if (num1_back_point_size > num2_back_point_size) // 如果被减数小数部分更长
            {
                num1_back_point_size -= num2_back_point_size; // 长出部分
                it1 = it1 + num1_back_point_size;             // 跳过长出部分
            }
            else
            { // 如果减数的小数部分更长，则需要给被减数补0
                int number = num2_back_point_size - num1_back_point_size;
                while (number != 0)
                {
                    num1.back_point.insert(num1.back_point.begin(), 0); // 缺少的位数补0
                    number--;
                }
                it1 = num1.back_point.begin(); // 插入后需要重新指向
                it2 = num2.back_point.begin();
            }
            while ((it1 != num1.back_point.end()) and (it2 != num2.back_point.end()))
            {
                (*it1) = (*it1) - (*it2) - borrow;
                borrow = 0;
                if ((*it1) < 0)
                {
                    borrow = 1;
                    (*it1) += 10;
                }
                it1++;
                it2++;
            }
            // 整数部分
            auto iter1 = num1.front_point.begin();
            auto iter2 = num2.front_point.begin();

            while (iter1 != num1.front_point.end() and iter2 != num2.front_point.end())
            {
                (*iter1) = (*iter1) - (*iter2) - borrow;
                borrow = 0;
                if ((*iter1) < 0)
                {
                    borrow = 1;
                    (*iter1) += 10;
                }
                iter1++;
                iter2++;
            }
            while (iter1 != num1.front_point.end())
            {
                (*iter1) = (*iter1) - borrow;
                borrow = 0;
                if ((*iter1) < 0)
                {
                    borrow = 1;
                    (*iter1) += 10;
                }
                else
                    break;
                iter1++;
            }
            num1.trim(); // 把多余的0去掉
            return num1;
        }
        else
        {
            //如果异号
            if (num1 > HFLOAT_ZERO)
            {
                high_float temp(-num2);
                return num1 += temp;
            }
            else
            {
                high_float temp(-num1);
                return num1 = -(num2 + temp);
            }
        }
        return return_float;
    }

    high_float operator*=(const high_float &num1, const high_float &num2)
    {
        high_float result(0);                           // 储存结果
        if (num1 == HFLOAT_ZERO or num2 == HFLOAT_ZERO) // 有0做乘数得0
            result = HFLOAT_ZERO;
        else
        {
            int size = 0;
            std::vector<char> temp_num1(num1.front_point.begin(), num1.front_point.end());                        // 一个临时变量，用于将整数部分与小数部分合并
            if (num1.back_point.size() != 1 or (num1.back_point.size() == 1 and (*num1.back_point.begin()) != 0)) // 如果被乘数有小数部分，插入小数
            {
                temp_num1.insert(temp_num1.begin(), num1.back_point.begin(), num1.back_point.end());
                size += num1.back_point.size();
            }

            std::vector<char> temp_num2(num2.front_point.begin(), num2.front_point.end());                        // 一个临时变量，用于将整数部分与小数部分合并
            if (num2.back_point.size() != 1 or (num2.back_point.size() == 1 and (*num2.back_point.begin()) != 0)) // 如果被乘数有小数部分，插入小数
            {
                temp_num2.insert(temp_num2.begin(), num2.back_point.begin(), num2.back_point.end());
                size += num2.back_point.size();
            }

            // 开始乘法
            auto iter2 = temp_num2.begin();
            while (iter2 != temp_num2.end())
            {
                if (*iter2 != 0)
                {
                    std::deque<char> temp(temp_num1.begin(), temp_num1.end());
                    char carry = 0; // 进位
                    auto iter1 = temp.begin();
                    while (iter1 != temp.end()) // 被乘数乘以某一位乘数
                    {
                        (*iter1) *= (*iter2);
                        (*iter1) += carry;
                        carry = (*iter1) / 10;
                        (*iter1) %= 10;
                        iter1++;
                    }
                    if (carry != 0)
                    {
                        temp.push_back(carry);
                    }
                    int num_of_zeros = iter2 - temp_num2.begin(); // 计算错位
                    while (num_of_zeros--)
                        temp.push_front(0); // 乘得结果后面添0
                    high_float temp2;
                    temp2.front_point.clear();
                    temp2.front_point.insert(temp2.front_point.end(), temp.begin(), temp.end());
                    temp2.trim();
                    result = result + temp2;
                }
                iter2++;
            }
            result.sign = ((num1.sign and num2.sign) or (!num1.sign and !num2.sign));

            // 由于我们将小数和整数合并在一起，因此下面要把小数点重新添上
            if (size != 0)
            {
                if (size >= result.front_point.size()) //说明需要补前导0
                {
                    int n = size - result.front_point.size();
                    for (int i = 0; i <= n; i++)
                        result.front_point.insert(result.front_point.end(), 0);
                }
                result.back_point.clear();
                result.back_point.insert(result.back_point.begin(), result.front_point.begin(), result.front_point.begin() + size);
                result.front_point.erase(result.front_point.begin(), result.front_point.begin() + size);
            }
        }
        num1 = result;
        num1.trim();
        return num1;
    }

    high_float operator/=(const high_float &num1, const high_float &num2)
    {
        if (num2 == HFLOAT_ZERO)
            throw DividedByZeroException();
        if (num1 == HFLOAT_ZERO)
            return num1;
        if (num1 == num2)
            return (num1 = HFLOAT_ONE);

        high_float temp_num1 = num1;
        high_float temp_num2 = num2;

        // 转换成无符号除法来做
        temp_num1.sign = true;
        temp_num2.sign = true;

        int front_point_Size = 0; // 整数部分应为几位
        if ((temp_num2.back_point.size() == 1) and (*(temp_num2.back_point.begin()) == 0))
        {
            // 如果除数没有小数部分，不做操作
        }
        else
        {
            // 否则把除数和乘数同时扩大，直到除数为整数（只对front_point部分运算）
            int t = temp_num2.back_point.size();
            while (t--)
            {
                temp_num1 = temp_num1 * HFLOAT_TEN;
                temp_num2 = temp_num2 * HFLOAT_TEN;
            }
        }
        if (temp_num1 < temp_num2) // 被除数小于除数，应该是0.xxx
        {
            while (temp_num1 < temp_num2)
            {
                temp_num1 *= HFLOAT_TEN;
                front_point_Size--;
            }
        }
        else
        {
            while (temp_num1 > temp_num2)
            {
                temp_num1.back_point.push_back(*temp_num1.front_point.begin());
                temp_num1.front_point.erase(temp_num1.front_point.begin());
                front_point_Size++;
            }
        }

        int k = ACCURACY;
        high_float quotient(0); // 商

        while (k--)
        {
            if (temp_num1 < temp_num2)
            {
                temp_num1 = temp_num1 * HFLOAT_TEN;
                quotient = quotient * HFLOAT_TEN;
            }
            else
            {
                int i;
                high_float compare;
                for (i = 1; i <= 10; i++) // “试商”
                {
                    high_float BF(i);
                    compare = temp_num2 * BF;
                    if (compare > temp_num1)
                        break;
                }
                compare -= temp_num2;
                temp_num1 -= compare;
                high_float index(i - 1);
                quotient = quotient + index;
            }
        }

        if (front_point_Size < 0) // 如果是小数除以大数，结果为0.xxx
        {
            std::vector<char> temp(quotient.front_point.begin(), quotient.front_point.end());
            quotient.front_point.clear();
            quotient.front_point.push_back(0); // 整数部分为0

            quotient.back_point.clear();
            int count_zero = -front_point_Size;
            // 下面先补充前导0
            while (--count_zero)
            {
                quotient.back_point.insert(quotient.back_point.begin(), 0);
            }
            quotient.back_point.insert(quotient.back_point.begin(), temp.begin(), temp.end());
        }
        else
        {
            if (quotient.front_point.size() > front_point_Size)
            {
                std::vector<char> temp(quotient.front_point.begin(), quotient.front_point.end());

                quotient.front_point.clear(); // 这里如果不清空会有错误

                quotient.front_point.assign(temp.end() - front_point_Size, temp.end());

                quotient.back_point.clear(); // 同理需要清空

                quotient.back_point.insert(quotient.back_point.begin(), temp.begin(), temp.end() - front_point_Size);
            }
            else
            {
                // 这一部分意义不明，我觉得不会走到这个分支
                int t = front_point_Size - quotient.front_point.size();
                while (t--)
                {
                    quotient = quotient * HFLOAT_TEN;
                }
            }
        }
        quotient.sign = ((num1.sign and num2.sign) or (!num1.sign and !num2.sign));
        num1 = quotient;
        num1.trim();
        return num1;
    }

    //输入输出重载
    std::ostream &operator<<(std::ostream &out, const high_float &num)
    {
        if (!num.sign) // 负数
        {
            out << "-";
        }

        for (auto iter = num.front_point.rbegin(); iter != num.front_point.rend(); iter++) // 输出整数部分
        {
            out << (char)((*iter) + '0');
        }

        out << '.';

        for (auto iter = num.back_point.rbegin(); iter != num.back_point.rend(); iter++) // 输出小数部分
        {
            out << (char)((*iter) + '0');
        }
        return out;
    }

    std::istream &operator>>(std::istream &in, high_float &num)
    {
        std::string str;
        in >> str;
        num = high_float(str);
        return in;
    }
}

using namespace swq;

high_float::high_float()
{
    sign = true;
    front_point.push_back(0);
    back_point.push_back(0);
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
        //按位倒序写入整数部分
        front_point.push_back((char)(input_num % 10));
        input_num /= 10;
    } while (input_num != 0);
    //小数部分写0
    back_point.push_back(0);
}

high_float::high_float(double input_num)
{
    *this = high_float(std::to_string(input_num));
}

high_float::high_float(const std::string &input_num)
{
    //用于判断小数与整数部分交界
    bool type = input_num.find('.') == std::string::npos ? false : true;

    //默认为正数，读到'-'再变为负数
    sign = true;

    //正向遍历
    for (auto iter = input_num.cbegin(); iter < input_num.cend(); iter++)
    {
        char ch = (*iter);
        //遇到小数点则开始向小数数部分写入
        if (ch == '.')
        {
            type = false;
            iter++;
        }
        //读取正负号
        if (iter == input_num.cbegin())
        {
            if (ch == '-')
            {
                sign = false;
            }
        }
        //写入时将字符串转换为数字存储
        if (type)
            front_point.push_back((char)((*iter) - '0'));
        else
            back_point.push_back((char)((*iter) - '0'));
    }
}

high_float::high_float(const char *input_num)
{
    *this = high_float(std::string(input_num));
}

high_float::high_float(const high_float &input_num)
{
    back_point = input_num.back_point;
    sign = input_num.sign;
}

high_float::high_float(high_float &&input_num) noexcept
{
    front_point.swap(input_num.front_point);
    back_point.swap(input_num.back_point);
    sign = input_num.sign;
}

high_float::operator double()
{
    return std::stod(std::string(*this));
}

high_float::operator std::string()
{
    return str();
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
    front_point.swap(input_num.front_point);
    back_point.swap(input_num.back_point);
    sign = input_num.sign;
    return (*this);
}

high_float high_float::abs() const
{
    if (sign)
        return (*this);
    else
        return -(*this);
}

bool high_float::empty() const
{
    return front_point.empty() and back_point.empty();
}

void high_float::clear()
{
    sign = true;
    front_point.clear();
    back_point.clear();
}

std::string high_float::str() const
{
    std::stringstream ss;
    if (!sign)
    {
        ss << "-";
    }

    for (auto iter = front_point.cbegin(); iter < front_point.cend(); iter++)
    {
        ss << (int)(*iter);
    }
    ss << ".";
    for (auto iter = back_point.cbegin(); iter < back_point.cend(); iter++)
    {
        ss << (int)(*iter);
    }
    return ss.str();
}

void high_float::trim()
{
    //如果整数、小数全为空
    if (front_point.size() == 0 and back_point.size() == 0)
    {
        sign = true;
        return;
    }
    //如果整数部分是0
    if (front_point.size() == 0)
    {
        front_point.push_back(0);
    }
    else
    {
        auto iter = front_point.begin();
        //对整数部分从头遍历，直到没有0为止
        while (!front_point.empty() and (*iter) == 0)
        {
            iter++;
        }
        //删除开头的全部0
        front_point.erase(front_point.begin(), iter);
    }
    //如果小数部分是0
    if (back_point.size() == 0)
    {
        back_point.push_back(0);
    }
    else
    {
        auto it = back_point.rbegin();
        // 对小数部分
        while (!back_point.empty() and (*it) == 0)
        {
            //小数部分只有结尾有0，如果有0直接删除即可
            back_point.pop_back();
            it = back_point.rbegin();
        }
    }
}