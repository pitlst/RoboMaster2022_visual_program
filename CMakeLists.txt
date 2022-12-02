# 要求并设置cmake的最低版本，版本不符合要求不允许编译
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
# 设置项目名
set(PROJECT_NAME cmake_study)
# 在这里直接指定项目为c++项目，节约查找tool chain的时间
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)
# 设置c++标准等级和编译后输出路径
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
# 导出执行命令到json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 强制化c++标准，当标准不符合要求时强制终止编译而不是采用更旧的标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# 设置默认的编译等级
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
# 设置默认的openvino安装路径
if(NOT DEFINED OpenVINO_DIR)
    #这里的路径仅针对开发者的电脑,更换部署环境的时候需要更改
    set(OpenVINO_DIR "/home/cheakf/openvino/runtime/cmake")
endif()
# 指定头文件在对应的文件夹中寻找
include_directories(include)
include_directories(3rd_part/include)
# 搜索src下源文件存入DIR_SRCS变量
FILE (GLOB DIR_SRCS "src/*.cpp")
# 找到所有声明，保存到变量里
FILE (GLOB M_INCLUDES "include/*.hpp")
# 找到所有第三方动态库，保存到变量里
FILE (GLOB 3rd_LIBS "3rd_part/lib/*.so")
# 找到所有第三方头文件，保存到变量里
FILE (GLOB 3rd_INCLUDE "3rd_part/include/*.h")
# 找到需要的依赖
find_package(OpenCV REQUIRED)
find_package(OpenVINO REQUIRED)
# 打印相关信息
message(STATUS "项目名称 ${PROJECT_NAME}")
message(STATUS "目标平台是 ${CMAKE_SYSTEM}")
message(STATUS "编译等级是 ${CMAKE_BUILD_TYPE}")
if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    message(STATUS "OpenCV版本是 ${OpenCV_VERSION}")
    message(STATUS "openvino的安装路径 ${OpenVINO_DIR}")
    message(STATUS "需要链接的源文件有 ${DIR_SRCS}")
    message(STATUS "需要链接的头文件有 ${M_INCLUDES}${3rd_INCLUDE}")
    message(STATUS "需要链接的动态库有 ${OpenCV_LIBS}${3rd_LIBS} pthread 和 openvino::runtime")
endif()

# 导入头文件
include_directories(${OpenCV_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})
# 生成目标可执行文件
add_executable(${PROJECT_NAME} ${DIR_SRCS} ${M_INCLUDES} ${3rd_INCLUDE})
# 链接动态库
target_link_libraries(${PROJECT_NAME} PRIVATE pthread ${OpenCV_LIBS} ${3rd_LIBS} openvino::runtime)

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
# >>>>>>>>测试部分:仅当在debug下启用并生成>>>>>>>>
    set(CTEST_OUTPUT_ON_FAILURE TRUE)
    # 生成测试用目标可执行文件
    add_executable(test_json ${PROJECT_SOURCE_DIR}/test/test_json.cpp ${PROJECT_SOURCE_DIR}/src/json.cpp ${PROJECT_SOURCE_DIR}/include/json.hpp)
    add_executable(test_kalman ${PROJECT_SOURCE_DIR}/test/test_kalman.cpp)
    target_link_libraries(test_kalman PRIVATE ${OpenCV_LIBS})
    add_executable(restart_video ${PROJECT_SOURCE_DIR}/test/restart_video.cpp)
    target_link_libraries(restart_video PRIVATE ${3rd_LIBS})
    # 启用ctest测试
    enable_testing()
    # 运行测试json生成文件
    add_test(NAME json_run COMMAND test_json)
    # 运行相机测试
    add_test(NAME restart_video COMMAND restart_video)
    # 运行卡尔曼demo
    add_test(NAME test_kalman COMMAND test_kalman)
# <<<<<<<<测试部分结束<<<<<<<<
elseif("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    # 生成相机检测文件
    add_executable(restart_video ${PROJECT_SOURCE_DIR}/test/restart_video.cpp)
    target_link_libraries(restart_video PRIVATE ${3rd_LIBS})
    # 启用ctest测试
    enable_testing()
    # 运行相机测试
    add_test(NAME restart_video COMMAND restart_video)
endif()
