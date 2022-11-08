#include "utils.hpp"

int main()
{
    std::string temp = swq::get_file_str("../asset/test.json");
    std::cout << temp << std::endl;
    return 0;
}