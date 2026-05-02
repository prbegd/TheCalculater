/**
 * @file winapi.cpp
 * @author prbegd
 * @date 2026-04-19
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "TheCalculater/macros.hpp"
#ifdef THECALCULATER_WINDOWS
# include <fcntl.h>
# include <io.h>
# include <minwindef.h>
# include <ntstatus.h>
# include <windows.h>
#endif

module tpmm.winapi;
#ifdef THECALCULATER_WINDOWS
namespace winapi {
    bool _FAILED(HRESULT hr)
    {
        return FAILED(hr);
    }

    void _ZeroMemory(PVOID Destination, SIZE_T Length)
    {
        ::memset(Destination, 0, Length);
    }
} // namespace winapi
#endif