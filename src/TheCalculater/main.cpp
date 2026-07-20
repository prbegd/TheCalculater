/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "config.h"

#include "thecalculater/macros.hpp"
// HACK: Dispatch an event whenever the program wants to exit, we listen the event here and call cleanup()
#include "thecalculater/cleanup.hpp"
#include "ui/mainwindow.h"

import std;
import prbegd.thecalculater.debugging;
import prbegd.thecalculater.settings;
import prbegd.thecalculater.translator;
import prbegd.thecalculater.util;

import thirdparty.core;
import thirdparty.extra;

namespace {
#ifdef THECALCULATER_WINDOWS
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
    if (hasError) {
        spdlog::error("One or more failed redirecting console output (Calling freopen_s). Console may not work correctly.");
    }
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

    static const std::function<std::wstring()> getExecutableDir = [] -> std::wstring {
        wchar_t buffer[winapi::__MAX_PATH];
        winapi::GetModuleFileNameW(nullptr, buffer, winapi::__MAX_PATH);
        return std::filesystem::path(buffer).parent_path().string<wchar_t>();
    };
    std::wstring cmd = LR"(wt.exe new-tab --title "TheCalculater Console" -- )" + getExecutableDir() + L"/HelperPipeReader.exe " + pipeName;

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
    if (error != 0) {
        spdlog::error("Failed to setvbuf. Errno {}", winapi::GetLastError());
    }

    spdlog::info("Windows Terminal allocated.");
}
#endif

struct CommandLineArguments {
#ifdef THECALCULATER_WINDOWS
    enum class ConsoleMode : int8_t {
        Off,
        WindowsConhost,
        WindowsTerminal,
    } consoleMode = ConsoleMode::Off;
#endif
    std::optional<spdlog::level::level_enum> consoleLogLevel;
    spdlog::level::level_enum fileLogLevel = spdlog::level::info;

    enum class StartAction : int8_t {
        NormalStart,
        DisplayCommandLineHelp,
        DisplayApplicationVersion,
        WarnInvalidArguments,
    } startAction = StartAction::NormalStart;
    std::optional<std::string> commandLineHelp;
    std::optional<std::string> invalidArgumentsWarning;
};
CommandLineArguments parseCommandLineArguments(int argc, char** argv)
{
    CommandLineArguments arguments;

    CLI::App app("TheCalculater: A simple toolbox for calculation, conversion, and more.");
    argv = app.ensure_utf8(argv);
    app.remove_option(app.get_option("-h"));

#ifdef THECALCULATER_WINDOWS
    CLI::Option* const optionConsole = app.add_flag_function("-c,--console", [&](std::int64_t) { arguments.consoleMode = CommandLineArguments::ConsoleMode::WindowsConhost; }, "Show console output in external console (conhost.exe). Mutually exclusive with option -C, --wt-console.");
    CLI::Option* const optionWtConsole = app.add_flag_function("-C,--wt-console", [&](std::int64_t) { arguments.consoleMode = CommandLineArguments::ConsoleMode::WindowsTerminal; }, "Show console output in Windows Terminal (wt.exe). Mutually exclusive with option -c, --console.");
    optionConsole->excludes(optionWtConsole);
#endif
    app.add_option_function<std::string>("-l,--log", [&](const std::string& value) {
        arguments.consoleLogLevel = spdlog::level::from_str(value);
        arguments.fileLogLevel = spdlog::level::from_str(value); }, "Set both console log level and file log level.")->check(CLI::IsMember({ "off", "trace", "debug", "info", "warn", "error", "critical" }));

    app.add_option_function<std::string>("--console-log", [&](const std::string& value) { arguments.consoleLogLevel = spdlog::level::from_str(value); }, "Set console log level.")->default_str("off")->check(CLI::IsMember({ "off", "trace", "debug", "info", "warn", "error", "critical" }));

    app.add_option_function<std::string>("--file-log", [&](const std::string& value) { arguments.fileLogLevel = spdlog::level::from_str(value); }, "Set file log level.")->default_str("info")->check(CLI::IsMember({ "off", "trace", "debug", "info", "warn", "error", "critical" }));

    app.add_flag_function("-h,--help", [&](std::int64_t) {
            arguments.startAction = CommandLineArguments::StartAction::DisplayCommandLineHelp;
            arguments.commandLineHelp = app.help(); }, "Show help information and exit.");
    app.add_flag_function("-v,--version", [&](std::int64_t) { arguments.startAction = CommandLineArguments::StartAction::DisplayApplicationVersion; }, "Show version information and exit.");

    app.add_option("--platform", "Controls what platform plugin to use. Provided by Qt. You can add/remove platform plugins by adding/deleting plugin files to 'platform' directory. Default value depends on your platform.")->expected(1)->type_name("TEXT");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        arguments.startAction = CommandLineArguments::StartAction::WarnInvalidArguments;
        arguments.invalidArgumentsWarning = (e.get_name() += ": ") += e.what();
    }

    return arguments;
}

