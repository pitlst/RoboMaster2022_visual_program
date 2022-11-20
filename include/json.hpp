#pragma once
#include <list>
#include <string>
#include <map>

#include "debug.hpp"

namespace swq
{
    class json
    {
    public:
        //json节点拥有的值的类型
        enum type
        {
            json_null,
            json_bool,
            json_object,
            json_array,
            json_string,
            json_int,
            json_double
        };

        //构造函数

        json();
        json(bool input_v);
        json(int input_v);
        json(double input_v);
        json(const char * input_v);
        json(const std::string &input_v);
        json(type input_t);
        json(const json & input_j);

        //类型转换重载

        operator bool();
        operator int();
        operator double();
        operator std::string();

        //运算符重载

        json & operator [] (int index);
        json & operator [] (const char * key);
        json & operator [] (const std::string & key);
        json & operator = (const json & other);
        bool operator == (const json & other);
        bool operator != (const json & other);

        //判断函数

        bool isempty() const;
        bool isNull() const;
        bool isBool() const;
        bool isInt() const;
        bool isDouble() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;
        bool has(int index);
        bool has(const char * key);
        bool has(const std::string & key);
        bool empty() const;
        int size() const;

        //强制类型转换

        bool Bool() const;
        int Int() const;
        double Double() const;
        unsigned long long ULLInt() const;
        std::string String() const;

        //常用方法实现

        void append(const json & other);
        void clear();
        void copy(const json & other);
        void remove(int index);
        void remove(const char * key);
        void remove(const std::string & key);
        std::string str() const;
        std::string get_typename() const;
        type get_type() const;

        //解析函数

        void parse(const std::string & str);

        //定义迭代器

        typedef std::list<json>::iterator iterator;
        iterator begin();
        iterator end();

    private:
        //一个json可能所拥有的值
        union value
        {
            bool m_bool;
            int m_int;
            double m_double;
            std::string *m_string;
            std::list<json> *m_array;
            std::map<std::string, json> *m_object;
        };

        type m_type;
        value m_value;
    };

    class parser_j
    {
    public:
        parser_j();
        parser_j(const std::string & str);

        void load(const std::string & str);
        json parse();

    private:
        char get_next_token();
        bool in_range(int x, int lower, int upper);

        json parse_null();
        json parse_bool();
        json parse_number();
        json parse_string();
        json parse_array();
        json parse_object();

        std::string m_str;
        size_t m_idx;
        bool end_label = false;
    };

}
