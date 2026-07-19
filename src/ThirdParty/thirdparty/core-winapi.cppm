/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
/*
 * If you want to use other Windows API functions, you can add them to this module.
 * For macros, use constexpr variables or inline functions instead.
 * NOTICE: Macro replacements should be named with a leading underscore to avoid name conflicts with the original macros.
 *
 * Also includes msvcrt functions.
 */
module;

#include "thecalculater/macros.hpp"
#ifdef THECALCULATER_WINDOWS
# include <cstdio> // IWYU pragma: keep
# include <fcntl.h>
# include <io.h>
# include <minwindef.h>
# include <ntstatus.h>
# include <windows.h>
#endif

export module thirdparty.core:winapi;

#ifdef THECALCULATER_WINDOWS

export namespace winapi {
// --- types and structs ---
using ::BOOL;
using ::BOOLEAN;
using ::BYTE;
using ::CHAR;
using ::DWORD;
using ::FARPROC;
using ::FILETIME;
using ::HANDLE;
using ::HBITMAP;
using ::HBRUSH;
using ::HDC;
using ::HFONT;
using ::HINSTANCE;
using ::HLOCAL;
using ::HMODULE;
using ::HPEN;
using ::HRESULT;
using ::HWND;
using ::LONG;
using ::LONGLONG;
using ::LPCSTR;
using ::LPCTSTR;
using ::LPCWSTR;
using ::LPDWORD;
using ::LPSTR;
using ::LPTSTR;
using ::LPVOID;
using ::LPWSTR;
using ::PBOOL;
using ::PCWSTR;
using ::POINT;
using ::PULONG;
using ::PVOID;
using ::PWSTR;
using ::RECT;
using ::SIZE;
using ::SIZE_T;
using ::SYSTEMTIME;
using ::TCHAR;
using ::UINT;
using ::ULONG;
using ::ULONGLONG;
using ::WCHAR;
using ::WINBOOL;
using ::WORD;

using ::_EXCEPTION_POINTERS;
using ::JOBOBJECT_EXTENDED_LIMIT_INFORMATION;
using ::JOBOBJECTINFOCLASS;
using ::LPOVERLAPPED;
using ::LPPROCESS_INFORMATION;
using ::LPSECURITY_ATTRIBUTES;
using ::LPSTARTUPINFOW;
using ::LPTOP_LEVEL_EXCEPTION_FILTER;
using ::PROCESS_INFORMATION;
using ::PVECTORED_EXCEPTION_HANDLER;
using ::STARTUPINFOA;
using ::STARTUPINFOW;

// --- constants --- (originally macros)
constexpr inline DWORD _TRUE = TRUE;
constexpr inline DWORD _FALSE = FALSE;
constexpr inline DWORD _CP_UTF8 = CP_UTF8;
constexpr inline DWORD _STD_OUTPUT_HANDLE = STD_OUTPUT_HANDLE;
constexpr inline DWORD _ENABLE_VIRTUAL_TERMINAL_PROCESSING = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
constexpr inline DWORD _PIPE_ACCESS_DUPLEX = PIPE_ACCESS_DUPLEX;
constexpr inline DWORD _PIPE_TYPE_BYTE = PIPE_TYPE_BYTE;
constexpr inline DWORD _PIPE_WAIT = PIPE_WAIT;
extern const HANDLE _INVALID_HANDLE_VALUE;
constexpr inline DWORD _ERROR_PIPE_CONNECTED = ERROR_PIPE_CONNECTED;
constexpr inline DWORD _DETACHED_PROCESS = DETACHED_PROCESS;
constexpr inline DWORD _CREATE_BREAKAWAY_FROM_JOB = CREATE_BREAKAWAY_FROM_JOB;
constexpr inline DWORD _JOB_OBJECT_SET_ATTRIBUTES = JOB_OBJECT_SET_ATTRIBUTES;
constexpr inline DWORD _JOB_OBJECT_QUERY = JOB_OBJECT_QUERY;
constexpr inline DWORD _JOB_OBJECT_LIMIT_BREAKAWAY_OK = JOB_OBJECT_LIMIT_BREAKAWAY_OK;
constexpr inline DWORD __O_TEXT = _O_TEXT;
constexpr inline LONG _GENERIC_READ = GENERIC_READ;
constexpr inline DWORD _OPEN_EXISTING = OPEN_EXISTING;
constexpr inline DWORD _THREAD_SET_LIMITED_INFORMATION = THREAD_SET_LIMITED_INFORMATION;
constexpr inline DWORD _THREAD_QUERY_LIMITED_INFORMATION = THREAD_QUERY_LIMITED_INFORMATION;
constexpr inline LONG _ERROR_INVALID_THREAD_ID = ERROR_INVALID_THREAD_ID;
constexpr inline DWORD _EXCEPTION_ACCESS_VIOLATION = EXCEPTION_ACCESS_VIOLATION;
constexpr inline DWORD _EXCEPTION_DATATYPE_MISALIGNMENT = EXCEPTION_DATATYPE_MISALIGNMENT;
constexpr inline DWORD _EXCEPTION_ARRAY_BOUNDS_EXCEEDED = EXCEPTION_ARRAY_BOUNDS_EXCEEDED;
constexpr inline DWORD _EXCEPTION_FLT_DENORMAL_OPERAND = EXCEPTION_FLT_DENORMAL_OPERAND;
constexpr inline DWORD _EXCEPTION_FLT_DIVIDE_BY_ZERO = EXCEPTION_FLT_DIVIDE_BY_ZERO;
constexpr inline DWORD _EXCEPTION_FLT_INEXACT_RESULT = EXCEPTION_FLT_INEXACT_RESULT;
constexpr inline DWORD _EXCEPTION_FLT_INVALID_OPERATION = EXCEPTION_FLT_INVALID_OPERATION;
constexpr inline DWORD _EXCEPTION_FLT_OVERFLOW = EXCEPTION_FLT_OVERFLOW;
constexpr inline DWORD _EXCEPTION_FLT_STACK_CHECK = EXCEPTION_FLT_STACK_CHECK;
constexpr inline DWORD _EXCEPTION_FLT_UNDERFLOW = EXCEPTION_FLT_UNDERFLOW;
constexpr inline DWORD _EXCEPTION_INT_DIVIDE_BY_ZERO = EXCEPTION_INT_DIVIDE_BY_ZERO;
constexpr inline DWORD _EXCEPTION_INT_OVERFLOW = EXCEPTION_INT_OVERFLOW;
constexpr inline DWORD _EXCEPTION_PRIV_INSTRUCTION = EXCEPTION_PRIV_INSTRUCTION;
constexpr inline DWORD _EXCEPTION_IN_PAGE_ERROR = EXCEPTION_IN_PAGE_ERROR;
constexpr inline DWORD _EXCEPTION_ILLEGAL_INSTRUCTION = EXCEPTION_ILLEGAL_INSTRUCTION;
constexpr inline DWORD _EXCEPTION_NONCONTINUABLE_EXCEPTION = EXCEPTION_NONCONTINUABLE_EXCEPTION;
constexpr inline DWORD _EXCEPTION_STACK_OVERFLOW = EXCEPTION_STACK_OVERFLOW;
constexpr inline DWORD _EXCEPTION_GUARD_PAGE = EXCEPTION_GUARD_PAGE;
constexpr inline DWORD _EXCEPTION_INVALID_HANDLE = EXCEPTION_INVALID_HANDLE;
constexpr inline DWORD _EXCEPTION_POSSIBLE_DEADLOCK = EXCEPTION_POSSIBLE_DEADLOCK;
constexpr inline DWORD _PAGE_READWRITE = PAGE_READWRITE;
constexpr inline DWORD _PAGE_EXECUTE_READWRITE = PAGE_EXECUTE_READWRITE;
constexpr inline LONG _EXCEPTION_CONTINUE_EXECUTION = EXCEPTION_CONTINUE_EXECUTION;
constexpr inline LONG _EXCEPTION_CONTINUE_SEARCH = EXCEPTION_CONTINUE_SEARCH;
constexpr inline DWORD __MAX_PATH = _MAX_PATH;

// --- functions --- (originally macros)

constexpr inline bool _FAILED(HRESULT hr)
{
    return FAILED(hr);
}
void _ZeroMemory(PVOID Destination, SIZE_T Length);

// --- functions ---

using ::AddVectoredExceptionHandler;
using ::AllocConsole;
using ::CloseHandle;
using ::ConnectNamedPipe;
using ::CreateFileW;
using ::CreateNamedPipeW;
using ::CreateProcessA;
using ::CreateProcessW;
using ::GetConsoleMode;
using ::GetCurrentProcess;
using ::GetCurrentThread;
using ::GetLastError;
using ::GetModuleFileNameW;
using ::GetModuleHandleW;
using ::GetProcAddress;
using ::GetStdHandle;
using ::GetThreadDescription;
using ::IsProcessInJob;
using ::LocalFree;
using ::OpenJobObjectA;
using ::OpenThread;
using ::ReadFile;
using ::SetConsoleMode;
using ::SetConsoleOutputCP;
using ::SetConsoleTitleW;
using ::SetInformationJobObject;
using ::SetLastError;
using ::SetThreadDescription;
using ::SetThreadStackGuarantee;
using ::SetUnhandledExceptionFilter;
using ::VirtualProtect;
using ::WriteProcessMemory;

using ::_close;
using ::_fdopen;
using ::_open_osfhandle;
} // namespace winapi

#endif