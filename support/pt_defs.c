/*
 * pt_defs.c - Thread definitions for using pthreads.
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
#if TIME_WITH_SYS_TIME
	#include <sys/time.h>
    #include <time.h>
#else
    #if HAVE_SYS_TIME_H
		#include <sys/time.h>
    #elif !defined(__palmos__)
        #include <time.h>
    #endif
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <errno.h>

#ifdef IL_USE_PTHREADS

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Thread-specific key that is used to store and retrieve thread objects.
 */
pthread_key_t _ILThreadObjectKey;

/*
 * Default mutex attribute.
 */
pthread_mutexattr_t _ILMutexAttr;

/*
 * Suspend until we receive IL_SIG_RESUME from another thread
 * in this process.
 *
 * This implementation currently assumes a Linux-like threads
 * implementation that uses one process per thread, and may not
 * work on other pthreads implementations.
 *
 * The "SIG_SUSPEND" signal is used by the garbage collector to stop
 * the world for garbage collection, and so we must not block it.
 *
 * The "PTHREAD_SIG_CANCEL" signal is used by pthreads itself to
 * cancel running threads, and must not be blocked by us.
 */
void _ILThreadSuspendUntilResumed(ILThread *thread)
{
	sigset_t mask;

	/* Set up the signal mask to allow through only selected signals */
	sigfillset(&mask);
	sigdelset(&mask, IL_SIG_RESUME);
#ifdef SIG_SUSPEND
	sigdelset(&mask, SIG_SUSPEND);
#endif
	sigdelset(&mask, PTHREAD_SIG_CANCEL);
	sigdelset(&mask, SIGINT);
	sigdelset(&mask, SIGQUIT);
	sigdelset(&mask, SIGTERM);
	sigdelset(&mask, SIGABRT);

	/* Suspend until we receive IL_SIG_RESUME from something in this process */
	do
	{
		sigsuspend(&mask);
	}
	while(!(thread->resumeRequested));
}

/*
 * Signal handler for IL_SIG_SUSPEND.
 */
static void SuspendSignal(int sig)
{
	ILThread *thread = _ILThreadGetSelf();

	/* Tell the "ILThreadSuspend" function that we are now suspended */
	_ILSemaphorePost(&(thread->suspendAck));

	/* Suspend until we are resumed by some other thread */
	_ILThreadSuspendUntilResumed(thread);

	/* Tell the "ILThreadResume" function that we have now resumed */
	_ILSemaphorePost(&(thread->resumeAck));
}

/*
 * Signal handler for IL_SIG_RESUME.
 */
static void ResumeSignal(int sig)
{
	/* There is nothing to do here - "_ILThreadSuspendUntilResumed"
	   will be interrupted in the call to "sigsuspend", and then
	   will test the "resumeRequested" flag to detect the request */
}

/*
 * Signal handler for IL_SIG_ABORT.
 */
static void AbortSignal(int sig)
{
	/* There is nothing to do here - this signal exists purely
	   to force system calls to exit with EINTR */
}

/*
 * This function is only used for initializing an ILThread
 * structure for threads not created by pnet.
 */
void _ILThreadInitHandleSelf(ILThread *thread)
{
	/* Set the thread handle and identifier for the thread */
	thread->handle = pthread_self();
	thread->identifier = thread->handle;
}

void _ILThreadDestroyHandleSelf(ILThread *thread)
{
	/* there is nothing to do with pthreads. */
}

