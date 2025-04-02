#pragma once
#include "ProgramArgs.hpp"
#include "spdlog/common.h"
// #include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <tuple>

#ifndef PROGRAM_VERSION
#define PROGRAM_VERSION "UNKNOWN"
#endif

namespace ProgramInfo {

    std::vector<std::string_view> programArgs = {
        "--help", "--version", "--log-level", "--console",
        "-l", "--no-color", "--nocolor"
    };
    App::ProgramArgs::AbbrMapType abbrMap = {
        { "-?", "--help" },
        { "-h", "--help" },
        { "-v", "--version" },
        { "-V", "--version" },
        { "-c", "--console" },
    };
    std::string_view helpContent = R"(Usage: TheCalculater.exe [options]

This is a GUI program. This help information only shows the arguments that can be used in the command line.
Please run the program to see more informations in the GUI.

Options:
    -?, -h, --help                      Show help message
    -v, -V, --version                   Show version
    -c, --console                       Show console
    -l <level>, --log-level=<level>     Set log level (trace, debug, info, warning, error, critical, off) Default: off
    
Examples:
    TheCalculater.exe -c -l info
)";
    std::string_view version = "TheCalculater v" PROGRAM_VERSION "by prbegd Windows";
} // namespace ProgramInfo

inline std::tuple<bool, spdlog::level::level_enum> handleArgs(int argc, char* argv[])
{
    bool showConsole = false;
    spdlog::level::level_enum logLevel = spdlog::level::off;

    std::vector<std::string_view> args = App::ProgramArgs::parse(argc, argv, ProgramInfo::abbrMap);
    if (!App::ProgramArgs::check(args, ProgramInfo::programArgs))
        QApplication::exit(1); // 参数错误退出程序
    for (int i = 0; i < args.size(); i++) {
        std::string_view arg = args[i];
        if (arg == "--version") {
            std::cout << ProgramInfo::version << "\n";
            QApplication::quit();
        } else if (arg == "--help") {
            std::cout << ProgramInfo::version << "\n\n"
                      << ProgramInfo::helpContent;
            QApplication::quit();
        } else if (arg == "--console") {
            showConsole = true;
        } else if (arg.starts_with("--log-level") || arg == "-l") {
            std::optional<std::string_view> value = App::ProgramArgs::getValue(args, i);
            if (!value.has_value()) {
                std::cout << "Error: --log-level doesn't have value\nTry 'TheCalculater.exe --help' for more information.\n";
                QApplication::exit(1);
            }
            logLevel = spdlog::level::from_str(value.value().data());
        }
    }
    return { showConsole, logLevel };
}

inline void init(int argc, char* argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    auto [showConsole, logLevel] = handleArgs(argc, argv);

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("\033[0;34m[%H:%M:%S.%e]\033[0m %^[%l]%$ "
                             "\033[0;35m[%t]\033[0m \033[0;36m(%!)\033[0m %v");

    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "log/log.log", 1024 * 1024 * 5, 5, true);
    fileSink->set_pattern("[%H:%M:%S.%e] [%l] [%t] (%!) %v");

    spdlog::sinks_init_list sinkList = { consoleSink, fileSink };
    auto logger = std::make_shared<spdlog::logger>("logger", sinkList);
    spdlog::set_default_logger(logger);

    spdlog::set_level(logLevel); // 针对控制台修改等级（保证文件日志不受影响）
    spdlog::flush_every(std::chrono::seconds(5)); // 每5秒刷新日志

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context,
                               const QString& msg) {
        const char* function = context.function ? context.function : "???";
        spdlog::log(
            spdlog::source_loc(nullptr, 1, function),
            [](QtMsgType type) {
                switch (type) {
                case QtDebugMsg:
                    return spdlog::level::debug;
                case QtInfoMsg:
                    return spdlog::level::info;
                case QtWarningMsg:
                    return spdlog::level::warn;
                case QtCriticalMsg:
                    return spdlog::level::err;
                case QtFatalMsg:
                    return spdlog::level::critical;
                default:
                    return spdlog::level::trace;
                }
            }(type),
            msg.toStdString());
    });
}