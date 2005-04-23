/*
 * lib_profiling.c - Internalcall methods for the "DotGNU.Profiling" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "engine.h"
#include "lib_defs.h"
#include "il_utils.h"
#include "il_sysio.h"
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_PWD_H
#include <pwd.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static void StartProfiling();
 */
void _IL_Profiling_StartProfiling(ILExecThread *thread)
{
#ifdef ENHANCED_PROFILER
	thread->profilingEnabled = 1;
#endif
}

/*
 * public static void StopProfiling();
 */
void _IL_Profiling_StopProfiling(ILExecThread *thread)
{
#ifdef ENHANCED_PROFILER
	thread->profilingEnabled = 0;
#endif
}

/*
 * public static bool IsProfilingEnabled();
 */
ILBool _IL_Profiling_IsProfilingEnabled(ILExecThread *thread)
{
#ifdef ENHANCED_PROFILER
	return (ILBool) thread->profilingEnabled;
#else
	return (ILBool) 0;
#endif
}

/*
 * public static bool IsProfilingSupported();
 */
ILBool _IL_Profiling_IsProfilingSupported(ILExecThread *thread)
{
#ifdef ENHANCED_PROFILER
	return (ILBool) (ILCoderGetFlags (thread->process->coder) & IL_CODER_FLAG_METHOD_PROFILE);
#else
	return (ILBool) 0;
#endif
}

#ifdef	__cplusplus
};
#endif