void _ILThreadInitSystem(ILThread *mainThread)
{
	struct sigaction action;
	sigset_t set;

	/* Set up the signal handlers that we require */
	ILMemZero(&action, sizeof(action));
	sigfillset(&(action.sa_mask));
	sigdelset(&(action.sa_mask), SIGINT);
	sigdelset(&(action.sa_mask), SIGQUIT);
	sigdelset(&(action.sa_mask), SIGTERM);
	sigdelset(&(action.sa_mask), SIGABRT);

	/* Abort signal - used to stop blocking IO,
	   SA_RESTART must not be set */
	action.sa_handler = AbortSignal;
	sigaction(IL_SIG_ABORT, &action, (struct sigaction *)0);

	action.sa_flags = SA_RESTART;
	action.sa_handler = SuspendSignal;
	sigaction(IL_SIG_SUSPEND, &action, (struct sigaction *)0);
	action.sa_handler = ResumeSignal;
	sigaction(IL_SIG_RESUME, &action, (struct sigaction *)0);

	/* We need a thread-specific key for storing thread objects */
	pthread_key_create(&_ILThreadObjectKey, (void (*)(void *))0);

	/* Initialize the mutexattr used on this system. */
	pthread_mutexattr_init(&_ILMutexAttr);
#ifdef PTHREAD_MUTEX_NORMAL
	pthread_mutexattr_settype(&_ILMutexAttr, PTHREAD_MUTEX_NORMAL);
#endif

	/* Block the IL_SIG_RESUME signal in the current thread.
	   It will be unblocked by "SuspendUntilResumed" */
	sigemptyset(&set);
	sigaddset(&set, IL_SIG_RESUME);
	pthread_sigmask(SIG_BLOCK, &set, (sigset_t *)0);

	/* Unblock the IL_SIG_SUSPEND and IL_SIG_ABORT signals */
	sigemptyset(&set);
	sigaddset(&set, IL_SIG_SUSPEND);
	sigaddset(&set, IL_SIG_ABORT);
	pthread_sigmask(SIG_UNBLOCK, &set, (sigset_t *)0);

	/* Set the thread handle and identifier for the main thread */
	mainThread->handle = pthread_self();
	mainThread->identifier = mainThread->handle;
}

void _ILThreadSetPriority(ILThread *thread, int priority)
{
}

int _ILThreadGetPriority(ILThread *thread)
{
	return IL_TP_NORMAL;
}

/*
 * Entry point for a pthread.  This initializes the thread
 * and then calls "_ILThreadRun" to do the real work.
 */
static void *ThreadStart(void *arg)
{
	ILThread *thread = (ILThread *)arg;
	sigset_t set;

	/* Store the thread identifier into the object */
	thread->handle = pthread_self();
	thread->identifier = thread->handle;

	/* Attach the thread object to the thread identifier */
	pthread_setspecific(_ILThreadObjectKey, (void *)thread);

	/* Detach ourselves because we won't be using "pthread_join"
	   to detect when the thread exits */
	pthread_detach(thread->handle);

	/* Set the cancel type to asynchronous */
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, (int *)0);

	/* Block the IL_SIG_RESUME signal in the current thread.
	   It will be unblocked by "SuspendUntilResumed" */
	sigemptyset(&set);
	sigaddset(&set, IL_SIG_RESUME);
	pthread_sigmask(SIG_BLOCK, &set, (sigset_t *)0);

	/* Unblock the IL_SIG_SUSPEND and IL_SIG_ABORT signals */
	sigemptyset(&set);
	sigaddset(&set, IL_SIG_SUSPEND);
	sigaddset(&set, IL_SIG_ABORT);
	pthread_sigmask(SIG_UNBLOCK, &set, (sigset_t *)0);

	/* Run the thread */
	_ILThreadRun(thread);

	/* The thread has exited back through its start function */
	return 0;
}

int _ILThreadCreateSystem(ILThread *thread)
{
	if(pthread_create((pthread_t *)&(thread->handle), (pthread_attr_t *)0,
					  ThreadStart, (void *)thread) == 0)
	{
		/* Under pthreads, the thread identifier is the same as the handle */
		thread->identifier = thread->handle;
		return 1;
	}
	else
	{
		return 0;
	}
}

int _ILCondVarTimedWait(_ILCondVar *cond, _ILCondMutex *mutex, ILUInt32 ms)
{
	struct timeval tv;
	struct timespec ts;
	int result;

	if(ms != IL_MAX_UINT32)
	{
		/* Convert the milliseconds value into an absolute timeout */
		gettimeofday(&tv, 0);
		ts.tv_sec = tv.tv_sec + (long)(ms / 1000);
		ts.tv_nsec = (tv.tv_usec + (long)((ms % 1000) * 1000)) * 1000L;
		if(ts.tv_nsec >= 1000000000L)
		{
			++(ts.tv_sec);
			ts.tv_nsec -= 1000000000L;
		}

		/* Wait until we are signalled or the timeout expires */
		do
		{
			result = pthread_cond_timedwait(cond, mutex, &ts);
		}
		while(result == EINTR);
		return ((result == 0) ? 1 : 0);
	}
	else
	{
		/* Wait forever until the condition variable is signalled */
		pthread_cond_wait(cond, mutex);
		return 1;
	}
}

