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
#include "TheCalculater/util.hpp"
#include "boost/core/demangle.hpp"
#include "spdlog/details/os.h"
#include "spdlog/spdlog.h"
#include <QAbstractButton>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QLocale>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSysInfo>
#include <QUrl>
#include <boost/stacktrace/stacktrace.hpp>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <sstream>
#include <typeinfo>



// ? 由于写得太烂，我决心重构整个崩溃处理逻辑。

// namespace TheCalculater::dbgutil {
//     static int currentSignal = 0;
//     static std::atomic<bool> terminateHandlerCalled(false);
//     // static std::unique_ptr<QMessageBox> crashDialog = nullptr;
//     // static QAbstractButton* restartBtn = nullptr;
//     // static QAbstractButton* openLogBtn = nullptr;
//     // static QAbstractButton* openGithubBtn = nullptr;
//
//     static const std::unordered_map<int, std::string> SIGNAL_STRINGS = {
//         { SIGABRT, "SIGABRT (Abort)" },
//         { SIGFPE, "SIGFPE (Floating-point exception)" },
//         { SIGILL, "SIGILL (Illegal instruction)" },
//         { SIGINT, "SIGINT (Interrupt)" },
//         { SIGSEGV, "SIGSEGV (Segmentation Fault~)" }, // :)
//         { SIGTERM, "SIGTERM (Termination)" }
//     };
//
//     static std::string signal2str(int signal)
//     {
//         auto it = SIGNAL_STRINGS.find(signal);
//         return it != SIGNAL_STRINGS.end()
//             ? it->second
//             : "UNKNOWN SIGNAL: " + std::to_string(signal);
//     }
//
//     static void collectExceptionInfo(std::string& info)
//     {
//         auto exception = std::current_exception();
//         if (!exception)
//             return;
//
//         try {
//             std::rethrow_exception(exception);
//         } catch (const std::exception& e) {
//             std::string type = boost::core::demangle(typeid(e).name());
//             info = type + ": " + e.what();
//         } catch (...) {
//             info = "UNKNOWN EXCEPTION";
//         }
//     }
//
//     void customTerminateHandler()
//     {
//         // Prevent recursive calls to terminateHandler.
//         if (terminateHandlerCalled.exchange(true)) {
//             std::abort();
//             return;
//         }
//
//         SPDLOG_CRITICAL("Program Terminated! Collecting crash information...");
//         std::string time = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString();
//
//         std::string exception_info;
//         collectExceptionInfo(exception_info);
//
//         const auto stacktrace = formatStacktrace(boost::stacktrace::stacktrace());
//         auto* const threadId = QThread::currentThreadId();
//         const auto pid = QCoreApplication::applicationPid();
//
//         std::ostringstream report;
//         report << "\n---------- TheCalculater Crash Report ----------\n\n"
//                << "Time: " << time << "\n"
//                << "Process ID: " << pid << ", Thread ID: " << threadId << "\n"
//                << "Version: " << THECALCULATER_VERSION_ALL << "\n"
//                << "Build Number: " << THECALCULATER_BUILD << ", Build Type: " << THECALCULATER_BUILD_TYPE
//                << "\n\n";
//
//         if (currentSignal != 0) {
//             report << "Signal: " << signal2str(currentSignal) << "\n";
//         }
//
//         if (!exception_info.empty()) {
//             report << "Exception: " << exception_info << "\n";
//         } else if (currentSignal == 0) {
//             report << "Unknown Termination Cause (possibly std::terminate() called directly)\n";
//         }
//         report << "\n";
//
//         report << "StackTrace:\n"
//                << stacktrace << "\n";
//         report << "System Info:\n"
//                << "OS: " << QSysInfo::prettyProductName().toStdString() << "\n"
//                << "CPU Architecture: " << QSysInfo::currentCpuArchitecture().toStdString() << "\n"
//                // TODO: Add user setted locale
//                << "System Locale: " << QLocale::system().name().toStdString() << "\n"
//                << "---------------------------------------------\n";
//
//         spdlog::default_logger()->set_pattern("%v");
//         SPDLOG_CRITICAL(report.str());
//         crashDialog->exec();
//         if (crashDialog->clickedButton() == restartBtn) {
//             auto args = QCoreApplication::arguments();
//             args.removeFirst();
//             QProcess::startDetached(QCoreApplication::applicationFilePath(), args, QDir::currentPath());
//         } else if (crashDialog->clickedButton() == openLogBtn) {
//             QDesktopServices::openUrl(QUrl("file:///" + QCoreApplication::applicationDirPath() + "/log/log.log"));
//         } else if (crashDialog->clickedButton() == openGithubBtn) {
//             QDesktopServices::openUrl(QUrl("https://github.com/prbegd/TheCalculater/issues"));
//         }
//
//         spdlog::shutdown();
//         std::abort();
//     }
//
//     std::string formatStacktrace(const boost::stacktrace::stacktrace& stk)
//     {
//         std::ostringstream oss;
//         for (size_t i = 0; i < stk.size(); i++) {
//             if (stk[i].empty())
//                 continue;
//             oss << "  #" << i << " " << stk[i].name();
//             if (stk[i].source_line() != 0) {
//                 oss << " at " << stk[i].source_file() << ":" << stk[i].source_line();
//             }
//             oss << " (" << stk[i].address() << ")\n";
//         }
//         return oss.str();
//     }
//
//     void init()
//     {
//         SPDLOG_TRACE("Initializing debug utility...");
//         std::set_terminate(customTerminateHandler);
//
//         std::signal(SIGINT, [](int signal) {
//             currentSignal = signal;
//             SPDLOG_INFO("Received Interrupt Signal");
//             spdlog::default_logger()->flush();
//             qApp->quit();
//         });
//         std::signal(SIGTERM, [](int signal) {
//             currentSignal = signal;
//             SPDLOG_INFO("Received Termination Signal");
//             qApp->quit();
//         });
//         std::signal(SIGSEGV, [](int signal) {
//             currentSignal = signal;
//             SPDLOG_CRITICAL("Segmentation Fault");
//             std::terminate();
//         });
//         std::signal(SIGABRT, [](int signal) {
//             currentSignal = signal;
//             if (!terminateHandlerCalled) {
//                 SPDLOG_CRITICAL("Abort Signal");
//                 std::terminate();
//             }
//         });
//         std::signal(SIGFPE, [](int signal) {
//             currentSignal = signal;
//             SPDLOG_CRITICAL("Floating-point Exception");
//             std::terminate();
//         });
//         std::signal(SIGILL, [](int signal) {
//             currentSignal = signal;
//             SPDLOG_CRITICAL("Illegal Instruction");
//             std::terminate();
//         });
//
//         SPDLOG_TRACE("Initializing crash dialog...");
//         crashDialog = std::make_unique<QMessageBox>(
//             QMessageBox::Critical,
//             QCoreApplication::translate("CrashDialog", "Oops! TheCalculater Ran Into a Problem"),
//             QCoreApplication::translate("CrashDialog", R"(Oh no! :(
// An unexpected error occurred and TheCalculater needs to close.
// Please restart TheCalculater and try again. If the problem persists, please report this issue on GitHub.
// The crash report has been saved to log/log.log file. Please attach this file when reporting the issue on GitHub.
// Sorry for the inconvenience. We're working on a fix!)"));
//         restartBtn = crashDialog->addButton(QCoreApplication::translate("CrashDialog", "Restart"), QMessageBox::AcceptRole);
//         openLogBtn = crashDialog->addButton(QCoreApplication::translate("CrashDialog", "Open log file"), QMessageBox::ActionRole);
//         openGithubBtn = crashDialog->addButton(QCoreApplication::translate("CrashDialog", "Open Github Issues"), QMessageBox::HelpRole);
//     }
// } // namespace TheCalculater::dbgutil

