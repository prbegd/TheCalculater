/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "thecalculater/macros.hpp"

module prbegd.thecalculater.util;
import thirdparty.core;
import std;

#ifdef THECALCULATER_POSIX

namespace thecalculater::util {
# ifndef THECALCULATER_APPLE
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
# ifndef THECALCULATER_APPLE
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
} // namespace thecalculater::util

#elifdef THECALCULATER_WINDOWS

namespace thecalculater::util {
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
} // namespace thecalculater::util

#else

namespace thecalculater::util {
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
} // namespace thecalculater::util
#endif

namespace thecalculater::util {
    ThreadIdT getCurrentThreadId()
    {
        return spdlog::details::os::thread_id();
    }
}