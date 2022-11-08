#include "xml.hpp"
#include <iostream>

int main() {
    swq::xml temp_0("student");
    std::cout << temp_0.name() << std::endl;
    temp_0.name("ss");
    std::cout << temp_0.name() << std::endl;
    temp_0.text("my name is swq");
    std::cout << temp_0.text() << std::endl;
	return 0;
}