/**
 * @file thread.cppm
 * @author prbegd
 * @date 2026-02-05
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "TheCalculater/macros.hpp"

export module TheCalculater.util.thread;
import tpmm.posixapi;
import tpmm.winapi;
import std;

namespace TheCalculater::util {
#ifdef THECALCULATER_WINDOWS
    export using ThreadHandleT = winapi::HANDLE;
    export using ThreadIdT = winapi::DWORD;
#elifdef THECALCULATER_POSIX
    export using ThreadHandleT = posixapi::pthread_t;
    export using ThreadIdT = posixapi::pid_t;
#else
# warning "Unknown platform, thread name functions will not be implemented."
    export using ThreadHandleT = void*;
    export using ThreadIdT = unsigned long long;
#endif

    /**
     * @brief Set the name of a thread identified by its handle.
     *
     * @warning The part of the name over 15 characters will be truncated.
     * @warning Make sure the name contains NO Unicode characters on Windows.
     *              Otherwise, you won't know what will happen when you try to get the name back.
     *
     * On Windows, threadHandle is the HANDLE returned by GetCurrentThread() or OpenThread().
     *     Ensure the handle has THREAD_SET_LIMITED_INFORMATION access right.
     * On POSIX systems, threadHandle is the pthread_t handle returned by pthread_create() or pthread_self().
     *
     * @param threadHandle The handle of the thread to set the name for.
     * @param name The name to set. **MUST** be less than 16 characters.
     * @return Whether the operation was successful.
     */
    export TCAPI bool setThreadNameByHandle(ThreadHandleT threadHandle, std::string_view name);
    /**
     * @brief Get the name of a thread identified by its handle.
     *
     * @param threadHandle The handle of the thread to get the name from.
     * @return The name of the thread.
     * @see setThreadNameByHandle(ThreadHandleT, std::string_view)
     */
    export TCAPI std::string getThreadNameByHandle(ThreadHandleT threadHandle);
    /**
     * @brief Set the name of a thread identified by its thread id.
     *
     * @warning The part of the name over 15 characters will be truncated.
     * @warning Make sure the name contains NO Unicode characters on Windows.
     *              Otherwise, you won't know what will happen when you try to get the name back.
     *
     * On Windows, threadId is the DWORD returned by GetCurrentThreadId().
     * On POSIX systems, threadId is the Light Weight Process ID (LWP ID): the thread id shown in /proc/[pid]/task/[tid]; pid_t.
     *
     * @param threadId The thread id of the thread to set the name for.
     * @param name The name to set. **MUST** be less than 16 characters.
     * @return Whether the operation was successful.
     */
    export TCAPI bool setThreadNameById(ThreadIdT threadId, std::string_view name);
    /**
     * @brief Get the name of a thread identified by its thread id.
     *
     * @param threadId The thread id of the thread to get the name from.
     * @return The name of the thread.
     * @see setThreadName(ThreadIdT, std::string_view)
     */
    export TCAPI std::string getThreadNameById(ThreadIdT threadId);

    export class _CurrentThreadT {
    public:
        enum class _Construct { _Token };
        constexpr _CurrentThreadT(_Construct) noexcept { }
    };
    export inline constexpr _CurrentThreadT currentThread { _CurrentThreadT::_Construct::_Token };

    /**
     * @brief Set the name of the current thread.
     *
     * @warning The part of the name over 15 characters will be truncated.
     * @warning Make sure the name contains NO Unicode characters on Windows.
     *              Otherwise, you won't know what will happen when you try to get the name back.
     *
     * @param name The name to set. **MUST** be less than 16 characters.
     * @return Whether the operation was successful.
     */
    export TCAPI bool setThreadName(_CurrentThreadT, std::string_view name);
    /**
     * @brief Get the name of the current thread.
     *
     * @return The name of the thread.
     */
    export TCAPI std::string getThreadName(_CurrentThreadT);

    /**
     * @brief Get the thread id of the current thread.
     *
     * On Windows, threadId is the DWORD returned by GetCurrentThreadId().
     * On POSIX systems, threadId is the Light Weight Process ID (LWP ID): the thread id shown in /proc/[pid]/task/[tid]; pid_t.
     *
     * @return ThreadIdT The thread id of the current thread.
     */
    export TCAPI ThreadIdT getCurrentThreadId();
} // namespace TheCalculater::util