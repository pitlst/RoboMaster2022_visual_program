#include <string>
#include <sstream>

#include "high_num.hpp"

//命名空间内的友元公共函数需要在同一命名空间下声明实现
namespace swq
{
    //基本运算符重载
    high_float operator+=(high_float &num1, const high_float &num2)
    {
        num1 = num1 + num2;
        return num1;
    }
    high_float operator-=(high_float &num1, const high_float &num2)
    {
        num1 = num1 - num2;
        return num1;
    }

    high_float operator*=(high_float &num1, const high_float &num2)
    {
        num1 = num1 * num2;
        return num1;
    }

    high_float operator/=(high_float &num1, const high_float &num2)
    {
        num1 = num1 / num2;
        return num1;
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
                if (num1.sign AND *iter1 < *iter2)
                    return true;
                if (num1.sign AND *iter1 > *iter2)
                    return false;
                if (!num1.sign AND *iter1 > *iter2)
                    return true;
                if (!num1.sign AND *iter1 < *iter2)
                    return false;
                iter1++;
                iter2++;
            }

            // 下面比较小数部分
            auto it1 = num1.back_point.rbegin();
            auto it2 = num2.back_point.rbegin();
            while (it1 != num1.back_point.rend() AND it2 != num2.back_point.rend())
            {
                if (num1.sign AND *it1 < *it2)
                    return true;
                if (num1.sign AND *it1 > *it2)
                    return false;
                if (!num1.sign AND *it1 > *it2)
                    return true;
                if (!num1.sign AND *it1 < *it2)
                    return false;
                it1++;
                it2++;
            }
            // 如果整数部分，而小数部分停止前全部一样，那么看谁的小数位更多
            return (num1.sign AND it2 != num2.back_point.rend()) OR (!num1.sign AND it1 != num1.back_point.rend());
        }
    }

    bool operator<=(const high_float &num1, const high_float &num2)
    {
        bool sign = (num1 < num2) OR (num1 == num2);
        return sign;
    }

    bool operator>(const high_float &num1, const high_float &num2)
    {
        bool sign = !(num1 <= num2);
        return sign;
    }

    bool operator>=(const high_float &num1, const high_float &num2)
    {
        bool sign = (num1 > num2) OR (num1 == num2);
        return sign;
    }

    //扩展运算符重载
    high_float operator+(const high_float &num1, const high_float &num2)
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
            //提前声明最大大小,保证迭代器能正常索引
            return_float_.back_point.resize(num_max_back_point_size);
            //校验长度
            if (num_min_back_point_size != num_max_back_point_size)
            {
                auto iter_ = num_max.back_point.begin();
                auto iter_re_ = return_float_.back_point.begin();
                //指向长出部分
                iter_ = iter_ + num_min_back_point_size;
                iter_re_ = iter_re_ + num_min_back_point_size;
                //将多余的小数交给返回值
                while (iter_ != num_max.back_point.end() OR iter_re_ != return_float_.back_point.end())
                {
                    (*iter_re_) = (*iter_);
                    iter_++;
                    iter_re_++;
                }
            }
            //迭代器声明
            auto iter_min = num_min.back_point.rbegin();
            auto iter_max = num_max.back_point.rbegin();
            auto iter_return = return_float_.back_point.rbegin();
            //将指向调整到一一对应的位置
            iter_max = iter_max + (num_max_back_point_size - num_min_back_point_size);
            iter_return = iter_return + (num_max_back_point_size - num_min_back_point_size);
            //开始遍历
            while (iter_min != num_min.back_point.rend() AND iter_max != num_max.back_point.rend())
            {
                (*iter_return) = (*iter_min) + (*iter_max) + carry;
                // 如果大于9则进位
                carry = 0;
                if ((*iter_return) > 9)
                    carry = 1;
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
            //处理小数部分
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

            //处理整数部分
            //进位
            char carry = 0;
            //如果有值,证明浮点部分有进位
            if (return_float.front_point.size() AND return_float.front_point[0] != 0)
            {
                carry = 1;
            }
            //提前声明整数位的最大大小,保证迭代器正常索引
            return_float.front_point.resize(std::max(num1.front_point.size(), num2.front_point.size()) + 1);
            //声明迭代器
            auto iter1 = num1.front_point.rbegin();
            auto iter2 = num2.front_point.rbegin();
            auto iter_return = return_float.front_point.rbegin();
            //从个位开始相加
            while (iter1 != num1.front_point.rend() AND iter2 != num2.front_point.rend())
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

    high_float operator-(const high_float &num1, const high_float &num2)
    {
        //返回值
        high_float return_float;
        //只处理同号，异号由+加法处理
        if (num1.sign == num2.sign)
        {
            //如果同为正且被减数小
            if (num1.sign AND num1 < num2)
            {
                return_float = -(num2 - num1);
            }
            //如果同为负，且被减数绝对值大
            else if (!num1.sign AND num1.abs() > num2.abs())
            {
                return_float = -((-num1) - (-num2));
            }
            //如果同为负，且减数绝对值大
            else if (!num1.sign AND num1.abs() <= num2.abs())
            {
                return_float = (-num2) - (-num1);
            }
            //下面是同为正，且减数小的情况
            else
            {
                //小数部分
                //借位
                char borrow = 0;
                int num1_back_point_size = num1.back_point.size();
                int num2_back_point_size = num2.back_point.size();
                //提前声明整数位的最大大小,保证迭代器正常索引
                return_float.back_point.resize(std::max(num1_back_point_size, num2_back_point_size));
                //缓存的临时变量
                std::deque<char> temp_num1(num1.back_point.begin(), num1.back_point.end());
                std::deque<char> temp_num2(num2.back_point.begin(), num2.back_point.end());
                auto iter1_f = temp_num1.rbegin();
                auto iter2_f = temp_num2.rbegin();
                auto iter_return_f = return_float.back_point.rbegin();
                //如果被减数小数部分更长
                if (num1_back_point_size > num2_back_point_size)
                {
                    //长出部分
                    num1_back_point_size -= num2_back_point_size;
                    //跳过长出部分
                    iter1_f = iter1_f + num1_back_point_size;
                }
                //如果被减数小数部分更长,需要补0
                else
                {
                    //如果减数的小数部分更长，则需要给被减数补0
                    int number = num2_back_point_size - num1_back_point_size;
                    while (number != 0)
                    {
                        //缺少的位数补0
                        temp_num1.emplace_back((char)0);
                        number--;
                    }
                    //插入后需要重新指向
                    iter1_f = temp_num1.rbegin();
                }
                //开始计算
                while ((iter1_f != temp_num1.rend()) AND (iter2_f != temp_num2.rend()))
                {
                    (*iter_return_f) = (*iter1_f) - (*iter2_f) - borrow;
                    borrow = 0;
                    if ((*iter_return_f) < 0)
                    {
                        borrow = 1;
                        (*iter_return_f) += 10;
                    }
                    iter1_f++;
                    iter2_f++;
                    iter_return_f++;
                }
                //整数部分
                //提前声明整数位的最大大小,保证迭代器正常索引
                return_float.front_point.resize(std::max(num1.front_point.size(), num2.front_point.size()));
                auto iter1_i = num1.front_point.rbegin();
                auto iter2_i = num2.front_point.rbegin();
                auto iter_return_i = return_float.front_point.rbegin();
                while (iter1_i != num1.front_point.rend() AND iter2_i != num2.front_point.rend())
                {
                    (*iter_return_i) = (*iter1_i) - (*iter2_i) - borrow;
                    borrow = 0;
                    if ((*iter_return_i) < 0)
                    {
                        borrow = 1;
                        (*iter_return_i) += 10;
                    }
                    iter1_i++;
                    iter2_i++;
                    iter_return_i++;
                }
                while (iter1_i != num1.front_point.rend())
                {
                    (*iter_return_i) = (*iter1_i) - borrow;
                    borrow = 0;
                    if ((*iter_return_i) < 0)
                    {
                        borrow = 1;
                        (*iter_return_i) += 10;
                    }
                    iter1_i++;
                    iter_return_i++;
                }
            }
        }
        else
        {
            //如果异号
            if (num1.sign)
            {
                return_float = num1 + (-num2);
            }
            else
            {
                return_float = -(num2 + (-num1));
            }
        }
        //把多余的0去掉
        return_float.trim();
        return return_float;
    }

    high_float operator*(const high_float &num1, const high_float &num2)
    {
        //返回值
        high_float return_float;
        //有0做乘数得0
        if (num1 == HFLOAT_ZERO OR num2 == HFLOAT_ZERO)
            return_float = HFLOAT_ZERO;
        else
        {
            //建立两个临时性变量，去除多余0
            high_float temp1(num1);
            high_float temp2(num2);
            //该变量用于保存小数点移动的位数
            int size = 0;
            //一个临时变量，用于将整数部分与小数部分合并
            std::deque<char> temp_num1(temp1.front_point.begin(), temp1.front_point.end());
            //如果被乘数有小数部分，插入小数
            if (temp1.back_point.size() > 1 OR (temp1.back_point.size() == 1 AND (*temp1.back_point.begin()) != 0))
            {
                temp_num1.insert(temp_num1.end(), temp1.back_point.begin(), temp1.back_point.end());
                size += temp1.back_point.size();
            }
            //一个临时变量，用于将整数部分与小数部分合并
            std::deque<char> temp_num2(temp2.front_point.begin(), temp2.front_point.end());
            //如果被乘数有小数部分，插入小数
            if (temp2.back_point.size() > 1 OR (temp2.back_point.size() == 1 AND (*temp2.back_point.begin()) != 0))
            {
                temp_num2.insert(temp_num2.end(), temp2.back_point.begin(), temp2.back_point.end());
                size += temp2.back_point.size();
            }
            temp1.clear();
            temp2.clear();
            //开始乘法
            auto iter2 = temp_num2.rbegin();
            while (iter2 != temp_num2.rend())
            {
                if (*iter2 != 0)
                {
                    std::deque<char> temp(temp_num1.begin(), temp_num1.end());
                    //进位
                    char carry = 0;
                    auto iter1 = temp.rbegin();
                    //被乘数乘以某一位乘数
                    while (iter1 != temp.rend())
                    {
                        (*iter1) *= (*iter2);
                        (*iter1) += carry;
                        carry = (*iter1) / 10;
                        (*iter1) %= 10;
                        iter1++;
                    }
                    if (carry != 0)
                    {
                        temp.emplace_front(carry);
                    }
                    int num_of_zeros = iter2 - temp_num2.rbegin(); //计算错位
                    while (num_of_zeros--)
                        temp.emplace_back(0); //乘得结果后面添0
                    high_float temp2;
                    temp2.front_point.clear();
                    temp2.front_point.insert(temp2.front_point.end(), temp.begin(), temp.end());
                    temp2.trim();
                    return_float = return_float + temp2;
                }
                iter2++;
            }
            return_float.sign = ((num1.sign AND num2.sign) OR (!num1.sign AND !num2.sign));
            //由于我们将小数和整数合并在一起，因此下面要把小数点重新添上
            if (size != 0)
            {
                //说明需要补前导0
                if (size >= return_float.front_point.size())
                {
                    int n = size - return_float.front_point.size();
                    for (int i = 0; i <= n; i++)
                        return_float.front_point.insert(return_float.front_point.begin(), 0);
                }
                //移动小数点
                size = return_float.front_point.size() - size;
                return_float.back_point.clear();
                return_float.back_point.insert(return_float.back_point.begin(), return_float.front_point.begin() + size, return_float.front_point.end());
                return_float.front_point.erase(return_float.front_point.begin() + size, return_float.front_point.end());
            }
        }
        return_float.trim();
        return return_float;
    }

    high_float operator/(const high_float &num1, const high_float &num2)
    {
        if (num2 == HFLOAT_ZERO)
        {
            throw std::logic_error("除数为0");
        }
        if (num1 == HFLOAT_ZERO)
        {
            return HFLOAT_ZERO;
        }
        if (num1 == num2)
        {
            return HFLOAT_ONE;
        }
        high_float temp_num1 = num1;
        high_float temp_num2 = num2;
        //转换成无符号除法来做
        temp_num1.sign = true;
        temp_num2.sign = true;
        //把除数和被除数变成整数
        if (temp_num2.back_point.size() > 1 OR ((temp_num2.back_point.size() == 1) AND (*(temp_num2.back_point.begin()) != 0)))
        {
            //有小数部分就把除数和乘数同时扩大，直到除数为整数
            int t = std::max(temp_num2.back_point.size(), temp_num1.back_point.size());
            while (t--)
            {
                temp_num1 = temp_num1 * HFLOAT_TEN;
                temp_num2 = temp_num2 * HFLOAT_TEN;
            }
        }
        int front_point_Size = 0;
        int back_point_size = 0;
        //被除数小于除数，应该是0.xxx
        if (temp_num1 < temp_num2)
        {
            while (temp_num1 < temp_num2)
            {
                temp_num1 *= HFLOAT_TEN;
                front_point_Size++;
            }
        }

        //极限迭代次数
        int k = ACCURACY;
        //商
        high_float return_float(0);
        //试商的被除数
        high_float compare;
        //试商的除数
        high_float divisor;
        //构造第一次试商的被除数
        decltype(compare.front_point) temp;
        auto it_num1 = temp_num1.front_point.begin();
        it_num1 += temp_num2.front_point.size();
        temp.insert(temp.begin(), temp_num1.front_point.begin(), it_num1);
        compare.front_point.swap(temp);
        //试商
        while (k--)
        {
            if (compare < temp_num2)
            {
                //如果被除数更小,乘10
                if (it_num1 != temp_num1.front_point.end())
                {
                    compare.front_point.emplace_back(*it_num1);
                    compare.trim();
                    it_num1++;
                }
                else
                {
                    compare.front_point.emplace_back((char)0);
                    compare.trim();
                    back_point_size++;
                }
            }
            int i;
            for (i = 1; i <= 10; i++)
            {
                high_float BF(i);
                divisor = temp_num2 * BF;
                if (divisor > compare)
                    break;
            }
            //真正的除数
            divisor -= temp_num2;
            //本次试商除完的被除数
            compare -= divisor;
            //真正的商
            high_float index(i - 1);
            return_float = return_float * HFLOAT_TEN + index;
        }
        //如果是小数除以大数，结果为0.xxx
        if (front_point_Size)
        {
            std::deque<char> temp(return_float.front_point.begin(), return_float.front_point.end());
            return_float.front_point.clear();
            return_float.front_point.emplace_back(0); //整数部分为0
            return_float.back_point.clear();
            //下面先补充前导0
            while (--front_point_Size)
            {
                return_float.back_point.emplace_front(0);
            }
            return_float.back_point.insert(return_float.back_point.end(), temp.begin(), temp.end());
        }
        //商有多的大数部分
        else if (back_point_size)
        {
            back_point_size = return_float.front_point.size() - back_point_size;
            std::deque<char> temp_e(return_float.front_point.begin() + back_point_size, return_float.front_point.end());
            std::deque<char> temp_f(return_float.front_point.begin(), return_float.front_point.begin() + back_point_size);
            return_float.front_point.clear();
            return_float.back_point.clear();
            return_float.front_point.insert(return_float.front_point.end(), temp_f.begin(), temp_f.end());
            return_float.back_point.insert(return_float.back_point.end(), temp_e.begin(), temp_e.end());
        }

        return_float.sign = ((num1.sign AND num2.sign) OR (!num1.sign AND !num2.sign));
        return_float.trim();
        return return_float;
    }

    high_float operator%(const high_float &num1, int num2_int)
    {
        high_float num2(num2_int);
        if (num1.abs() < num2.abs())
        {
            return num1;
        }
        if (num2 == HFLOAT_ZERO)
        {
            throw std::logic_error("除数为0");
        }
        if (num2 == HFLOAT_ONE)
        {
            return HFLOAT_ZERO;
        }
        high_float temp_num1 = num1;
        high_float temp_num2 = num2;
        //转换成无符号除法来做
        temp_num1.sign = true;
        temp_num2.sign = true;
        //把除数和被除数变成整数
        if (temp_num2.back_point.size() > 1 OR ((temp_num2.back_point.size() == 1) AND (*(temp_num2.back_point.begin()) != 0)))
        {
            //有小数部分就把除数和乘数同时扩大，直到除数为整数
            int t = std::max(temp_num2.back_point.size(), temp_num1.back_point.size());
            while (t--)
            {
                temp_num1 = temp_num1 * HFLOAT_TEN;
                temp_num2 = temp_num2 * HFLOAT_TEN;
            }
        }
        //试商的被除数
        high_float compare;
        //试商的除数
        high_float divisor;
        //构造第一次试商的被除数
        decltype(compare.front_point) temp;
        auto it_num1 = temp_num1.front_point.begin();
        it_num1 += temp_num2.front_point.size();
        temp.insert(temp.begin(), temp_num1.front_point.begin(), it_num1);
        compare.front_point.swap(temp);
        //试商
        while (true)
        {
            if (compare < temp_num2)
            {
                //如果被除数更小,乘10
                if (it_num1 != temp_num1.front_point.end())
                {
                    compare.front_point.emplace_back(*it_num1);
                    compare.trim();
                    it_num1++;
                }
                //如果原数字使用完则证明结束
                else
                {
                    break;
                }
            }
            int i;
            for (i = 1; i <= 10; i++)
            {
                high_float BF(i);
                divisor = temp_num2 * BF;
                if (divisor > compare)
                    break;
            }
            //真正的除数
            divisor -= temp_num2;
            //本次试商除完的被除数
            compare -= divisor;
        }
        return compare;
    }

    //输入输出重载
    std::ostream &operator<<(std::ostream &out, const high_float &num)
    {
        if (!num.sign) // 负数
        {
            out << "-";
        }

        for (auto iter = num.front_point.begin(); iter != num.front_point.end(); iter++) // 输出整数部分
        {
            out << (int)((*iter));
        }

        out << '.';

        for (auto iter = num.back_point.begin(); iter != num.back_point.end(); iter++) // 输出小数部分
        {
            out << (int)((*iter));
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

    high_float min(const high_float &num1, const high_float &num2)
    {
        if (num1 > num2)
        {
            return num2;
        }
        return num1;
    }

    high_float max(const high_float &num1, const high_float &num2)
    {
        if (num1 > num2)
        {
            return num1;
        }
        return num2;
    }
}

using namespace swq;

high_float::high_float()
{
    sign = true;
    front_point.emplace_back(0);
    back_point.emplace_back(0);
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
        //按位正序写入整数部分
        front_point.emplace_front((char)(input_num % 10));
        input_num /= 10;
    } while (input_num != 0);
    //去除多余的0
    trim();
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
            front_point.emplace_back((char)((*iter) - '0'));
        else
            back_point.emplace_back((char)((*iter) - '0'));
    }

    //去除多余的0
    trim();
}

