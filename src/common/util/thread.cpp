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
module TheCalculater.util.thread;
import tpmm.spdlog;
import tpmm.posixapi;
import tpmm.winapi;
import std;

#ifdef _POSIX_VERSION

namespace TheCalculater::util {
# ifndef __APPLE__
    bool setThreadNameByHandle(ThreadHandleT threadHandle, std::string_view name)
    {
        std::string threadName = std::string(name).substr(0, 15);
        int result = posixapi::pthread_setname_np(threadHandle, threadName.c_str());
        return result == 0;
    }
# else
    bool setThreadNameByHandle(ThreadHandleT threadHandle, std::string_view name)
    {
        // Can only set current thread name
        if (threadHandle != posixapi::pthread_self()) return false;

        std::string threadName = std::string(name).substr(0, 15);
        int result = posixapi::pthread_setname_np(threadName.c_str());
        return result == 0;
    }
# endif
    std::string getThreadNameByHandle(ThreadHandleT threadHandle)
    {
        char name[16] = { 0 };
        int result = posixapi::pthread_getname_np(threadHandle, name, sizeof(name));
        if (result != 0) {
            return { };
        }
        return { name };
    }
    bool setThreadNameById(ThreadIdT threadId, std::string_view name)
    {
        std::string threadName = std::string(name).substr(0, 15);

        std::string path = std::format("/proc/{}/task/{}/comm", posixapi::getpid(), threadId);
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
        std::string path = std::format("/proc/{}/task/{}/comm", posixapi::getpid(), threadId);
        std::ifstream file(path);
        if (!file.is_open()) {
            return { };
        }
        std::string threadName;
        std::getline(file, threadName);
        file.close();
        return threadName;
    }
# ifndef __APPLE__
    bool setThreadName(_CurrentThreadT, std::string_view name)
    {
        std::string threadName = std::string(name).substr(0, 15);
        int result = posixapi::pthread_setname_np(posixapi::pthread_self(), threadName.c_str());
        return result == 0;
    }
# else
    bool setThreadName(_CurrentThreadT, std::string_view name)
    {
        std::string threadName = std::string(name).substr(0, 15);
        int result = posixapi::pthread_setname_np(threadName.c_str());
        return result == 0;
    }
# endif
    std::string getThreadName(_CurrentThreadT)
    {
        char name[16] = { 0 };
        int result = posixapi::pthread_getname_np(posixapi::pthread_self(), name, sizeof(name));
        if (result != 0) {
            return { };
        }
        return { name };
    }
} // namespace TheCalculater::util

#elif defined(_WIN32)

namespace TheCalculater::util {
    bool setThreadNameByHandle(ThreadHandleT threadHandle, std::string_view name)
    {
        std::wstring wname(name.begin(), name.end());
        if (winapi::HRESULT res = winapi::SetThreadDescription(threadHandle, wname.c_str()); winapi::_FAILED(res)) { // NOLINT(performance-no-int-to-ptr)
            winapi::SetLastError(res);
            return false;
        }
        return true;
    }
    std::string getThreadNameByHandle(ThreadHandleT threadHandle)
    {
        winapi::PWSTR wname = nullptr;
        if (winapi::HRESULT res = winapi::GetThreadDescription(threadHandle, &wname); winapi::_FAILED(res)) { // NOLINT(performance-no-int-to-ptr)
            winapi::SetLastError(res);
            return { };
        }
        std::wstring ws(wname);
        winapi::LocalFree(wname);
        return { ws.begin(), ws.end() };
    }
    bool setThreadNameById(ThreadIdT threadId, std::string_view name)
    {
        winapi::HANDLE hThread = winapi::OpenThread(winapi::_THREAD_SET_LIMITED_INFORMATION, winapi::_FALSE, threadId);
        if (hThread == nullptr) {
            winapi::SetLastError(winapi::_ERROR_INVALID_THREAD_ID);
            return false;
        }

        std::wstring wname(name.begin(), name.end());
        if (winapi::HRESULT res = winapi::SetThreadDescription(hThread, wname.c_str()); winapi::_FAILED(res)) { // NOLINT(performance-no-int-to-ptr)
            winapi::SetLastError(res);
            winapi::CloseHandle(hThread);
            return false;
        }
        winapi::CloseHandle(hThread);
        return true;
    }
    std::string getThreadNameById(ThreadIdT threadId)
    {
        winapi::HANDLE hThread = winapi::OpenThread(winapi::_THREAD_QUERY_LIMITED_INFORMATION, winapi::_FALSE, threadId);
        if (hThread == nullptr) {
            winapi::SetLastError(winapi::_ERROR_INVALID_THREAD_ID);
            return { };
        }

        winapi::PWSTR wname = nullptr;
        if (winapi::HRESULT res = winapi::GetThreadDescription(hThread, &wname); winapi::_FAILED(res)) { // NOLINT(performance-no-int-to-ptr)
            winapi::SetLastError(res);
            winapi::CloseHandle(hThread);
            return { };
        }
        std::wstring ws(wname);
        winapi::LocalFree(wname);
        winapi::CloseHandle(hThread);
        return { ws.begin(), ws.end() };
    }
    bool setThreadName(_CurrentThreadT, std::string_view name)
    {
        std::wstring wname(name.begin(), name.end());
        if (winapi::HRESULT res = winapi::SetThreadDescription(winapi::GetCurrentThread(), wname.c_str()); winapi::_FAILED(res)) {
            winapi::SetLastError(res);
            return false;
        }

        return true;
    }
    std::string getThreadName(_CurrentThreadT)
    {
        winapi::PWSTR wname = nullptr;
        if (winapi::HRESULT res = winapi::GetThreadDescription(winapi::GetCurrentThread(), &wname); winapi::_FAILED(res)) {
            winapi::SetLastError(res);
            return { };
        }
        std::wstring ws(wname);
        winapi::LocalFree(wname);
        return { ws.begin(), ws.end() };
    }
} // namespace TheCalculater::util

#else

namespace TheCalculater::util {
    bool setThreadNameByHandle(ThreadHandleT threadHandle, std::string_view name)
    {
        return false;
    }
    std::string getThreadNameByHandle(ThreadHandleT threadHandle)
    {
        return { };
    }
    bool setThreadNameById(ThreadIdT threadId, std::string_view name)
    {
        return false;
    }
    std::string getThreadNameById(ThreadIdT threadId)
    {
        return { };
    }
    bool setThreadName(_CurrentThreadT, std::string_view name)
    {
        return false;
    }
    std::string getThreadName(_CurrentThreadT)
    {
        return { };
    }
} // namespace TheCalculater::util
#endif

namespace TheCalculater::util {
    ThreadIdT getCurrentThreadId()
    {
        return spdlog::details::os::thread_id();
    }
}