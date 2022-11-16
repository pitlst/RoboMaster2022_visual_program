#pragma once
#include <string>
#include <vector>
#include <deque>

#include "debug.hpp"

namespace swq
{
    class hyperint
    {
    public:
        // 构造函数

        hyperint();
        hyperint(bool input);
        hyperint(short int input);
        hyperint(unsigned short int input);
        hyperint(int input);
        hyperint(unsigned int input);
        hyperint(long int input);
        hyperint(unsigned long int input);
        hyperint(long long int input);
        hyperint(unsigned long long int input);
        hyperint(const std::string & input);
        hyperint(const char * input);
        hyperint(const int * input, int size);
        hyperint(const hyperint & other);

        // 重载运算符

        hyperint & operator=(const hyperint & input);
        hyperint & operator+(const hyperint & input);
        hyperint & operator-(const hyperint & input);
        hyperint & operator*(const hyperint & input);
        hyperint & operator/(const hyperint & input);
	    bool operator >(const hyperint & input);
        bool operator>=(const hyperint &input);
	    bool operator <(const hyperint & input);
        bool operator<=(const hyperint &input);
        bool operator==(const hyperint &input);

        // 常用方法
        //释放空间，由于数据结构管理的是指针，所以数据不能跟着类的生命周期释放，计划之后使用智能指针管理
        void clear();         
        //浅拷贝，只复制了指针                  
        void copy(const hyperint & input);    
        //深拷贝，包括对堆空间的复制 
        void deepcopy(const hyperint & input);  

        // 重载类型转换函数

        operator bool();
        operator int();
        operator double();
        operator float();
        operator std::string();

        // 判断所存变量范围

        bool is_out_sint();
        bool is_out_usint();
        bool is_out_int();
        bool is_out_uint();
        bool is_out_lint();
        bool is_out_ulint();
        bool is_out_llint();
        bool is_out_ullint();

        //类型转换函数

        std::string str();
        std::string to_str();
        short int to_sint();
        unsigned short int to_usint();
        int to_int();
        unsigned int to_uint();
        long int to_lint();
        unsigned long int to_ulint();
        long long int to_llint();
        unsigned long long int to_ullint();

    private:
        //设置正负号标志位，默认true
        void set_sign();
        //设置正负号标志位
        void set_sign(bool input);
        //设置正负号标志位，根据字符串首字符
        void set_sign(const std::string & input);
        //检查字符串是不是数字
        void check_str(const std::string & input_str) const;
        //移除字符串中多余的，补位的0
        void remove_excess_zero(std::string & input_str);
        //移除数组中多余的，补位的0
        void remove_excess_zero(std::vector<int> & input_int);
        //移除类中双端队列中多余的，补位的0
        void remove_excess_zero();
        //十进制转二进制
        void vector_to_deque(std::vector<int> & input_int);
        //十进制除以2，保存在原数组中
        void vector_div_2(std::vector<int> & input_int);
        //检查数组是否为0
        bool check_vector(std::vector<int> & input_int);
        //二进制转十进制
        std::vector<int> deque_to_vector();
        
        bool sign;
        std::deque<bool> *m_value;
    };
}