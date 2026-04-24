/**
 * @file spdloglib.cpp
 * @author prbegd
 * @date 2026-04-19
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#include <memory>
// Following is file splices in directory https://github.com/gabime/spdlog/tree/v1.x/src with some modifications
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
#ifndef SPDLOG_COMPILED_LIB
#error Please define SPDLOG_COMPILED_LIB to compile this file.
#endif

// async.cpp
#include <spdlog/async.h>
#include <spdlog/async_logger-inl.h>
#include <spdlog/details/periodic_worker-inl.h>
#include <spdlog/details/thread_pool-inl.h>

// cfg.cpp
#include <spdlog/cfg/helpers-inl.h>

// color_sinks.cpp
#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>
//
// color sinks
//
// #ifdef _WIN32
#include <spdlog/sinks/wincolor_sink-inl.h>
template class SPDLOG_API spdlog::sinks::wincolor_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::wincolor_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::wincolor_stdout_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::wincolor_stdout_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::wincolor_stderr_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::wincolor_stderr_sink<spdlog::details::console_nullmutex>;
// #else
#include <spdlog/sinks/ansicolor_sink-inl.h>
template class SPDLOG_API spdlog::sinks::ansicolor_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stdout_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stdout_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stderr_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::ansicolor_stderr_sink<spdlog::details::console_nullmutex>;
// #endif

// factory methods for color loggers
#include <spdlog/sinks/stdout_color_sinks-inl.h>
template SPDLOG_API std::shared_ptr<spdlog::logger>
spdlog::stdout_color_mt<spdlog::synchronous_factory>(const std::string &logger_name,
                                                     color_mode mode);
template SPDLOG_API std::shared_ptr<spdlog::logger>
spdlog::stdout_color_st<spdlog::synchronous_factory>(const std::string &logger_name,
                                                     color_mode mode);
template SPDLOG_API std::shared_ptr<spdlog::logger>
spdlog::stderr_color_mt<spdlog::synchronous_factory>(const std::string &logger_name,
                                                     color_mode mode);
template SPDLOG_API std::shared_ptr<spdlog::logger>
spdlog::stderr_color_st<spdlog::synchronous_factory>(const std::string &logger_name,
                                                     color_mode mode);

template SPDLOG_API std::shared_ptr<spdlog::logger> spdlog::stdout_color_mt<spdlog::async_factory>(
    const std::string &logger_name, color_mode mode);
template SPDLOG_API std::shared_ptr<spdlog::logger> spdlog::stdout_color_st<spdlog::async_factory>(
    const std::string &logger_name, color_mode mode);
template SPDLOG_API std::shared_ptr<spdlog::logger> spdlog::stderr_color_mt<spdlog::async_factory>(
    const std::string &logger_name, color_mode mode);
template SPDLOG_API std::shared_ptr<spdlog::logger> spdlog::stderr_color_st<spdlog::async_factory>(
    const std::string &logger_name, color_mode mode);

// file_sinks.cpp
#include <spdlog/details/file_helper-inl.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink-inl.h>
#include <spdlog/sinks/basic_file_sink-inl.h>

#include <mutex>

template class SPDLOG_API spdlog::sinks::basic_file_sink<std::mutex>;
template class SPDLOG_API spdlog::sinks::basic_file_sink<spdlog::details::null_mutex>;

#include <spdlog/sinks/rotating_file_sink-inl.h>
template class SPDLOG_API spdlog::sinks::rotating_file_sink<std::mutex>;
template class SPDLOG_API spdlog::sinks::rotating_file_sink<spdlog::details::null_mutex>;

// spdlog.cpp
#include <spdlog/common-inl.h>
#include <spdlog/details/backtracer-inl.h>
#include <spdlog/details/log_msg-inl.h>
#include <spdlog/details/log_msg_buffer-inl.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/os-inl.h>
#include <spdlog/details/registry-inl.h>
#include <spdlog/logger-inl.h>
#include <spdlog/pattern_formatter-inl.h>
#include <spdlog/sinks/base_sink-inl.h>
#include <spdlog/sinks/sink-inl.h>
#include <spdlog/spdlog-inl.h>

// template instantiate logger constructor with sinks init list
template SPDLOG_API spdlog::logger::logger(std::string name,
                                           sinks_init_list::iterator begin,
                                           sinks_init_list::iterator end);
template class SPDLOG_API spdlog::sinks::base_sink<std::mutex>;
template class SPDLOG_API spdlog::sinks::base_sink<spdlog::details::null_mutex>;

// stdout_sinks.cpp
#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/stdout_sinks-inl.h>

template class SPDLOG_API spdlog::sinks::stdout_sink_base<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::stdout_sink_base<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::stdout_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::stdout_sink<spdlog::details::console_nullmutex>;
template class SPDLOG_API spdlog::sinks::stderr_sink<spdlog::details::console_mutex>;
template class SPDLOG_API spdlog::sinks::stderr_sink<spdlog::details::console_nullmutex>;

template SPDLOG_API std::shared_ptr<spdlog::logger>
spdlog::stdout_logger_mt<spdlog::synchronous_factory>(const std::string &logger_name);
template SPDLOG_API std::shared_ptr<spdlog::logger>
spdlog::stdout_logger_st<spdlog::synchronous_factory>(const std::string &logger_name);
template SPDLOG_API std::shared_ptr<spdlog::logger>
spdlog::stderr_logger_mt<spdlog::synchronous_factory>(const std::string &logger_name);
template SPDLOG_API std::shared_ptr<spdlog::logger>
spdlog::stderr_logger_st<spdlog::synchronous_factory>(const std::string &logger_name);

template SPDLOG_API std::shared_ptr<spdlog::logger> spdlog::stdout_logger_mt<spdlog::async_factory>(
    const std::string &logger_name);
template SPDLOG_API std::shared_ptr<spdlog::logger> spdlog::stdout_logger_st<spdlog::async_factory>(
    const std::string &logger_name);
template SPDLOG_API std::shared_ptr<spdlog::logger> spdlog::stderr_logger_mt<spdlog::async_factory>(
    const std::string &logger_name);
template SPDLOG_API std::shared_ptr<spdlog::logger> spdlog::stderr_logger_st<spdlog::async_factory>(
    const std::string &logger_name);