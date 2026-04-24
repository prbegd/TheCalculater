/**
 * @file winapi.cppm
 * @author prbegd
 * @date 2026-04-04
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
/*
 * If you want to use other Windows API functions, you can add them to this module.
 * For macros, use constexpr variables or inline functions instead.
 * NOTICE: Macro replacements should be named with a leading underscore to avoid name conflicts with the original macros.
 *
 * Also includes msvcrt functions.
 */
module;

#include "TheCalculater/macros.hpp"
#ifdef THECALCULATER_WINDOWS
# include <minwindef.h>
# include <ntstatus.h>
# include <cstdio>
# include <fcntl.h>
# include <io.h>
# include <windows.h>
#endif

export module tpmm.winapi;

#ifdef THECALCULATER_WINDOWS

export namespace winapi {
    // --- types and structs ---
    using BOOL = ::BOOL;
    using BOOLEAN = ::BOOLEAN;
    using BYTE = ::BYTE;
    using WORD = ::WORD;
    using DWORD = ::DWORD;
    using UINT = ::UINT;
    using ULONG = ::ULONG;
    using LONG = ::LONG;
    using LONGLONG = ::LONGLONG;
    using ULONGLONG = ::ULONGLONG;
    using HRESULT = ::HRESULT;
    using HANDLE = ::HANDLE;
    using HWND = ::HWND;
    using HINSTANCE = ::HINSTANCE;
    using HMODULE = ::HMODULE;
    using HDC = ::HDC;
    using HBRUSH = ::HBRUSH;
    using HPEN = ::HPEN;
    using HFONT = ::HFONT;
    using HBITMAP = ::HBITMAP;
    using CHAR = ::CHAR;
    using WCHAR = ::WCHAR;
    using TCHAR = ::TCHAR;
    using LPSTR = ::LPSTR;
    using LPCSTR = ::LPCSTR;
    using LPWSTR = ::LPWSTR;
    using LPCWSTR = ::LPCWSTR;
    using LPTSTR = ::LPTSTR;
    using LPCTSTR = ::LPCTSTR;
    using POINT = ::POINT;
    using RECT = ::RECT;
    using SIZE = ::SIZE;
    using FILETIME = ::FILETIME;
    using SYSTEMTIME = ::SYSTEMTIME;
    using LPVOID = ::LPVOID;
    using PVOID = ::PVOID;
    using PBOOL = ::PBOOL;
    using SIZE_T = ::SIZE_T;
    using LPDWORD = ::LPDWORD;
    using WINBOOL = ::WINBOOL;
    using HlOCAL = ::HLOCAL;
    using PWSTR = ::PWSTR;
    using PCWSTR = ::PCWSTR;
    using FARPROC = ::FARPROC;
    using PULONG = ::PULONG;

    using LPSECURITY_ATTRIBUTES = ::LPSECURITY_ATTRIBUTES;
    using STARTUPINFOW = ::STARTUPINFOW;
    using STARTUPINFOA = ::STARTUPINFOA;
    using LPSTARTUPINFOW = ::LPSTARTUPINFOW;
    using PROCESS_INFORMATION = ::PROCESS_INFORMATION;
    using LPPROCESS_INFORMATION = ::LPPROCESS_INFORMATION;
    using LPOVERLAPPED = ::LPOVERLAPPED;
    using JOBOBJECTINFOCLASS = ::JOBOBJECTINFOCLASS;
    using JOBOBJECT_EXTENDED_LIMIT_INFORMATION = ::JOBOBJECT_EXTENDED_LIMIT_INFORMATION;
    using _EXCEPTION_POINTERS = ::_EXCEPTION_POINTERS;
    using LPTOP_LEVEL_EXCEPTION_FILTER = ::LPTOP_LEVEL_EXCEPTION_FILTER;
    using PVECTORED_EXCEPTION_HANDLER = ::PVECTORED_EXCEPTION_HANDLER;

