/**
 * @file main.cpp
 * @author prbegd
 * @brief The main entry point of TheCalculater.
 * @date 2025-05-25
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */

#include "CLI/CLI11.hpp"
#include "TheCalculater/core.hpp"
#include "TheCalculater/dbgutil.hpp"
#include "TheCalculater/settings.hpp"
#include "TheCalculater/translator.hpp"
#include "TheCalculater/util.hpp"
#include "config.h"
#include "spdlog/async.h"
#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"
#include "ui/mainwindow.h"
#include "json/value.h" // IWYU pragma: keep
#include <QApplication>
#include <QMessageBox>
#include <QResource>
#include <QUuid>
#include <chrono>
#include <sstream>
#include <unordered_map>

#ifdef _WIN32
#include <fcntl.h>
#include <windows.h>
#endif

namespace {
#ifdef _WIN32
    void showConsole()
    {
        int result = AllocConsole();
        if (result == 0) {
            SPDLOG_ERROR("Failed to alloc console. Errno {}", GetLastError());
            return;
        }

        FILE* stream = nullptr;
        int error = 0;
        error = freopen_s(&stream, "CONOUT$", "w+", stdout);
        error = freopen_s(&stream, "CONOUT$", "w+", stderr);
        error = freopen_s(&stream, "CONIN$", "r+t", stdin);
        if (error != 0)
            SPDLOG_ERROR("One or more failed redirecting console output (Calling freopen_s). Console may not work correctly.");
        SetConsoleTitle(L"TheCalculater Console");
        SetConsoleOutputCP(CP_UTF8);
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD consoleMode = 0;
        GetConsoleMode(hConsole, &consoleMode);
        consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, consoleMode);
        SPDLOG_INFO("Console allocated.");
    }

    void showWTConsole()
    {
        // We use uuid (actually guid?) to prevent multiple instances of TheCalculater try to
        // open the same pipe.

        // How it works: we create a named pipe, then open HelperPipeReader in wt,
        // The HelperPipeReader receives data from pipe and print it to console.
        // And here we redirect stdout and stderr to the pipe. So when we print,
        // the text go through the pipe and then HelperPipeReader prints it to console.
        std::wstring pipeName = LR"(\\.\pipe\TheCalculaterConsolePipe)" + QUuid::createUuid().toString().toStdWString();
        HANDLE hPipe = CreateNamedPipeW(pipeName.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_WAIT,
            1, 4096, 4096, 0, nullptr);

        if (hPipe == INVALID_HANDLE_VALUE) {
            SPDLOG_ERROR("Failed to create pipe. Errno {}", GetLastError());
            return;
        }
        std::wstring cmd = LR"(wt.exe new-tab --title "TheCalculater Console" -- )" + QCoreApplication::applicationDirPath().toStdWString() + L"/HelperPipeReader.exe " + pipeName;

        STARTUPINFOW si = { sizeof(si) }; // NOLINT
        PROCESS_INFORMATION pi;
        if (!CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            CloseHandle(hPipe);
            SPDLOG_ERROR("Failed to create process. Errno {}", GetLastError());
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (!ConnectNamedPipe(hPipe, nullptr)) {
            if (GetLastError() != ERROR_PIPE_CONNECTED) {
                CloseHandle(hPipe);
                SPDLOG_ERROR("Failed to connect pipe. Errno {}", GetLastError());
                return;
            }
        }

        int fd = _open_osfhandle(reinterpret_cast<intptr_t>(hPipe), _O_TEXT);
        if (fd == -1) {
            CloseHandle(hPipe);
            SPDLOG_ERROR("Failed to open osfhandle. Errno {}", GetLastError());
            return;
        }
        FILE* fp = _fdopen(fd, "w");
        if (!fp) {
            _close(fd);
            CloseHandle(hPipe);
            SPDLOG_ERROR("Failed to open file descriptor. Errno {}", GetLastError());
            return;
        }
        int error = 0;
        *stdout = *fp; // NOLINT
        error = setvbuf(stdout, nullptr, _IONBF, 0);
        *stderr = *fp; // NOLINT
        error = setvbuf(stderr, nullptr, _IONBF, 0);
        std::ios::sync_with_stdio();
        if (error != 0)
            SPDLOG_ERROR("Failed to setvbuf. Errno {}", GetLastError());

        SPDLOG_INFO("Windows Terminal allocated.");
    }
#else
    void showConsole() { }
    void showWTConsole() { }
#endif

    constexpr std::array<const char*, 3> consoleModeString = { "off or inline", "default (conhost)", "windows terminal" };

    std::tuple<int, spdlog::level::level_enum, spdlog::level::level_enum> handleArgs(int argc, char** argv)
    {
        int consoleMode = 0;
        std::string consoleLogLevel = "off";
        std::string fileLogLevel = "info";

        CLI::App app("TheCalculater: A simple toolbox for calculation, conversion, and more.");
        argv = app.ensure_utf8(argv);

        app.remove_option(app.get_option("-h"));
#ifdef _WIN32
        const auto& aConsole = app.add_flag_function("-c,--console", [&](std::int64_t) { consoleMode = 1; }, "Show console output in external console. (conhost.exe) Mutually exclusive with option -C, --wt-console.");
        const auto& aWtConsole = app.add_flag_function("-C,--wt-console", [&](std::int64_t) { consoleMode = 2; }, "Show console output in Windows Terminal. (Prettier console than conhost.exe) Mutually exclusive with option -c, --console.");
        aConsole->excludes(aWtConsole);
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
        std::exit(0); }, "Show help information and exit."); // NOLINT
        app.add_flag_function("-v,--version", [&](std::int64_t) {
        const char* version = THECALCULATER_VERSION_ALL "\nBuild Number: " THECALCULATER_BUILD ", Build Type: " THECALCULATER_BUILD_TYPE;
#ifdef WIN32
        QPushButton copyBtn("Copy");
        QMessageBox::information(nullptr, "TheCalculater Version", version, QMessageBox::Ok);
#else
        std::cout << version << "\n";
#endif
        std::exit(0); }, "Show version information and exit."); // NOLINT

        try {
            app.parse(argc, argv);
        } catch (const CLI::ParseError& e) {
            QMessageBox::warning(nullptr, "TheCalculater: Invalid Commandline Arguments", QString::fromStdString(e.get_name() + ": " + e.what() + "\n\nRun '" + argv[0] + " --help' for more information.\nThe program will not be started."));
            std::exit(2); // NOLINT
        }

        return { consoleMode, spdlog::level::from_str(consoleLogLevel), spdlog::level::from_str(fileLogLevel) };
    }

    void initLogger(spdlog::level::level_enum console, spdlog::level::level_enum file) // NOLINT
    {
        auto consoleSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>(spdlog::color_mode::always);
        consoleSink->set_pattern("\033[0;34m[%H:%M:%S.%e]\033[0m %^[%l]%$ "
                                 "\033[0;35m[%t]\033[0m \033[0;36m(%!)\033[0m %v");

        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "log/log.log", 1024ULL * 1024 * 5, 5, true);
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
        {
            using namespace std::chrono_literals;
            spdlog::flush_every(5s);
        }
        if (!std::atexit([]() {
                spdlog::shutdown();
            }))
            SPDLOG_ERROR("Failed to register atexit function.");

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
        auto [consoleMode, consoleLogLevel, fileLogLevel] = handleArgs(argc, argv);
        initLogger(consoleLogLevel, fileLogLevel);
        switch (consoleMode) {
        case 1:
            showConsole();
            break;
        case 2:
            showWTConsole();
            break;
        default:
            break;
        }
        TheCalculater::dbgutil::init(argc, argv);
        SPDLOG_INFO("Initialization parameters: \nconsoleMode: {}\nconsoleLogLevel: {}\nfileLogLevel: {}", consoleModeString[consoleMode], spdlog::level::to_string_view(consoleLogLevel), spdlog::level::to_string_view(fileLogLevel));

        if (!QResource::registerResource("./resources.rcc")) {
            SPDLOG_CRITICAL("Failed to load resource file");
            QMessageBox::critical(nullptr, "Failed to load resource file", "Unable to load resource file, program startup failed!\nThe resources.rcc in the program directory may have been deleted or damaged. You can try reinstalling the program to solve this problem.");
            std::exit(1); // NOLINT
        }
        SPDLOG_INFO("Resource file loaded.");

        // TODO: make this read from settings
        TheCalculater::translator::loadTranslations(
            TheCalculater::util::parse(std::string_view(
                                           TheCalculater::util::readResourcesFile(":/resources/data/translations.json5").constData()),
                TheCalculater::core::ErrorHandleType::LogError));
        TheCalculater::translator::switchLanguage();
        SPDLOG_INFO("Translations loaded.");

        TheCalculater::settings::setSettingsFilePath("settings.json5");
        TheCalculater::settings::loadConfigTemplate(TheCalculater::util::parse(TheCalculater::util::readResourcesFile(":/resources/data/config_template.json5").constData()));
        std::unordered_map<std::string, std::string> errors;
        TheCalculater::settings::parseSettings(errors);
        if (!errors.empty()) {
            std::ostringstream oss;
            for (const auto& [key, value] : errors) {
                oss << "Key: '" << key << "' Error: '" << value << "'\n";
            }
            SPDLOG_ERROR("Errors parsing settings:\n{}", oss.str());
        }
        SPDLOG_INFO("Settings loaded.");
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

    return QApplication::exec();
}
