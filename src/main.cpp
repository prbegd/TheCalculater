/**
 * @file main.cpp
 * @author prbegd
 * @brief The main entry point of TheCalculater.
 * @date 2025-05-25
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */

#include "CLI/CLI11.hpp"
#include "TheCalculater/core.hpp"
#include "TheCalculater/dbgutil.hpp"
#include "TheCalculater/mainwindow.h"
#include "TheCalculater/translator.hpp"
#include "TheCalculater/util.hpp"
#include "config.h"
#include "spdlog/async.h"
#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"
#include "json/json.h"
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QResource>


#ifdef _WIN32
#include <windows.h>
#endif

namespace {
    constexpr size_t logFileMaxSize = 1024ULL * 1024 * 5; // byte, 5MiB
    constexpr size_t logFileMaxFiles = 5;
    constexpr std::chrono::seconds logFlushInterval(5);

#ifdef _WIN32
    void showConsole()
    {
        int result = AllocConsole();
        if (result == 0) {
            SPDLOG_ERROR("Failed to alloc console. Errno {}", GetLastError());
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
        SPDLOG_INFO("Console allocated.");
    }
#else
    void showConsole() { }
#endif

    std::tuple<bool, spdlog::level::level_enum, spdlog::level::level_enum> handleArgs(int argc, char** argv)
    {
        bool showConsole = false;
        std::string consoleLogLevel = "off";
        std::string fileLogLevel = "info";

        CLI::App app("TheCalculater: A simple toolbox for calculation, conversion, and more.");
        argv = app.ensure_utf8(argv);

        app.remove_option(app.get_option("-h"));
#ifdef _WIN32
        app.add_flag("-c,--console", showConsole, "Show console output in external console.");
#endif
        app.add_option_function<std::string>("-l,--log", [&](const std::string& value) {
        consoleLogLevel = value;
        fileLogLevel = value; }, "Set both console log level and file log level.");
        app.add_option("--console-log", consoleLogLevel, "Set console log level (trace, debug, info, warn, error, critical, off).\nDefault: off. If the value is invalid, it will be ignored(off).");
        app.add_option("--file-log", fileLogLevel, "Set file log level (trace, debug, info, warn, error, critical, off).\nDefault: info. If the value is invalid, it will be ignored(info).");
        app.add_flag_function("-h,--help", [&](std::int64_t) {
        std::string help = app.help();
            // fix issue that the Windows GUI program could not output to the console.
#ifdef WIN32
        QMessageBox::information(nullptr, "TheCalculater Help", QString::fromStdString(help));
#else
        std::cout << help << "\n";
#endif
        std::exit(0); }, "Show help information and exit.");
        app.add_flag_function("-v,--version", [&](std::int64_t) {
        const char* version = THECALCULATER_VERSION_ALL "\nBuild Number: " THECALCULATER_BUILD ", Build Type: " THECALCULATER_BUILD_TYPE;
#ifdef WIN32
        QPushButton copyBtn("Copy");
        QMessageBox::information(nullptr, "TheCalculater Version", version, QMessageBox::Ok);
#else
        std::cout << version << "\n";
#endif
        std::exit(0); }, "Show version information and exit.");

        try {
            app.parse(argc, argv);
        } catch (const CLI::ParseError& e) {
            QMessageBox::warning(nullptr, "TheCalculater: Invalid Commandline Arguments", QString::fromStdString(e.get_name() + ": " + e.what() + "\n\nRun '" + argv[0] + " --help' for more information.\nThe program will not be started."));
            std::exit(2);
        }

        return { showConsole, spdlog::level::from_str(consoleLogLevel), spdlog::level::from_str(fileLogLevel) };
    }

    void initLogger(spdlog::level::level_enum console, spdlog::level::level_enum file) // NOLINT
    {
        auto consoleSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>(spdlog::color_mode::always);
        consoleSink->set_pattern("\033[0;34m[%H:%M:%S.%e]\033[0m %^[%l]%$ "
                                 "\033[0;35m[%t]\033[0m \033[0;36m(%!)\033[0m %v");

        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "log/log.log", logFileMaxSize, logFileMaxFiles, true);
        fileSink->set_pattern("[%H:%M:%S.%e] [%l] [%t] (%!) %v");

        spdlog::sinks_init_list sinkList = { consoleSink, fileSink };
        spdlog::init_thread_pool(8192, 1);
        auto logger = std::make_shared<spdlog::async_logger>("logger", sinkList, spdlog::thread_pool());

        spdlog::register_logger(logger);
        TheCalculater::core::registerLogger(logger);
        spdlog::set_default_logger(logger);

        logger->set_level(console < file ? console : file);
        consoleSink->set_level(console);
        fileSink->set_level(file);

        spdlog::flush_every(logFlushInterval);
        std::atexit([]() {
            spdlog::shutdown();
        });

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

    void init(int argc, char** argv)
    {
        auto [isShowConsole, consoleLogLevel, fileLogLevel] = handleArgs(argc, argv);
        initLogger(consoleLogLevel, fileLogLevel);
        if (isShowConsole)
            showConsole();
        TheCalculater::dbgutil::init(argc, argv);
        SPDLOG_INFO("Initialization parameters:\nshowConsole: {}\nconsoleLogLevel: {}\nfileLogLevel: {}", isShowConsole, spdlog::level::to_string_view(consoleLogLevel), spdlog::level::to_string_view(fileLogLevel));

        if (!QResource::registerResource("./resources.rcc")) {
            SPDLOG_CRITICAL("Failed to load resource file");
            QMessageBox::critical(nullptr, "Failed to load resource file", "Unable to load resource file, program startup failed!\nThe resources.rcc in the program directory may have been deleted or damaged. You can try reinstalling the program to solve this problem.");
            std::exit(1);
        }
        SPDLOG_INFO("Resource file loaded.");

        // TODO: make this read from settings
        TheCalculater::translator::loadTranslations(
            TheCalculater::util::parse(
                TheCalculater::util::readResourcesFileAllText(":/resources/data/translations.json5"),
                TheCalculater::core::ErrorHandleType::LogError));
        TheCalculater::translator::switchLanguage("zh_CN");
    }
} // namespace

int main(int argc, char* argv[]) // NOLINT
{
    spdlog::stopwatch timer;
    QApplication app(argc, argv);
    init(argc, argv);
    VMainWindow window;
    window.show();
    SPDLOG_INFO("Initialization done, took {}ms.", timer.elapsed_ms().count());

    // TheCalculater::throw_with_trace(std::runtime_error("test error!"));

    return QApplication::exec();
}
