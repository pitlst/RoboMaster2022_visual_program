#pragma once
#include <string>
#include <memory>
#include <deque>
#include <iostream>

#include "debug.hpp"

//二分法逼近精度
#define PRECISION high_float("0.001")

namespace swq
{
    class high_float
    {
        //基本运算符重载
        friend high_float operator+(const high_float &num1, const high_float &num2); //加法重载
        friend high_float operator-(const high_float &num1, const high_float &num2); //减法重载
        friend high_float operator*(const high_float &num1, const high_float &num2); //乘法重载
        friend high_float operator/(const high_float &num1, const high_float &num2); //除法重载
        friend high_float operator%(const high_float &num1, int num2_int);           //取模重载
        friend high_float operator-(const high_float &num);                          //负号重载

        //比较重载
        friend bool operator==(const high_float &num1, const high_float &num2); //等于重载
        friend bool operator!=(const high_float &num1, const high_float &num2); //不等于重载
        friend bool operator<(const high_float &num1, const high_float &num2);  //小于重载
        friend bool operator<=(const high_float &num1, const high_float &num2); //小于等于重载
        friend bool operator>(const high_float &num1, const high_float &num2);  //大于重载
        friend bool operator>=(const high_float &num1, const high_float &num2); //大于等于重载

        //扩展运算符重载
        friend high_float operator+=(high_float &num1, const high_float &num2); //加等重载
        friend high_float operator-=(high_float &num1, const high_float &num2); //减等重载
        friend high_float operator*=(high_float &num1, const high_float &num2); //乘等重载
        friend high_float operator/=(high_float &num1, const high_float &num2); //除等重载

        //输入输出重载
        friend std::ostream &operator<<(std::ostream &out, const high_float &num); //输出重载
        friend std::istream &operator>>(std::istream &in, high_float &num);        //输入重载

        friend high_float min(const high_float &num1, const high_float &num2); //获取小值
        friend high_float max(const high_float &num1, const high_float &num2); //获取大值

    public:
        //构造函数
        high_float();
        high_float(int input_num);                   //用一个整数构造
        high_float(double input_num);                //用一个浮点数构造
        high_float(const std::string &input_num);    //用一个字符串构造
        high_float(const char *input_num);           //用一个c格式的字符串构造
        high_float(const high_float &input_num);     //用一个高精度数构造
        high_float(high_float &&input_num) noexcept; //移动构造
        ~high_float() = default;

        //类型转换重载
        //浮点转换
        operator double();
        //字符串转换
        operator std::string();

        //运算符重载

        high_float operator=(const high_float &input_num);     //赋值函数
        high_float operator=(high_float &&input_num) noexcept; //移动赋值

        //常用函数
        //检测是否有数
        bool empty() const;
        //转换成字符串
        std::string str() const;
        //清空保存的数
        void clear();
        //取绝对值
        high_float abs() const;
        //开二次根号 二分法逼近 计算速度过慢 不建议使用
        high_float square_root() const;

        static const high_float &ZERO()
        {
            static high_float zero(0);
            return zero;
        };

        static const high_float &ONE()
        {
            static high_float one(1);
            return one;
        };

        static const high_float &TWO()
        {
            static high_float two(2);
            return two;
        };

        static const high_float &TEN()
        {
            static high_float ten(10);
            return ten;
        };

    private:
        //将多余的零删去
        void trim();
        //用于判断正负的标志位,true为非负,false为负
        bool sign = true;
        //小数点前的每一位
        std::deque<char> front_point = {0};
        //小数点后的每一位
        std::deque<char> back_point = {0};
    };

#define HFLOAT_ZERO high_float::ZERO()
#define HFLOAT_ONE high_float::ONE()
#define HFLOAT_TWO high_float::TWO()
#define HFLOAT_TEN high_float::TEN()

}