class LogFormatter : public spdlog::formatter {
public:
    LogFormatter(bool useColor = true)
        : useColor_(useColor),
          basicFormat_(useColor_ ? "\033[0;34m[%H:%M:%S.%e]\033[0m %^[%l]%$ "
                                   "\033[0;35m[{}]\033[0m \033[0;36m(%!)\033[0m %v"
                                 : "[%H:%M:%S.%e] [%l] [{}] (%!) %v")
    { }
    void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override
    {
        std::string thread = thecalculater::util::getThreadNameById(msg.thread_id);
        if (thread.empty()) {
            thread = std::to_string(msg.thread_id);
        }
        std::string format = std::vformat(basicFormat_, std::make_format_args(thread));

        patternFormatter_.set_pattern(format);
        patternFormatter_.format(msg, dest);
    }
    std::unique_ptr<formatter> clone() const override
    {
        return std::make_unique<LogFormatter>(useColor_);
    }

private:
    bool useColor_;
    std::string_view basicFormat_;
    spdlog::pattern_formatter patternFormatter_;
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
    spdlog::init_thread_pool(8192, 1, [] { thecalculater::util::setThreadName(thecalculater::util::currentThread, "LoggerThread"); }, [] { });
    auto logger = std::make_shared<spdlog::async_logger>("thecalc_logger", sinkList, spdlog::thread_pool());

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    logger->set_level(console < file ? console : file);
    consoleSink->set_level(console);
    fileSink->set_level(file);

    logFlushThread = std::jthread([](const std::stop_token& stop) {
        thecalculater::util::setThreadName(thecalculater::util::currentThread, "LogFlushThread");
        std::condition_variable_any cv;
        std::mutex mutex;
        std::unique_lock<std::mutex> lock(mutex);
        while (!stop.stop_requested()) {
            spdlog::details::registry::instance()
                .flush_all();
            cv.wait_for(lock, stop, std::chrono::seconds(5), [] { return false; });
        }
    });

    if (std::atexit(thecalculater::cleanup)) {
        spdlog::warn("Failed to register atexit function.");
    }

    qInstallMessageHandler(qtMessageHandler);
}

QByteArray readResourcesFile(const std::string_view& fileName)
{
    return QResource(fileName.data()).uncompressedData();
}
} // namespace

namespace thecalculater {
void cleanup()
{
    spdlog::info("Exiting...");
    logFlushThread.request_stop();
    logFlushThread.join();
    spdlog::shutdown();
}
}

