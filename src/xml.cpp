#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>

#include "xml.hpp"

using namespace swq;

xml::xml()
{
    m_status = nullptr;
    m_child = nullptr;
}

xml::xml(const std::string & name)
{
    m_status = nullptr;
    m_child = nullptr; 
    m_name = name;
}

xml::xml(const char * name)
{
    m_status = nullptr;
    m_child = nullptr;
    m_name = name;
}

xml::xml(const xml & other)
{
    m_name = other.m_name;
    m_text = other.m_text;
    m_status = other.m_status;
    m_child = other.m_child; 
}

std::string xml::name() const
{
    return m_name;
}

void xml::name(std::string name)
{
    m_name = name;
}

std::string xml::text() const
{
    return m_text;
}
void xml::text(std::string text)
{
    m_text = text;
}

std::string xml::status(const std::string & key) const
{
    if (m_status == nullptr)
    {
        return "";
    }
    else
    {
        return (*m_status)[key]; 
    }
}

void xml::status(const std::string & key, const std::string &val)
{
    if (m_status == nullptr)
    {
        m_status = new std::map<std::string, std::string>();
    }
    (*m_status)[key] = val;
}

std::string xml::str()
{
    std::stringstream ss;
    ss << "<" << m_name;
    if (m_status != nullptr)
    {
        for (auto it = m_status->begin(); it != m_status->end(); it++)
        {
            ss << " " << it->first << "\"" << (std::string)it->second << "\"";
        }
    }
    ss << ">";
    if (m_child != nullptr)
    {
        for (auto it = m_child->begin(); it != m_child->end(); it++)
        {
            ss << it->str();
        }
    }
    ss << m_text;
    ss << "</" << m_name << ">";
    return ss.str();
}

void xml::append(const xml & other)
{
    if (m_child == nullptr)
    {
        m_child = new std::list<xml>();
    }
    m_child->push_back(other);
}

xml & xml::operator [] (int index)
{
    if (m_child == nullptr)
    {
        m_child = new std::list<xml>();
    }
    auto size = m_child->size();
    if (index < 0)
    {
        index = size - index;
    }
    if (index >= size)
    {
        for (int i = size; i < index; i++)
        {
            m_child->push_back(xml());
        }
        return m_child->back();
    }
    else
    {
        auto it = m_child->begin();
        for (int i = 0; i < index; i++)
        {
            it++;
        }
        return *it;
    }
}

xml & xml::operator [] (const char * name)
{
    return (*this)[std::string(name)];
}

xml & xml::operator [] (const std::string & name)
{
    if (m_child == nullptr)
    {
        m_child = new std::list<xml>();
    }
    for (auto it = m_child->begin(); it != m_child->end(); it++)
    {
        if (it->name() == name)
        {
            return *it;
        }
    }
    m_child->push_back(xml(name));
    return m_child->back();
}

void xml::remove(int index)
{
    if (m_child == nullptr)
    {
        m_child = new std::list<xml>();
    }
    auto size = m_child->size();
    if (index < 0)
    {
        index = size - index;
    }
    if (index < size)
    {
        auto it = m_child->begin();
        for (int i = 0; i < index; i++)
        {
            it++;
        }
        it->clear();
        m_child->erase(it);
    }
}

void xml::remove(const char * name)
{
    return remove(std::string(name));
}

void xml::remove(const std::string & name)
{
    for (auto it = m_child->begin(); it != m_child->end();)
    {
        if (it->name() == name)
        {
            it->clear();
            it = m_child->erase(it);
        }
        else
        {
            it++;
        }
    }
}

void xml::clear()
{
    if (m_name != "")
    {
        m_name = "";
    }
    if (m_text != "")
    {
        m_text = "";
    }
    if (m_status != nullptr)
    {
        delete m_status;
        m_status = nullptr;
    }
    if (m_child != nullptr)
    {
        for (auto it = m_child->begin(); it != m_child->end(); it++)
        {
            it->clear();
        }
        delete m_child;
        m_child = nullptr;
    }
}

void xml::parse(const std::string & str)
{
    parser_x p(str);
    *this = p.parse();
}

int xml::size() const
{
    if (m_child == nullptr)
    {
        return 0;
    }
    return m_child->size();
}

xml::iterator xml::begin()
{
    return m_child->begin();
}

xml::iterator xml::end()
{
    return m_child->end();
}

xml::iterator xml::erase(xml::iterator it)
{
    it->clear();
    return m_child->erase(it);
}

parser_x::parser_x()
{
    m_str = "";
    m_idx = 0;
}

parser_x::parser_x(const std::string & str)
{
    m_str = str;
    m_idx = 0;
}