/*
 * Define some functions from the Timeouts options that need not be provided
 * in all implementations.
 */

#ifndef HAVE_PTHREAD_MUTEX_TIMEDLOCK
int pthread_mutex_timedlock (pthread_mutex_t *mutex,
			    const struct timespec *abs_timeout)
{
	struct timeval tv;
	struct timespec ts;
	int retcode;
	
	/* To avoid a completely busy wait we sleep for 10 ms between two tries. */
	/* As a consequence the resolution is at most 10 ms instead of 1 ms. */
	ts.tv_sec = 0;
	ts.tv_nsec = 10000000;	/* 10ms */
	
	while((retcode = pthread_mutex_trylock(mutex)) == EBUSY)
	{
		gettimeofday(&tv, NULL);
		
		if(tv.tv_sec >= abs_timeout->tv_sec &&
		    (tv.tv_usec * 1000) >= abs_timeout->tv_nsec)
		{
			return ETIMEDOUT;
		}
		nanosleep(&ts, NULL);
	}
	return retcode;
}
#endif /* !HAVE_PTHREAD_MUTEX_TIMEDLOCK */

#ifndef HAVE_SEM_TIMEDWAIT
int sem_timedwait(sem_t *sem,
				  const struct timespec *abs_timeout)
{
	struct timeval tv;
	struct timespec ts;
	int retcode;
	
	/* To avoid a completely busy wait we sleep for 10 ms between two tries. */
	/* As a consequence the resolution is at most 10 ms instead of 1 ms. */
	ts.tv_sec = 0;
	ts.tv_nsec = 10000000;	/* 10ms */
	
	while((retcode = sem_trywait(sem)) == EAGAIN)
	{
		gettimeofday(&tv, NULL);
		
		if(tv.tv_sec >= abs_timeout->tv_sec &&
		    (tv.tv_usec * 1000) >= abs_timeout->tv_nsec)
		{
			return ETIMEDOUT;
		}
		nanosleep(&ts, NULL);
	}
	return retcode;
}
#endif	/* !HAVE_SEM_TIMEDWAIT */

int _ILCondMutexTimedLockUnsafe(_ILCondMutex *mutex, ILUInt32 ms)
{
	if((ms == IL_MAX_UINT32) || (ms <= IL_MAX_INT32))
	{
		int result;

		if(ms == IL_MAX_UINT32)
		{
			/* This is the same as the call to _ILCondMutexLockUnsafe. */
			result = pthread_mutex_lock(mutex);
		}
		else if(ms == 0)
		{
			/* This is the same as the call to _ILCondMutexTryLock. */
			result = pthread_mutex_trylock(mutex);
		}
		else
		{
			struct timeval tv;
			struct timespec ts;

			gettimeofday(&tv, 0);
			ts.tv_sec = tv.tv_sec + (long)(ms / 1000);
			ts.tv_nsec = (tv.tv_usec + (long)((ms % 1000) * 1000)) * 1000L;
			if(ts.tv_nsec >= 1000000000L)
			{
				++(ts.tv_sec);
				ts.tv_nsec -= 1000000000L;
			}
			result = pthread_mutex_timedlock(mutex, &ts);
		}
		switch(result)
		{
			case 0:
			{
				return IL_THREAD_OK;
			}
			break;

			case EBUSY:
			case ETIMEDOUT:
			{
				return IL_THREAD_BUSY;
			}
			break;
		}
		return IL_THREAD_ERR_UNKNOWN;
	}
	else
	{
		/* Invalid negative value for ms. */
		return IL_THREAD_ERR_INVALID_TIMEOUT;
	}
}

int _ILSemaphorePostMultiple(_ILSemaphore *sem, ILUInt32 count)
{
	if((count > 0) && (count < IL_MAX_INT32))
	{
		ILUInt32 current;

		for(current = 0; current < count; current++)
		{
			if(sem_post(sem))
			{
				/* An error occured. */
				return IL_THREAD_ERR_UNKNOWN;
			}
		}
		return IL_THREAD_OK;
	}
	/* Invalid value for the release count. */
	return IL_THREAD_ERR_INVALID_RELEASECOUNT;
}

