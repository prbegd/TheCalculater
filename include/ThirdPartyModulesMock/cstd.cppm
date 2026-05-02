/**
 * @file cstd.cppm
 * @author prbegd
 * @date 2026-04-06
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "TheCalculater/macros.hpp"
#include <cerrno>
#include <csignal>
#include <cstdio>


export module tpmm.cstd;

struct Errno {
    operator int() const { return errno; }
};

export namespace cstd {
    inline std::FILE* const _stdout = stdout;
    inline std::FILE* const _stderr = stderr;
    inline std::FILE* const _stdin = stdin;

    inline constexpr int _SIGSEGV = SIGSEGV;
    inline constexpr int _SIGFPE = SIGFPE;
    inline constexpr int _SIGILL = SIGILL;
    inline constexpr int _SIGABRT = SIGABRT;

    TPMMAPI inline const Errno _errno;
} // namespace cstd