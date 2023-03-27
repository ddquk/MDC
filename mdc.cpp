#include "mdc.h"

#include <unordered_map>
#include <string>
#include <D:\c++\pthread-win32-master\pthread.h>

namespace MDC {

    // 定义一个线程局部存储的结构体，用于存储每个线程的MDC数据
    struct ThreadData {
        std::unordered_map<std::string, std::string> mdc_data;
    };

    // 使用静态变量存储MDC数据，同时使用线程局部存储保证线程安全
    static pthread_key_t s_mdc_key;
    static std::unordered_map<pthread_t, ThreadData*> s_thread_data_map;

    // 在线程结束时调用的函数，用于释放线程局部存储的MDC数据
    void thread_data_destructor(void* ptr) {
        ThreadData* data = static_cast<ThreadData*>(ptr);
        if (data) {
            delete data;
        }
    }

    // 在MDC结构体初始化时调用的函数，用于初始化pthread_key_t

    void MDC::MDCInitializer() {
        pthread_key_create(&s_mdc_key, thread_data_destructor);
    }

    // 获取当前线程的MDC数据
    std::unordered_map<std::string, std::string>& MDC::getContext() {
        // 先从线程局部存储中获取MDC数据，如果没有则创建一个新的ThreadData并存储到线程局部存储中
        ThreadData* data = static_cast<ThreadData*>(pthread_getspecific(s_mdc_key));
        if (data == nullptr) {
            data = new ThreadData;
            pthread_setspecific(s_mdc_key, data);
            s_thread_data_map[pthread_self()] = data;
        }
        return data->mdc_data;
    }

    // 清空当前线程的MDC数据
    void MDC::clear() {
        std::unordered_map<std::string, std::string>& data = getContext();
        data.clear();
    }

    // 将MDC数据拷贝到子线程中
    void MDC::copyToChildThread(pthread_t child_thread) {
        // 先获取父线程的MDC数据
        ThreadData* parent_data = static_cast<ThreadData*>(pthread_getspecific(s_mdc_key));
        if (parent_data == nullptr) {
            return;
        }
        std::unordered_map<std::string, std::string>& parent_mdc_data = parent_data->mdc_data;

        // 将父线程的MDC数据拷贝到子线程中
        ThreadData* child_data = s_thread_data_map[child_thread];
        if (child_data == nullptr) {
            child_data = new ThreadData;
            s_thread_data_map[child_thread] = child_data;
        }
        std::unordered_map<std::string, std::string>& child_mdc_data = child_data->mdc_data;
        child_mdc_data.insert(parent_mdc_data.begin(), parent_mdc_data.end());
    }

    // 设置当前线程的MDC数据
    void MDC::put(const std::string& key, const std::string& value) {
        std::unordered_map<std::string, std::string>& data = getContext();
        data[key] = value;
    }

} // namespace MDC
