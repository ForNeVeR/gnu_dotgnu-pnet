/*
 * wait.c - Wait handle objects for the threading sub-system.
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

/*

Note: the code in this module is generic to all platforms.  It implements
the correct CLI wait handle semantics based on the primitives in "*_defs.h".
You normally won't need to modify or replace this file when porting.

*/

#include "thr_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

int ILWaitHandleClose(ILWaitHandle *handle)
{
	int result = (*(handle->closeFunc))(handle);
	if(result == IL_WAITCLOSE_FREE)
	{
		ILFree(handle);
	}
	return (result != IL_WAITCLOSE_OWNED);
}

/*
 * Enter the "wait/sleep/join" state on the current thread.
 */
int _ILEnterWait(ILThread *thread)
{
	int result = 0;

	_ILMutexLock(&(thread->lock));

	if((thread->state & (IL_TS_ABORT_REQUESTED)) != 0)
	{
		result = IL_WAIT_ABORTED;

		_ILWakeupCancelInterrupt(&(thread->wakeup));
		thread->state &= ~(IL_TS_INTERRUPTED);
	}
	else
	{
		thread->state |= IL_TS_WAIT_SLEEP_JOIN;
	}

	_ILMutexUnlock(&(thread->lock));

	return result;
}

/*
 * Leave the "wait/sleep/join" state on the current thread.
 */
int _ILLeaveWait(ILThread *thread, int result)
{
	_ILMutexLock(&(thread->lock));

	/* The double checks for result == IL_WAIT_* are needed to bubble down
	   results even if the threadstate has been reset which may happen
	   if enter/leavewait are called recursively */

	/* Abort has more priority over interrupt */
	if((thread->state & (IL_TS_ABORT_REQUESTED)) != 0
		|| result == IL_WAIT_ABORTED)
	{
		result = IL_WAIT_ABORTED;
	}
	else if((thread->state & IL_TS_INTERRUPTED) != 0
		|| result == IL_WAIT_INTERRUPTED)
	{		 
		result = IL_WAIT_INTERRUPTED;
	}

	_ILWakeupCancelInterrupt(&(thread->wakeup));
	
	if ((thread->state & IL_TS_SUSPEND_REQUESTED) != 0)
	{
	        thread->state &= ~IL_TS_SUSPEND_REQUESTED;
		thread->state |= IL_TS_SUSPENDED | IL_TS_SUSPENDED_SELF;
		thread->resumeRequested = 0;

		thread->state &= ~(IL_TS_INTERRUPTED);

		/* Unlock the thread object prior to suspending */
		_ILMutexUnlock(&(thread->lock));

		/* Suspend until we receive notification from another thread */
		_ILThreadSuspendSelf(thread);

		_ILMutexLock(&(thread->lock));		
		thread->state &= ~(IL_TS_WAIT_SLEEP_JOIN | IL_TS_INTERRUPTED);
		_ILMutexUnlock(&(thread->lock));
	}
	else
	{	
		thread->state &= ~(IL_TS_WAIT_SLEEP_JOIN | IL_TS_INTERRUPTED);

		_ILMutexUnlock(&(thread->lock));
	}

	return result;
}

/*
 * Leave the "wait/sleep/join" state, and release a wait
 * handle if the leave was interrupted or aborted.  This
 * is used when we thought we acquired the handle, but the
 * leave then fails.
 */
static int _ILLeaveWaitHandle(ILThread *thread, ILWaitHandle *handle, int ok)
{
	int result = _ILLeaveWait(thread, 0);
	if(result != 0)
	{
		(*(handle->unregisterFunc))(handle, &(thread->wakeup), 1);
		return result;
	}
	else
	{
		return ok;
	}
}