int _ILSemaphoreTimedWait(_ILSemaphore *sem, ILUInt32 ms, ILInt32 interruptable)
{
	if((ms == IL_MAX_UINT32) || (ms <= IL_MAX_INT32))
	{
		int result;

		if(ms == IL_MAX_UINT32)
		{
			/* This is the same as the call to _ILSemaphoreWait. */
			do
			{
				result = sem_wait(sem);
			} while(!interruptable && (result == EINTR));
		}
		else if(ms == 0)
		{
			/* This is the same as the call to _ILSemaphoreTryWait. */
			do
			{
				result = sem_trywait(sem);
			} while(!interruptable && (result == EINTR));
		}
		else
		{
			struct timeval tv;
			struct timespec ts;

			gettimeofday(&tv, 0);
			ts.tv_sec = tv.tv_sec + (long)(ms / 1000);
			ts.tv_nsec = (tv.tv_usec + (long)((ms % 1000) * 1000)) * 1000L;
			if(ts.tv_nsec >= 1000000000L)
			{
				++(ts.tv_sec);
				ts.tv_nsec -= 1000000000L;
			}
			do
			{
				result = sem_timedwait(sem, &ts);
			} while(!interruptable && (result == EINTR));
		}
		switch(result)
		{
			case 0:
			{
				return IL_THREAD_OK;
			}
			break;

			case EINTR:
			{
				return IL_THREAD_ERR_INTERRUPT;
			}
			break;

			case EAGAIN:
			case ETIMEDOUT:
			{
				return IL_THREAD_BUSY;
			}
			break;
		}
		return IL_THREAD_ERR_UNKNOWN;
	}
	else
	{
		/* Invalid negative value for ms. */
		return IL_THREAD_ERR_INVALID_TIMEOUT;
	}
}

/*
 * Release a number of waiting threads from the count semaphore.
 */
