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
#include <unordered_map>

namespace TheCalculater::dbgutil {
    static int currentSignal = 0;
    static std::atomic<bool> terminateHandlerCalled(false);
    sig_atomic_t tyhing;

    static const std::unordered_map<int, std::string> SIGNAL_STRINGS = {
        { SIGABRT, "SIGABRT (Abort)" },
        { SIGFPE, "SIGFPE (Floating-point exception)" },
        { SIGILL, "SIGILL (Illegal instruction)" },
        { SIGINT, "SIGINT (Interrupt)" },
        { SIGSEGV, "SIGSEGV (Segmentation Fault~)" }, // :)
        { SIGTERM, "SIGTERM (Termination)" }
    };

    static std::string signal2str(int signal)
    {
        auto it = SIGNAL_STRINGS.find(signal);
        return it != SIGNAL_STRINGS.end()
            ? it->second
            : "UNKNOWN SIGNAL: " + std::to_string(signal);
    }

    std::string currentISO8601TimeUTC()
    {
        auto now = std::chrono::system_clock::now();
        auto itt = std::chrono::system_clock::to_time_t(now);

        std::ostringstream oss;
        struct tm tm_buf;
#if defined(_WIN32)
        gmtime_s(&tm_buf, &itt);
#else
        gmtime_r(&itt, &tm_buf);
#endif
        oss << std::put_time(&tm_buf, "%FT%TZ");
        return oss.str();
    }

    static void collectExceptionInfo(std::string& info)
    {
        auto exception = std::current_exception();
        if (!exception)
            return;

        try {
            std::rethrow_exception(exception);
        } catch (const std::exception& e) {
            info = std::string(typeid(e).name()) + ": " + e.what();
        } catch (...) {
            info = "UNKNOWN EXCEPTION";
        }
    }

    void customTerminateHandler()
    {
        if (terminateHandlerCalled.exchange(true))
            return;

        SPDLOG_CRITICAL("Program Terminated! Collecting crash information...");
        std::string time = currentISO8601TimeUTC();

        std::string exception_info;
        collectExceptionInfo(exception_info);

        const auto stacktrace = formatStacktrace(boost::stacktrace::stacktrace());
        auto* const threadId = QThread::currentThreadId();
        const auto pid = QCoreApplication::applicationPid();

        std::ostringstream report;
        report << "\n---------- TheCalculater Crash Report ----------\n\n"
               << "Time: " << time << "\n"
               << "Process ID: " << pid << ", Thread ID: " << threadId << "\n"
               << "Version: " << THECALCULATER_VERSION_ALL << "\n\n";

        if (currentSignal != 0) {
            report << "Signal: " << signal2str(currentSignal) << "\n";
        }

        if (!exception_info.empty()) {
            report << "Exception: " << exception_info << "\n";
        } else if (currentSignal == 0) {
            report << "Termination Cause: Unknown (possibly std::terminate() called directly)\n";
        }
        report << "\n";

        report << "StackTrace:\n"
               << stacktrace << "\n";
        report << "System Info:\n"
               << "OS: " << QSysInfo::prettyProductName().toStdString() << "\n"
               << "CPU Architecture: " << QSysInfo::currentCpuArchitecture().toStdString() << "\n"
               << "---------------------------------------------\n";

        spdlog::default_logger()->set_pattern("%v");
        SPDLOG_CRITICAL(report.str());
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
        SPDLOG_DEBUG("Initializing debug utility...");
        std::set_terminate(customTerminateHandler);

        std::signal(SIGINT, [](int signal) {
            currentSignal = signal;
            SPDLOG_INFO("Received Interrupt Signal");
            spdlog::default_logger()->flush();
            qApp->quit();
        });
        std::signal(SIGTERM, [](int signal) {
            currentSignal = signal;
            SPDLOG_INFO("Received Termination Signal");
            qApp->quit();
        });
        std::signal(SIGSEGV, [](int signal) {
            currentSignal = signal;
            SPDLOG_CRITICAL("Segmentation Fault");
            std::terminate();
        });
        std::signal(SIGABRT, [](int signal) {
            currentSignal = signal;
            if (!terminateHandlerCalled) {
                SPDLOG_CRITICAL("Abort Signal");
                std::terminate();
            }
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