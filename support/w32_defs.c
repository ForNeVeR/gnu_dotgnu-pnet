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

/*
 * This function is only used for initializing an ILThread
 * structure for threads not created by pnet.
 * This Thread MUST NOT BE USED to run managed code or create
 * managed objects because this thread is not controled by the GC.
 */
void _ILThreadInitHandleSelf(ILThread *thread)
{
	/* Initialize the thread's handle and identifier.  We have
	   to duplicate the thread handle because "GetCurrentThread()" returns
	   a pseudo-handle and not a real one. We need the real one */
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
					GetCurrentProcess(), (HANDLE *)(&(thread->handle)),
					0, 0, DUPLICATE_SAME_ACCESS);
	thread->identifier = GetCurrentThreadId();
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
 * This function is simply a wrapper around the WaitForSingleObject function
 * to handle the errors and convert them to the unified error codes.
 */
int	_ILWaitHandleTimedWait(HANDLE handle, ILUInt32 ms)
{
	if((ms == IL_MAX_UINT32) || (ms <= IL_MAX_INT32))
	{
		DWORD result;

		if(ms == IL_MAX_UINT32)
		{
			result = WaitForSingleObject(handle, INFINITE);
		}
		else
		{
			result = WaitForSingleObject(handle, (DWORD)ms);
		}

		if(result == WAIT_OBJECT_0)
		{
			return IL_THREAD_OK;
		}
		else
		{
			switch(result)
			{
				case WAIT_ABANDONED:
				{
					return IL_THREAD_ERR_ABANDONED;
				}
				break;

				case WAIT_TIMEOUT:
				{
					return IL_THREAD_BUSY;
				}
				break;
			}
		}
		return IL_THREAD_ERR_UNKNOWN;
	}
	else
	{
		return IL_THREAD_ERR_INVALID_TIMEOUT;
	}
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

/*
 * Release a number of waiting threads from the count semaphore.
 */
int _ILCountSemaphoreSignalCount(_ILCountSemaphore *sem, ILUInt32 count)
{
	int result = IL_THREAD_OK;

	/* Lock the count semaphore object. */
	_ILMutexLock(&(sem->_lock));

	sem->_waiting -= count;

	result = (ReleaseSemaphore(sem->_sem, (LONG)count, 0) != 0) ? 
						IL_THREAD_OK : IL_THREAD_ERR_UNKNOWN;

	/* Unlock the count semaphore object. */
	_ILMutexUnlock(&(sem->_lock));

	return result;
}

/*
 * Release all waiting threads from the count semaphore.
 */
int _ILCountSemaphoreSignalAll(_ILCountSemaphore *sem)
{
	int result = IL_THREAD_OK;

	if(sem->_waiting > 0)
	{
		/* Lock the count semaphore object. */
		_ILMutexLock(&(sem->_lock));

		/* We have to recheck because of possible race conditions. */
		if(sem->_waiting > 0)
		{
			result = (ReleaseSemaphore(sem->_sem, sem->_waiting, 0) != 0) ? 
						IL_THREAD_OK : IL_THREAD_ERR_UNKNOWN;

			sem->_waiting = 0;
		}

		/* Unlock the count semaphore object. */
		_ILMutexUnlock(&(sem->_lock));
	}
	return result;
}

/*
 * Wait on a count semaphore.
 */
int _ILCountSemaphoreTimedWait(_ILCountSemaphore *sem, ILUInt32 ms)
{
	if((ms == IL_MAX_UINT32) || (ms <= IL_MAX_INT32))
	{
		int result = IL_THREAD_OK;

		/* Lock the count semaphore object. */
		_ILMutexLock(&(sem->_lock));

		sem->_waiting += 1;

		/* Unlock the count semaphore object. */
		_ILMutexUnlock(&(sem->_lock));

		if((result = _ILWaitHandleTimedWait(sem->_sem, ms)) != IL_THREAD_OK)
		{
			/* We have to decrement the counter again because the call failed. */

			/* Lock the count semaphore object. */
			_ILMutexLock(&(sem->_lock));

			sem->_waiting -= 1;

			/* Unlock the count semaphore object. */
			_ILMutexUnlock(&(sem->_lock));
		}
		return result;
	}
	else
	{
		return IL_THREAD_ERR_INVALID_TIMEOUT;
	}
}

int _ILMonitorTimedTryEnter(_ILMonitor *mon, ILUInt32 ms)
{
	DWORD result;

	if((ms == IL_MAX_UINT32) || (ms <= IL_MAX_INT32))
	{
		result = _ILWaitHandleTimedWait(mon->_mutex, ms);
	}
	else
	{
		result = IL_THREAD_ERR_INVALID_TIMEOUT;
	}
	return result;
}

int _ILMonitorTimedWait(_ILMonitor *mon, ILUInt32 ms,
						ILMonitorPredicate predicate, void *arg)
{
	DWORD result;

	if((ms == IL_MAX_UINT32) || (ms <= IL_MAX_INT32))
	{
		/* Increment the number of waiters. */
		/* Lock the count semaphore object. */
		_ILMutexLock(&(mon->_sem._lock));

		mon->_sem._waiting += 1;

		/* Unlock the count semaphore object. */
		_ILMutexUnlock(&(mon->_sem._lock));

		/* Unlock the mutex and wait on the semaphore. */
		result = SignalObjectAndWait(mon->_mutex, mon->_sem._sem,
									 (DWORD)ms, FALSE);

		if(result == WAIT_OBJECT_0)
		{
			/* Now wait until the mutex can be acquired. */
			result = _ILWaitHandleWait(mon->_mutex);
			if(result == IL_THREAD_OK)
			{
				/*
				 * Call the predicate function which should return != 0 on
				 * windows platforms in all cases.
				 */
				if(!predicate(arg))
				{
					return IL_THREAD_ERR_UNKNOWN;
				}
			}
			return result;
		}
		else
		{
			/* Lock the count semaphore object. */
			_ILMutexLock(&(mon->_sem._lock));

			if(mon->_sem._waiting > 0)
			{
				/* Decrement the number of waiters because we didn't get */
				/* signaled and we didn't miss the signal.*/
				mon->_sem._waiting -= 1;
			}

			/* Unlock the count semaphore object. */
			_ILMutexUnlock(&(mon->_sem._lock));

			if(result == WAIT_TIMEOUT)
			{
				/* The timeout expired on waiting to get pulsed. */
				result = _ILWaitHandleWait(mon->_mutex);

				if(result == IL_THREAD_OK)
				{
					/*
					 * Call the predicate function which should return != 0 on
					 * windows platforms in all cases.
					 */
					if(!predicate(arg))
					{
						return IL_THREAD_ERR_UNKNOWN;
					}
					/* Returm that we timed out. */
					return IL_THREAD_BUSY;
				}
				/* In cas of an other error simpy return the error. */
				return result;
			}
			else
			{
				return IL_THREAD_ERR_UNKNOWN;
			}
		}
	}
	else
	{
		return IL_THREAD_ERR_INVALID_TIMEOUT;
	}
}


#ifdef	__cplusplus
};
#endif

#endif	/* IL_USE_WIN32_THREADS */