int ILSignalAndWait(ILWaitHandle *signalHandle, ILWaitHandle *waitHandle, ILUInt32 timeout)
{
	ILThread *thread = ILThreadSelf();
	_ILWakeup *wakeup = &(thread->wakeup);
	int result;

	/* Enter the "wait/sleep/join" state */
	result = _ILEnterWait(thread);
	if(result != 0)
	{
		return result;
	}

	if (signalHandle->signalFunc == 0)
	{
		return IL_WAIT_FAILED;
	}

	/* Set the limit for the thread's wakeup object */
	if(!_ILWakeupSetLimit(wakeup, 1))
	{
		return _ILLeaveWait(thread, IL_WAIT_INTERRUPTED);
	}

	/* Register this thread with the handle */
	result = (*(waitHandle->registerFunc))(waitHandle, wakeup);
		
	signalHandle->signalFunc(signalHandle);
	
	if(result == IL_WAITREG_ACQUIRED)
	{
		/* We were able to acquire the wait handle immediately */
		_ILWakeupAdjustLimit(wakeup, 0);		
		return _ILLeaveWaitHandle(thread, waitHandle, 0);
	}
	else if(result == IL_WAITREG_FAILED)
	{
		/* Something serious happened which prevented registration */
		_ILWakeupAdjustLimit(wakeup, 0);
		return _ILLeaveWait(thread, IL_WAIT_FAILED);
	}

	/* Wait until we are signalled, timed out, or interrupted */
	result = _ILWakeupWait(wakeup, timeout, 0);

	/* Unregister the thread from the wait handle */
	(*(waitHandle->unregisterFunc))(waitHandle, wakeup, (result <= 0));

	/* Tell the caller what happened */
	if(result > 0)
	{
		/* We have to account for "_ILLeaveWait" detecting interrupt
		or abort after we already acquired the wait handle */
		return _ILLeaveWaitHandle(thread, waitHandle, 0);
	}
	else if(result == 0)
	{
		return _ILLeaveWait(thread, IL_WAIT_TIMEOUT);
	}
	else
	{
		return _ILLeaveWait(thread, IL_WAIT_INTERRUPTED);
	}
}

int ILWaitOne(ILWaitHandle *handle, ILUInt32 timeout)
{
	ILThread *thread = ILThreadSelf();
	_ILWakeup *wakeup = &(thread->wakeup);
	int result;

	/* Enter the "wait/sleep/join" state */
	result = _ILEnterWait(thread);
	if(result != 0)
	{
		return result;
	}

	/* Set the limit for the thread's wakeup object */
	if(!_ILWakeupSetLimit(wakeup, 1))
	{
		return _ILLeaveWait(thread, IL_WAIT_INTERRUPTED);
	}

	/* Register this thread with the handle */
	result = (*(handle->registerFunc))(handle, wakeup);

	if(result == IL_WAITREG_ACQUIRED)
	{
		/* We were able to acquire the wait handle immediately */
		_ILWakeupAdjustLimit(wakeup, 0);
		return _ILLeaveWaitHandle(thread, handle, 0);
	}
	else if(result == IL_WAITREG_FAILED)
	{
		/* Something serious happened which prevented registration */
		_ILWakeupAdjustLimit(wakeup, 0);
		return _ILLeaveWait(thread, IL_WAIT_FAILED);
	}

	/* Wait until we are signalled, timed out, or interrupted */
	result = _ILWakeupWait(wakeup, timeout, 0);

	/* Unregister the thread from the wait handle */
	(*(handle->unregisterFunc))(handle, wakeup, (result <= 0));

	/* Tell the caller what happened */
	if(result > 0)
	{
		/* We have to account for "_ILLeaveWait" detecting interrupt
		   or abort after we already acquired the wait handle */
		return _ILLeaveWaitHandle(thread, handle, 0);
	}
	else if(result == 0)
	{
		return _ILLeaveWait(thread, IL_WAIT_TIMEOUT);
	}
	else
	{
		return _ILLeaveWait(thread, IL_WAIT_INTERRUPTED);
	}
}

