/**
 * @file core.hpp
 * @author prbegd
 * @brief Core header file for TheCalculater.
 * @date 2025-07-28
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once
#include "boost/exception/all.hpp" // IWYU pragma: keep
#include "boost/stacktrace/stacktrace.hpp"
#include <memory>
#include <stdexcept>

#ifdef _WIN32
#ifdef THECALCULATERCOMMON_EXPORTS
#define THECALC_API __declspec(dllexport)
#else
#define THECALC_API __declspec(dllimport)
#endif
#else
#define THECALC_API __attribute__((visibility("default")))
#endif

#define THECALCULATER_DEFINE_EXCEPTION(name, base) \
    class name : public base {                     \
    public:                                        \
        explicit name(const std::string& message)  \
            : base(message)                        \
        { }                                        \
    }

namespace TheCalculater::core {
    THECALCULATER_DEFINE_EXCEPTION(UnexpectedException, std::logic_error);
    THECALCULATER_DEFINE_EXCEPTION(IOException, std::runtime_error);
    THECALCULATER_DEFINE_EXCEPTION(FileNotFoundException, IOException);
    THECALCULATER_DEFINE_EXCEPTION(WeakPointerExpiredException, std::runtime_error);

    struct ThrowExData {
        boost::stacktrace::stacktrace trace;
        std::exception_ptr cause;

        ThrowExData(boost::stacktrace::stacktrace trace, std::exception_ptr cause);
        ThrowExData(boost::stacktrace::stacktrace trace);
    };
    using ThrowExDataErrorInfo = boost::error_info<struct tag_throw_ex_data, ThrowExData>;

    THECALC_API void registerLogger(const std::shared_ptr<spdlog::logger>& logger);
} // namespace TheCalculater::core
namespace TheCalculater {
    template <typename E>
    [[noreturn]] void throwEx(const E& e)
        requires(std::is_base_of_v<std::exception, E>)
    {
        throw boost::enable_error_info(e)
            << core::ThrowExDataErrorInfo(core::ThrowExData(boost::stacktrace::stacktrace()));
    }
    template <typename E>
    [[noreturn]] void throwEx(const E& e, const std::exception_ptr& cause)
        requires(std::is_base_of_v<std::exception, E>)
    {
        if (!cause)
            throw boost::enable_error_info(e)
                << core::ThrowExDataErrorInfo(core::ThrowExData(boost::stacktrace::stacktrace(), std::current_exception()));
        throw boost::enable_error_info(e)
            << core::ThrowExDataErrorInfo(core::ThrowExData(boost::stacktrace::stacktrace(), cause));
    }
}