#pragma once
#include <string>
#include <list>
#include <map>

namespace swq
{
    class xml
    {
    public:

        //构建函数

        xml();
        xml(const std::string & name);
        xml(const char * name);
        xml(const xml & other);

        //常用方法实现

        std::string name() const;
        void name(std::string name);
        std::string text() const;
        void text(std::string text);
        std::string status(const std::string & key) const;
        void status(const std::string & key, const std::string &val);
        std::string str();
        int size() const;
        void append(const xml & other);
        void remove(int index);
        void remove(const char * name);
        void remove(const std::string & name);
        void clear();

        //运算符重载

        xml & operator [] (int index);
        xml & operator [] (const char * name);
        xml & operator [] (const std::string & name);

        //解析函数
        
        void parse(const std::string & str);

        //定义迭代器

        typedef std::list<xml>::iterator iterator;
        iterator begin();
        iterator end();
        iterator erase(iterator it);

    private:
        std::string m_name;
        std::string m_text;
        std::map<std::string, std::string> * m_status;
        std::list<xml> * m_child;
    };

    class parser_x
    {
    public:
        parser_x();
        parser_x(const std::string & str);

        void load(const std::string & str);
        xml parse();

    private:
        void skip_white_space();
        bool parse_declaration();
        bool parse_comment();
        xml parse_element();
        std::string parse_element_name();
        std::string parse_element_text();
        std::string parse_element_attr_key();
        std::string parse_element_attr_val();

        std::string m_str;
        int m_idx;
    };

}