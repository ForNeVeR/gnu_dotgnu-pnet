/*
 * pt_wait.c - Wait handle management, based on pthreads.
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

#include "il_gc.h"
#include "pt_include.h"

#ifdef IL_USE_PTHREADS

#ifdef	__cplusplus
extern	"C" {
#endif

void _ILWaitQueueInit(ILWaitQueue *queue)
{
	queue->first = 0;
	queue->last = 0;
	queue->spaceUsed = 0;
}

int _ILWaitQueueAdd(ILWaitQueue *queue, ILThread *thread)
{
	if(!(queue->first))
	{
		/* Easy case: the queue is empty */
		queue->first = &(queue->space);
		queue->last = &(queue->space);
		queue->space.thread = thread;
		queue->space.next = 0;
		queue->spaceUsed = 1;
		return 1;
	}
	else if(!(queue->spaceUsed))
	{
		/* The pre-allocated "space" item can be reused */
		queue->last->next = &(queue->space);
		queue->last = &(queue->space);
		queue->space.thread = thread;
		queue->space.next = 0;
		queue->spaceUsed = 1;
		return 1;
	}
	else
	{
		/* We need to allocate space for an item from the heap */
		ILWaitItem *item = (ILWaitItem *)ILMalloc(sizeof(ILWaitItem));
		if(!item)
		{
			return 0;
		}
		queue->last->next = item;
		queue->last = item;
		item->thread = thread;
		item->next = 0;
		return 1;
	}
}

void _ILWaitQueueRemove(ILWaitQueue *queue, ILThread *thread)
{
	/* TODO */
}

ILThread *_ILWaitQueueFirst(ILWaitQueue *queue)
{
	ILWaitItem *item = queue->first;
	ILThread *thread;
	if(item)
	{
		if((queue->first = item->next) == 0)
		{
			queue->last = 0;
		}
		thread = item->thread;
		if(item == &(queue->space))
		{
			queue->spaceUsed = 0;
		}
		else
		{
			ILFree(item);
		}
		return thread;
	}
	else
	{
		return 0;
	}
}

int _ILWaitWakeupThread(ILThread *thread)
{
	int result;

	/* Acquire the thread's wakeup mutex */
	pthread_mutex_lock(&(thread->wakeupMutex));

	/* Determine how to perform the wakeup */
	if(thread->wakeupType == IL_WAKEUP_NONE)
	{
		/* The thread was already woken, and shouldn't be again */
		result = 0;
	}
	else if(thread->wakeupType == IL_WAKEUP_ONCE)
	{
		/* The thread needs to be woken up once only */
		thread->wakeupType = IL_WAKEUP_NONE;
		sem_post(&(thread->wakeup));
		result = 1;
	}
	else
	{
		/* The thread may need to be woken up multiple times */
		sem_post(&(thread->wakeup));
		result = 1;
	}

	/* Release the thread's wakeup mutex and return */
	pthread_mutex_unlock(&(thread->wakeupMutex));
	return result;
}

int ILWaitHandleDestroy(ILWaitHandle *handle)
{
	/* TODO */
	ILFree(handle);
	return 1;
}

/*
 * Destroy a mutex wait handle.
 */
static int MutexDestroy(ILWaitMutex *mutex)
{
	/* TODO */
	return 1;
}

/*
 * Register a thread with a mutex wait handle.
 */
static int MutexRegister(ILWaitMutex *mutex, ILThread *thread)
{
	int result;

	/* Lock down the mutex */
	pthread_mutex_lock(&(mutex->parent.lock));

	/* Determine what to do based on the mutex's current state */
	if(mutex->owner == 0)
	{
		/* No one owns the mutex, so grab it for ourselves */
		mutex->owner = thread;
		mutex->count = 1;
		result = IL_WAITREG_ACQUIRED;
	}
	else if(mutex->owner == thread)
	{
		/* The mutex is already owned by this thread, so re-acquire it */
		++(mutex->count);
		result = IL_WAITREG_ACQUIRED;
	}
	else
	{
		/* Someone else owns the mutex, so add ourselves to the wait queue */
		if(_ILWaitQueueAdd(&(mutex->queue), thread))
		{
			result = IL_WAITREG_OK;
		}
		else
		{
			result = IL_WAITREG_FAILED;
		}
	}

	/* Unlock the mutex and return */
	pthread_mutex_unlock(&(mutex->parent.lock));
	return result;
}

/*
 * Unregister a thread from a mutex wait handle.
 */
static void MutexUnregister(ILWaitMutex *mutex, ILThread *thread)
{
	/* Lock down the mutex */
	pthread_mutex_lock(&(mutex->parent.lock));

	/* Remove ourselves from the wait queue if we are currently on it */
	_ILWaitQueueRemove(&(mutex->queue), thread);

	/* Unlock the mutex and return */
	pthread_mutex_unlock(&(mutex->parent.lock));
}

