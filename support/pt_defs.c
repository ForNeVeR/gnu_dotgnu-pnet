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

void _ILThreadInitSystem(ILThread *mainThread)
{
	struct sigaction action;
	sigset_t set;

	/* Set up the signal handlers that we require */
	ILMemZero(&action, sizeof(action));
	action.sa_flags = SA_RESTART;
	sigfillset(&(action.sa_mask));
	sigdelset(&(action.sa_mask), SIGINT);
	sigdelset(&(action.sa_mask), SIGQUIT);
	sigdelset(&(action.sa_mask), SIGTERM);
	sigdelset(&(action.sa_mask), SIGABRT);
	action.sa_handler = SuspendSignal;
	sigaction(IL_SIG_SUSPEND, &action, (struct sigaction *)0);
	action.sa_handler = ResumeSignal;
	sigaction(IL_SIG_RESUME, &action, (struct sigaction *)0);
	action.sa_handler = AbortSignal;
	sigaction(IL_SIG_ABORT, &action, (struct sigaction *)0);

	/* We need a thread-specific key for storing thread objects */
	pthread_key_create(&_ILThreadObjectKey, (void (*)(void *))0);

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

#ifdef	__cplusplus
};
#endif

#endif	/* IL_USE_PTHREADS */
