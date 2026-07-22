/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

// #define THECALCULATER_DEFINE_EXCEPTION(name, base) \
//     class [[deprecated]] name : public base {      \
//     public:                                        \
//         [[deprecated]]                             \
//         explicit name(const std::string& message)  \
//             : base(message)                        \
//         { }                                        \
//     }
#define THECALCULATER_DEFINE_EXCEPTION(name, base) \
    class name : public base {      \
    public:                                        \
        explicit name(const std::string& message)  \
            : base(message)                        \
        { }                                        \
    }

#ifdef _WIN32
# define THECALCULATER_WINDOWS
#elif defined(__unix__) || defined(__unix)
# define THECALCULATER_POSIX
#elif defined(__APPLE__) || defined(__MACH__)
# define THECALCULATER_POSIX
# define THECALCULATER_APPLE
#endif

#ifdef THECALCULATER_WINDOWS
# ifdef TCAPI_DLLEXPORT
#  define TCAPI __declspec(dllexport)
# else
#  define TCAPI __declspec(dllimport)
# endif
#else
# define TCAPI __attribute__((visibility("default")))
#endif

#if defined(__GNUC__) || defined(__clang__)
#define THECALCULATER_PRAGMA(_cmd_) _Pragma(#_cmd_)
#define THECALCULATER_DIAGNOSTIC(_action_) THECALCULATER_PRAGMA(GCC diagnostic _action_)
#else
#define THECALCULATER_DIAGNOSTIC(_action_)
#endif