int ILWaitAny(ILWaitHandle **handles, ILUInt32 numHandles, ILUInt32 timeout)
{
	ILThread *thread = ILThreadSelf();
	_ILWakeup *wakeup = &(thread->wakeup);
	int result;
	ILUInt32 index, index2;
	ILWaitHandle *handle;
	ILWaitHandle *resultHandle;

	/* Enter the "wait/sleep/join" state */
	result = _ILEnterWait(thread);
	if(result != 0)
	{
		return result;
	}

	/* Set the limit for the thread's wakeup object */
	if(!_ILWakeupSetLimit(wakeup, 1))
	{
		return _ILLeaveWait(thread, IL_WAIT_INTERRUPTED);
	}

	/* Register this thread with all of the wait handles */
	for(index = 0; index < numHandles; ++index)
	{
		handle = handles[index];
		result = (*(handle->registerFunc))(handle, wakeup);
		if(result == IL_WAITREG_ACQUIRED)
		{
			/* We were able to acquire this wait handle immediately */
			_ILWakeupAdjustLimit(wakeup, 0);

			/* Unregister the handles we have registered so far */
			for(index2 = 0; index2 < index; ++index2)
			{
				handle = handles[index2];
				(*(handle->unregisterFunc))(handle, wakeup, 1);
			}
			return _ILLeaveWaitHandle(thread, handles[index], index);
		}
		else if(result == IL_WAITREG_FAILED)
		{
			/* Something serious happened which prevented registration */
			_ILWakeupAdjustLimit(wakeup, 0);

			/* Unregister the handles we have registered so far */
			for(index2 = 0; index2 < index; ++index2)
			{
				handle = handles[index2];
				(*(handle->unregisterFunc))(handle, wakeup, 1);
			}
			return _ILLeaveWait(thread, IL_WAIT_FAILED);
		}
	}

	/* Wait until we are signalled, timed out, or interrupted */
	resultHandle = 0;
	result = _ILWakeupWait(wakeup, timeout, (void *)&resultHandle);

	/* Unregister the thread from the wait handles */
	index2 = 0;
	for(index = 0; index < numHandles; ++index)
	{
		handle = handles[index];
		if(handle == resultHandle && result > 0)
		{
			index2 = index;
			(*(handle->unregisterFunc))(handle, wakeup, 0);
		}
		else
		{
			(*(handle->unregisterFunc))(handle, wakeup, 1);
		}
	}

	/* Tell the caller what happened */
	if(result > 0)
	{
		/* We have to account for "_ILLeaveWait" detecting interrupt
		   or abort after we already acquired the wait handle */
		return _ILLeaveWaitHandle(thread, resultHandle, (int)index2);
	}
	else if(result == 0)
	{
		return _ILLeaveWait(thread, IL_WAIT_TIMEOUT);
	}
	else
	{
		return _ILLeaveWait(thread, IL_WAIT_INTERRUPTED);
	}
}

