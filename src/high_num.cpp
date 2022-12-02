#include "high_num.hpp"

using namespace swq;

high_float::high_float()
{
}

high_float::high_float(double input_num)
{
    if (input_num < 0)
    {
        sign = false;
    }
    auto temp_num = std::abs(input_num);
}

void high_float::trim()
{
    
}