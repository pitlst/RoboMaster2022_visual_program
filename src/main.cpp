#include <iostream>
#include <thread>

#include "get_frame.hpp"
#include "aimbot.hpp"
#include "energy.hpp"
#include "utils.hpp"
#include "json.hpp"
#include "hint.hpp"
#include "logger.hpp"
#include "xml.hpp"

int main()
{
    log_debug("程序开始");
    std::cout << ov::get_openvino_version() << std::endl;
    ov::Core core;
    auto model = core.read_model("../asset/model/bestyao_13_416.xml");
    ov::CompiledModel compiled_model = core.compile_model(model, "GPU");
    ov::InferRequest infer_request = compiled_model.create_infer_request();
    log_debug("程序结束");
    return 0;
}
