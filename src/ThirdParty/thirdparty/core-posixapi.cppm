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
#ifdef THECALCULATER_POSIX
# include <pthread.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# ifdef THECALCULATER_APPLE
#  include <sys/_types/_pid_t.h>
# endif
#endif

export module thirdparty.core:posixapi;

#ifdef THECALCULATER_POSIX
export namespace posixapi {
using ::execvp;
using ::fork;
using ::getpid;
using ::pthread_getname_np;
using ::pthread_self;
using ::pthread_setname_np;
using ::setsid;

using pthread_t = ::pthread_t;
using pid_t = ::pid_t;
}
#endif