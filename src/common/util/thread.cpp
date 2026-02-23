/**
 * @file thread.cpp
 * @author prbegd
 * @date 2026-02-05
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */

#include "TheCalculater/util/thread.hpp"

#ifdef __POSIX__

#include <fstream>
#include <pthread.h>
#include <sys/types.h>

namespace TheCalculater::util {
    bool setThreadNameByHandle(ThreadHandleT threadHandle, std::string_view name)
    {
        std::string threadName = std::string(name).substr(0, 15);
        int result = pthread_setname_np(threadHandle, threadName.c_str());
        return result == 0;
    }
    std::string getThreadNameByHandle(ThreadHandleT threadHandle)
    {
        char name[16] = { 0 };
        int result = pthread_getname_np(threadHandle, name, sizeof(name));
        if (result != 0) {
            return {};
        }
        return { name };
    }
    bool setThreadNameById(ThreadIdT threadId, std::string_view name)
    {
        std::string threadName = std::string(name).substr(0, 15);

        std::string path = std::format("/proc/{}/task/{}/comm", getpid(), threadId);
        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }
        file << threadName;
        if (file.fail()) {
            return false;
        }
        file.close();
        return true;
    }
    std::string getThreadNameById(ThreadIdT threadId)
    {
        std::string path = std::format("/proc/{}/task/{}/comm", getpid(), threadId);
        std::ifstream file(path);
        if (!file.is_open()) {
            return {};
        }
        std::string threadName;
        std::getline(file, threadName);
        file.close();
        return threadName;
    }
#ifndef __APPLE__
    bool setThreadName(_CurrentThreadT, std::string_view name)
    {
        std::string threadName = std::string(name).substr(0, 15);
        int result = pthread_setname_np(pthread_self(), threadName.c_str());
        return result == 0;
    }
    std::string getThreadName(_CurrentThreadT)
    {
        char name[16] = { 0 };
        int result = pthread_getname_np(pthread_self(), name, sizeof(name));
        if (result != 0) {
            return {};
        }
        return { name };
    }
#else
    bool setThreadName(_CurrentThreadT, std::string_view name)
    {
        std::string threadName = std::string(name).substr(0, 15);
        int result = pthread_setname_np(threadName.c_str());
        return result == 0;
    }
    std::string getThreadName(_CurrentThreadT)
    {
        char name[16] = { 0 };
        int result = pthread_getname_np(name, sizeof(name));
        if (result != 0) {
            return {};
        }
        return { name };
    }
#endif
} // namespace TheCalculater::util

#elif defined(_WIN32)

#include <iostream>
#include <processthreadsapi.h>
#include <winerror.h>
#include <winnt.h>
namespace TheCalculater::util {
    bool setThreadNameByHandle(ThreadHandleT threadHandle, std::string_view name)
    {
        std::wstring wname(name.begin(), name.end());
        if (HRESULT res = SetThreadDescription(threadHandle, wname.c_str()); FAILED(res)) { // NOLINT(performance-no-int-to-ptr)
            SetLastError(res);
            return false;
        }
        return true;
    }
    std::string getThreadNameByHandle(ThreadHandleT threadHandle)
    {
        PWSTR wname = nullptr;
        if (HRESULT res = GetThreadDescription(threadHandle, &wname); FAILED(res)) { // NOLINT(performance-no-int-to-ptr)
            SetLastError(res);
            return {};
        }
        std::wstring ws(wname);
        LocalFree(wname);
        return { ws.begin(), ws.end() };
    }
    bool setThreadNameById(ThreadIdT threadId, std::string_view name)
    {
        HANDLE hThread = OpenThread(THREAD_SET_LIMITED_INFORMATION, FALSE, threadId);
        if (hThread == nullptr) {
            SetLastError(ERROR_INVALID_THREAD_ID);
            return false;
        }

        std::wstring wname(name.begin(), name.end());
        if (HRESULT res = SetThreadDescription(hThread, wname.c_str()); FAILED(res)) { // NOLINT(performance-no-int-to-ptr)
            SetLastError(res);
            CloseHandle(hThread);
            return false;
        }
        CloseHandle(hThread);
        return true;
    }
    std::string getThreadNameById(ThreadIdT threadId)
    {
        HANDLE hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadId);
        if (hThread == nullptr) {
            SetLastError(ERROR_INVALID_THREAD_ID);
            return {};
        }

        PWSTR wname = nullptr;
        if (HRESULT res = GetThreadDescription(hThread, &wname); FAILED(res)) { // NOLINT(performance-no-int-to-ptr)
            SetLastError(res);
            CloseHandle(hThread);
            return {};
        }
        std::wstring ws(wname);
        LocalFree(wname);
        CloseHandle(hThread);
        return { ws.begin(), ws.end() };
    }
    bool setThreadName(_CurrentThreadT, std::string_view name)
    {
        std::wstring wname(name.begin(), name.end());
        if (HRESULT res = SetThreadDescription(GetCurrentThread(), wname.c_str()); FAILED(res)) {
            SetLastError(res);
            return false;
        }

        return true;
    }
    std::string getThreadName(_CurrentThreadT)
    {
        PWSTR wname = nullptr;
        if (HRESULT res = GetThreadDescription(GetCurrentThread(), &wname); FAILED(res)) {
            SetLastError(res);
            return {};
        }
        std::wstring ws(wname);
        LocalFree(wname);
        return { ws.begin(), ws.end() };
    }
} // namespace TheCalculater::util

#else

namespace TheCalculater::util {
    bool setThreadName(std::thread&, std::string_view)
    {
        return false;
    }
    std::string getThreadName(std::thread&)
    {
        return {};
    }
    bool setThreadName(_CurrentThreadT, std::string_view)
    {
        return false;
    }
    std::string getThreadName(_CurrentThreadT)
    {
        return {};
    }
    bool setThreadName(uintptr_t, std::string_view)
    {
        return false;
    }
    std::string getThreadName(uintptr_t)
    {
        return {};
    }
} // namespace TheCalculater::util
#endif