int main(int argc, char* argv[])
{
    try {
        spdlog::stopwatch timer;

        CommandLineArguments arguments = parseCommandLineArguments(argc, argv);
        if (arguments.startAction == CommandLineArguments::StartAction::NormalStart) {
            thecalculater::util::setThreadName(thecalculater::util::currentThread, "TheCalculater");
            initLogger(arguments.consoleLogLevel.value_or(spdlog::level::off), arguments.fileLogLevel);
#ifdef THECALCULATER_WINDOWS
            std::string_view consoleMode;
            switch (arguments.consoleMode) {
            case CommandLineArguments::ConsoleMode::WindowsConhost:
                consoleMode = "WindowsConhost";
                showConsole();
                break;
            case CommandLineArguments::ConsoleMode::WindowsTerminal:
                consoleMode = "WindowsTerminal";
                showWTConsole();
                break;
            default:
                consoleMode = "Off";
                break;
            };
            spdlog::info("Initialization parameters: \nconsoleMode: {}\nconsoleLogLevel: {}\nfileLogLevel: {}", consoleMode, spdlog::level::to_string_view(arguments.consoleLogLevel.value_or(spdlog::level::off)), spdlog::level::to_string_view(arguments.fileLogLevel));
#else
            spdlog::info("Initialization parameters: \nconsoleLogLevel: {}\nfileLogLevel: {}", spdlog::level::to_string_view(arguments.consoleLogLevel.value_or(spdlog::level::off)), spdlog::level::to_string_view(arguments.fileLogLevel));
#endif
            if (!arguments.consoleLogLevel.has_value()) {
                std::cout << "TheCalculater: Console logging is disabled. To enable it, please use the `--console-log` option and set it to a higher level than 'off'. Use `--console-log=off` to silence this message. Use the `--help` option for more information.\n";
            }
            thecalculater::debugging::init(argc, argv);
        }

        QApplication app(argc, argv);
        switch (arguments.startAction) {
        case CommandLineArguments::StartAction::NormalStart:
            break;
#ifdef THECALCULATER_WINDOWS
        case CommandLineArguments::StartAction::WarnInvalidArguments: {
            QMessageBox::warning(nullptr, "TheCalculater: Invalid Command Line Arguments", QString::fromStdString(std::format("{}\n\nRun '{} --help' to see command line help information.", *arguments.invalidArgumentsWarning, argv[0])), QMessageBox::StandardButtons(QMessageBox::Ok));
            std::exit(2); // NOLINT
            break;
        }
        case CommandLineArguments::StartAction::DisplayCommandLineHelp: {
            QMessageBox::question(nullptr, "TheCalculater Command Line Help Information", QString::fromStdString(*arguments.commandLineHelp), QMessageBox::StandardButtons(QMessageBox::Ok));
            std::exit(2); // NOLINT
            break;
        }
        case CommandLineArguments::StartAction::DisplayApplicationVersion: {
            QMessageBox::question(nullptr, "TheCalculater Version", THECALCULATER_VERSION_ALL " Build " THECALCULATER_BUILD, QMessageBox::StandardButtons(QMessageBox::Ok));
            std::exit(2); // NOLINT
            break;
        }
#else
        case CommandLineArguments::StartAction::WarnInvalidArguments:
            std::cerr << *arguments.invalidArgumentsWarning << "\n\nRun '" << argv[0] << " --help' to see command line help information.\n";
            break;
        case CommandLineArguments::StartAction::DisplayCommandLineHelp:
            std::cout << *arguments.commandLineHelp << "\n";
            std::exit(0); // NOLINT
            break;
        case CommandLineArguments::StartAction::DisplayApplicationVersion:
            std::cout << THECALCULATER_VERSION_ALL " Build " THECALCULATER_BUILD << "\n";
            std::exit(0); // NOLINT
            break;
#endif
        }

        if (!QResource::registerResource("./resources.rcc")) {
            spdlog::critical("Failed to load resource file");
            QMessageBox::critical(nullptr, "Failed to load resource file", "Unable to load resource file, program startup failed!\nThe resources.rcc in the program directory may have been deleted or damaged. You can try reinstalling the program to solve this problem.");
            std::exit(1); // NOLINT
        }
        spdlog::info("Resource file loaded.");

        thecalculater::settings::setSettingsFilePath("settings.json5");
        thecalculater::settings::loadConfigTemplate(thecalculater::util::parse(readResourcesFile(":/resources/data/config_template.json5").constData()));

        std::unordered_map<std::string, std::string> errors;
        thecalculater::settings::parseSettings(errors);
        if (!errors.empty()) {
            std::ostringstream oss;
            for (const auto& [key, value] : errors) {
                oss << "Key: '" << key << "' Error: '" << value << "'\n";
            }
            spdlog::error("Errors parsing settings:\n{}", oss.str());
        }

        thecalculater::translator::loadTranslations(
            thecalculater::util::parse(readResourcesFile(":/resources/data/translations.json5").constData()));
        thecalculater::translator::switchLanguage(thecalculater::settings::readString("general.language").stringRef());
        VMainWindow window;
        window.show();
        spdlog::info("Initialization done, took {}ms.", timer.elapsed_ms().count());

        return QApplication::exec();
    } catch (...) {
        thecalculater::debugging::unhandledException(std::current_exception());
    }
}
