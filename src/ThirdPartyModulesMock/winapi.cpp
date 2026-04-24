/**
 * @file winapi.cpp
 * @author prbegd
 * @date 2026-04-19
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "TheCalculater/macros.hpp"
#ifdef THECALCULATER_WINDOWS
# include <cstdio>
# include <fcntl.h>
# include <io.h>
# include <minwindef.h>
# include <ntstatus.h>
# include <windows.h>
#endif

module tpmm.winapi;
#ifdef THECALCULATER_WINDOWS
namespace winapi {
    bool _FAILED(HRESULT hr)
    {
        return FAILED(hr);
    }
    BOOL AllocConsole()
    {
        return ::AllocConsole();
    }

    DWORD GetLastError()
    {
        return ::GetLastError();
    }

    void SetLastError(DWORD dwErrorCode)
    {
        ::SetLastError(dwErrorCode);
    }

    BOOL SetConsoleTitleW(LPCWSTR lpConsoleTitle)
    {
        return ::SetConsoleTitleW(lpConsoleTitle);
    }

    BOOL SetConsoleOutputCP(UINT wCodePageID)
    {
        return ::SetConsoleOutputCP(wCodePageID);
    }

    HANDLE GetStdHandle(DWORD nStdHandle)
    {
        return ::GetStdHandle(nStdHandle);
    }
    HMODULE GetModuleHandleW(LPCWSTR lpModuleName)
    {
        return ::GetModuleHandleW(lpModuleName);
    }

    BOOL GetConsoleMode(HANDLE hConsoleHandle, LPDWORD lpMode)
    {
        return ::GetConsoleMode(hConsoleHandle, lpMode);
    }

    BOOL SetConsoleMode(HANDLE hConsoleHandle, DWORD dwMode)
    {
        return ::SetConsoleMode(hConsoleHandle, dwMode);
    }

    FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
    {
        return ::GetProcAddress(hModule, lpProcName);
    }

    HLOCAL LocalFree(HLOCAL hMem)
    {
        return ::LocalFree(hMem);
    }

    HANDLE CreateNamedPipeW(
        LPCWSTR lpName,
        DWORD dwOpenMode,
        DWORD dwPipeMode,
        DWORD nMaxInstances,
        DWORD nOutBufferSize,
        DWORD nInBufferSize,
        DWORD nDefaultTimeOut,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes)
    {
        return ::CreateNamedPipeW(lpName, dwOpenMode, dwPipeMode,
                                  nMaxInstances, nOutBufferSize, nInBufferSize,
                                  nDefaultTimeOut, lpSecurityAttributes);
    }

    BOOL CreateProcessW(
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation)
    {
        return ::CreateProcessW(lpApplicationName, lpCommandLine,
                                lpProcessAttributes, lpThreadAttributes,
                                bInheritHandles, dwCreationFlags, lpEnvironment,
                                lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    }

    BOOL CreateProcessA(
        LPCSTR lpApplicationName,
        LPSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCSTR lpCurrentDirectory,
        LPSTARTUPINFOA lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation)
    {
        return ::CreateProcessA(lpApplicationName, lpCommandLine,
                                lpProcessAttributes, lpThreadAttributes,
                                bInheritHandles, dwCreationFlags, lpEnvironment,
                                lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    }

    BOOL CloseHandle(HANDLE hObject)
    {
        return ::CloseHandle(hObject);
    }

    BOOL ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped)
    {
        return ::ConnectNamedPipe(hNamedPipe, lpOverlapped);
    }

    void _ZeroMemory(PVOID Destination, SIZE_T Length)
    {
        ::memset(Destination, 0, Length);
    }

    BOOL IsProcessInJob(HANDLE ProcessHandle, HANDLE JobHandle, PBOOL Result)
    {
        return ::IsProcessInJob(ProcessHandle, JobHandle, Result);
    }

    HANDLE GetCurrentProcess()
    {
        return ::GetCurrentProcess();
    }

    HANDLE OpenJobObjectA(DWORD dwDesiredAccess, BOOL bInheritHandles, LPCSTR lpName)
    {
        return ::OpenJobObjectA(dwDesiredAccess, bInheritHandles, lpName);
    }

    BOOL SetInformationJobObject(
        HANDLE hJob,
        JOBOBJECTINFOCLASS JobObjectInformationClass,
        LPVOID lpJobObjectInformation,
        DWORD cbJobObjectInformationLength)
    {
        return ::SetInformationJobObject(hJob, JobObjectInformationClass,
                                         lpJobObjectInformation, cbJobObjectInformationLength);
    }
    HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
    {
        return ::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
                             lpSecurityAttributes, dwCreationDisposition,
                             dwFlagsAndAttributes, hTemplateFile);
    }
    WINBOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
    {
        return ::ReadFile(hFile, lpBuffer, nNumberOfBytesToRead,
                          lpNumberOfBytesRead, lpOverlapped);
    }

    BOOL WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten)
    {
        return ::WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
    }

    BOOL VirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, LPDWORD lpflOldProtect)
    {
        return ::VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect);
    }
    HANDLE OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
    {
        return ::OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
    }
    HANDLE GetCurrentThread()
    {
        return ::GetCurrentThread();
    }
    HRESULT SetThreadDescription(HANDLE hThread, PCWSTR lpThreadDescription)
    {
        return ::SetThreadDescription(hThread, lpThreadDescription);
    }
    HRESULT GetThreadDescription(HANDLE hThread, PWSTR* ppszThreadDescription)
    {
        return ::GetThreadDescription(hThread, ppszThreadDescription);
    }

    LPTOP_LEVEL_EXCEPTION_FILTER SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
    {
        return ::SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
    }
    PVOID AddVectoredExceptionHandler(ULONG First, PVECTORED_EXCEPTION_HANDLER Handler)
    {
        return ::AddVectoredExceptionHandler(First, Handler);
    }
    BOOL SetThreadStackGuarantee(PULONG StackSizeInBytes)
    {
        return ::SetThreadStackGuarantee(StackSizeInBytes);
    }

    int _open_osfhandle(intptr_t osfhandle, int flags)
    {
        return ::_open_osfhandle(osfhandle, flags);
    }
    std::FILE* _fdopen(int fd, const char* mode)
    {
        return ::_fdopen(fd, mode);
    }
    int _close(int fd)
    {
        return ::_close(fd);
    }
} // namespace winapi
#endif