int _ILCountSemaphoreSignalCount(_ILCountSemaphore *sem, ILUInt32 count)
{
	int result = IL_THREAD_OK;

	if((count > 0) && (count <= IL_MAX_INT32))
	{
		ILUInt32 current;

		/* Lock the count semaphore object. */
		_ILMutexLock(&(sem->_lock));

		for(current = 0; current < count; current++)
		{
			if(sem_post(&(sem->_sem)))
			{
				/* An error occured. */

				/* Unlock the count semaphore object. */
				_ILMutexUnlock(&(sem->_lock));

				return IL_THREAD_ERR_UNKNOWN;
			}
			--(sem->_waiting);
		}
		/* Unlock the count semaphore object. */
		_ILMutexUnlock(&(sem->_lock));
	}
	else
	{
		/* Invalid value for the release count. */
		result = IL_THREAD_ERR_INVALID_RELEASECOUNT;
	}
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
		while(sem->_waiting > 0)
		{
			if(sem_post(&(sem->_sem)))
			{
				/* An error occured. */

				/* Unlock the count semaphore object. */
				_ILMutexUnlock(&(sem->_lock));

				return IL_THREAD_ERR_UNKNOWN;
			}
			--sem->_waiting;
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

		++sem->_waiting;

		/* Unlock the count semaphore object. */
		_ILMutexUnlock(&(sem->_lock));

		if((result = _ILSemaphoreTimedWait(&(sem->_sem), ms, 0)) != IL_THREAD_OK)
		{
			/* We have to decrement the counter again because the call failed. */

			/* Lock the count semaphore object. */
			_ILMutexLock(&(sem->_lock));

			if(sem->_waiting > 0)
			{
				--sem->_waiting;
			}

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

int _ILMonitorExit(_ILMonitor *mon)
{
	int result;

	if(!(result = _ILCondMutexUnlockUnsafe(&(mon->_mutex))))
	{
		/* NOTE: fast mutexes return 0 even if the current thread doesn't */
		/* own the mutex. */
		return IL_THREAD_OK;
	}
	return result == EPERM ? IL_THREAD_ERR_SYNCLOCK : IL_THREAD_ERR_UNKNOWN;
}

int _ILMonitorTimedTryEnter(_ILMonitor *mon, ILUInt32 ms)
{
	if(ms == IL_MAX_UINT32)
	{
		if(!_ILCondMutexLockUnsafe(&(mon->_mutex)))
		{
			return IL_THREAD_OK;
		}
		return IL_THREAD_ERR_UNKNOWN;
	}
	else if(ms == 0)
	{
		int result;

		if(!(result = _ILCondMutexTryLockUnsafe(&(mon->_mutex))))
		{
			return IL_THREAD_OK;
		}
		return result == EBUSY ? IL_THREAD_BUSY : IL_THREAD_ERR_UNKNOWN;
	}
	else if(ms <= IL_MAX_INT32)
	{
		struct timeval tv;
		struct timespec ts;
		int result;

		gettimeofday(&tv, 0);
		ts.tv_sec = tv.tv_sec + (long)(ms / 1000);
		ts.tv_nsec = (tv.tv_usec + (long)((ms % 1000) * 1000)) * 1000L;
		if(ts.tv_nsec >= 1000000000L)
		{
			++(ts.tv_sec);
			ts.tv_nsec -= 1000000000L;
		}
		if((result = pthread_mutex_timedlock(&(mon->_mutex), &ts)) != 0)
		{
			if(result == ETIMEDOUT)
			{
				return IL_THREAD_BUSY;
			}
			return IL_THREAD_ERR_UNKNOWN;
		}
		return IL_THREAD_OK;
	}
	else
	{
		/* Invalid negative value for ms. */
		return IL_THREAD_ERR_INVALID_TIMEOUT;
	}
}

int _ILMonitorTimedWait(_ILMonitor *mon, ILUInt32 ms,
						ILMonitorPredicate predicate, void *arg)
{
	int result = 0;

	if(ms == IL_MAX_UINT32)
	{
		do
		{
			result = pthread_cond_wait(&(mon->_cond), &(mon->_mutex));
			switch(result)
			{
				case 0:
				{
					if(predicate(arg))
					{
						return IL_THREAD_OK;
					}
				}
				break;

				case EINVAL:
				{
					return IL_THREAD_ERR_INVALID_TIMEOUT;
				}
				break;

				case EPERM:
				{
					return IL_THREAD_ERR_SYNCLOCK;
				}
				break;
			}
			return IL_THREAD_ERR_UNKNOWN;
		} while(1);
	}
	else if(ms == 0)
	{
		/*
		 * In this case we don't have to enter the waiting queue but simply
		 * release the mutex and reacquire it again
		 * that's (release and reenter the ready state).
		 */
		if(!(result = _ILCondMutexUnlockUnsafe(&(mon->_mutex))))
		{
			result = _ILCondMutexLockUnsafe(&(mon->_mutex));
			if(!result)
			{
				/*
				 * In this case the predicate should always return a
				 * value != 0
				 */
				if(predicate(arg))
				{
					return IL_THREAD_OK;
				}
				return IL_THREAD_ERR_UNKNOWN;
			}
		}
		return result;
	}
	else if(ms <= IL_MAX_INT32)
	{
		struct timeval tv;
		struct timespec ts;

		gettimeofday(&tv, 0);
		ts.tv_sec = tv.tv_sec + (long)(ms / 1000);
		ts.tv_nsec = (tv.tv_usec + (long)((ms % 1000) * 1000)) * 1000L;
		if(ts.tv_nsec >= 1000000000L)
		{
			++(ts.tv_sec);
			ts.tv_nsec -= 1000000000L;
		}

		/* Wait until we are signalled or the timeout expires */
		do
		{
			result = pthread_cond_timedwait(&(mon->_cond),
										    &(mon->_mutex), &ts);
			if((result == 0) || (result == ETIMEDOUT))
			{
				if(predicate(arg))
				{
					return result == 0 ? IL_THREAD_OK : IL_THREAD_BUSY;
				}
			}
			else
			{
				return IL_THREAD_ERR_UNKNOWN;
			}
		} while(1);
	}
	else
	{
		/* Invalid negative value for ms. */
		return IL_THREAD_ERR_INVALID_TIMEOUT;
	}
}

#ifdef	__cplusplus
};
#endif

#endif	/* IL_USE_PTHREADS */
