#include "json.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace swq;

json::json()
{
    m_type = json_null;
}

json::json(bool input_v)
{
    m_type = json_bool;
    m_value.m_bool = input_v;
}

json::json(int input_v)
{
    m_type = json_int;
    m_value.m_int = input_v;
}

json::json(double input_v)
{
    m_type = json_double;
    m_value.m_double = input_v;
}

json::json(const char *input_v)
{
    m_type = json_string;
    m_value.m_string = new std::string(input_v);
}

json::json(const std::string &input_v)
{
    m_type = json_string;
    m_value.m_string = new std::string(input_v);
}

json::json(const json &input_j)
{
    m_type = input_j.m_type;
    m_value = input_j.m_value;
}

json::json(type input_t)
{
    m_type = input_t;
    switch (input_t)
    {
    case json_null:
        break;
    case json_bool:
        m_value.m_bool = false;
        break;
    case json_int:
        m_value.m_int = 0;
        break;
    case json_double:
        m_value.m_double = 0.0;
        break;
    case json_string:
        m_value.m_string = new std::string();
        break;
    case json_array:
        m_value.m_array = new std::list<json>();
        break;
    case json_object:
        m_value.m_object = new std::map<std::string, json>();
        break;
    default:
        break;
    }
}

json::operator bool()
{
    switch (m_type)
    {
    case json_null:
        return false;
    case json_bool:
        return m_value.m_bool;
    case json_int:
        return bool(m_value.m_int);
    case json_double:
        return bool(m_value.m_double);
    case json_string:
    case json_array:
    case json_object:
        throw std::logic_error("type error: type is not bool");
    default:
        return false;
    }
    return false;
}

json::operator int()
{
    switch (m_type)
    {
    case json_null:
        return 0;
    case json_bool:
        return int(m_value.m_bool);
    case json_int:
        return m_value.m_int;
    case json_double:
        return int(m_value.m_double);
    case json_string:
    case json_array:
    case json_object:
        throw std::logic_error("type error: type is not int");
    default:
        return 0;
    }
    return 0;
}

json::operator double()
{
    switch (m_type)
    {
    case json_null:
        return 0.0;
    case json_bool:
        return double(m_value.m_bool);
    case json_int:
        return double(m_value.m_int);
    case json_double:
        return m_value.m_double;
    case json_string:
    case json_array:
    case json_object:
        throw std::logic_error("type error: type is not double");
    default:
        return 0.0;
    }
    return 0.0;
}

json::operator std::string()
{
    if (m_type != json_string)
    {
        throw std::logic_error("type error: type is not string");
    }
    return *(m_value.m_string);
}

bool json::Bool() const
{
    if (m_type != json_bool)
    {
        throw std::logic_error("type error: type is not bool");
    }
    return m_value.m_bool;
}

int json::Int() const
{
    if (m_type != json_int)
    {
        throw std::logic_error("type error: type is not int");
    }
    return m_value.m_int;
}

double json::Double() const
{
    if (m_type != json_double)
    {
        throw std::logic_error("type error: type is not double");
    }
    return m_value.m_double;
}

unsigned long long json::ULLInt() const
{
    switch (m_type)
    {
    case json_null:
        return 0;
    case json_bool:
        return int(m_value.m_bool);
    case json_int:
        return m_value.m_int;
    case json_double:
        return (unsigned long long int)m_value.m_double;
    case json_string:
        throw std::logic_error("type error: type is string");
    case json_array:
        throw std::logic_error("type error: type is array");
    case json_object:
        throw std::logic_error("type error: type is object");
    default:
        break;
    }
    return 0;
}

std::string json::String() const
{
    if (m_type != json_string)
    {
        throw std::logic_error("type error: type is not string");
    }
    return *(m_value.m_string);
}

bool json::isNull() const
{
    return m_type == json_null;
}

bool json::isBool() const
{
    return m_type == json_bool;
}

bool json::isInt() const
{
    return m_type == json_int;
}

bool json::isDouble() const
{
    return m_type == json_double;
}

bool json::isString() const
{
    return m_type == json_string;
}

bool json::isArray() const
{
    return m_type == json_array;
}

bool json::isObject() const
{
    return m_type == json_object;
}