int ILWaitAll(ILWaitHandle **handles, ILUInt32 numHandles, ILUInt32 timeout)
{
	ILThread *thread = ILThreadSelf();
	_ILWakeup *wakeup = &(thread->wakeup);
	int result;
	ILUInt32 index, index2;
	ILWaitHandle *handle;
	ILUInt32 limit;

	/* Enter the "wait/sleep/join" state */
	result = _ILEnterWait(thread);
	if(result != 0)
	{
		return result;
	}

	/* Set the limit for the thread's wakeup object.  This may
	   be reduced later if we were able to acquire some objects
	   during the registration step */
	limit = numHandles;
	if(!_ILWakeupSetLimit(wakeup, limit))
	{
		return _ILLeaveWait(thread, IL_WAIT_INTERRUPTED);
	}

	/* Register this thread with all of the wait handles */
	for(index = 0; index < numHandles; ++index)
	{
		handle = handles[index];
		result = (*(handle->registerFunc))(handle, wakeup);
		if(result == IL_WAITREG_ACQUIRED)
		{
			/* We were able to acquire this wait handle immediately */
			--limit;
		}
		else if(result == IL_WAITREG_FAILED)
		{
			/* Something serious happened which prevented registration */
			_ILWakeupAdjustLimit(wakeup, 0);

			/* Unregister the handles we have registered so far */
			for(index2 = 0; index2 < index; ++index2)
			{
				handle = handles[index2];
				(*(handle->unregisterFunc))(handle, wakeup, 1);
			}
			return _ILLeaveWait(thread, IL_WAIT_FAILED);
		}
	}

	/* Adjust the wait limit to reflect handles we already acquired */
	_ILWakeupAdjustLimit(wakeup, limit);
	
	if (limit == 0)
	{
		/* No need to wait since we managed to aquire every handle immediately. */		
		result = 1;
	}
	else
	{
		/* Wait until we are signalled, timed out, or interrupted */
		result = _ILWakeupWait(wakeup, timeout, 0);
	}

	/* Unregister the thread from the wait handles */
	for(index = 0; index < numHandles; ++index)
	{
		handle = handles[index];
		(*(handle->unregisterFunc))(handle, wakeup, (result <= 0));
	}

	/* Tell the caller what happened */
	if(result > 0)
	{
		/* We have to account for "_ILLeaveWait" detecting interrupt
		   or abort after we already acquired the wait handles */
		result = _ILLeaveWait(thread, 0);
		if(result == 0)
		{
			return 0;
		}
		else
		{
			for(index = 0; index < numHandles; ++index)
			{
				handle = handles[index];
				(*(handle->unregisterFunc))(handle, wakeup, 1);
			}
			return result;
		}
	}
	else if(result == 0)
	{
		return _ILLeaveWait(thread, IL_WAIT_TIMEOUT);
	}
	else
	{
		return _ILLeaveWait(thread, IL_WAIT_INTERRUPTED);
	}
}

/*
 * Waits for a handle and if an interrupt or abort is encountered during the
 * wait sequence the function will return the proper wait error code but only
 * after it has managed to aquire the handle (by continuously retrying).
 * If the wait times out, the handle will not be aquired and the function will
 * return with either IL_WAIT_TIMEOUT, IL_WAIT_ABORTED or IL_WAIT_INTERRUPTED.
 */
int _ILWaitOneBackupInterruptsAndAborts(ILWaitHandle *handle, int timeout)
{	
	ILThread *thread = ILThreadSelf();
	int result, retval = 0, threadstate = 0;
	
	for (;;)
	{
		/* Wait to re-acquire the monitor (add ourselves to the "ready queue") */
		result = ILWaitOne(handle, timeout);
		
		if (result < IL_WAIT_TIMEOUT)
		{
			/* We were aborted or interrupted.  Save the thread state
				and keep trying to reaquire the monitor */
			
			_ILMutexLock(&thread->lock);
			
			threadstate |= thread->state;
			
			if (result == IL_WAIT_INTERRUPTED)
			{
				/* Interrupted is cleared by ILWaitOne so save it manually */					
				threadstate |= IL_TS_INTERRUPTED;
			}
			
			thread->state &= ~(IL_TS_ABORT_REQUESTED | IL_TS_INTERRUPTED);

			if (result < retval)
			{
				retval = result;
			}
			
			_ILMutexUnlock(&thread->lock);
			
			continue;
		}
		else
		{	
			if (threadstate != 0)
			{					
				_ILMutexLock(&thread->lock);

				/* Set the thread state to the thread state that was stored 
					and clear the interrupted flag */
				thread->state |= (threadstate & ~IL_TS_INTERRUPTED);

				_ILMutexUnlock(&thread->lock);
			}	
			else
			{
				retval = result;
			}
			
			return retval;
		}
	}
}

#ifdef	__cplusplus
};
#endif