ILWaitHandle *ILWaitMutexCreate(int initiallyOwned)
{
	ILWaitMutex *mutex;

	/* Allocate memory for the mutex object */
	if((mutex = (ILWaitMutex *)ILMalloc(sizeof(ILWaitMutex))) == 0)
	{
		return 0;
	}

	/* Initialize the mutex object */
	pthread_mutex_init(&(mutex->parent.lock), (pthread_mutexattr_t *)0);
	mutex->parent.kind = IL_WAIT_MUTEX;
	mutex->parent.destroyFunc = (ILWaitHandleDestroyFunc)MutexDestroy;
	mutex->parent.registerFunc = (ILWaitHandleRegisterFunc)MutexRegister;
	mutex->parent.unregisterFunc = (ILWaitHandleUnregisterFunc)MutexUnregister;
	if(initiallyOwned)
	{
		mutex->owner = ILThreadSelf();
		mutex->count = 1;
	}
	else
	{
		mutex->owner = 0;
		mutex->count = 0;
	}
	_ILWaitQueueInit(&(mutex->queue));

	/* The mutex is ready to go */
	return &(mutex->parent);
}

/*
 * Internal version of "ILWaitMutexRelease".
 */
static int MutexRelease(ILWaitHandle *handle, int kind)
{
	ILWaitMutex *mutex = (ILWaitMutex *)handle;
	int result;

	/* Lock down the mutex */
	pthread_mutex_lock(&(mutex->parent.lock));

	/* Determine what to do based on the mutex's state */
	if(mutex->parent.kind != kind)
	{
		/* This isn't actually a mutex of the correct kind */
		result = -1;
	}
	else if(mutex->owner != ILThreadSelf())
	{
		/* This thread doesn't currently own the mutex */
		result = 0;
	}
	else if(--(mutex->count) == 0)
	{
		/* The count has returned to zero, so give up the mutex */
		while((mutex->owner = _ILWaitQueueFirst(&(mutex->queue))) != 0)
		{
			/* Wake up another thread.  This may fail if the thread is
			   doing a "WaitForAny" and some other wait handle has already
			   been accepted as the "any".  In that case, we skip this
			   thread and search for another to be woken */
			if(_ILWaitWakeupThread(mutex->owner))
			{
				mutex->count = 1;
				break;
			}
		}
		result = 1;
	}
	else
	{
		/* The current thread still owns the mutex */
		result = 1;
	}

	/* Unlock the mutex and return */
	pthread_mutex_unlock(&(mutex->parent.lock));
	return result;
}

int ILWaitMutexRelease(ILWaitHandle *handle)
{
	return MutexRelease(handle, IL_WAIT_MUTEX);
}

/*
 * Destroy a monitor wait handle.
 */
static int MonitorDestroy(ILWaitMonitor *mon)
{
	/* TODO */
	return 1;
}

ILWaitHandle *ILWaitMonitorCreate(void)
{
	ILWaitMonitor *mon;

	/* Allocate memory for the monitor object */
	if((mon = (ILWaitMonitor *)ILMalloc(sizeof(ILWaitMonitor))) == 0)
	{
		return 0;
	}

	/* Initialize the monitor object */
	pthread_mutex_init(&(mon->mutex.parent.lock), (pthread_mutexattr_t *)0);
	mon->mutex.parent.kind = IL_WAIT_MONITOR;
	mon->mutex.parent.destroyFunc = (ILWaitHandleDestroyFunc)MonitorDestroy;
	mon->mutex.parent.registerFunc = (ILWaitHandleRegisterFunc)MutexRegister;
	mon->mutex.parent.unregisterFunc =
		(ILWaitHandleUnregisterFunc)MutexUnregister;
	mon->mutex.owner = 0;
	mon->mutex.count = 0;
	_ILWaitQueueInit(&(mon->mutex.queue));
	_ILWaitQueueInit(&(mon->signalQueue));

	/* The monitor is ready to go */
	return &(mon->mutex.parent);
}

int ILWaitMonitorEnter(ILWaitHandle *handle)
{
	/* TODO */
	return 0;
}

int ILWaitMonitorExit(ILWaitHandle *handle)
{
	/* Exiting from a monitor is identical to releasing the underlying mutex */
	return MutexRelease(handle, IL_WAIT_MONITOR);
}

int ILWaitMonitorTryEnter(ILWaitHandle *handle)
{
	/* TODO */
	return 0;
}

int ILWaitMonitorWait(ILWaitHandle *handle)
{
	/* TODO */
	return 0;
}

int ILWaitMonitorPulse(ILWaitHandle *handle)
{
	/* TODO */
	return 0;
}

int ILWaitMonitorPulseAll(ILWaitHandle *handle)
{
	/* TODO */
	return 0;
}

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_PTHREADS */