bool json::has(int index)
{
    if (m_type != json_array)
    {
        return false;
    }
    int size = (m_value.m_array)->size();
    if (index < 0)
    {
        index = size + index; 
    }
    return index < size;
}

bool json::has(const char * key)
{
    std::string name(key);
    return has(name);
}

bool json::has(const std::string & key)
{
    if (m_type != json_object)
    {
        return false;
    }
    return (m_value.m_object)->find(key) != (m_value.m_object)->end();
}

bool json::empty() const
{
    switch (m_type)
    {
        case json_null:
            return true;
        case json_array:
            return (m_value.m_array)->empty();
        case json_object:
            return (m_value.m_object)->empty();
        default:
            break;
    }
    return false;
}

int json::size() const
{
    switch (m_type)
    {
        case json_array:
            return (m_value.m_array)->size();
        case json_object:
            return (m_value.m_object)->size();
        default:
            break;
    }
    throw std::logic_error("type error: type is not array or object");
}

//这里函数前一定要加&，表示返回值是栈中创建的引用，也就是直接把栈中创建的对象返回出来，而不是值的复制，
json & json::operator[](int index)
{
    if (m_type != json_array)
    {
        clear();
        m_type = json_array;
        m_value.m_array = new std::list<json>();
    }
    int size = (m_value.m_array)->size();
    if (index < 0)
    {
        index = size + index;
    }
    if (index >= size)
    {
        for (int i = size; i <= index; i++)
        {
            (m_value.m_array)->emplace_back(json());
        }
        return (m_value.m_array)->back();
    }
    else
    {
        auto it = (m_value.m_array)->begin();
        for (int i = 0; i < index; i++)
        {
            it++;
        }
        return *it;
    }
}

json & json::operator [] (const char * key)
{
    std::string name = key;  
    //这里只是对c形式的字符串进行转换，真正的逻辑实现在下方的重载上
    return (*this)[name];
}

json & json::operator [] (const std::string & key)
{
    if (m_type != json_object)
    {
        clear();
        m_type = json_object;
        m_value.m_object = new std::map<std::string, json>();
    }
    return (*(m_value.m_object))[key];
}

json & json::operator = (const json & other)
{
    clear();
    m_type = other.m_type;
    m_value = other.m_value;
    return (*this);
}

bool json::operator == (const json & other)
{
    if (m_type != other.get_type())
    {
        return false;
    }
    switch (m_type)
    {
        case json_null:
            return true;
        case json_bool:
            return (m_value.m_bool == other.m_value.m_bool);
        case json_int:
            return (m_value.m_int == other.m_value.m_int);
        case json_double:
            return (m_value.m_double == other.m_value.m_double);
        case json_string:
            return *(m_value.m_string) == *(other.m_value.m_string);
        case json_array:
            return m_value.m_array == other.m_value.m_array;
        case json_object:
            return m_value.m_object == other.m_value.m_object;
        default:
            break;
    }
    return false;
}

bool json::operator != (const json & other)
{
    return !(*this == other);
}

void json::append(const json & value)
{
    if (m_type != json_array)
    {   
        clear();
        m_type = json_array;
        m_value.m_array = new std::list<json>();
    }
    (m_value.m_array)->emplace_back(value);
}

bool json::isempty() const
{
    switch (m_type)
    {
        case json_null:
            return true;
        case json_array:
            return (m_value.m_array)->empty();
        case json_object:
            return (m_value.m_object)->empty();
        default:
            break;
    }
    return false;
}

std::string json::str() const
{
    std::stringstream ss;
    switch (m_type)
    {
        case json_null:
            ss << "null";
            break;
        case json_bool:
            if (m_value.m_bool)
            {
                ss << "true";
            }
            else
            {
                ss << "false";
            }
            break;
        case json_int:
            ss << m_value.m_int;
            break;
        case json_double:
            ss << m_value.m_double;
            break;
        case json_string:
            ss << '\"' << *(m_value.m_string) << '\"';
            break;
        case json_array:
            {
                ss << '[';
                int label = 0;
                //这里和下面对array的遍历一定要用指针或者迭代器的形式而不要用c++11的冒号表达式
                //冒号表达式相当于把array中的每一个拿出来复制了一份，重新调用了构造函数，而我们需要直接索引，读取的也是本身的值
                for (auto it = (m_value.m_array)->begin(); it != (m_value.m_array)->end(); it++)
                {
                    if (label != 0)
                    {
                        ss << ',';
                    }
                    label++;
                    ss << (*it).str();
                }
                ss << ']';
            }
            break;
        case json_object:
            {
                ss << '{';
                int label = 0;
                for (auto it = (m_value.m_object)->begin(); it != (m_value.m_object)->end(); it++)
                {
                    if (label != 0)
                    {
                        ss << ',';
                    }
                    label++;
                    ss << '\"' << it->first << '\"' << ':' << it->second.str();
                }
                ss << '}';
            }
            break;
        default:
            break;
    }
    return ss.str();
}

