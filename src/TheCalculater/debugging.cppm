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
#include "TheCalculater/macros.hpp"
#include "config.h"

export module TheCalculater.debugging;
import TheCalculater.util;
import TheCalculaterQtBridge.qtmock;
import tpmm.spdlog;
import tpmm.winapi;
import tpmm.posixapi;
import tpmm.boost;
import tpmm.cstd;
import std;
namespace TheCalculater::debugging {
    std::unique_ptr<std::vector<std::string_view>> g_programCliArg = nullptr;

    bool startDetachedProcess(std::string_view programPath, const std::vector<std::string_view>& args)
    {
#ifdef THECALCULATER_WINDOWS
        std::string cmd(programPath);
        for (const auto& arg : args) {
            cmd += " \"" + std::string(arg) + '"';
        }

        winapi::STARTUPINFOA si;
        winapi::PROCESS_INFORMATION pi;
        winapi::_ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        winapi::_ZeroMemory(&pi, sizeof(pi));

        winapi::BOOL res = winapi::CreateProcessA(
            nullptr,
            cmd.data(),
            nullptr,
            nullptr,
            winapi::_FALSE,
            winapi::_DETACHED_PROCESS | winapi::_CREATE_BREAKAWAY_FROM_JOB,
            nullptr,
            nullptr,
            &si,
            &pi);
        if (!res) {
            spdlog::error("Failed to create process: {}", winapi::GetLastError());
            return false;
        }
        winapi::CloseHandle(pi.hProcess);
        winapi::CloseHandle(pi.hThread);
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

        pid_t pid = posixapi::fork();
        if (pid < 0) {
            spdlog::error("fork failed: {}", std::strerror(errno));
            return false;
        }

        if (pid > 0) {
            return true;
        }

        if (posixapi::setsid() < 0) {
            std::cerr << "setsid failed: " << std::strerror(errno) << std::endl;
            std::_Exit(1);
        }

        posixapi::execvp(argv[0], argv.data());

        std::cerr << "execvp failed for " << argv[0]
                  << ": " << std::strerror(errno) << std::endl;
        std::_Exit(1);
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
        /// @param nonCppException signal name that caused the crash, empty if it's not a signal
        /// @return crash report file name
        std::string logCrash(std::string_view nonCppException = { }) noexcept
        {
            try {
                std::string fileName = std::format("log/crash_{}.log", QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss").toStdString());

                std::ofstream ofs(fileName, std::ios::out | std::ios::trunc);

                ofs << "----- TheCalculater Crash Report -----\n"
                    << "Time: " << QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString() << '\n'
                    << "Process ID: " << QCoreApplication::applicationPid() << ", Thread ID: " << util::getCurrentThreadId() << '\n'
                    << "Version: " << THECALCULATER_VERSION_ALL << ", Build Number: " << THECALCULATER_BUILD << ", Build Type: " << THECALCULATER_BUILD_TYPE << '\n'
                    << "Compiler: " THECALCULATER_COMPILER "\n"
                    << '\n';

                if (!nonCppException.empty()) {
                    ofs << nonCppException << '\n';
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
                        << stacktrace
                        << '\n';
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
                std::_Exit(1);
            }
        }
#ifdef THECALCULATER_WINDOWS
        winapi::LONG structuredExceptionHandler(winapi::_EXCEPTION_POINTERS* exceptionPointers)
        {
            // If the exception is a C++ exception, return immediately.
            if (exceptionPointers->ExceptionRecord->ExceptionCode == 0x20474343 /* ' GCC' */)
                return winapi::_EXCEPTION_CONTINUE_SEARCH;
            std::string exception;
            switch (exceptionPointers->ExceptionRecord->ExceptionCode) {
            case winapi::_EXCEPTION_ACCESS_VIOLATION:
                exception = "Structured Exception: EXCEPTION_ACCESS_VIOLATION (Access violation)";
                if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 0)
                    exception += "\n    Attempted to read inaccessible memory.";
                else if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 1)
                    exception += "\n    Attempted to write to inaccessible address.";
                else if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 8)
                    exception += "\n    User-mode data execution prevention (DEP) violation.";
                exception += std::format("\n    Faulting address: 0x{:x}", exceptionPointers->ExceptionRecord->ExceptionInformation[1]);
                if (exceptionPointers->ExceptionRecord->ExceptionInformation[1] == 0) 
                    exception += " (null pointer)";
                break;
            case winapi::_EXCEPTION_DATATYPE_MISALIGNMENT:
                exception = "Structured Exception: EXCEPTION_DATATYPE_MISALIGNMENT (Data type misalignment)";
                break;
            case winapi::_EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                exception = "Structured Exception: EXCEPTION_ARRAY_BOUNDS_EXCEEDED (Array bounds exceeded)";
                break;
            case winapi::_EXCEPTION_FLT_DENORMAL_OPERAND:
                exception = "Structured Exception: EXCEPTION_FLT_DENORMAL_OPERAND (Denormal floating-point operand)";
                break;
            case winapi::_EXCEPTION_FLT_DIVIDE_BY_ZERO:
                exception = "Structured Exception: EXCEPTION_FLT_DIVIDE_BY_ZERO (Floating-point divide by zero)";
                break;
            case winapi::_EXCEPTION_FLT_INEXACT_RESULT:
                exception = "Structured Exception: EXCEPTION_FLT_INEXACT_RESULT (Inexact floating-point result)";
                break;
            case winapi::_EXCEPTION_FLT_INVALID_OPERATION:
                exception = "Structured Exception: EXCEPTION_FLT_INVALID_OPERATION (Invalid floating-point operation)";
                break;
            case winapi::_EXCEPTION_FLT_OVERFLOW:
                exception = "Structured Exception: EXCEPTION_FLT_OVERFLOW (Floating-point overflow)";
                break;
            case winapi::_EXCEPTION_FLT_STACK_CHECK:
                exception = "Structured Exception: EXCEPTION_FLT_STACK_CHECK (Floating-point stack check)";
                break;
            case winapi::_EXCEPTION_FLT_UNDERFLOW:
                exception = "Structured Exception: EXCEPTION_FLT_UNDERFLOW (Floating-point underflow)";
                break;
            case winapi::_EXCEPTION_INT_DIVIDE_BY_ZERO:
                exception = "Structured Exception: EXCEPTION_INT_DIVIDE_BY_ZERO (Integer divide by zero)";
                break;
            case winapi::_EXCEPTION_INT_OVERFLOW:
                exception = "Structured Exception: EXCEPTION_INT_OVERFLOW (Integer overflow)";
                break;
            case winapi::_EXCEPTION_PRIV_INSTRUCTION:
                exception = "Structured Exception: EXCEPTION_PRIV_INSTRUCTION (Privileged instruction)";
                break;
            case winapi::_EXCEPTION_IN_PAGE_ERROR:
                exception = "Structured Exception: EXCEPTION_IN_PAGE_ERROR (In-page error)";
                if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 0)
                    exception += "\n    Attempted to read inaccessible memory.";
                else if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 1)
                    exception += "\n    Attempted to write to inaccessible address.";
                else if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 8)
                    exception += "\n    User-mode data execution prevention (DEP) violation.";
                exception += std::format("\n    Faulting address: 0x{:x}", exceptionPointers->ExceptionRecord->ExceptionInformation[1]);
                if (exceptionPointers->ExceptionRecord->ExceptionInformation[1] == 0) 
                    exception += " (null pointer)";
                exception += std::format("\n    NTSTATUS code: 0x{:x}", exceptionPointers->ExceptionRecord->ExceptionInformation[2]);
                break;
            case winapi::_EXCEPTION_ILLEGAL_INSTRUCTION:
                exception = "Structured Exception: EXCEPTION_ILLEGAL_INSTRUCTION (Illegal instruction)";
                break;
            case winapi::_EXCEPTION_NONCONTINUABLE_EXCEPTION:
                exception = "Structured Exception: EXCEPTION_NONCONTINUABLE_EXCEPTION (Noncontinuable exception)";
                break;
            case winapi::_EXCEPTION_STACK_OVERFLOW:
                exception = "Structured Exception: EXCEPTION_STACK_OVERFLOW (Stack overflow)";
                break;
            case winapi::_EXCEPTION_GUARD_PAGE:
                exception = "Structured Exception: EXCEPTION_GUARD_PAGE (Guard page)";
                break;
            case winapi::_EXCEPTION_INVALID_HANDLE:
                exception = "Structured Exception: EXCEPTION_INVALID_HANDLE (Invalid handle)";
                break;
            case winapi::_EXCEPTION_POSSIBLE_DEADLOCK:
                exception = "Structured Exception: EXCEPTION_POSSIBLE_DEADLOCK (Possible deadlock)";
                break;
            default:
                return winapi::_EXCEPTION_CONTINUE_SEARCH;
            }
            if (crashed.exchange(true))
                return 1;
            exception += std::format("\nAt address: 0x{:x}", reinterpret_cast<std::uintptr_t>(exceptionPointers->ExceptionRecord->ExceptionAddress)); 

            const auto crashReportFile = logCrash(
                exception
            );

            std::vector<std::string_view> args = *g_programCliArg;
            args.insert(args.begin(), crashReportFile);
            startDetachedProcess(std::filesystem::current_path().string() + "/CrashHandler", args);

            std::_Exit(1);
        }
