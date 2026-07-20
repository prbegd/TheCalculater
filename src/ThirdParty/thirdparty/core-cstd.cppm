/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include <cerrno>
#include <csignal>
#include <cstdio>

export module thirdparty.core:cstd;

struct Errno {
    operator int&() const { return errno; }
};
struct Stdout {
    operator std::FILE*() const { return stdout; }
};
struct Stderr {
    operator std::FILE*() const { return stderr; }
};
struct Stdin {
    operator std::FILE*() const { return stdin; }
};

export namespace cstd {
// TODO: Change these leading underscores to suffixes, along with winapi.
inline const Stdout _stdout;
inline const Stderr _stderr;
inline const Stdin _stdin;

constexpr inline int _SIGSEGV = SIGSEGV;
constexpr inline int _SIGFPE = SIGFPE;
constexpr inline int _SIGILL = SIGILL;
constexpr inline int _SIGABRT = SIGABRT;

inline const Errno _errno;
} // namespace cstd