std::string json::get_typename() const
{
    std::stringstream ss;
    switch (m_type)
    {
    case json_null:
        ss << "null";
        break;
    case json_bool:
        ss << "bool";
        break;
    case json_int:
        ss << "int";
        break;
    case json_double:
        ss << "double";
        break;
    case json_string:
        ss << "string";
        break;
    case json_array:
        ss << "array";
        break;
    case json_object:
        ss << "object";
        break;
    default:
        break;
    }
    return ss.str();
}

json::type json::get_type() const
{
    return m_type;
}

void json::clear()
{
    switch (m_type)
    {
    case json_null:
    case json_bool:
    case json_int:
    case json_double:
        break;
    case json_string:
        {
            delete m_value.m_string;
        }
        break;
    case json_array:
        {
            //这里的空指针判断是因为copy函数只是复制了指针，有可能外部有其他json对象已经释放了对应的数组或对象，所以需要判断一下保证不保存
            if (!(m_value.m_array))
            {
                for (auto it = (m_value.m_array)->begin(); it != (m_value.m_array)->end(); it++)
                {
                    it->clear();
                }
                delete m_value.m_array;             
            }
        }
        break;
    case json_object:
        {
            if (!(m_value.m_array))
            {
                for (auto it = (m_value.m_object)->begin(); it != (m_value.m_object)->end(); it++)
                {
                    it->second.clear();
                }
                delete m_value.m_object;
            }
        }
        break;
    default:
        break;
    }
    m_type = json_null;
}

void json::copy(const json & other)
{   
    clear();
    m_type = other.m_type;
    m_value = other.m_value;
}

void json::remove(int index)
{
    if (m_type != json_array)
    {
        return;
    }
    int size = (m_value.m_array)->size();
    if (index < 0)
    {
        index = size + index;
    }
    else if (index >= size)
    {
        for (int i = size; i <= index; i++)
        {
            (m_value.m_array)->emplace_back(json());
        }
    }
    auto it = (m_value.m_array)->begin();
    for (int i = 0; i < index; i++)
    {
        it++;
    }
    it->clear();
    (m_value.m_array)->erase(it);
}

void json::remove(const char * key)
{
    std::string name = key;
    remove(name);
}

void json::remove(const std::string & key)
{
    if (m_type != json_object)
    {
        return;
    }
    auto it = (m_value.m_object)->find(key);
    if (it != (m_value.m_object)->end())
    {
        it->second.clear();
        (m_value.m_object)->erase(key);
    }
}

json::iterator json::begin()
{
    if (m_type != json_array)
    {
        throw std::logic_error("type error: type is not array");
    }
    return (m_value.m_array)->begin();
}

json::iterator json::end()
{
    if (m_type != json_array)
    {
        throw std::logic_error("type error: type is not array");
    }
    return (m_value.m_array)->end();
}

void json::parse(const std::string & str)
{
    parser_j p(str);
    *this = p.parse();
}

parser_j::parser_j()
{
    m_str = "";
    m_idx = 0;
}
parser_j::parser_j(const std::string & str)
{
    m_str = str;
    m_idx = 0;
}

void parser_j::load(const std::string & str)
{
    m_str = str;
    m_idx = 0;
}

char parser_j::get_next_token()
{
    if(!end_label)
    {
        while (m_str[m_idx] == ' ' || m_str[m_idx] == '\r' || m_str[m_idx] == '\n' || m_str[m_idx] == '\t')
        {
            m_idx++;
        }
        if (m_idx == m_str.size())
        {
            end_label = true;
            return m_str[-1];
        }
        else
        {
            return m_str[m_idx++]; 
        }
    }
    else
    {
        return m_str[-1];
    }

}