    // --- constants --- (originally macros)
    inline constexpr DWORD _TRUE = TRUE;
    inline constexpr DWORD _FALSE = FALSE;
    inline constexpr DWORD _CP_UTF8 = CP_UTF8;
    inline constexpr DWORD _STD_OUTPUT_HANDLE = STD_OUTPUT_HANDLE;
    inline constexpr DWORD _ENABLE_VIRTUAL_TERMINAL_PROCESSING = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    inline constexpr DWORD _PIPE_ACCESS_DUPLEX = PIPE_ACCESS_DUPLEX;
    inline constexpr DWORD _PIPE_TYPE_BYTE = PIPE_TYPE_BYTE;
    inline constexpr DWORD _PIPE_WAIT = PIPE_WAIT;
    inline const HANDLE _INVALID_HANDLE_VALUE = INVALID_HANDLE_VALUE;
    inline constexpr DWORD _ERROR_PIPE_CONNECTED = ERROR_PIPE_CONNECTED;
    inline constexpr DWORD _DETACHED_PROCESS = DETACHED_PROCESS;
    inline constexpr DWORD _CREATE_BREAKAWAY_FROM_JOB = CREATE_BREAKAWAY_FROM_JOB;
    inline constexpr DWORD _JOB_OBJECT_SET_ATTRIBUTES = JOB_OBJECT_SET_ATTRIBUTES;
    inline constexpr DWORD _JOB_OBJECT_QUERY = JOB_OBJECT_QUERY;
    inline constexpr DWORD _JOB_OBJECT_LIMIT_BREAKAWAY_OK = JOB_OBJECT_LIMIT_BREAKAWAY_OK;
    inline constexpr DWORD __O_TEXT = _O_TEXT;
    inline constexpr LONG _GENERIC_READ = GENERIC_READ;
    inline constexpr DWORD _OPEN_EXISTING = OPEN_EXISTING;
    inline constexpr DWORD _THREAD_SET_LIMITED_INFORMATION = THREAD_SET_LIMITED_INFORMATION;
    inline constexpr DWORD _THREAD_QUERY_LIMITED_INFORMATION = THREAD_QUERY_LIMITED_INFORMATION;
    inline constexpr LONG _ERROR_INVALID_THREAD_ID = ERROR_INVALID_THREAD_ID;
    inline constexpr DWORD _EXCEPTION_ACCESS_VIOLATION = EXCEPTION_ACCESS_VIOLATION;
    inline constexpr DWORD _EXCEPTION_DATATYPE_MISALIGNMENT = EXCEPTION_DATATYPE_MISALIGNMENT;
    inline constexpr DWORD _EXCEPTION_ARRAY_BOUNDS_EXCEEDED = EXCEPTION_ARRAY_BOUNDS_EXCEEDED;
    inline constexpr DWORD _EXCEPTION_FLT_DENORMAL_OPERAND = EXCEPTION_FLT_DENORMAL_OPERAND;
    inline constexpr DWORD _EXCEPTION_FLT_DIVIDE_BY_ZERO = EXCEPTION_FLT_DIVIDE_BY_ZERO;
    inline constexpr DWORD _EXCEPTION_FLT_INEXACT_RESULT = EXCEPTION_FLT_INEXACT_RESULT;
    inline constexpr DWORD _EXCEPTION_FLT_INVALID_OPERATION = EXCEPTION_FLT_INVALID_OPERATION;
    inline constexpr DWORD _EXCEPTION_FLT_OVERFLOW = EXCEPTION_FLT_OVERFLOW;
    inline constexpr DWORD _EXCEPTION_FLT_STACK_CHECK = EXCEPTION_FLT_STACK_CHECK;
    inline constexpr DWORD _EXCEPTION_FLT_UNDERFLOW = EXCEPTION_FLT_UNDERFLOW;
    inline constexpr DWORD _EXCEPTION_INT_DIVIDE_BY_ZERO = EXCEPTION_INT_DIVIDE_BY_ZERO;
    inline constexpr DWORD _EXCEPTION_INT_OVERFLOW = EXCEPTION_INT_OVERFLOW;
    inline constexpr DWORD _EXCEPTION_PRIV_INSTRUCTION = EXCEPTION_PRIV_INSTRUCTION;
    inline constexpr DWORD _EXCEPTION_IN_PAGE_ERROR = EXCEPTION_IN_PAGE_ERROR;
    inline constexpr DWORD _EXCEPTION_ILLEGAL_INSTRUCTION = EXCEPTION_ILLEGAL_INSTRUCTION;
    inline constexpr DWORD _EXCEPTION_NONCONTINUABLE_EXCEPTION = EXCEPTION_NONCONTINUABLE_EXCEPTION;
    inline constexpr DWORD _EXCEPTION_STACK_OVERFLOW = EXCEPTION_STACK_OVERFLOW;
    inline constexpr DWORD _EXCEPTION_GUARD_PAGE = EXCEPTION_GUARD_PAGE;
    inline constexpr DWORD _EXCEPTION_INVALID_HANDLE = EXCEPTION_INVALID_HANDLE;
    inline constexpr DWORD _EXCEPTION_POSSIBLE_DEADLOCK = EXCEPTION_POSSIBLE_DEADLOCK;
    inline constexpr DWORD _PAGE_READWRITE = PAGE_READWRITE;
    inline constexpr DWORD _PAGE_EXECUTE_READWRITE = PAGE_EXECUTE_READWRITE;
    inline constexpr LONG _EXCEPTION_CONTINUE_EXECUTION  = EXCEPTION_CONTINUE_EXECUTION;
    inline constexpr LONG _EXCEPTION_CONTINUE_SEARCH  = EXCEPTION_CONTINUE_SEARCH;

