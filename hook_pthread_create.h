#ifndef HOOK_PTHREAD_CREATE_H
#define HOOK_PTHREAD_CREATE_H

#include "D:/c++/pthread-win32-master/pthread.h"
#include <unordered_map>
#include <memory>
#include "mdc.h"
#include "D:/xianyu/NewREPtrace1/MDC/packages/EasyHook.2.7.7097/easyhook.h"
#pragma once

// 定义原始的 pthread_create 函数指针类型
typedef int (*PthreadCreateFn)(pthread_t*, const pthread_attr_t*, void *(*)(void*), void*);

// 用于存储每个线程对应的 MDC
thread_local std::shared_ptr<std::unordered_map<std::string, std::string>> MDC_map;

// 定义拦截器的 pthread_create 函数
extern "C" int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void *(*start_routine)(void*), void* arg);


int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void *(*start_routine)(void*), void* arg);


// 拦截器的 pthread_create 函数
int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);


// 拦截器的入口函数
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

#endif  // HOOK_PTHREAD_CREATE_H
