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
#include <QThread>
#include <chrono>
#include <csignal>


namespace TheCalculater::dbgutil {
    int currentSignal = 0;

    std::string currentISO8601TimeUTC()
    {
        auto now = std::chrono::system_clock::now();
        auto itt = std::chrono::system_clock::to_time_t(now);

        std::ostringstream ss;
        ss << std::put_time(gmtime(&itt), "%FT%TZ");
        return ss.str();
    }
    void customTerminateHandler()
    {
        // TODO: 把terminate信息更细节
        std::string time = currentISO8601TimeUTC();
        SPDLOG_CRITICAL("Program Terminated!");
        spdlog::default_logger()->set_pattern("%v");

        spdlog::info("---------- TheCalculater Terminate Report ----------");
        spdlog::info("");
        spdlog::info("Time: {}", time);
        spdlog::info("Process ID: {}, Thread ID: {}", QCoreApplication::applicationPid(), QThread::currentThreadId());
        spdlog::info("Version: {}", THECALCULATER_VERSION_ALL);
        spdlog::info("");

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

        // std::signal(SIG)
    }
} // namespace TheCalculater::dbgutil