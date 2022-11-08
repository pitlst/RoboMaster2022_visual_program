#include "hint.hpp"
#include "logger.hpp"

#include <iostream>
#include <sstream>
#include <bitset>

using namespace swq;

hyperint::hyperint()
{
    set_sign();
    m_value = new std::deque<bool>();
}

hyperint::hyperint(bool input)
{
    set_sign();
    m_value = new std::deque<bool>();
    m_value->emplace_front(input);
}

hyperint::hyperint(short int input)
{
    set_sign(bool(input));
    m_value = new std::deque<bool>();
    std::bitset<4> temp((unsigned short int)(input));
    for (size_t i = 0; i < temp.size(); i++)
    {
        m_value->emplace_front(temp[i]);
    }
    remove_excess_zero();
}

hyperint::hyperint(unsigned short int input)
{
    set_sign(bool(input));
    m_value = new std::deque<bool>();
    std::bitset<4> temp(input);
    for (size_t i = 0; i < temp.size(); i++)
    {
        m_value->emplace_front(temp[i]);
    }
    remove_excess_zero();
}

hyperint::hyperint(int input)
{
    set_sign(bool(input));
    m_value = new std::deque<bool>();
    std::bitset<8> temp((unsigned int)(input));
    for (size_t i = 0; i < temp.size(); i++)
    {
        m_value->emplace_front(temp[i]);
    }
    remove_excess_zero();
}

hyperint::hyperint(unsigned int input)
{
    set_sign(bool(input));
    m_value = new std::deque<bool>();
    std::bitset<8> temp(input);
    for (size_t i = 0; i < temp.size(); i++)
    {
        m_value->emplace_front(temp[i]);
    }
    remove_excess_zero();
}

hyperint::hyperint(long int input)
{
    set_sign(bool(input));
    m_value = new std::deque<bool>();
    std::bitset<16> temp((unsigned long int)(input));
    for (size_t i = 0; i < temp.size(); i++)
    {
        m_value->emplace_front(temp[i]);
    }
    remove_excess_zero();
}

hyperint::hyperint(unsigned long int input)
{
    set_sign(bool(input));
    m_value = new std::deque<bool>();
    std::bitset<16> temp(input);
    for (size_t i = 0; i < temp.size(); i++)
    {
        m_value->emplace_front(temp[i]);
    }
    remove_excess_zero();
}

hyperint::hyperint(long long int input)
{
    set_sign(bool(input));
    m_value = new std::deque<bool>();
    std::bitset<16> temp((unsigned long long int)(input));
    for (size_t i = 0; i < temp.size(); i++)
    {
        m_value->emplace_front(temp[i]);
    }
    remove_excess_zero();
}

hyperint::hyperint(unsigned long long int input)
{
    set_sign(bool(input));
    m_value = new std::deque<bool>();
    std::bitset<16> temp(input);
    for (size_t i = 0; i < temp.size(); i++)
    {
        m_value->emplace_front(temp[i]);
    }
    remove_excess_zero();
}

hyperint::hyperint(const std::string &input)
{
    std::string temp_str(input);
    set_sign(temp_str);
    m_value = new std::deque<bool>();
    check_str(temp_str);
    remove_excess_zero(temp_str);
    std::vector<int> temp;
    for (auto ch : temp_str)
    {
        temp.emplace_back(ch - '0');
    }
    vector_to_deque(temp);
}

hyperint::hyperint(const char *input)
{
    std::string temp_str(input);
    set_sign(temp_str);
    m_value = new std::deque<bool>();
    check_str(temp_str);
    remove_excess_zero(temp_str);
    std::vector<int> temp;
    for (auto ch : temp_str)
    {
        temp.emplace_back(ch - '0');
    }
    vector_to_deque(temp);
}

