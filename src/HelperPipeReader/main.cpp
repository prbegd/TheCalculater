/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "thecalculater/macros.hpp"
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

#ifdef THECALCULATER_WINDOWS
# include <errhandlingapi.h>
# include <handleapi.h>
# include <windows.h>
# include <winerror.h>

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
        std::_Exit(130);
    });

    std::wstring pipeName = std::wstring(argv[1], argv[1] + strlen(argv[1]));
    HANDLE hPipe { };
    for (unsigned i = 0;; i++) {
        hPipe = CreateFileW(pipeName.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);

        if (hPipe != INVALID_HANDLE_VALUE) {
            break;
        }

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