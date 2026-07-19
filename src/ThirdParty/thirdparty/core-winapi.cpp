/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "thecalculater/macros.hpp"
#ifdef THECALCULATER_WINDOWS
# include <fcntl.h>
# include <io.h>
# include <minwindef.h>
# include <ntstatus.h>
# include <windows.h>
#endif
module thirdparty.core;

#ifdef THECALCULATER_WINDOWS
namespace winapi {
const HANDLE _INVALID_HANDLE_VALUE = INVALID_HANDLE_VALUE;

void _ZeroMemory(PVOID Destination, SIZE_T Length)
{
    ::memset(Destination, 0, Length);
}
} // namespace winapi
#endif