hyperint::hyperint(const int *input, int size)
{
    set_sign();
    m_value = new std::deque<bool>();
    std::vector<int> temp;
    for (size_t i = 0; i < size; i++)
    {
        if (*input >= 0)
        {
            temp.emplace_back(*input);
            input++;
        }
        else
        {
            throw std::logic_error("type error: unknown number");
        }
    }
    remove_excess_zero(temp);
    vector_to_deque(temp);
    remove_excess_zero();
}

hyperint::hyperint(const hyperint &other)
{
    sign = other.sign;
    m_value = other.m_value;
}

void hyperint::clear()
{
    set_sign();
    delete m_value;
}

void hyperint::copy(const hyperint &input)
{
    clear();
    sign = input.sign;
    m_value = input.m_value;
}

void hyperint::deepcopy(const hyperint &input)
{
    clear();
    sign = input.sign;
    m_value = new std::deque<bool>();
    for (auto ch : (*(input.m_value)))
    {
        m_value->emplace_back(ch);
    }
}

hyperint::operator bool()
{
    return sign;
}

// hyperint::operator int()
// {
//     return to_int();
// }

// hyperint::operator double()
// {
//     return double(to_int());
// }

hyperint::operator std::string()
{
    return str();
}

bool hyperint::is_out_sint()
{
    remove_excess_zero();
    if(m_value->size() >= 16)
    {
        return false;
    }
    return true;
}

bool hyperint::is_out_usint()
{
    return is_out_sint();
}

bool hyperint::is_out_int()
{
    remove_excess_zero();
    if(m_value->size() >= 32)
    {
        return false;
    }
    return true;   
}

bool hyperint::is_out_uint()
{
    return is_out_int();
}

bool hyperint::is_out_lint()
{
    remove_excess_zero();
    if(m_value->size() >= 64)
    {
        return false;
    }
    return true;   
}

bool hyperint::is_out_ulint()
{
    return is_out_lint();
}

bool hyperint::is_out_llint()
{
    remove_excess_zero();
    if(m_value->size() >= 128)
    {
        return false;
    }
    return true; 
}

bool hyperint::is_out_ullint()
{
    return is_out_llint();
}


hyperint &hyperint::operator=(const hyperint &input)
{
    clear();
    sign = input.sign;
    m_value = input.m_value;
    return (*this);
}

