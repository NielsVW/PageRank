/*
 * Copyright (c) 2012 by Albert-Jan N. Yzelman
 *
 * This file is part of MulticoreBSP in C --
 *        a port of the original Java-based MulticoreBSP.
 *
 * MulticoreBSP for C is distributed as part of the original
 * MulticoreBSP and is free software: you can redistribute
 * it and/or modify it under the terms of the GNU Lesser 
 * General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 * MulticoreBSP is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU Lesser General Public 
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General 
 * Public License along with MulticoreBSP. If not, see
 * <http://www.gnu.org/licenses/>.
 */

//public function defs,
//for documentation see the .h file:
#include "mcbsp.h"

//affinity interface
#include "mcbsp-affinity.h"

#ifdef __GNUC__
 #ifndef _GNU_SOURCE
  #define _GNU_SOURCE
 #endif
#endif

//hidden includes (from library user's perspective)
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <pthread.h>

#ifdef __MACH__
 #include <mach/mach.h>
 #include <mach/mach_error.h>
 #include <mach/thread_policy.h>
#endif

#ifdef _WIN32
 #include <windows.h>
#endif

#include "mcinternal.h"
#include "mcutil.h"

