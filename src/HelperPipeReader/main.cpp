/**
 * @file main.cpp
 * @author prbegd
 * @brief HelperPipeReader.exe
 * @date 2025-09-30
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <errhandlingapi.h>
#include <handleapi.h>
#include <windows.h>
#include <winerror.h>

const char* programName = "HelperPipeReader.exe";

int main(int argc, char* argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        std::cerr << "Helper to read data from pipe and print data to the console.\nUsage: " << argv[0] << "  <pipe_name>\n";
        return 2;
    }
    programName = argv[0];
    (void)std::signal(SIGINT, [](int) {
        std::cerr << "\n\033[91mCtrl+C pressed!\033[93m\nNote: Interrupting this console won't influence TheCalculater from running. \033[0m\n";
        _exit(130);
    });

    std::wstring pipeName = std::wstring(argv[1], argv[1] + strlen(argv[1]));
    HANDLE hPipe {};
    for (unsigned i = 0;; i++) {
        hPipe = CreateFileW(pipeName.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        if (i == 5) {
            std::cerr << argv[0] << ": Unable to connect console: Errno " << GetLastError() << ". Retry failed, exiting...\n";
            return 1;
        }

        if (GetLastError() == ERROR_PIPE_BUSY) {
            std::cerr << argv[0] << ": Unable to connect console: Pipe is busy. Retrying... (" << (i + 1) << "/5)\n";
        } else {
            std::cerr << argv[0] << ": Unable to connect console: Errno " << GetLastError() << ". Retrying... (" << (i + 1) << "/5)\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    SetConsoleOutputCP(CP_UTF8);
    char buffer[4096];
    DWORD bytesRead = 0;
    while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
        buffer[bytesRead] = '\0';
        std::cout.write(buffer, bytesRead);
        std::cout.flush();
    }
    CloseHandle(hPipe);
}
#else
int main(int argc, char* argv[])
{
    std::cerr << "This program is only supported on Windows.\n";
    return 1;
}
#endif