#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include "utils.hpp"
#include "logger.hpp"
#include "debug.hpp"

namespace swq
{
    // 定义除零错误
    class DividedByZeroException : std::exception
    {
    public:
        char const *what() const noexcept override
        {
            return "Divided By Zero Exception!";
        }
    };

    class high_float
    {
    public:
        //构造函数
        high_float();
        high_float(int input_num);
        high_float(double input_num);
        high_float(const std::string &input_num);
        high_float(const char *input_num);
        high_float(const high_float &input_num);
        ~high_float() = default;

        //类型转换重载
        //浮点转换
        operator double();
        //字符串转换
        operator std::string();

        //运算符重载

        //基本运算符重载
        friend high_float operator+(const high_float &, const high_float &);                                    //加法重载
        friend high_float operator-(const high_float &, const high_float &);                                    //减法重载
        friend high_float operator*(const high_float &, const high_float &);                                    //乘法重载
        friend high_float operator/(const high_float &, const high_float &) throw(swq::DividedByZeroException); //除法重载
        friend high_float operator-(const high_float &);                                                        //负号重载

        //比较重载
        friend bool operator==(const high_float &, const high_float &); //等于重载
        friend bool operator!=(const high_float &, const high_float &); //不等于重载
        friend bool operator<(const high_float &, const high_float &);  //小于重载
        friend bool operator<=(const high_float &, const high_float &); //小于等于重载
        friend bool operator>(const high_float &, const high_float &);  //大于重载
        friend bool operator>=(const high_float &, const high_float &); //大于等于重载

        //扩展运算符重载
        friend high_float operator+=(high_float &, const high_float &); //加等重载
        friend high_float operator-=(high_float &, const high_float &); //减等重载
        friend high_float operator*=(high_float &, const high_float &); //乘等重载
        friend high_float operator/=(high_float &, const high_float &); //除等重载

        //输入输出重载
        friend std::ostream &operator<<(std::ostream &, const high_float &); //输出重载
        friend std::istream &operator>>(std::istream &, high_float &);       //输入重载

        //禁止构造一些其它常用的运算符重载
        high_float &operator[](const high_float &) = delete;

        //常用函数
        //相加
        high_float &add(const high_float &other);
        //相乘
        high_float &multiply(const high_float &other);
        //幂运算
        high_float &pow(const high_float &other);
        //检测是否有数
        bool empty() const;
        //返回数的迭代精度
        int max_size() const;
        //转换成字符串
        std::string str() const;
        //获取正负
        bool get_sign() const;
        //清空保存的数
        void clear();
        //取绝对值
        high_float abs() const;

        static const high_float &ZERO()
        {
            static high_float zero{0};
            return zero;
        };

        static const high_float &ONE()
        {
            static high_float one{1};
            return one;
        };

        static const high_float &TEN()
        {
            static high_float ten{10};
            return ten;
        };

        #define WFLOAT_ZERO WFloat::ZERO()
        #define WFLOAT_ONE WFloat::ONE()
        #define WFLOAT_TEN WFloat::TEN()

    private:
        //将多余的零删去
        void trim();
        //用于判断正负的标志位,true为非负,false为负
        bool sign = true;
        //小数点前的每一位
        std::shared_ptr<std::vector<char>> front_point;
        //小数点后的每一位
        std::shared_ptr<std::vector<char>> back_point;
    };

    class high_int
    {
    public:
        //构造函数
        high_int();
        high_int(int input_num);
        high_int(const std::string &input_num);
        high_int(const char *input_num);
        high_int(const high_int &input_num);
        ~high_int() = default;

        //类型转换重载
        //浮点转换
        operator double();
        //字符串转换
        operator std::string();

        //运算符重载

        high_int &operator=(const high_int &other);
        high_int &operator+(high_int &other);
        high_int &operator-(high_int &other);
        high_int &operator*(high_int &other);
        high_int &operator/(high_int &other);
        bool operator==(const high_int &other);
        bool operator!=(const high_int &other);

        //禁止构造一些其它常用的运算符重载
        high_int &operator[](const high_int &) = delete;

        //常用函数
        //相加
        high_int &add(const high_int &other);
        //相乘
        high_int &multiply(const high_int &other);
        //幂运算
        high_int &pow(const high_int &other);
        //检测是否有数
        bool empty() const;
        //返回数的迭代精度
        int max_size() const;
        //转换成字符串
        std::string str() const;
        //获取正负
        bool get_sign() const;
        //清空保存的数
        void clear();

    private:
        //将多余的零删去
        void trim();
        //用于判断正负的标志位,true为非负,false为负
        bool sign = true;
        //存储数字的每一位
        std::shared_ptr<std::vector<unsigned short int>> number;
    };
}