/*
 * wakeup.c - Implementation of "thread wakeup" objects.
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

Wakeup objects are similar to condition variables, except that they
also support "signal at N" and "interrupt" semantics.  The full set
of semantics are as follows:

*/

#include "thr_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void _ILWakeupCreate(_ILWakeup *wakeup)
{
	_ILCondMutexCreate(&(wakeup->lock));
	_ILCondVarCreate(&(wakeup->condition));
	wakeup->count = 0;
	wakeup->limit = 0;
	wakeup->interrupted = 0;
	wakeup->object = 0;
}

void _ILWakeupDestroy(_ILWakeup *wakeup)
{
	_ILCondVarDestroy(&(wakeup->condition));
	_ILCondMutexDestroy(&(wakeup->lock));
}

int _ILWakeupWait(_ILWakeup *wakeup, ILUInt32 ms,
				  ILUInt32 limit, void **object)
{
	int result;

	/* Lock down the wakeup object.  We use an "unsafe" mutex lock
	   because we will be giving up the lock later on.  It is OK
	   to do this because the current thread will always be in the
	   "wait/sleep/join" state, which prevents thread suspension */
	_ILCondMutexLockUnsafe(&(wakeup->lock));

	/* Is there a pending interrupt? */
	if(!(wakeup->interrupted))
	{
		/* Reset the control variables */
		wakeup->count = 0;
		wakeup->limit = limit;
		wakeup->object = 0;

		/* Give up the lock and wait for someone to signal us */
		if(_ILCondVarTimedWait(&(wakeup->condition), &(wakeup->lock), ms))
		{
			if(wakeup->interrupted)
			{
				/* The wakeup object was interrupted */
				wakeup->interrupted = 0;
				result = -1;
			}
			else
			{
				/* All signals that we were expecting have arrived */
				if(object)
				{
					/* Return the last object that was signalled */
					*object = wakeup->object;
				}
				result = 1;
			}
		}
		else
		{
			/* The wakeup object timed out.  We still check for interrupt
			   because we may have been interrupted just after timeout,
			   but before this thread was re-awoken */
			result = (wakeup->interrupted ? -1 : 0);
			wakeup->interrupted = 0;
		}
		wakeup->count = 0;
		wakeup->limit = 0;
	}
	else
	{
		/* The thread was already interrupted before we got this far */
		wakeup->interrupted = 0;
		result = -1;
	}

	/* Unlock the wakeup object and return */
	_ILCondMutexUnlockUnsafe(&(wakeup->lock));
	return result;

}

int _ILWakeupSignal(_ILWakeup *wakeup, void *object)
{
	int result;

	/* Lock down the wakeup object */
	_ILCondMutexLock(&(wakeup->lock));

	/* Determine what to do based on the wakeup object's state */
	if(wakeup->interrupted || wakeup->count >= wakeup->limit)
	{
		/* The wakeup was interrupted or we have already reached the limit */
		result = 0;
	}
	else
	{
		/* Increase the wakeup count */
		++(wakeup->count);

		/* Record the object to be returned from "_ILWakeupWait" */
		wakeup->object = object;

		/* Signal the waiting thread if we have reached the limit */
		if(wakeup->count >= wakeup->limit)
		{
			_ILCondVarSignal(&(wakeup->condition));
		}

		/* The signal operation succeeded */
		result = 1;
	}

	/* Unlock the wakeup object and return */
	_ILCondMutexUnlock(&(wakeup->lock));
	return result;
}

void _ILWakeupInterrupt(_ILWakeup *wakeup)
{
	/* Lock down the wakeup object */
	_ILCondMutexLock(&(wakeup->lock));

	/* Nothing to do if the thread is already marked for interruption */
	if(!(wakeup->interrupted))
	{
		/* Mark the thread for interruption */
		wakeup->interrupted = 1;

		/* Signal anyone who is waiting that interruption occurred */
		if(wakeup->count < wakeup->limit || wakeup->limit == 0)
		{
			_ILCondVarSignal(&(wakeup->condition));
		}
	}

	/* Unlock the wakeup object */
	_ILCondMutexUnlock(&(wakeup->lock));
}

#ifdef	__cplusplus
};
#endif
