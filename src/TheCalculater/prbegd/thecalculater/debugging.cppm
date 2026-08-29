/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
module;
#include "config.h"
#include "thecalculater/macros.hpp"
#include "thecalculater/cleanup.hpp"

export module prbegd.thecalculater.debugging;
import prbegd.thecalculater.util;
import thirdparty.core;
import thirdparty.extra;
import std;
namespace thecalculater::debugging {
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

    posixapi::pid_t pid = posixapi::fork();
    if (pid < 0) {
        spdlog::error("fork failed: {}", std::strerror(cstd::_errno));
        return false;
    }

    if (pid > 0) {
        return true;
    }

    if (posixapi::setsid() < 0) {
        std::cerr << "setsid failed: " << std::strerror(cstd::_errno) << std::endl;
        std::_Exit(1);
    }

    posixapi::execvp(argv[0], argv.data());

    std::cerr << "execvp failed for " << argv[0]
              << ": " << std::strerror(cstd::_errno) << std::endl;
    std::_Exit(1);
#endif
}
export enum class UnhandledExceptionType : std::uint8_t {
    TopLevelUnhandled,
    TerminateCaught,
};
namespace {
    std::atomic<bool> crashed(false);
    // REFACTOR(P3): Use {fmt} format functions instead of std::format
    [[noreturn]]
    void finalizeCrash(const std::string& crashReportFilePath)
    {
        // FIXME(P3): This is somehow dangerous because we may modify g_programCliArg
        std::vector<std::string_view> args = *g_programCliArg;
        args.insert(args.begin(), crashReportFilePath);
        startDetachedProcess(std::filesystem::current_path().string() + "/CrashHandler", args);

        spdlog::shutdown();
        std::_Exit(1);
    }
    std::string logCrash(const std::function<void(std::ostream&)>& witness) noexcept
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

            witness(ofs);
            ofs << '\n';

            try {
                ofs << "Stacktrace:\n";
                util::printStacktrace(ofs, boost::stacktrace::stacktrace(0, -1));
                ofs << '\n';
            } catch (...) {
                ofs << "Unable to capture stacktrace.\n";
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
        if (exceptionPointers->ExceptionRecord->ExceptionCode == 0x20474343 /* ' GCC' */) {
            return winapi::_EXCEPTION_CONTINUE_SEARCH;
        }
        std::string exception;
        switch (exceptionPointers->ExceptionRecord->ExceptionCode) {
        case winapi::_EXCEPTION_ACCESS_VIOLATION:
            exception = "Structured Exception: EXCEPTION_ACCESS_VIOLATION (Access violation)";
            if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 0) {
                exception += "\n    Attempted to read inaccessible memory.";
            } else if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 1) {
                exception += "\n    Attempted to write to inaccessible address.";
            } else if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 8) {
                exception += "\n    User-mode data execution prevention (DEP) violation.";
            }
            exception += std::format("\n    Faulting address: 0x{:x}", exceptionPointers->ExceptionRecord->ExceptionInformation[1]);
            if (exceptionPointers->ExceptionRecord->ExceptionInformation[1] == 0) {
                exception += " (null pointer)";
            }
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
            if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 0) {
                exception += "\n    Attempted to read inaccessible memory.";
            } else if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 1) {
                exception += "\n    Attempted to write to inaccessible address.";
            } else if (exceptionPointers->ExceptionRecord->ExceptionInformation[0] == 8) {
                exception += "\n    User-mode data execution prevention (DEP) violation.";
            }
            exception += std::format("\n    Faulting address: 0x{:x}", exceptionPointers->ExceptionRecord->ExceptionInformation[1]);
            if (exceptionPointers->ExceptionRecord->ExceptionInformation[1] == 0) {
                exception += " (null pointer)";
            }
            exception += std::format("\n    NTSTATUS code: 0x{:x}", exceptionPointers->ExceptionRecord->ExceptionInformation[2]);
            break;
        case winapi::_EXCEPTION_ILLEGAL_INSTRUCTION:
            exception = "Structured Exception: EXCEPTION_ILLEGAL_INSTRUCTION (Illegal instruction)";
            break;
        case winapi::_EXCEPTION_NONCONTINUABLE_EXCEPTION:
            exception = "Structured Exception: EXCEPTION_NONCONTINUABLE_EXCEPTION (Non-continuable exception)";
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
        if (crashed.exchange(true)) {
            return 1;
        }
        exception += std::format("\nAt address: 0x{:x}", reinterpret_cast<std::uintptr_t>(exceptionPointers->ExceptionRecord->ExceptionAddress));

        finalizeCrash(logCrash([&exception](std::ostream& os) {
            os << exception << '\n';
        }));
    }
