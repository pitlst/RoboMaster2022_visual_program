# 个人c++学习项目

## 主要目的

主要是学习c++，目前目标是将实验室的机器人视觉项目全部拿c++重写一遍。

## 文件结构

include中存放头文件声明

src存放源文件

test存放测试文件

asset存放其他的各项资源，包括测试用的视频，存放参数的json等等。

.vscode是存放vscode的配置文件

## 部署方式

本项目使用cmake组织项目，依赖于opencv,openvino和eigen3

在这里介绍本项目开发时环境的搭建，用于作为演示。

本人开发机器Ubuntu20.04，i7-1165G7 16g内存 256g固态。

理论上只要是ubuntu20.04就不存在编译器版本问题。

首先安装eigen3

> sudo apt-get install libeigen3-dev

就可以了

其次是安装一些用到的系统应用

>sudo apt install g++ git zip make cmake wget python3-pip python3-numpy python3-venv ffmpeg -y

再其次安装opencv的编译依赖

>sudo apt install cmake build-essential pkg-config libgtk2.0-dev libavcodec-dev libavformat-dev libswscale-dev libjpeg-dev libpng-dev -y

然后获取opencv，开发时使用的版本为opencv4.5.3

>cd ~/Downloads
>
>wget opencv.zip https://github.com/opencv/opencv/archive/4.x.zip

然后解压并编译，注意这里我们一般直接修改cmakelists开启intel的tbb加速，当然不是必选项。

>unzip opencv.zip
>
>mkdir -p build && cd build
>
>cmake ..
>
>make --jobs=$(nproc --all)

然后等待编译完成即可，一般不会报错。

在编译完成后，执行默认安装。

>sudo make install

在安装完成后，可以尝试在python中导入cv2模块用于验证安装，这里不做演示。

接下来则是openvino的编译安装，openvino可以使用官方的客户端进行安装，更加直观，这里演示编译安装不是必选项，具体可以上openvino官网上的[安装教程](https://docs.openvino.ai/latest/index.html)，非常详细。

首先，我们使用GPU进行推演，所以需要先安装opencl。

>mkdir neo && cd neo
>
>wget https://github.com/intel/compute-runtime/releases/download/19.41.14441/intel-gmmlib_19.3.2_amd64.deb
>
>wget https://github.com/intel/compute-runtime/releases/download/19.41.14441/intel-igc-core_1.0.2597_amd64.deb
>
>wget https://github.com/intel/compute-runtime/releases/download/19.41.14441/intel-igc-opencl_1.0.2597_amd64.deb
>
>wget https://github.com/intel/compute-runtime/releases/download/19.41.14441/intel-opencl_19.41.14441_amd64.deb
>
>wget https://github.com/intel/compute-runtime/releases/download/19.41.14441/intel-ocloc_19.41.14441_amd64.deb
>
>sudo dpkg -i *.deb
>
>cd ..

安装完成后，可以安装clinfo用于验证opencl的安装，这里不做演示。

之后，我们获取openvino。

>git clone https://github.com/openvinotoolkit/openvino.git
>
>cd openvino
>
>git submodule update --init --recursive

注意这里需要保持一个良好的网络环境，强烈建议科学上网，不要相信官网的gitee脚本，该不行还是不行。

然后执行对依赖的安装。

>chmod +x install_build_dependencies.sh
>
>./install_build_dependencies.sh

然后开始编译，注意我们开启的项目，而且我们是c++项目，所以不开启对pythonAPI的构建。

>mkdir build && cd build
>
>cmake -DCMAKE_BUILD_TYPE=Release DENABLE_INTEL_CPU=ON  -DENABLE_INTEL_GPU=ON -DENABLE_OV_ONNX_FRONTEND=ON ..
>
>make --jobs=$(nproc --all)

最后安装，可以自己指定安装位置。

>cmake --install \<BUILDDIR> --prefix \<INSTALLDIR>

在依赖的部署完成后，可以部署本项目。

> git clone https://github.com/pitlst/cpp_study.git
>
> mkdir build
>
> cd build
> 
> cmake ..
>
> make --jobs=$(nproc --all)

## 注意

1. 对于json类和xml类的下标索引，是循环式的，-1表示为最后一个

2. 对于json和xml，内部都是动态增加，自动扩容的，实现了内存回收，但是需要手动在生命周期后调用

3. 本项目使用cmake组织项目，并且使用c++11，启用测试，暂时还没有编写单元测试

4. 对于大整数类，后续有时间再做优化，目前以实现功能为主，暂时只实现了加减法，使用二进制存储

5. 对于相机的调用，目前没有支持GigE海康相机的打算，主要是实验室没有相关设备无法测试，但是仍然正常打包了所有海康的动态库，理论上可以使用

6. 对于openvino的编译安装教程，是为了适应对应的第三方库，更快捷的方式是直接调用asset/openvino-GPU的安装脚本。

7. 对于openvino调用GPU，需要更新驱动，已经将脚本单独放在了asset/openvino-GPU中，执行即可。

8. 对于一些常用调试，写在了debug.hpp里，通过宏定义的方式管理