#else
        void signalHandler(int signal)
        {
            if (crashed.exchange(true))
                return;

            std::string sigName;
            switch (signal) {
            case cstd::_SIGSEGV:
                sigName = "Signal: SIGSEGV (Segmentation fault)";
                break;
            case cstd::_SIGFPE:
                sigName = "Signal: SIGFPE (Floating point exception)";
                break;
            case cstd::_SIGILL:
                sigName = "Signal: SIGILL (Illegal instruction)";
                break;
            case cstd::_SIGABRT:
                sigName = "Signal: SIGABRT (Abort signal)";
                break;
            default:
                sigName = "Signal: UNKNOWN (" + std::to_string(signal) + ")";
                break;
            }
            const auto crashReportFile = logCrash(sigName);

            std::vector<std::string_view> args = *g_programCliArg;
            args.insert(args.begin(), crashReportFile);
            startDetachedProcess(std::filesystem::current_path().string() + "/CrashHandler", args);

            std::_Exit(1);
        }
#endif
        void terminateHandler()
        {
            if (crashed.exchange(true))
                return;

            const auto crashReportFile = logCrash();

            std::vector<std::string_view> args = *g_programCliArg;
            args.insert(args.begin(), crashReportFile);
            startDetachedProcess(std::filesystem::current_path().string() + "/CrashHandler", args);

            spdlog::shutdown();
            std::_Exit(1);
        }

