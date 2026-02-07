/**
 * @file thread.hpp
 * @author prbegd
 * @date 2026-02-05
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#pragma once
#include "TheCalculater/core.hpp"
#include <string_view>

#ifdef _WIN32
#include <windef.h>
#else
#include <pthread.h>
#endif

namespace TheCalculater::util {
#ifdef _WIN32
    using ThreadHandleT = HANDLE;
    using ThreadIdT = DWORD;
#else
    using ThreadHandleT = pthread_t;
    using ThreadIdT = pid_t;
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
    THECALC_API bool setThreadNameByHandle(ThreadHandleT threadHandle, std::string_view name);
    /**
     * @brief Get the name of a thread identified by its handle.
     *
     * @param threadHandle The handle of the thread to get the name from.
     * @return The name of the thread.
     * @see setThreadNameByHandle(ThreadHandleT, std::string_view)
     */
    THECALC_API std::string getThreadNameByHandle(ThreadHandleT threadHandle);
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
    THECALC_API bool setThreadNameById(ThreadIdT threadId, std::string_view name);
    /**
     * @brief Get the name of a thread identified by its thread id.
     *
     * @param threadId The thread id of the thread to get the name from.
     * @return The name of the thread.
     * @see setThreadName(ThreadIdT, std::string_view)
     */
    THECALC_API std::string getThreadNameById(ThreadIdT threadId);

    class _CurrentThreadT {
    public:
        enum class _Construct { _Token };
        constexpr _CurrentThreadT(_Construct) noexcept { }
    };
    inline constexpr _CurrentThreadT currentThread { _CurrentThreadT::_Construct::_Token };

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
    THECALC_API bool setThreadName(_CurrentThreadT, std::string_view name);
    /**
     * @brief Get the name of the current thread.
     *
     * @return The name of the thread.
     */
    THECALC_API std::string getThreadName(_CurrentThreadT);
} // namespace TheCalculater::util