hyperint &hyperint::operator+(const hyperint &input)
{
    if (m_value->size() == 0)
    {
        copy(input);
        return (*this);
    }
    else if (input.m_value->size() == 0)
    {
        return (*this);
    }
    hyperint temp_finally;
    auto carry_label = false;
    auto it_self = m_value->cbegin();
    auto it_input = input.m_value->cbegin();
    if (sign == input.sign)
    {
        //同符号视为加法
        while (true)
        {
            if (it_self != m_value->cend() && it_input != input.m_value->cend())
            {
                int temp_step = *it_self + *it_input + carry_label;
                switch (temp_step)
                {
                case 0:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(false);
                    break;
                case 1:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(true);
                    break;
                case 2:
                    carry_label = true;
                    temp_finally.m_value->emplace_back(false);
                    break;
                case 3:
                    carry_label = true;
                    temp_finally.m_value->emplace_back(true);
                    break;
                default:
                    break;
                }
                it_input++;
                it_self++;
            }
            else if (it_self == m_value->cend() && it_input != input.m_value->cend())
            {
                int temp_step = *it_input + carry_label;
                switch (temp_step)
                {
                case 0:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(false);
                    break;
                case 1:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(true);
                    break;
                case 2:
                    carry_label = true;
                    temp_finally.m_value->emplace_back(false);
                    break;
                default:
                    break;
                }
                it_input++;
            }
            else if (it_self != m_value->cend() && it_input == input.m_value->cend())
            {
                int temp_step = *it_self + carry_label;
                switch (temp_step)
                {
                case 0:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(false);
                    break;
                case 1:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(true);
                    break;
                case 2:
                    carry_label = true;
                    temp_finally.m_value->emplace_back(false);
                    break;
                default:
                    break;
                }
                it_self++;
            }
            else
            {
                int temp_step = carry_label;
                switch (temp_step)
                {
                case 0:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(false);
                    break;
                case 1:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(true);
                    break;
                default:
                    break;
                }
                break;
            }
        }
    }
    else
    {
        //先判断符号
        int label = 0;
        if(m_value->size() > input.m_value->size())
        {
            temp_finally.sign = sign;
        }
        else if(m_value->size() < input.m_value->size())
        {
            temp_finally.sign = input.sign;
            label = 1;
        }
        else
        {
            auto it_temp_self = m_value->cbegin();
            auto it_temp_input = input.m_value->cbegin();
            while (true)
            {
                if(*it_temp_input > *it_temp_self)
                {
                    temp_finally.sign = input.sign;
                    label = 1;
                    break;
                }
                else if(*it_temp_input < *it_temp_self)
                {
                    temp_finally.sign = sign;
                    break;  
                }
            } 
        }
        //再进行计算
        while (true)
        {
            if(it_self != m_value->cend() && it_input != input.m_value->cend())
            {
                int temp_step = 0;
                if(label)
                {
                    temp_step = *it_input - *it_self - carry_label;
                }
                else
                {
                    temp_step = *it_self - *it_input  - carry_label;
                }
                switch (temp_step)
                {
                case 1:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(true);
                    break;
                case 0:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(false);
                    break;
                case -1:
                    carry_label = true;
                    temp_finally.m_value->emplace_back(true);
                    break;
                case -2:
                    carry_label = true;
                    temp_finally.m_value->emplace_back(false);
                    break;
                default:
                    break;
                }
                it_input++;
                it_self++;
            }
            else if((it_self == m_value->cend() && it_input != input.m_value->cend()))
            {
                int temp_step = *it_input - carry_label;
                switch (temp_step)
                {
                case 1:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(true);
                    break;
                case 0:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(false);
                    break;
                case -1:
                    carry_label = true;
                    temp_finally.m_value->emplace_back(true);
                    break;
                default:
                    break;
                }
                it_input++;
            }
            else if((it_self != m_value->cend() && it_input == input.m_value->cend()))
            {
                int temp_step = *it_self - carry_label;
                switch (temp_step)
                {
                case 1:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(true);
                    break;
                case 0:
                    carry_label = false;
                    temp_finally.m_value->emplace_back(false);
                    break;
                case -1:
                    carry_label = true;
                    temp_finally.m_value->emplace_back(true);
                    break;
                default:
                    break;
                }
                it_self++;
            }
            else
            {
                std::cout << "error" << std::endl;
                break;
            }
        }
        
    }
    clear();
    sign = temp_finally.sign;
    m_value = temp_finally.m_value;
    return (*this);
}

hyperint &hyperint::operator-(const hyperint &input)
{
    hyperint temp_input = input;
    temp_input.sign = !input.sign;
    operator+(temp_input);
    return (*this);
}

// hyperint &hyperint::operator*(const hyperint &input)
// {
// }

// hyperint &hyperint::operator/(const hyperint &input)
// {
// }

