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
#include "config.h"

#include "ui/mainwindow.h"

import std;
import tpmm.cli11;
import tpmm.cstd;
import tpmm.winapi;
import tpmm.spdlog;
import tpmm.jsoncpp;
import TheCalculater.libTheCalculaterCommon;
import TheCalculater.debugging;
import TheCalculater.settings;
import TheCalculater.translator;
import TheCalculater.util;
import TheCalculaterQtBridge.resources;
import TheCalculaterQtBridge.qtmock;
import TheCalculater.throwEx;
namespace {
#ifdef _WIN32
    void showConsole()
    {
        int result = winapi::AllocConsole();
        if (result == 0) {
            spdlog::error("Failed to alloc console. Errno {}", winapi::GetLastError());
            return;
        }

        std::FILE* stream = nullptr;
        bool hasError = false;
        stream = std::freopen("CONOUT$", "w+", cstd::_stdout);
        hasError |= (stream == nullptr);
        stream = std::freopen("CONOUT$", "w+", cstd::_stderr);
        hasError |= (stream == nullptr);
        stream = std::freopen("CONIN$", "r+t", cstd::_stdin);
        hasError |= (stream == nullptr);
        if (hasError)
            spdlog::error("One or more failed redirecting console output (Calling freopen_s). Console may not work correctly.");
        winapi::SetConsoleTitleW(L"TheCalculater Console");
        winapi::SetConsoleOutputCP(winapi::_CP_UTF8);
        winapi::HANDLE hConsole = winapi::GetStdHandle(winapi::_STD_OUTPUT_HANDLE);
        winapi::DWORD consoleMode = 0;
        winapi::GetConsoleMode(hConsole, &consoleMode);
        consoleMode |= winapi::_ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        winapi::SetConsoleMode(hConsole, consoleMode);
        spdlog::info("Console allocated.");
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
        winapi::HANDLE hPipe = winapi::CreateNamedPipeW(pipeName.c_str(), winapi::_PIPE_ACCESS_DUPLEX, winapi::_PIPE_TYPE_BYTE | winapi::_PIPE_WAIT,
                                                        1, 4096, 4096, 0, nullptr);

        if (hPipe == winapi::_INVALID_HANDLE_VALUE) {
            spdlog::error("Failed to create pipe. Errno {}", winapi::GetLastError());
            return;
        }
        std::wstring cmd = LR"(wt.exe new-tab --title "TheCalculater Console" -- )" + QCoreApplication::applicationDirPath().toStdWString() + L"/HelperPipeReader.exe " + pipeName;

        winapi::STARTUPINFOW si = { sizeof(si) }; // NOLINT
        winapi::PROCESS_INFORMATION pi;
        if (!winapi::CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, winapi::_FALSE, 0, nullptr, nullptr, &si, &pi)) {
            winapi::CloseHandle(hPipe);
            spdlog::error("Failed to create process. Errno {}", winapi::GetLastError());
            return;
        }
        winapi::CloseHandle(pi.hProcess);
        winapi::CloseHandle(pi.hThread);

        if (!winapi::ConnectNamedPipe(hPipe, nullptr)) {
            if (winapi::GetLastError() != winapi::_ERROR_PIPE_CONNECTED) {
                winapi::CloseHandle(hPipe);
                spdlog::error("Failed to connect pipe. Errno {}", winapi::GetLastError());
                return;
            }
        }

        int fd = winapi::_open_osfhandle(reinterpret_cast<intptr_t>(hPipe), winapi::__O_TEXT);
        if (fd == -1) {
            winapi::CloseHandle(hPipe);
            spdlog::error("Failed to open osfhandle. Errno {}", winapi::GetLastError());
            return;
        }
        FILE* fp = winapi::_fdopen(fd, "w");
        if (!fp) {
            winapi::_close(fd);
            winapi::CloseHandle(hPipe);
            spdlog::error("Failed to open file descriptor. Errno {}", winapi::GetLastError());
            return;
        }
        int error = 0;
        *stdout = *fp; // NOLINT
        error = setvbuf(stdout, nullptr, _IONBF, 0);
        *stderr = *fp; // NOLINT
        error = setvbuf(stderr, nullptr, _IONBF, 0);
        std::ios::sync_with_stdio();
        if (error != 0)
            spdlog::error("Failed to setvbuf. Errno {}", winapi::GetLastError());

