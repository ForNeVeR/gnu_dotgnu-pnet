/*
 * lib_thread.c - Internalcall methods for "System.Threading.*".
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static void Enter(Object obj);
 */
static void System_Threading_Monitor_Enter(ILExecThread *thread,
										   ILObject *obj)
{
	/* The current implementation is single-threaded,
	   so we have nothing to do here */
}

/*
 * public static void Exit(Object obj);
 */
static void System_Threading_Monitor_Exit(ILExecThread *thread,
										  ILObject *obj)
{
	/* The current implementation is single-threaded,
	   so we have nothing to do here */
}

/*
 * Method table for the "System.Threading.Monitor" class.
 */
IL_METHOD_BEGIN(_ILSystemThreadingMonitorMethods)
	IL_METHOD("Enter",
			  "(oSystem.Object;)V",
			  System_Threading_Monitor_Enter)
	IL_METHOD("Exit",
			  "(oSystem.Object;)V",
			  System_Threading_Monitor_Exit)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
