#include <iostream>
#include <unordered_map>
#include <memory>
#include <D:/xianyu/NewREPtrace1/MDC/packages/EasyHook.2.7.7097/easyhook.h>
#include "hook_pthread_create.h"
#include "mdc.h"

// 原始的 pthread_create 函数指针
PthreadCreateFn original_pthread_create = nullptr;

// DLL 入口函数
void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
    // 安装钩子
    HOOK_TRACE_INFO hookInfo = { nullptr };
    NTSTATUS status = LhInstallHook(
        GetProcAddress(GetModuleHandle(TEXT("pthreadVC2.dll")), "pthread_create"),
        reinterpret_cast<LPVOID>(pthread_create),
        nullptr,
        &hookInfo
    );

    if (FAILED(status)) {
        std::cout << "Failed to install hook, error: " << status << std::endl;
        return;
    }

    std::cout << "Hook installed successfully!" << std::endl;

    // 启动钩子
    status = LhSetExclusiveACL(reinterpret_cast<ULONG*>(&hookInfo), 1, nullptr);

    if (FAILED(status)) {
        std::cout << "Failed to set ACL, error: " << status << std::endl;
        return;
    }

    std::cout << "Hook started successfully!" << std::endl;

    // 等待钩子被移除
    Sleep(INFINITE);
}

// 拦截器的 pthread_create 函数
int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg) {
    // 备份父线程的 MDC
    std::unordered_map<std::string, std::string> parent_mdc_map;
    auto parent_mdc = MDC_NAMESPACE::MDC::getContext();
    if (parent_mdc) {
        for (const auto& item : *parent_mdc) {
            parent_mdc_map[item.first] = item.second;
        }
    }

    // 调用原始的 pthread_create 函数创建线程
    static PthreadCreateFn real_pthread_create = nullptr;
    if (!real_pthread_create) {
        // 在 Windows 平台上，使用 GetProcAddress 函数获取 DLL 中导出函数的地址
        real_pthread_create = reinterpret_cast<PthreadCreateFn>(GetProcAddress(GetModuleHandle(TEXT("pthreadVC2.dll")), "pthread_create"));
    }
    int result = real_pthread_create(thread, attr, start_routine, arg);

    if (result == 0) {  // 如果创建线程成功
        // 将 MDC 从父线程复制到新线程
        auto new_mdc_map = MDC_NAMESPACE::MDC::getContext();
        if (new_mdc_map) {
            for (const auto& item : parent_mdc_map) {
                (*new_mdc_map)[item.first] = item.second;
            }
        }
    }

    return result;
}
