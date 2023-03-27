#pragma once

#include <unordered_map>
#include <string>
#include "D:/c++/pthread-win32-master/pthread.h"

namespace MDC {
    // 定义MDC类
    class MDC {
    public:
        // 获取当前线程的MDC数据
        static std::unordered_map<std::string, std::string>& getContext();

        // 清空当前线程的MDC数据
        static void clear();

        // 将MDC数据拷贝到子线程中
        static void copyToChildThread(pthread_t child_thread);

        // 设置当前线程的MDC数据
        static void put(const std::string& key, const std::string& value);
    };

    // 定义一个线程局部存储的结构体，用于存储每个线程的MDC数据
    struct ThreadData {
        std::unordered_map<std::string, std::string> mdc_data;
    };

    // 使用静态变量存储MDC数据，同时使用线程局部存储保证线程安全
    extern pthread_key_t s_mdc_key;
    extern std::unordered_map<pthread_t, ThreadData*> s_thread_data_map;

    // 在线程结束时调用的函数，用于释放线程局部存储的MDC数据
    void thread_data_destructor(void* ptr);

    // 在MDC结构体初始化时调用的函数，用于初始化pthread_key_t
    struct MDCInitializer {
        MDCInitializer();
    };
}

