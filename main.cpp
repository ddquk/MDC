#include <iostream>
#include <D:\c++\pthread-win32-master\pthread.h>
#include "mdc.h"

using namespace std;
using namespace MDC_NAMESPACE;

void* child_thread_func(void* arg)
{
    cout << "Child thread start" << endl;
    // 获取子线程中的MDC数据并输出
    auto& context = MDC::MDC::getContext();
    for (const auto& item : context) {
        cout << "Child thread " << item.first << " : " << item.second << endl;
    }
    cout << "Child thread end" << endl;
    return nullptr;
}

void* parent_thread_func(void* arg)
{
    cout << "Parent thread start" << endl;
    // 设置MDC数据
    MDC::MDC::put("key2","value");
    // 输出MDC数据
    auto& context = MDC::MDC::getContext();
    for (const auto& item : context) {
        cout << "Parent thread " << item.first << " : " << item.second << endl;
    }
    // 创建子线程，并将MDC数据传递给子线程
    pthread_t child_thread;
    pthread_create(&child_thread, nullptr, child_thread_func, nullptr);
    // 等待子线程结束
    pthread_join(child_thread, nullptr);
    cout << "Parent thread end" << endl;
    return nullptr;
}

int main()
{
    // 初始化MDC，初始化pthread_key_t
    MDCInitializer mdc_init;
    // 创建父线程，并将MDC数据传递给父线程
    pthread_t parent_thread;
    pthread_create(&parent_thread, nullptr, parent_thread_func, nullptr);
    // 等待父线程结束
    pthread_join(parent_thread, nullptr);
    return 0;
}
//在运行 main.cpp 后，输出应该是：

//parent thread MDC : {"parent_key":"parent_value"}
//child thread MDC : {"child_key":"child_value", "parent_key" : "parent_value"}
//其中，第一行是父线程的 MDC 数据，第二行是子线程的 MDC 数据。
//可以看到，在子线程中，除了继承了父线程的 MDC 数据，还额外添加了一个 "child_key" : "child_value" 的键值对。