/*
 * w32_defs.c - Thread definitions for using Win32 threads.
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

#include "thr_defs.h"

#ifdef IL_USE_WIN32_THREADS

/* Don't include windows.h here otherwise it will override
   CreateThread macros from gc.h.  It should be included by thr_choose.h */
   
#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Thread-specific key that is used to store and retrieve the thread object.
 */
DWORD _ILThreadObjectKey;

/*
 *	Sets the thread priority.
 */
void _ILThreadSetPriority(ILThread *thread, int priority)
{
	switch (priority)
	{
	case IL_TP_LOWEST:
		SetThreadPriority(thread->handle, THREAD_PRIORITY_LOWEST);
		break;
	case IL_TP_BELOW_NORMAL:
		SetThreadPriority(thread->handle, THREAD_PRIORITY_BELOW_NORMAL);
		break;
	case IL_TP_NORMAL:
		SetThreadPriority(thread->handle, THREAD_PRIORITY_NORMAL);
		break;
	case IL_TP_ABOVE_NORMAL:
		SetThreadPriority(thread->handle, THREAD_PRIORITY_ABOVE_NORMAL);
		break;
	case IL_TP_HIGHEST:
		SetThreadPriority(thread->handle, THREAD_PRIORITY_HIGHEST);
		break;
	}	
}

/*
 *	Gets the thread priority.
 */
int _ILThreadGetPriority(ILThread *thread)
{
	switch (GetThreadPriority(thread->handle))
	{
	case THREAD_PRIORITY_IDLE:
	case THREAD_PRIORITY_LOWEST:		
		return IL_TP_LOWEST;

	case THREAD_PRIORITY_BELOW_NORMAL:		
		return IL_TP_BELOW_NORMAL;

	case THREAD_PRIORITY_NORMAL:		
		return IL_TP_NORMAL;

	case THREAD_PRIORITY_ABOVE_NORMAL:		
		return IL_TP_ABOVE_NORMAL;

	case THREAD_PRIORITY_HIGHEST:
	case THREAD_PRIORITY_TIME_CRITICAL:
		return IL_TP_HIGHEST;

	default:
		return IL_TP_NORMAL;
	}
}

void _ILThreadInitSystem(ILThread *mainThread)
{
	/* Allocate a TLS key for storing thread objects */
	_ILThreadObjectKey = TlsAlloc();

	/* Initialize the "main" thread's handle and identifier.  We have
	   to duplicate the thread handle because "GetCurrentThread()" returns
	   a pseudo-handle and not a real one.  We need the real one */
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
					GetCurrentProcess(), (HANDLE *)(&(mainThread->handle)),
					0, 0, DUPLICATE_SAME_ACCESS);
	mainThread->identifier = GetCurrentThreadId();
}

/*
 * Main Win32 entry point for a thread.
 */
static DWORD WINAPI ThreadStart(LPVOID arg)
{
	ILThread *thread = (ILThread *)arg;

	/* Attach the thread object to the thread */
	TlsSetValue(_ILThreadObjectKey, thread);

	/* Run the thread */
	_ILThreadRun(thread);

	/* Exit from the thread */
	return 0;
}

int _ILThreadCreateSystem(ILThread *thread)
{
	thread->handle = CreateThread(NULL, 0, ThreadStart,
								  thread, 0, (DWORD *)&(thread->identifier));
	return (thread->handle != NULL);
}

/*
 * Note: this implementation is not fully atomic.  There is a
 * window of opportunity between when the current thread notices
 * that the condition is signalled and when the mutex is regained.
 * The caller is expected to code around this.
 */
int _ILCondVarTimedWait(_ILCondVar *cond, _ILCondMutex *mutex, ILUInt32 ms)
{
	DWORD result;
	if(ms != IL_MAX_UINT32)
	{
		result = SignalObjectAndWait(*mutex, *cond, (DWORD)ms, FALSE);
	}
	else
	{
		result = SignalObjectAndWait(*mutex, *cond, INFINITE, FALSE);
	}
	WaitForSingleObject(*mutex, INFINITE);
	return (result == WAIT_OBJECT_0);
}

#ifdef	__cplusplus
};
#endif

#endif	/* IL_USE_WIN32_THREADS */