#ifdef THECALCULATER_WINDOWS
        void initJob()
        {
            // I just don't want the fricking vscode debugger to terminate
            // the child process after parent process exits!
            // (You have no idea how much I've been messing with this thing all day...)
            winapi::BOOL isInJob = winapi::_FALSE;
            winapi::IsProcessInJob(winapi::GetCurrentProcess(), nullptr, &isInJob);
            if (!isInJob)
                return;

            winapi::HANDLE hJob = winapi::OpenJobObjectA(
                winapi::_JOB_OBJECT_QUERY | winapi::_JOB_OBJECT_SET_ATTRIBUTES,
                winapi::_FALSE,
                R"(Local\Gdb-Wrapper)");
            if (hJob) {
                winapi::JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = { };
                info.BasicLimitInformation.LimitFlags = winapi::_JOB_OBJECT_LIMIT_BREAKAWAY_OK;
                if (!winapi::SetInformationJobObject(hJob, winapi::JOBOBJECTINFOCLASS::JobObjectExtendedLimitInformation, &info, sizeof(info)))
                    spdlog::warn("Unable to set limit to job object! Errno {}", winapi::GetLastError());
            } else
                spdlog::warn("Unable to open job object! Errno {}", winapi::GetLastError());
        }
#endif
    } // namespace
    export void init(int argc, char* argv[])
    {
        g_programCliArg = std::make_unique<std::vector<std::string_view>>(argv + 1, argv + argc);

        std::set_terminate(terminateHandler);

#ifdef WIN32
        initJob();
        winapi::ULONG emergencyStackSize = 64 * 1024;
        if (!winapi::SetThreadStackGuarantee(&emergencyStackSize))
            spdlog::warn("Failed to set emergency stack. Errno {}", winapi::GetLastError());
        winapi::AddVectoredExceptionHandler(1, structuredExceptionHandler);
#else
        (void)std::signal(cstd::_SIGSEGV, signalHandler);
        (void)std::signal(cstd::_SIGFPE, signalHandler);
        (void)std::signal(cstd::_SIGILL, signalHandler);
        (void)std::signal(cstd::_SIGABRT, signalHandler);
#endif
    }
} // namespace TheCalculater::debugging