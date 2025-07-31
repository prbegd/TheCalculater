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
#include "TheCalculater/util.hpp"
#include "config.h"
#include "spdlog/details/os.h"
#include "spdlog/spdlog.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QLocale>
#include <QSysInfo>
#include <boost/core/demangle.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <sstream>
#include <typeinfo>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace TheCalculater::dbgutil {
    std::unique_ptr<std::vector<std::string_view>> g_programArgs = nullptr;

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
    bool startDetachedProcess(std::string_view programPath, const std::vector<std::string_view>& args)
    {
#ifdef _WIN32
        std::string cmd(programPath);
        for (const auto& arg : args) {
            cmd += " \"" + std::string(arg) + "\"";
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
                return "";
            try {
                std::rethrow_exception(exception);
            } catch (const std::exception& e) {
                std::ostringstream oss;
                std::string type = boost::core::demangle(typeid(e).name());
                const boost::stacktrace::stacktrace* st = boost::get_error_info<util::traced>(e);
                if (st) {
                    // the type name of traced exception is boost::exception_detail::error_info_injector<T>
                    // we need to get the template type name to make it more readable
                    std::string realType;
                    size_t templateStart = type.find_first_of('<');
                    size_t templateEnd = type.find_last_of('>');
                    if (templateStart != std::string::npos && templateEnd != std::string::npos)
                        realType = type.substr(templateStart + 1, templateEnd - templateStart - 1);
                    else
                        realType = type;

                    oss << realType << ": " << e.what() << "\n"
                        << formatStacktrace(*st);
                } else {
                    oss << type << ": " << e.what() << "\n";
                }
                return oss.str();
            } catch (const boost::exception& e) {
                std::string type = boost::core::demangle(typeid(e).name());
                const boost::stacktrace::stacktrace* st = boost::get_error_info<util::traced>(e);
                if (st)
                    return type + "\n" + formatStacktrace(*st);
                else
                    return type + "\n";
            } catch (...) {
                return "UNKNOWN EXCEPTION";
            }
        }
        /// @param signalName signal name that caused the crash, empty if it's not a signal
        /// @return crash report file name
        std::string logCrash(std::string_view signalName = "") noexcept
        {
            try {
                std::string fileName = std::format("log/crash_{}.log", QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss").toStdString());

                static auto logger = spdlog::basic_logger_mt("crash_logger", fileName, true);
                logger->flush_on(spdlog::level::critical);
                logger->set_pattern("%v");

                logger->critical("----- TheCalculater Crash Report -----\n");
                logger->critical("Time: {}", QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString());
                // Use spdlog::details::os::thread_id() to keep consistent with the log
                logger->critical("Process ID: {}, Thread ID: {}", QCoreApplication::applicationPid(), spdlog::details::os::thread_id());
                logger->critical("Version: {}, Build Number: {}, Build Type: {}", THECALCULATER_VERSION_ALL, THECALCULATER_BUILD, THECALCULATER_BUILD_TYPE);
                logger->critical("Compiler: " THECALCULATER_COMPILER "\n");

                if (!signalName.empty()) {
                    logger->critical("Signal: {}", signalName);
                } else {
                    std::string exception_info = std::move(collectExceptionInfo());
                    if (!exception_info.empty())
                        logger->critical("Exception:\n{}", exception_info);
                    else
                        logger->critical("Unknown Termination Cause");
                }

                // Because using spdlog is inherently async signal unsafe,
                // so we can capture the stacktrace here (
                try {
                    const auto stacktrace = formatStacktrace(boost::stacktrace::stacktrace());
                    logger->critical("Stacktrace:\n{}", stacktrace);
                } catch (...) {
                    logger->critical("Stacktrace: Unable to capture stacktrace\n");
                }

                logger->critical("OS: {}", QSysInfo::prettyProductName().toStdString());
                logger->critical("CPU Architecture: {}", QSysInfo::currentCpuArchitecture().toStdString());
                logger->critical("System Locale: {}", QLocale::system().name().toStdString());

                return fileName;
            } catch (const std::exception& e) {
                try {
                    SPDLOG_ERROR("EXCEPTION WHILE LOGGING CRASH REPORT! what: {}", e.what());
                } catch (...) {
                    SPDLOG_ERROR("EXCEPTION WHILE LOGGING CRASH REPORT!");
                }
                _exit(1);
            } catch (...) {
                SPDLOG_ERROR("ERROR WHILE LOGGING CRASH REPORT!");
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
            SPDLOG_CRITICAL("Program crashed! Crash report saved to: {}", crashReportFile);

            std::vector<std::string_view> args = *g_programArgs;
            args.insert(args.begin(), crashReportFile);
            startDetachedProcess(std::filesystem::current_path().string() + "/CrashHandler", args);
            SPDLOG_INFO("Crash handler launched.");
            
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
                // This is kinda a hack... But it works...
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
    void init(int argc, char* argv[])
    {
        g_programArgs = std::make_unique<std::vector<std::string_view>>(argv + 1, argv + argc);

        std::set_terminate(terminateHandler);

        signal(SIGSEGV, signalHandler);
        signal(SIGFPE, signalHandler);
        signal(SIGILL, signalHandler);
        signal(SIGABRT, signalHandler);

#ifdef WIN32
        initJob();
#endif
    }
} // namespace TheCalculater::dbgutil