high_float::high_float(const char *input_num)
{
    *this = high_float(std::string(input_num));
}

high_float::high_float(const high_float &input_num) : front_point(input_num.front_point), back_point(input_num.back_point), sign(input_num.sign)
{
}

high_float::high_float(high_float &&input_num) noexcept
{
    front_point.swap(input_num.front_point);
    back_point.swap(input_num.back_point);
    sign = input_num.sign;
}

high_float::operator double()
{
    return std::stod((*this).str());
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

//开根号 使用二分法逼近
high_float high_float::square_root() const
{
    high_float min, max, mid;
    high_float self((*this));
    min = 0;
    if (self <= HFLOAT_ONE)
    {
        self = 1;
    }
    max = self;
    mid = self / HFLOAT_TWO;
    while (mid * mid > self + PRECISION || mid * mid < self - PRECISION)
    {
        mid = (max + min) / HFLOAT_TWO;
        if (mid * mid < self + PRECISION)
        {
            min = mid; //根值偏小，升高下边界
        };
        if (mid * mid > self - PRECISION)
        {
            max = mid; //根值偏大，降低上边界
        }
    }
    return mid;
}

bool high_float::empty() const
{
    return front_point.empty() AND back_point.empty();
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
    if (front_point.size() == 0 AND back_point.size() == 0)
    {
        sign = true;
        return;
    }
    //如果整数部分是0
    if (front_point.size() == 0)
    {
        front_point.emplace_back(0);
    }
    else
    {
        auto iter = front_point.begin();
        //对整数部分从头遍历，直到没有0为止
        while (!front_point.empty() AND (*iter) == 0)
        {
            //删除开头的全部0
            front_point.erase(iter);
            iter = front_point.begin();
        }
        if (front_point.empty())
        {
            front_point.emplace_back(0);
        }
    }
    //如果小数部分是0
    if (back_point.size() == 0)
    {
        back_point.emplace_back(0);
    }
    else
    {
        auto it = back_point.rbegin();
        // 对小数部分
        while (!back_point.empty() AND (*it) == 0)
        {
            //小数部分只有结尾有0，如果有0直接删除即可
            back_point.pop_back();
            it = back_point.rbegin();
        }
        if (back_point.empty())
        {
            back_point.emplace_back(0);
        }
    }
}