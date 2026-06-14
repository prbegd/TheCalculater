/**
 * @file cstd.cppm
 * @author prbegd
 * @date 2026-04-06
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