bool hyperint::operator>(const hyperint &input)
{
    if (sign != input.sign)
    {
        if (sign == true)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (m_value->size() > input.m_value->size())
        {
            return true;
        }
        else if (m_value->size() < input.m_value->size())
        {
            return false;
        }
        else
        {
            auto size = m_value->size();
            auto it_self = m_value->begin();
            auto it_input = input.m_value->begin();
            for (size_t i = 0; i < size; i++)
            {
                if (*it_self == *it_input)
                {
                    continue;
                }
                else if (*it_self > *it_input)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return false;
}

bool hyperint::operator<(const hyperint &input)
{
    if (sign != input.sign)
    {
        if (sign == true)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (m_value->size() > input.m_value->size())
        {
            return false;
        }
        else if (m_value->size() < input.m_value->size())
        {
            return true;
        }
        else
        {
            auto size = m_value->size();
            auto it_self = m_value->begin();
            auto it_input = input.m_value->begin();
            for (size_t i = 0; i < size; i++)
            {
                if (*it_self == *it_input)
                {
                    continue;
                }
                else if (*it_self > *it_input)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool hyperint::operator<=(const hyperint &input)
{
    return !operator>(input);
}

bool hyperint::operator>=(const hyperint &input)
{
    return !operator<(input);
}

bool hyperint::operator==(const hyperint &input)
{
    if (sign != input.sign)
    {
        return false;
    }
    else if (m_value->size() != input.m_value->size())
    {
        return false;
    }
    else
    {
        auto size = m_value->size();
        auto it_self = m_value->begin();
        auto it_input = input.m_value->begin();
        for (size_t i = 0; i < size; i++)
        {
            if (*it_self == *it_input)
            {
                continue;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

std::string hyperint::to_str()
{
    return str();
}

std::string hyperint::str()
{
    std::stringstream ss;
    for (auto it = m_value->rbegin(); it != m_value->rend(); it++)
    {
        ss << *it;
    }
    return ss.str();
}

// short int hyperint::to_sint()
// {
// }

// unsigned short int hyperint::to_usint()
// {
// }

// int hyperint::to_int()
// {
// }

// unsigned int hyperint::to_uint()
// {
// }

// long int hyperint::to_lint()
// {
// }

// unsigned long int hyperint::to_ulint()
// {
// }

// long long int hyperint::to_llint()
// {
// }

// unsigned long long int hyperint::to_ullint()
// {
// }


void hyperint::set_sign()
{
    sign = true;
}

void hyperint::set_sign(bool input)
{
    sign = input;
}

void hyperint::set_sign(const std::string &input)
{
    if (input[0] == '-')
    {
        sign = false;
    }
    else
    {
        sign = true;
    }
}

void hyperint::check_str(const std::string &input_str) const
{
    if (sign == false && input_str[0] != '-')
    {
        throw std::logic_error("type error: unknown str");
    }
    for (auto &ch : input_str)
    {
        if (ch < '0' || ch > '9')
        {
            throw std::logic_error("type error: unknown str");
        }
    }
}

void hyperint::remove_excess_zero(std::string &input_str)
{
    auto it = input_str.begin();
    if (sign == false)
    {
        it = input_str.erase(it);
    }
    while (it != input_str.end())
    {
        if (*it == '0')
        {
            it = input_str.erase(it);
        }
        else
        {
            break;
        }
    }
}

void hyperint::remove_excess_zero(std::vector<int> &input_int)
{
    auto it = input_int.begin();
    while (it != input_int.end())
    {
        if (*it == 0)
        {
            it = input_int.erase(it);
        }
        else
        {
            break;
        }
    }
}

void hyperint::remove_excess_zero()
{
    for (auto it = m_value->rbegin(); it != m_value->rend(); it++)
    {
        if(*it == false)
        {
            m_value->pop_back();
        }
        else
        {
            break;
        }
    }    
}

void hyperint::vector_to_deque(std::vector<int> &input_int)
{
    while (check_vector(input_int))
    {
        m_value->emplace_back((input_int[input_int.size()-1]%2));
        vector_div_2(input_int);
    }
}

std::vector<int> hyperint::deque_to_vector()
{
    std::vector<int> return_int;
    return return_int;
}

void hyperint::vector_div_2(std::vector<int> &input_int)
{
    std::vector<int> empty_int;
    std::vector<int> input_copy = input_int;
    auto it_input = input_copy.begin();
    int temp = 0;
    while (true)
    {
        temp = temp + *it_input;
        empty_int.emplace_back(temp / 2);
        it_input++;
        if (it_input != input_copy.end())
        {
            temp = (temp % 2) * 10;
        }
        else
        {
            break;
        }
    }
    input_int.assign(empty_int.begin(), empty_int.end());
}

bool hyperint::check_vector(std::vector<int> &input_int)
{
    for (auto ch : input_int)
    {
        if (ch != 0)
        {
            return true;
        }
    }
    return false;
}
