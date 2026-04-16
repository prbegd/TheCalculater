/**
 * @file posixapi.cppm
 * @author prbegd
 * @date 2026-04-12
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;

#include "TheCalculater/macros.hpp"
#ifdef THECALCULATER_POSIX
# include <pthread.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# ifdef THECALCULATER_APPLE
#  include <sys/_types/_pid_t.h>
# endif
#endif

export module tpmm.posixapi;

#ifdef THECALCULATER_POSIX
export namespace posixapi {
    using ::pthread_setname_np;
    using ::pthread_getname_np;
    using ::pthread_self;
    using ::getpid;
    using ::fork;
    using ::setsid;
    using ::execvp;

    using pthread_t = ::pthread_t;
    using pid_t = ::pid_t;
}
#endif