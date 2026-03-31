/**
 * @file debugging.cppm
 * @author prbegd
 * @brief Some debug utilities.
 * @date 2025-05-25
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
module;
#include "config.h"
#include "spdlog/details/os.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QLocale>
#include <QSysInfo>
#include <boost/core/demangle.hpp>
#include <boost/stacktrace/stacktrace.hpp>


#ifdef _WIN32
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
#endif

export module TheCalculater.debugging;
import TheCalculater.util;
import std.compat;

// This code below is just shit. I have no idea how to make it better.
// Be careful.

namespace TheCalculater::debugging {
    std::unique_ptr<std::vector<std::string_view>> g_programArgs = nullptr;

    bool startDetachedProcess(std::string_view programPath, const std::vector<std::string_view>& args)
    {
#ifdef _WIN32
        std::string cmd(programPath);
        for (const auto& arg : args) {
            cmd += " \"" + std::string(arg) + '"';
        }

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        BOOL res = CreateProcessA(
            nullptr,
            cmd.data(),
            nullptr,
            nullptr,
            FALSE,
            DETACHED_PROCESS | CREATE_BREAKAWAY_FROM_JOB,
            nullptr,
            nullptr,
            &si,
            &pi);
        if (!res) {
            SPDLOG_ERROR("Failed to create process: {}", GetLastError());
            return false;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
#else
        std::vector<std::string> argStorage;
        argStorage.reserve(args.size() + 1);

        argStorage.emplace_back(programPath);

        for (auto& arg : args) {
            argStorage.emplace_back(arg);
        }

        std::vector<char*> argv;
        for (auto& str : argStorage) {
            argv.push_back(str.data());
        }
        argv.push_back(nullptr);

        pid_t pid = fork();
        if (pid < 0) {
            SPDLOG_ERROR("fork failed: {}", strerror(errno));
            return false;
        }

        if (pid > 0) {
            return true;
        }

        if (setsid() < 0) {
            std::cerr << "setsid failed: " << strerror(errno) << std::endl;
            _exit(EXIT_FAILURE);
        }

        execvp(argv[0], argv.data());

        std::cerr << "execvp failed for " << argv[0]
                  << ": " << strerror(errno) << std::endl;
        _exit(EXIT_FAILURE);
#endif
    }
    namespace {
        std::atomic<bool> crashed(false);

        std::string collectExceptionInfo()
        {
            auto exception = std::current_exception();
            if (!exception)
                return { };
            try {
                std::rethrow_exception(exception);
            } catch (const std::exception& e) {
                return util::formatException(e);
            } catch (...) {
                return "UNKNOWN EXCEPTION";
            }
        }
        /// @param signalName signal name that caused the crash, empty if it's not a signal
        /// @return crash report file name
        std::string logCrash(std::string_view signalName = { }) noexcept
        {
            try {
                std::string fileName = std::format("log/crash_{}.log", QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss").toStdString());

                std::ofstream ofs(fileName, std::ios::out | std::ios::trunc);

                ofs << "----- TheCalculater Crash Report -----\n"
                    << "Time: " << QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString() << '\n'
                    << "Process ID: " << QCoreApplication::applicationPid() << ", Thread ID: " << spdlog::details::os::thread_id() << '\n'
                    << "Version: " << THECALCULATER_VERSION_ALL << ", Build Number: " << THECALCULATER_BUILD << ", Build Type: " << THECALCULATER_BUILD_TYPE << '\n'
                    << "Compiler: " THECALCULATER_COMPILER "\n"
                    << '\n';

                if (!signalName.empty()) {
                    ofs << "Signal: " << signalName << '\n';
                } else {
                    std::string exception_info = collectExceptionInfo();
                    if (!exception_info.empty())
                        ofs << "Exception:\n"
                            << exception_info << "\n";
                    else
                        ofs << "Unknown Termination Cause\n";
                }
                ofs << '\n';

                try {
                    const auto stacktrace = util::formatStacktrace();
                    ofs << "Stacktrace:\n"
                        << stacktrace;
                } catch (...) {
                    ofs << "Stacktrace: Unable to capture stacktrace\n";
                }
                ofs << '\n';

                ofs << "OS: " << QSysInfo::prettyProductName().toStdString() << '\n'
                    << "CPU Architecture: " << QSysInfo::currentCpuArchitecture().toStdString() << '\n'
                    << "System Locale: " << QLocale::system().name().toStdString() << '\n';

                ofs.close();

                return fileName;
            } catch (...) {
                _exit(1);
            }
        }

        void signalHandler(int signal)
        {
            if (crashed.exchange(true))
                return;

            std::string sigName;
            switch (signal) {
            case SIGSEGV:
                sigName = "SIGSEGV (Segmentation fault)";
                break;
            case SIGFPE:
                sigName = "SIGFPE (Floating point exception)";
                break;
            case SIGILL:
                sigName = "SIGILL (Illegal instruction)";
                break;
            case SIGABRT:
                sigName = "SIGABRT (Abort signal)";
                break;
            default:
                sigName = "UNKNOWN (" + std::to_string(signal) + ")";
                break;
            }
            const auto crashReportFile = logCrash(sigName);

            std::vector<std::string_view> args = *g_programArgs;
            args.insert(args.begin(), crashReportFile);
            startDetachedProcess(std::filesystem::current_path().string() + "/CrashHandler", args);

            _exit(1);
        }
        void terminateHandler()
        {
            if (crashed.exchange(true))
                return;

            const auto crashReportFile = logCrash();

            std::vector<std::string_view> args = *g_programArgs;
            args.insert(args.begin(), crashReportFile);
            startDetachedProcess(std::filesystem::current_path().string() + "/CrashHandler", args);

            spdlog::shutdown();
            _exit(1);
        }

#ifdef _WIN32
        void initJob()
        {
            // I just don't want the fricking vscode debugger to terminate
            // the child process after parent process exits!
            // (You have no idea how much I've been messing with this thing all day...)
            BOOL isInJob = FALSE;
            IsProcessInJob(GetCurrentProcess(), nullptr, &isInJob);
            if (!isInJob)
                return;

            HANDLE hJob = OpenJobObjectA(
                JOB_OBJECT_QUERY | JOB_OBJECT_SET_ATTRIBUTES,
                FALSE,
                R"(Local\Gdb-Wrapper)");
            if (hJob) {
                JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = { 0 };
                info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_BREAKAWAY_OK;
                if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &info, sizeof(info)))
                    SPDLOG_WARN("Unable to set limit to job object! Errno {}", GetLastError());
            } else
                SPDLOG_WARN("Unable to open job object! Errno {}", GetLastError());
        }
#endif
    } // namespace
    export void init(int argc, char* argv[])
    {
        g_programArgs = std::make_unique<std::vector<std::string_view>>(argv + 1, argv + argc);

        std::set_terminate(terminateHandler);

        (void)signal(SIGSEGV, signalHandler);
        (void)signal(SIGFPE, signalHandler);
        (void)signal(SIGILL, signalHandler);
        (void)signal(SIGABRT, signalHandler);

#ifdef WIN32
        initJob();
#endif
    }
} // namespace TheCalculater::debugging