void parser_x::skip_white_space()
{
    while (m_str[m_idx] == ' ' OR m_str[m_idx] == '\r' OR m_str[m_idx] == '\n' OR m_str[m_idx] == '\t')
    {
        m_idx++;
    }
}

xml parser_x::parse()
{
    skip_white_space();
    if (m_str.compare(m_idx, 5, "<?xml") == 0)
    {
        if (!parse_declaration())
        {
            throw std::logic_error("parse declaration error");
        }
    }
    skip_white_space();
    while (m_str.compare(m_idx, 4, "<!--") == 0)
    {
        if (!parse_comment())
        {
            throw std::logic_error("parse comment error");
        }
        skip_white_space();
    }
    if (m_str[m_idx] == '<' AND (isalpha(m_str[m_idx+1]) OR m_str[m_idx+1] == '_'))
    {
        return parse_element();
    }
    throw std::logic_error("parse element error");
}

bool parser_x::parse_declaration()
{
    if (m_str.compare(m_idx, 5, "<?xml") != 0)
    {
        return false;
    }
    m_idx += 5;
    size_t pos = m_str.find("?>", m_idx);
    if (pos == std::string::npos)
    {
        return false;
    }
    m_idx = pos + 2;
    return true;
}

bool parser_x::parse_comment()
{
    if (m_str.compare(m_idx, 4, "<!--") != 0)
    {
        return false;
    }
    m_idx += 4;
    size_t pos = m_str.find("-->", m_idx);
    if (pos == std::string::npos)
    {
        return false;
    }
    m_idx = pos + 3;
    return true;
}

xml parser_x::parse_element()
{
    xml elem;
    m_idx++;
    skip_white_space();
    const std::string & name = parse_element_name();
    elem.name(name);
    while (m_str[m_idx] != '\0')
    {
        skip_white_space();
        if (m_str[m_idx] == '/')
        {
            if (m_str[m_idx+1] == '>')
            {
                m_idx += 2;
                break;
            }
            else
            {
                throw std::logic_error("xml empty element is error");
            }
        }
        else if (m_str[m_idx] == '>')
        {
            m_idx++;
            std::string text = parse_element_text();
            if (text != "")
            {
                elem.text(text);
            }
        }
        else if (m_str[m_idx] == '<')
        {
            if (m_str[m_idx+1] == '/')
            {
                // find the end tag
                std::string end_tag = "</" + name + ">";
                size_t pos = m_str.find(end_tag, m_idx);
                if (pos == std::string::npos)
                {
                    throw std::logic_error("xml element " + name + " end tag not found");
                }
                m_idx = (pos + end_tag.size());
                break;
            }
            else if (m_str.compare(m_idx, 4, "<!--") == 0)
            {
                // parse xml comment
                if (!parse_comment())
                {
                    throw std::logic_error("xml comment is error");
                }
            }
            else
            {
                // parse child element
                elem.append(parse_element());
            }
        }
        else
        {
            // parse elem's attr
            std::string key = parse_element_attr_key();
            skip_white_space();
            if (m_str[m_idx] != '=')
            {
                throw std::logic_error("xml element attr is error" + key);
            }
            m_idx++;
            skip_white_space();
            std::string val = parse_element_attr_val();
            elem.status(key, val);
        }
    }
    return elem;
}

std::string parser_x::parse_element_name()
{
    int pos = m_idx;
    if (isalpha(m_str[m_idx]) OR (m_str[m_idx] == '_'))
    {
        m_idx++;
        while (isalnum(m_str[m_idx]) OR (m_str[m_idx] == '_') OR (m_str[m_idx] == '-') OR (m_str[m_idx] == ':') OR (m_str[m_idx] == '.'))
        {
            m_idx++;
        }
    }
    return m_str.substr(pos, m_idx - pos);
}

std::string parser_x::parse_element_text()
{
    int pos = m_idx;
    while (m_str[m_idx] != '<')
    {
        m_idx++;
    }
    return m_str.substr(pos, m_idx - pos);
}

std::string parser_x::parse_element_attr_key()
{
    int pos = m_idx;
    if (isalpha(m_str[m_idx]) OR (m_str[m_idx] == '_'))
    {
        m_idx++;
        while (isalnum(m_str[m_idx]) OR (m_str[m_idx] == '_') OR (m_str[m_idx] == '-') OR (m_str[m_idx] == ':'))
        {
            m_idx++;
        }
    }
    return m_str.substr(pos, m_idx - pos);
}

std::string parser_x::parse_element_attr_val()
{
    if (m_str[m_idx] != '"')
    {
        throw std::logic_error("xml element attr value should be in double quotes");
    }
    m_idx++;
    int pos = m_idx;
    while (m_str[m_idx] != '"')
    {
        m_idx++;
    }
    m_idx++;
    return m_str.substr(pos, m_idx - pos - 1);
}