    // --- functions --- (originally macros)

    TCAPI bool _FAILED(HRESULT hr);

    // --- functions ---

    TCAPI BOOL AllocConsole();
    TCAPI DWORD GetLastError();
    TCAPI void SetLastError(DWORD dwErrorCode);
    TCAPI BOOL SetConsoleTitleW(LPCWSTR lpConsoleTitle);
    TCAPI BOOL SetConsoleOutputCP(UINT wCodePageID);
    TCAPI HANDLE GetStdHandle(DWORD nStdHandle);
    TCAPI HMODULE GetModuleHandleW(LPCWSTR lpModuleName);
    TCAPI BOOL GetConsoleMode(HANDLE hConsoleHandle, LPDWORD lpMode);
    TCAPI BOOL SetConsoleMode(HANDLE hConsoleHandle, DWORD dwMode);
    TCAPI FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

    TCAPI HLOCAL LocalFree(HLOCAL hMem);

    TCAPI HANDLE CreateNamedPipeW(
        LPCWSTR lpName,
        DWORD dwOpenMode,
        DWORD dwPipeMode,
        DWORD nMaxInstances,
        DWORD nOutBufferSize,
        DWORD nInBufferSize,
        DWORD nDefaultTimeOut,
        LPSECURITY_ATTRIBUTES lpSecurityAttributes);

    TCAPI BOOL CreateProcessW(
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation);

    TCAPI BOOL CreateProcessA(
        LPCSTR lpApplicationName,
        LPSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCSTR lpCurrentDirectory,
        LPSTARTUPINFOA lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation);

    TCAPI BOOL CloseHandle(HANDLE hObject);
    TCAPI BOOL ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped);
    TCAPI void _ZeroMemory(PVOID Destination, SIZE_T Length);
    TCAPI BOOL IsProcessInJob(HANDLE ProcessHandle, HANDLE JobHandle, PBOOL Result);
    TCAPI HANDLE GetCurrentProcess();
    TCAPI HANDLE OpenJobObjectA(DWORD dwDesiredAccess, BOOL bInheritHandles, LPCSTR lpName);
    TCAPI BOOL SetInformationJobObject(
        HANDLE hJob,
        JOBOBJECTINFOCLASS JobObjectInformationClass,
        LPVOID lpJobObjectInformation,
        DWORD cbJobObjectInformationLength);

    TCAPI HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
    TCAPI WINBOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);

    TCAPI BOOL WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten);
    TCAPI BOOL VirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, LPDWORD lpflOldProtect);

    TCAPI HANDLE OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
    TCAPI HANDLE GetCurrentThread();
    TCAPI HRESULT SetThreadDescription(HANDLE hThread, PCWSTR lpThreadDescription);
    TCAPI HRESULT GetThreadDescription(HANDLE hThread, PWSTR* ppszThreadDescription);

    TCAPI LPTOP_LEVEL_EXCEPTION_FILTER SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
    TCAPI PVOID AddVectoredExceptionHandler(ULONG First, PVECTORED_EXCEPTION_HANDLER Handler);
    TCAPI BOOL SetThreadStackGuarantee(PULONG StackSizeInBytes);

    TCAPI int _open_osfhandle(intptr_t osfhandle, int flags);
    TCAPI std::FILE* _fdopen(int fd, const char* mode);
    TCAPI int _close(int fd);
} // namespace winapi

#endif