#else
    void signalHandler(int signal)
    {
        if (crashed.exchange(true)) {
            return;
        }

        finalizeCrash(logCrash([signal](std::ostream& os) {
            switch (signal) {
            case cstd::_SIGSEGV:
                os << "Signal: SIGSEGV (Segmentation fault)\n";
                break;
            case cstd::_SIGFPE:
                os << "Signal: SIGFPE (Floating point exception)\n";
                break;
            case cstd::_SIGILL:
                os << "Signal: SIGILL (Illegal instruction)\n";
                break;
            case cstd::_SIGABRT:
                os << "Signal: SIGABRT (Abort signal)\n";
                break;
            default:
                os << "Signal: UNKNOWN (" + std::to_string(signal) + ")\n";
                break;
            }
        }));
    }
#endif
    void terminateHandler()
    {
        if (crashed.exchange(true)) {
            return;
        }
        if (std::exception_ptr exception = std::current_exception(); exception) {
            try {
                std::rethrow_exception(exception);
            } catch (const std::exception& e) {
                finalizeCrash(logCrash([&e](std::ostream& os) {
                    os << "std::terminate with Exception: \n";
                    util::printException(os, e);
                    os << '\n';
                }));
            } catch (...) {
                finalizeCrash(logCrash([](std::ostream& os) {
                    os << "std::terminate with Non-std::exception.\n";
                }));
            }
        }

        finalizeCrash(logCrash([](std::ostream& os) {
            os << "std::terminate Straight Call.\n";
        }));
    }

#ifdef THECALCULATER_WINDOWS
    void initJob()
    {
        // I just don't want the fricking vscode debugger to terminate
        // the child process after parent process exits!
        // (You have no idea how much I've been messing with this thing all day...)
        winapi::BOOL isInJob = winapi::_FALSE;
        winapi::IsProcessInJob(winapi::GetCurrentProcess(), nullptr, &isInJob);
        if (!isInJob) {
            return;
        }

        winapi::HANDLE hJob = winapi::OpenJobObjectA(
            winapi::_JOB_OBJECT_QUERY | winapi::_JOB_OBJECT_SET_ATTRIBUTES,
            winapi::_FALSE,
            R"(Local\Gdb-Wrapper)");
        if (hJob) {
            winapi::JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = { };
            info.BasicLimitInformation.LimitFlags = winapi::_JOB_OBJECT_LIMIT_BREAKAWAY_OK;
            if (!winapi::SetInformationJobObject(hJob, winapi::JOBOBJECTINFOCLASS::JobObjectExtendedLimitInformation, &info, sizeof(info))) {
                spdlog::warn("Unable to set limit to job object! Errno {}", winapi::GetLastError());
            }
        }
    }
#endif
} // namespace
export void unhandledException(std::exception_ptr exception)
{
    try {
        std::rethrow_exception(exception);
    } catch (const std::exception& e) {
        finalizeCrash(logCrash([&e](std::ostream& os) {
            os << "Unhandled Exception: \n";
            util::printException(os, e);
            os << "\n";
        }));
    } catch (...) {
        finalizeCrash(logCrash([](std::ostream& os) {
            os << "Unhandled Non-std::exception.\n";
        }));
    }
}
export void init(int argc, char* argv[])
{
    // XXX(P2): Some of these things and thread name are thread-isolated and need to be set individually.
    g_programCliArg = std::make_unique<std::vector<std::string_view>>(argv + 1, argv + argc);

    std::set_terminate(terminateHandler);

#ifdef WIN32
    initJob();
    winapi::ULONG emergencyStackSize = 64 * 1024;
    if (!winapi::SetThreadStackGuarantee(&emergencyStackSize)) {
        spdlog::warn("Failed to set emergency stack. Errno {}", winapi::GetLastError());
    }
    winapi::AddVectoredExceptionHandler(1, structuredExceptionHandler);
#else
    (void)std::signal(cstd::_SIGSEGV, signalHandler);
    (void)std::signal(cstd::_SIGFPE, signalHandler);
    (void)std::signal(cstd::_SIGILL, signalHandler);
    (void)std::signal(cstd::_SIGABRT, signalHandler);
#endif
}
} // namespace thecalculater::debugging