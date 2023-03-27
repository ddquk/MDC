 cmake_minimum_required(VERSION 3.24)
 project(MDC)
 set(log4cplus_DIR "D:/c++/vcpkg/installed/x64-windows/share/log4cplus")

 set(CMAKE_CXX_STANDARD 11)

 set(SOURCE_FILES main.cpp mdc.cpp hook_pthread_create.h hook_pthread_create.cpp)

 # 指定 VCPKG 安装路径和工具链文件
 set(VCPKG_DIR "D:/c++/vcpkg")
 set(CMAKE_TOOLCHAIN_FILE "${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")

 # 查找并链接 log4cplus 库
 find_package(log4cplus REQUIRED)
 add_executable(test ${SOURCE_FILES})
