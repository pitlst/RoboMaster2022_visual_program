#include "get_frame.hpp"
#include "logger.hpp"

int main()
{
    log_debug("程序开始");
    swq::GetFrame GetFrame_class("../asset/1.avi", 0); 
    GetFrame_class.GetOneFrame();
    GetFrame_class.EndCamera();
    log_debug("程序结束");
    return 0;
}