json parser_j::parse()
{
    char ch = get_next_token();
    if (end_label)
    {
        return json();
    }
    else
    {
        switch (ch)
        {
            case 'n':
                m_idx--;
                return parse_null();
            case 't':
            case 'f':
                m_idx--;
                return parse_bool();
            case '-':
            case '+':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                m_idx--;
                return parse_number();
            case '"':
                return parse_string();
            case '[':
                return parse_array();
            case '{':
                return parse_object();
            default:
                break;
        }
        throw std::logic_error("unexpected character in parse json");
    }
}

bool parser_j::in_range(int x, int lower, int upper)
{
    return (x >= lower && x <= upper);
}

json parser_j::parse_null()
{
    if (m_str.compare(m_idx, 4, "null") == 0)
    {
        m_idx += 4;
        return json();
    }
    throw std::logic_error("parse null error");
}

json parser_j::parse_bool()
{
    if (m_str.compare(m_idx, 4, "true") == 0)
    {
        m_idx += 4;
        return json(true);
    }
    if (m_str.compare(m_idx, 5, "false") == 0)
    {
        m_idx += 5;
        return json(false);
    }
    throw std::logic_error("parse bool error");
}

json parser_j::parse_number()
{
    size_t pos = m_idx;
    //校验有没有负号
    if (m_str[m_idx] == '-')
        m_idx++;
    // 校验是否是数字
    if (m_str[m_idx] == '0')
    {
        m_idx++;
    }
    else if (in_range(m_str[m_idx], '1', '9'))
    {
        m_idx++;
        while (in_range(m_str[m_idx], '0', '9'))
        {
            m_idx++;
        }
    }
    else
    {
        throw std::logic_error("invalid character in number");
    }
    //校验是不是整数
    if (m_str[m_idx] != '.')
    {
        //atoi在遇到非数字字符时自动停止转换
        return json(std::atoi((m_str.c_str() + pos)));
    }
    else
    {
        //如果是浮点数
        m_idx++;
        if (!in_range(m_str[m_idx], '0', '9'))
        {
            throw std::logic_error("at least one digit required in fractional part");
        }
        while (in_range(m_str[m_idx], '0', '9'))
        {
            m_idx++;
        }
        return json(std::atof(m_str.c_str() + pos));
    }
}

json parser_j::parse_string()
{
    size_t pos = m_idx;
    while (true)
    {
        if (m_idx == m_str.size())
        {
            throw std::logic_error("unexpected end of input in string");
        }

        char ch = m_str[m_idx++];
        if (ch == '"')
        {
            break;
        }

        // The usual case: non-escaped characters
        if (ch == '\\')
        {
            ch = m_str[m_idx++];
            switch (ch)
            {
                case 'b':
                case 't':
                case 'n':
                case 'f':
                case 'r':
                case '"':
                case '\\':
                    break;
                case 'u':
                    m_idx += 4;
                    break;
                default:
                    break;
            }
        }
    }
    return m_str.substr(pos, m_idx - pos - 1);
}

json parser_j::parse_array()
{
    json arr(json::json_array);
    char ch = get_next_token();
    if (ch == ']')
    {
        return arr;
    }
    m_idx--;
    while (true)
    {
        arr.append(parse());
        ch = get_next_token();
        if (ch == ']')
        {
            break;
        }
        if (ch != ',')
        {
            throw std::logic_error("expected ',' in array");
        }
    }
    return arr;
}

json parser_j::parse_object()
{
    json obj(json::json_object);
    char ch = get_next_token();
    if (ch == '}')
    {
        return obj;
    }
    m_idx--;
    while (true)
    {
        ch = get_next_token();
        if (ch != '"')
        {
            throw std::logic_error("expected '\"' in object");
        }
        std::string key = parse_string();
        ch = get_next_token();
        if (ch != ':')
        {
            throw std::logic_error("expected ':' in object");
        }
        obj[key] = parse();
        ch = get_next_token();
        if (ch == '}')
        {
            break;
        }
        if (ch != ',')
        {
            throw std::logic_error("expected ',' in object");
        }
    }
    return obj;
}