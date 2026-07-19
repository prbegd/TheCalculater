/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
/*
 * A simple wrapper for spdlog. Replaced spdlog::source_loc with std::source_location so that we don't need macros to log the source location.
 * Others are just re-exports of spdlog.
 *
 * Reference: https://github.com/gabime/spdlog/pull/2667
 */
module;
#define SPDLOG_COMPILED_LIB
#include "thecalculater/macros.hpp"
#include <spdlog/spdlog.h>

#include <spdlog/async.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/details/registry.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/dup_filter_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/stopwatch.h>

export module thirdparty.core:spdlog;
import std;

template <typename T>
struct format_string_wrapper {
    template <size_t N>
    consteval format_string_wrapper(const char (&fs)[N], std::source_location loc = std::source_location::current())
        : format_string_(fs),
          loc_ { loc }
    { }
    operator T()
    {
        return format_string_;
    }
    T format_string_;
    std::source_location loc_;

    [[nodiscard]]
    spdlog::source_loc source_loc() const
    {
        return { loc_.file_name(), static_cast<std::int32_t>(loc_.line()), loc_.function_name() };
        ;
    }
};

template <typename... Args>
using format_string_t = format_string_wrapper<fmt::format_string<Args...>>;

export namespace spdlog {
using spdlog::source_loc;

template <typename... Args>
void _log(level::level_enum lvl, ::format_string_t<Args...> fmt, Args&&... args)
{
    details::registry::instance().get_default_raw()->log(fmt.source_loc(), lvl, fmt.format_string_, std::forward<Args>(args)...);
}
template <typename T>
void _log(source_loc source,
          level::level_enum lvl,
          T msg)
{
    details::registry::instance().get_default_raw()->log(source, lvl, msg);
}

template <typename... Args>
void trace(::format_string_t<Args...> fmt, Args&&... args)
{
    _log(level::level_enum::trace, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void debug(::format_string_t<Args...> fmt, Args&&... args)
{
    _log(level::level_enum::debug, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void info(::format_string_t<Args...> fmt, Args&&... args)
{
    _log(level::level_enum::info, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void warn(::format_string_t<Args...> fmt, Args&&... args)
{
    _log(level::level_enum::warn, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void error(::format_string_t<Args...> fmt, Args&&... args)
{
    _log(level::level_enum::err, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void critical(::format_string_t<Args...> fmt, Args&&... args)
{
    _log(level::level_enum::critical, fmt, std::forward<Args>(args)...);
}

// --- re-exports ---

using spdlog::async_logger;
using spdlog::formatter;
using spdlog::logger;
using spdlog::pattern_formatter;
using spdlog::source_loc;
using spdlog::spdlog_ex;

// log level
namespace level {
    using spdlog::level::critical;
    using spdlog::level::debug;
    using spdlog::level::err;
    using spdlog::level::from_str;
    using spdlog::level::info;
    using spdlog::level::level_enum;
    using spdlog::level::off;
    using spdlog::level::to_short_c_str;
    using spdlog::level::to_string_view;
    using spdlog::level::trace;
    using spdlog::level::warn;
} // namespace level

// pattern time type
using pattern_time_type = spdlog::pattern_time_type;

// logger management
using spdlog::apply_all;
using spdlog::default_logger;
using spdlog::drop;
using spdlog::drop_all;
using spdlog::flush_every;
using spdlog::flush_on;
using spdlog::get;
using spdlog::get_level;
using spdlog::initialize_logger;
using spdlog::register_logger;
using spdlog::set_automatic_registration;
using spdlog::set_default_logger;
using spdlog::set_error_handler;
using spdlog::set_formatter;
using spdlog::set_level;
using spdlog::set_pattern;
using spdlog::should_log;
using spdlog::shutdown;

// factory functions (synchronous)
using spdlog::basic_logger_mt;
using spdlog::basic_logger_st;
using spdlog::daily_logger_mt;
using spdlog::daily_logger_st;
using spdlog::null_logger_mt;
using spdlog::null_logger_st;
using spdlog::rotating_logger_mt;
using spdlog::rotating_logger_st;
using spdlog::stderr_logger_mt;
using spdlog::stderr_logger_st;
using spdlog::stdout_logger_mt;
using spdlog::stdout_logger_st;

// asynchronous logging
using spdlog::create;
using spdlog::create_async;
using spdlog::create_async_nb;
using spdlog::init_thread_pool;
using spdlog::thread_pool;

// sink namespace
namespace sinks {
    // base sink
    using spdlog::sinks::sink;

    // stdout/stderr
    using spdlog::sinks::stderr_sink_mt;
    using spdlog::sinks::stderr_sink_st;
    using spdlog::sinks::stdout_sink_mt;
    using spdlog::sinks::stdout_sink_st;

    // file sinks
    using spdlog::sinks::basic_file_sink_mt;
    using spdlog::sinks::basic_file_sink_st;
    using spdlog::sinks::daily_file_sink_mt;
    using spdlog::sinks::daily_file_sink_st;
    using spdlog::sinks::rotating_file_sink_mt;
    using spdlog::sinks::rotating_file_sink_st;

    // other sinks
    using spdlog::sinks::dup_filter_sink_mt;
    using spdlog::sinks::null_sink_mt;
    using spdlog::sinks::null_sink_st;
    using spdlog::sinks::ostream_sink;
    using spdlog::sinks::ringbuffer_sink_mt;
    using spdlog::sinks::ringbuffer_sink_st;

#ifdef SPDLOG_USE_SYSLOG
    using spdlog::sinks::syslog_sink_mt;
    using spdlog::sinks::syslog_sink_st;
#endif

    // color support (ansicolor)
    using spdlog::sinks::ansicolor_sink;
    using spdlog::sinks::ansicolor_stderr_sink_mt;
    using spdlog::sinks::ansicolor_stderr_sink_st;
    using spdlog::sinks::ansicolor_stdout_sink_mt;
    using spdlog::sinks::ansicolor_stdout_sink_st;
} // namespace sinks
using spdlog::sinks_init_list;

using spdlog::stopwatch;

namespace details {
    using spdlog::details::log_msg;
    using spdlog::details::registry;
    using spdlog::details::thread_pool;
    namespace os {
        using spdlog::details::os::thread_id;
    }
}
using spdlog::memory_buf_t;

using spdlog::color_mode;
} // namespace spdlog