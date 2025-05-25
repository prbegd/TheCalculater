//
//  _____  _             ____        _               _         _
// |_   _|| |__    ___  / ___| __ _ | |  ___  _   _ | |  __ _ | |_  ___  _ __
//   | |  | '_ \  / _ \| |    / _` || | / __|| | | || | / _` || __|/ _ \| '__|
//   | |  | | | ||  __/| |___| (_| || || (__ | |_| || || (_| || |_|  __/| |
//   |_|  |_| |_| \___| \____|\__,_||_| \___| \__,_||_| \__,_| \__|\___||_|
//
// Copyright (c) 2025 prbegd
// Distributed under the MIT License (https://opensource.org/licenses/MIT)

#include "CLI/CLI11.hpp"
#include "TheCalculater/dbgutil.hpp"
#include "appdef.hpp"
#include "TheCalculater/mainwindow.h"
#include "spdlog/spdlog.h"
#include <QApplication>
#include <QMessageBox>
#include <QResource>
#include <boost/stacktrace/stacktrace.hpp>
#include <QMessagebox>

#ifdef _WIN32
#include <windows.h>
#endif

constexpr static size_t logFileMaxSize = 1024ULL * 1024 * 5; // byte, 5MiB
constexpr static size_t logFileMaxFiles = 5;
constexpr static int logFlushInterval = 5; // sec

#ifdef _WIN32
void static showConsole()
{
    SPDLOG_DEBUG("Allocing console...");
    int result = AllocConsole();
    if (result == 0) { // 文档是这样写的，See https://learn.microsoft.com/en-us/windows/console/allocconsole
        SPDLOG_ERROR("Failed to alloc console. Error code: {}", GetLastError());
        return;
    }

    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w+", stdout);
    freopen_s(&stream, "CONOUT$", "w+", stderr);
    freopen_s(&stream, "CONIN$", "r+t", stdin);
    SetConsoleTitle(L"TheCalculater Console");
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(hConsole, &consoleMode);
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, consoleMode);
}
#else
void static showConsole() { }
#endif

static std::tuple<bool, spdlog::level::level_enum, spdlog::level::level_enum> handleArgs(int argc, char** argv)
{
    bool showConsole = false;
    std::string consoleLogLevel = "off";
    std::string fileLogLevel = "info";

    CLI::App app("TheCalculater: A simple toolbox for calculation, conversion, and more.");
    argv = app.ensure_utf8(argv);

#ifdef _WIN32
    app.add_flag("-c,--console", showConsole, "Show console output in external console.");
#endif
    app.add_option_function<std::string>("-l,--log", [&](const std::string& value) {
        consoleLogLevel = value;
        fileLogLevel = value; }, "Set both console log level and file log level.");
    app.add_option("--console-log", consoleLogLevel, "Set console log level (trace, debug, info, warn, error, critical, off).\nDefault: off. If the value is invalid, it will be ignored(off).");
    app.add_option("--file-log", fileLogLevel, "Set file log level (trace, debug, info, warn, error, critical, off).\nDefault: info. If the value is invalid, it will be ignored(info).");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        QMessageBox::critical(nullptr, "Invalid Commandline Arguments", QString::fromStdString(e.get_name() + ": " + e.what() + "\n\nRun '" + argv[0] + " --help' for more information."));
        std::exit(1);
    }

    return { showConsole, spdlog::level::from_str(consoleLogLevel), spdlog::level::from_str(fileLogLevel) };
}

static void initLogger(spdlog::level::level_enum console, spdlog::level::level_enum file) // NOLINT
{
    auto consoleSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>(spdlog::color_mode::always);
    consoleSink->set_pattern("\033[0;34m[%H:%M:%S.%e]\033[0m %^[%l]%$ "
                             "\033[0;35m[%t]\033[0m \033[0;36m(%!)\033[0m %v");

    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "log/log.log", logFileMaxSize, logFileMaxFiles, true);
    fileSink->set_pattern("[%H:%M:%S.%e] [%l] [%t] (%!) %v");

    spdlog::sinks_init_list sinkList = { consoleSink, fileSink };
    auto logger = std::make_shared<spdlog::logger>("logger", sinkList);
    spdlog::set_default_logger(logger);

    logger->set_level(console < file ? console : file);
    consoleSink->set_level(console);
    fileSink->set_level(file);

    spdlog::flush_every(std::chrono::seconds(logFlushInterval));

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

static void init(int argc, char** argv)
{
    auto [isShowConsole, consoleLogLevel, fileLogLevel] = handleArgs(argc, argv);
    initLogger(consoleLogLevel, fileLogLevel);
    if (isShowConsole)
        showConsole();
    TheCalculater::dbgutil::init();
    SPDLOG_INFO("Initialization parameters:\nshowConsole: {}\nconsoleLogLevel: {}\nfileLogLevel: {}", isShowConsole, spdlog::level::to_string_view(consoleLogLevel), spdlog::level::to_string_view(fileLogLevel));

    SPDLOG_DEBUG("Set custom terminate handler.");

    if (!QResource::registerResource("./resources.rcc")) {
        SPDLOG_CRITICAL("Failed to load resource file.(resources.rcc)");
        QMessageBox::critical(nullptr, QTTRC("Init", "Failed to load resource file"), QTTRC("Init", "Unable to load resource file, program startup failed!\nThe resources.rcc in the program directory may have been deleted or damaged. You can try reinstalling the program to solve this problem."));
        std::exit(1);
    }
    SPDLOG_INFO("Resources loaded.");
}

int main(int argc, char* argv[]) // NOLINT
{
    spdlog::stopwatch timer;
    QApplication app(argc, argv);
    init(argc, argv);
    VMainWindow window;
    window.show();
    SPDLOG_INFO("Initialization took {}ms.", timer.elapsed_ms().count());

    return QApplication::exec();
}