namespace TheCalculater::dbgutil {
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
                    std::string realType;
                    size_t templateStart = type.find_last_of('<');
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
                else return type + "\n";
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
                logger->critical("Version: {}, Build Number: {}, Build Type: {}\n", THECALCULATER_VERSION_ALL, THECALCULATER_BUILD, THECALCULATER_BUILD_TYPE);

                if (!signalName.empty()) {
                    logger->critical("Signal: {}", signalName);
                } else {
                    std::string exception_info = std::move(collectExceptionInfo());
                    if (!exception_info.empty())
                        logger->critical("Exception:\n{}", exception_info);
                    else
                        logger->critical("Unknown Termination Cause");
                }

                if (signalName.empty()) {
                    try {
                        const auto stacktrace = formatStacktrace(boost::stacktrace::stacktrace());
                        logger->critical("Stacktrace:\n{}", stacktrace);
                    } catch (...) {
                        logger->critical("Stacktrace: Unable to capture stacktrace\n");
                    }
                }

                logger->critical("OS: {}", QSysInfo::prettyProductName().toStdString());
                logger->critical("CPU Architecture: {}", QSysInfo::currentCpuArchitecture().toStdString());
                logger->critical("System Locale: {}", QLocale::system().name().toStdString());

                logger->critical("--------------------------------------");

                // FILE* file = fopen(fileName.c_str(), "w");
                // if (file) {
                //     fprintf(file, "%s", report.str().c_str());
                //     fclose(file);
                // }

                return fileName;
            } catch (const std::exception& e) {
                try {
                    SPDLOG_CRITICAL("EXCEPTION WHILE LOGGING CRASH REPORT! what: {}", e.what());
                } catch (...) {
                    SPDLOG_CRITICAL("EXCEPTION WHILE LOGGING CRASH REPORT! UNABLE to get exception info! // Is the world ending? 😨");
                }
                _exit(1);
            } catch (...) {
                SPDLOG_CRITICAL("ERROR WHILE LOGGING CRASH REPORT!");
                _exit(1);
            }
        }

        void launchCrashHandler(const std::string& crashReportFileName)
        {
            QProcess::startDetached(QCoreApplication::applicationDirPath() + "/crash_handler"
#ifdef WIN32
                                                                             ".exe"
#endif
                ,
                { QString::fromStdString(crashReportFileName) }, QDir::currentPath());
        }

        void terminateHandler()
        {
            if (crashed.exchange(true))
                return;

            auto crashReportFileName = logCrash();
            SPDLOG_CRITICAL("Crash report saved to: {}", crashReportFileName);

            // TODO: Uncomment this when crash handler is ready
            // SPDLOG_INFO("Launching crash handler...");
            // launchCrashHandler(crashReportFileName);

            _exit(1);
        }
    } // namespace
    void init()
    {
        std::set_terminate(terminateHandler);
    }
} // namespace TheCalculater::dbgutil