        spdlog::info("Windows Terminal allocated.");
    }
#else
    void showConsole() { }
    void showWTConsole() { }
#endif

    constexpr std::array<const char*, 3> consoleModeString = { "off or inline", "default (conhost)", "windows terminal" };

    std::tuple<int, spdlog::level::level_enum, spdlog::level::level_enum> handleArgs(int argc, char** argv)
    {
        int consoleMode = 0;
        std::string consoleLogLevel;
        std::string fileLogLevel;

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
        fileLogLevel = value; }, "Set both console log level and file log level.")->check(CLI::IsMember({ "off", "trace", "debug", "info", "warn", "error", "critical" }));

        app.add_option("--console-log", consoleLogLevel, "Set console log level.")
            ->default_str("off")
            ->check(CLI::IsMember({ "off", "trace", "debug", "info", "warn", "error", "critical" }));

        app.add_option("--file-log", fileLogLevel, "Set file log level.")
            ->default_str("info")
            ->check(CLI::IsMember({ "off", "trace", "debug", "info", "warn", "error", "critical" }));

        app.add_flag_function("-h,--help", [&](std::int64_t) {
        std::string help = app.help();
            // fix issue that the Windows GUI program could not output to the console.
#ifdef WIN32
        QMessageBox::information(nullptr, "TheCalculater Help", QString::fromStdString(help));
#else
        std::cout << help << '\n';
#endif
        std::exit(0); }, "Show help information and exit."); // NOLINT
        app.add_flag_function("-v,--version", [&](std::int64_t) {
            const char* version = THECALCULATER_VERSION_ALL "\nBuild Number: " THECALCULATER_BUILD ", Build Type: " THECALCULATER_BUILD_TYPE;
#ifdef _WIN32
            QMessageBox::information(nullptr, "TheCalculater Version", version, QMessageBox::Ok);
#else
            std::cout << version << '\n';
#endif
            std::exit(0); }, "Show version information and exit."); // NOLINT

        app.add_option("--platform", "Controls what platform plugin to use. Provided by Qt. You can add/remove platform plugins by adding/deleting plugin files to 'platform' directory. Default value depends on your platform.")->expected(1)->type_name("TEXT");

        try {
            app.parse(argc, argv);
        } catch (const CLI::ParseError& e) {
            QMessageBox::warning(nullptr, "TheCalculater: Invalid Commandline Arguments", QString::fromStdString(e.get_name() + ": " + e.what() + "\n\nRun '" + argv[0] + " --help' for more information.\nThe program will not be started."));
            std::exit(2); // NOLINT
        }

        return { consoleMode, spdlog::level::from_str(consoleLogLevel), spdlog::level::from_str(fileLogLevel) };
    }

    class LogFormatter : public spdlog::formatter {
    public:
        LogFormatter(bool useColor = true)
            : useColor_(useColor)
        { }
        void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override
        {
            std::string format = useColor_ ? "\033[0;34m[%H:%M:%S.%e]\033[0m %^[%l]%$ "
                                             "\033[0;35m[{}]\033[0m \033[0;36m(%!)\033[0m %v"
                                           : "[%H:%M:%S.%e] [%l] [{}] (%!) %v";

            std::string thread = TheCalculater::util::getThreadNameById(msg.thread_id);
            if (thread.empty())
                thread = std::to_string(msg.thread_id);
            format = std::vformat(format, std::make_format_args(thread));

            spdlog::memory_buf_t formatted;
            spdlog::pattern_formatter(format).format(msg, formatted);

            dest.append(formatted.data(), formatted.data() + formatted.size());
        }
        std::unique_ptr<formatter> clone() const override
        {
            return std::make_unique<LogFormatter>(useColor_);
        }

    private:
        bool useColor_;
    };

    std::jthread logFlushThread;

    spdlog::level::level_enum qtMessageTypeToSpdlogLevel(QtMsgType type)
    {
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
    }
    void qtMessageHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
    {
        spdlog::_log(
            spdlog::source_loc(nullptr, 1, "#Qt#"),
            qtMessageTypeToSpdlogLevel(type),
            msg.toStdString());
    }

    void initLogger(spdlog::level::level_enum console, spdlog::level::level_enum file) // NOLINT
    {
        auto consoleSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>(spdlog::color_mode::always);
        consoleSink->set_formatter(std::make_unique<LogFormatter>(true));

        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "log/log.log", 1024ULL * 1024 * 5, 5, true);
        fileSink->set_formatter(std::make_unique<LogFormatter>(false));

        spdlog::sinks_init_list sinkList = { consoleSink, fileSink };
        spdlog::init_thread_pool(8192, 1, [] { TheCalculater::util::setThreadName(TheCalculater::util::currentThread, "SpdlogThredPool"); }, [] { });
        auto logger = std::make_shared<spdlog::async_logger>("thecalc_logger", sinkList, spdlog::thread_pool());

        spdlog::register_logger(logger);
        TheCalculater::registerLogger(logger);
        spdlog::set_default_logger(logger);

        logger->set_level(console < file ? console : file);
        consoleSink->set_level(console);
        fileSink->set_level(file);

        if (console == spdlog::level::off)
            std::cout << "TheCalculater: Console logging is disabled. To enable it, please use the --console-log option and set it to a higher level than 'off'. Use the --help option for more information.\n";

        logFlushThread = std::jthread([](const std::stop_token& stop) {
            TheCalculater::util::setThreadName(TheCalculater::util::currentThread, "LogFlushThread");
            std::condition_variable_any cv;
            std::mutex mutex;
            std::unique_lock<std::mutex> lock(mutex);
            while (!stop.stop_requested()) {
                spdlog::details::registry::instance()
                    .flush_all();
                cv.wait_for(lock, stop, std::chrono::seconds(5), [] { return false; });
            }
        });

        if (std::atexit([]() {
                spdlog::info("Exiting...");
                logFlushThread.request_stop();
                logFlushThread.join();
                spdlog::shutdown();
            }))
            spdlog::warn("Failed to register atexit function.");

        qInstallMessageHandler(qtMessageHandler);
    }

    void init(int argc, char** argv)
    {
        auto [consoleMode, consoleLogLevel, fileLogLevel] = handleArgs(argc, argv);
        TheCalculater::util::setThreadName(TheCalculater::util::currentThread, "TheCalculater");
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
        TheCalculater::debugging::init(argc, argv);
        spdlog::info("Initialization parameters: \nconsoleMode: {}\nconsoleLogLevel: {}\nfileLogLevel: {}", consoleModeString[consoleMode], spdlog::level::to_string_view(consoleLogLevel), spdlog::level::to_string_view(fileLogLevel));

        if (!QResource::registerResource("./resources.rcc")) {
            spdlog::critical("Failed to load resource file");
            QMessageBox::critical(nullptr, "Failed to load resource file", "Unable to load resource file, program startup failed!\nThe resources.rcc in the program directory may have been deleted or damaged. You can try reinstalling the program to solve this problem.");
            std::exit(1); // NOLINT
        }
        spdlog::info("Resource file loaded.");

        TheCalculater::settings::setSettingsFilePath("settings.json5");
        TheCalculater::settings::loadConfigTemplate(TheCalculater::util::parse(TheCalculaterQtBridge::readResourcesFile(":/resources/data/config_template.json5").constData()));

        std::unordered_map<std::string, std::string> errors;
        TheCalculater::settings::parseSettings(errors);
        if (!errors.empty()) {
            std::ostringstream oss;
            for (const auto& [key, value] : errors) {
                oss << "Key: '" << key << "' Error: '" << value << "'\n";
            }
            spdlog::error("Errors parsing settings:\n{}", oss.str());
        }

        TheCalculater::translator::loadTranslations(
            TheCalculater::util::parse(
                TheCalculaterQtBridge::readResourcesFile(":/resources/data/translations.json5").constData()));
        TheCalculater::translator::switchLanguage(TheCalculater::settings::readString("general.language").stringRef());
    }
} // namespace

int main(int argc, char* argv[]) // NOLINT
{

    spdlog::stopwatch timer;
    QApplication app(argc, argv);
    init(argc, argv);
    VMainWindow window;
    window.show();
    spdlog::info("Initialization done, took {}ms.", timer.elapsed_ms().count());

    return QApplication::exec();
}
