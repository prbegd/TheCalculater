/**
 * @file dbgutil.cpp
 * @author prbegd
 * @brief Some debug utilities.
 * @date 2025-05-25
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/dbgutil.hpp"
#include "TheCalculater/appdef.hpp"
#include "spdlog/spdlog.h"
#include <QCoreApplication>
#include <QSysInfo>
#include <QThread>
#include <boost/stacktrace/safe_dump_to.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <chrono>
#include <csignal>
#include <exception>
#include <sstream>
#include <typeinfo>

namespace TheCalculater::dbgutil {
    static int currentSignal = 0;
    // 防止多次调用terminateHandler
    static bool terminateHandlerCalled = false;

    static std::string signal2str(int signal)
    {
        switch (signal) {
        case SIGABRT:
            return "SIGABRT (Abort)";
        case SIGFPE:
            return "SIGFPE (Floating-point exception)";
        case SIGILL:
            return "SIGILL (Illegal instruction)";
        case SIGINT:
            return "SIGINT (Interrupt)";
        case SIGSEGV:
            return "SIGSEGV (Segmentation Fault~)"; // :)
        case SIGTERM:
            return "SIGTERM (Termination)";
        default:
            return "UNKNOWN SIGNAL: " + std::to_string(signal);
        }
    }

    std::string currentISO8601TimeUTC()
    {
        auto now = std::chrono::system_clock::now();
        auto itt = std::chrono::system_clock::to_time_t(now);

        std::ostringstream oss;
        oss << std::put_time(gmtime(&itt), "%FT%TZ");
        return oss.str();
    }
    void customTerminateHandler()
    {
        // TODO: 把terminate信息更细节
        std::string time = currentISO8601TimeUTC();
        terminateHandlerCalled = true;
        SPDLOG_CRITICAL("Program Terminated!Collecting crash information...");
        SPDLOG_DEBUG("Collecting exception information...");
        std::string exception_info;
        std::string exception_stacktrace;
        {
            std::exception_ptr exception = std::current_exception();
            try {
                if (exception) {
                    std::rethrow_exception(exception);
                }
            } catch (const std::exception& e) {
                exception_info = std::string(typeid(e).name()) + ": " + e.what();
                exception_stacktrace = formatStacktrace(boost::stacktrace::stacktrace::from_current_exception());
            } catch (...) {
                exception_info = "UNKNOWN EXCEPTION";
                exception_stacktrace = formatStacktrace(boost::stacktrace::stacktrace::from_current_exception());
            }
        }
        std::string stacktrace = formatStacktrace(boost::stacktrace::stacktrace());

        spdlog::default_logger()->set_pattern("%v");
        spdlog::info("---------- TheCalculater Crash Report ----------");
        spdlog::info("");

        spdlog::info("Time: {}", time);
        spdlog::info("Process ID: {}, Thread ID: {}", QCoreApplication::applicationPid(), QThread::currentThreadId());
        spdlog::info("Version: {}", THECALCULATER_VERSION_ALL);
        spdlog::info("");

        if (currentSignal != 0)
            spdlog::info("Signal: {}", signal2str(currentSignal));
        ;
        if (!exception_info.empty())
            spdlog::info("Exception: {}", exception_info);
        if (!exception_stacktrace.empty())
            spdlog::info("Exception StackTrace:\n{}", exception_stacktrace);
        spdlog::info("");

        spdlog::info("StackTrace:\n{}", stacktrace);
        spdlog::info("");

        spdlog::info("OS: {}", QSysInfo::prettyProductName());
        spdlog::info("CPU Architecture: {}", QSysInfo::currentCpuArchitecture());

        spdlog::shutdown();
        std::abort();
    }

    std::string formatStacktrace(const boost::stacktrace::stacktrace& stk)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < stk.size(); i++) {
            if (stk[i].empty())
                continue;
            oss << "  #" << i << " " << stk[i].name();
            if (stk[i].source_line() != 0) {
                oss << " at " << stk[i].source_file() << ":" << stk[i].source_line();
            }
            oss << " (" << stk[i].address() << ")\n";
        }
        return oss.str();
    }

    void init()
    {
        std::set_terminate(customTerminateHandler);

        std::signal(SIGINT, [](int signal) {
            currentSignal = signal;
            SPDLOG_INFO("Received Interrupt Signal...");
            qApp->quit();
        });
        std::signal(SIGTERM, [](int signal) {
            currentSignal = signal;
            SPDLOG_INFO("Received Termination Request(Signal)...");
            qApp->quit();
        });
        std::signal(SIGSEGV, [](int signal) {
            currentSignal = signal;
            SPDLOG_CRITICAL("Segmentation Fault");
            std::terminate();
        });
        std::signal(SIGABRT, [](int signal) {
            currentSignal = signal;
            SPDLOG_CRITICAL("Abort Signal Received");
            if (!terminateHandlerCalled)
                std::terminate();
        });
        std::signal(SIGFPE, [](int signal) {
            currentSignal = signal;
            SPDLOG_CRITICAL("Floating-point Exception");
            std::terminate();
        });
        std::signal(SIGILL, [](int signal) {
            currentSignal = signal;
            SPDLOG_CRITICAL("Illegal Instruction");
            std::terminate();
        });
    }
} // namespace TheCalculater::dbgutil