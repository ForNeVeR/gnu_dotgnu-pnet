/*
 * interlocked.h - Implementation of interlocked functions.
 *
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
 *
 * Authors: Thong Nguyen (tum@veridicus.com)
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

#include "interrupt.h"

#include "thr_defs.h"
#include "il_thread.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#ifndef IL_INTERRUPT_SUPPORTS

void _ILInterruptInit()
{
}

void _ILInterruptDeinit()
{
}

#endif

#ifdef IL_INTERRUPT_SUPPORTS_ILLEGAL_MEMORY_ACCESS

int ILThreadRegisterIllegalMemoryAccessHandler(ILThread *thread, ILIllegalMemoryAccessHandler handler)
{
	thread->illegalMemoryAccessHandler = handler;

	return 0;
}

int ILThreadUnregisterIllegalMemoryAccessHandler(ILThread *thread, ILIllegalMemoryAccessHandler handler)
{
	if (thread->illegalMemoryAccessHandler == handler)
	{
		thread->illegalMemoryAccessHandler = 0;
	}

	return 0;
}

#endif

#ifdef	__cplusplus
};
#endif
