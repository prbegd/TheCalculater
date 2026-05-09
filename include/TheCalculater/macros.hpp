/**
 * @file macros.hpp
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#pragma once

#define THECALCULATER_DEFINE_EXCEPTION(name, base) \
    class name : public base {                     \
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
# ifdef TPMMAPI_DLLEXPORT
#  define TPMMAPI __declspec(dllexport)
# else
#  define TPMMAPI __declspec(dllimport)
# endif
# ifdef TCQTAPI_DLLEXPORT
#  define TCQTAPI __declspec(dllexport)
# else
#  define TCQTAPI __declspec(dllimport)
# endif
#else
# define TCAPI   __attribute__((visibility("default")))
# define TPMMAPI __attribute__((visibility("default")))
# define TCQTAPI __attribute__((visibility("default")))
#endif  