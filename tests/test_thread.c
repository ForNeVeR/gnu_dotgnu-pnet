/*
 * test_thread.c - Test the thread routines in "support".
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

#include "ilunit.h"
#include "il_thread.h"
#include "il_gc.h"
#if HAVE_UNISTD_H
	#include <unistd.h>
#endif
#ifdef IL_WIN32_NATIVE
	#include <windows.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Put the current thread to sleep for a number of "time steps".
 */
static void sleepFor(int steps)
{
#ifdef HAVE_USLEEP
	/* Time steps are 100ms in length */
	usleep(steps * 100000);
#define	STEPS_TO_MS(steps)	(steps * 100)
#else
#ifdef IL_WIN32_NATIVE
	/* Time steps are 100ms in length */
	Sleep(steps * 100);
#define	STEPS_TO_MS(steps)	(steps * 100)
#else
	/* Time steps are 1s in length */
	sleep(steps);
#define	STEPS_TO_MS(steps)	(steps * 1000)
#endif
#endif
}

/*
 * Test that the descriptor for the main thread is not NULL.
 */
static void thread_main_nonnull(void *arg)
{
	if(!ILThreadSelf())
	{
		ILUnitFailed("main thread is null");
	}
}

/*
 * Test setting and getting the object on the main thread.
 */
static void thread_main_object(void *arg)
{
	ILThread *thread = ILThreadSelf();

	/* The value should be NULL initially */
	if(ILThreadGetObject(thread))
	{
		ILUnitFailed("object for main thread not initially NULL");
	}

	/* Change the value to something else and then check it */
	ILThreadSetObject(thread, (void *)0xBADBEEF);
	if(ILThreadGetObject(thread) != (void *)0xBADBEEF)
	{
		ILUnitFailed("object for main thread could not be changed");
	}
}

/*
 * Test that the "main" thread is initially in the running state.
 */
static void thread_main_running(void *arg)
{
	if(ILThreadGetState(ILThreadSelf()) != IL_TS_RUNNING)
	{
		ILUnitFailed("main thread is not running");
	}
}

/*
 * Test that the "main" thread is initially a foreground thread.
 */
static void thread_main_foreground(void *arg)
{
	if(ILThreadGetBackground(ILThreadSelf()))
	{
		ILUnitFailed("main thread is not a foreground thread");
	}
	if((ILThreadGetState(ILThreadSelf()) & IL_TS_BACKGROUND) != 0)
	{
		ILUnitFailed("main thread state is not consistent "
					 "with background flag");
	}
}

/*
 * Global flag that may be modified by started threads.
 */
static int volatile globalFlag;

/*
 * Thread start function that checks that its argument is set correctly.
 */
static void checkValue(void *arg)
{
	globalFlag = (arg == (void *)0xBADBEEF);
}

/*
 * Thread start function that sets a global flag.
 */
static void setFlag(void *arg)
{
	globalFlag = 1;
}

/*
 * Test that when a thread is created, it has the correct argument.
 */
static void thread_create_arg(void *arg)
{
	ILThread *thread;

	/* Set the global flag to "not modified" */
	globalFlag = -1;

	/* Create the thread */
	thread = ILThreadCreate(checkValue, (void *)0xBADBEEF);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Start the thread running */
	ILThreadStart(thread);

	/* Wait for the thread to exit */
	sleepFor(1);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Determine if the test was successful or not */
	if(globalFlag == -1)
	{
		ILUnitFailed("thread start function was never called");
	}
	else if(!globalFlag)
	{
		ILUnitFailed("wrong value passed to thread start function");
	}
}

/*
 * Test that when a thread is created, it is initially suspended.
 */
static void thread_create_suspended(void *arg)
{
	ILThread *thread;
	int savedFlag1;
	int savedFlag2;

	/* Clear the global flag */
	globalFlag = 0;

	/* Create the thread */
	thread = ILThreadCreate(setFlag, 0);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Wait for the thread to get settled */
	sleepFor(1);

	/* Save the current state of the flag */
	savedFlag1 = globalFlag;

	/* Start the thread running */
	ILThreadStart(thread);

	/* Wait for the thread to exit */
	sleepFor(1);

	/* Get the new flag state */
	savedFlag2 = globalFlag;

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Determine if the test was successful or not */
	if(savedFlag1)
	{
		ILUnitFailed("thread did not suspend on creation");
	}
	if(!savedFlag2)
	{
		ILUnitFailed("thread did not unsuspend after creation");
	}
}

/*
 * A thread procedure that sleeps for a number of time steps.
 */
static void sleepThread(void *arg)
{
	sleepFor((int)(ILNativeInt)arg);
}

/*
 * Test that when a thread is created, it is initially in
 * the "unstarted" state, and then transitions to the
 * "running" state, and finally to the "stopped" state.
 */
static void thread_create_state(void *arg)
{
	ILThread *thread;
	int state1;
	int state2;
	int state3;

	/* Create the thread */
	thread = ILThreadCreate(sleepThread, (void *)2);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Get the thread's state (should be "unstarted") */
	state1 = ILThreadGetState(thread);

	/* Start the thread */
	ILThreadStart(thread);

	/* Wait 1 time step and then get the state again */
	sleepFor(1);
	state2 = ILThreadGetState(thread);

	/* Wait 2 more time steps for the thread to exit */
	sleepFor(2);
	state3 = ILThreadGetState(thread);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(state1 != IL_TS_UNSTARTED)
	{
		ILUnitFailed("thread did not begin in the `unstarted' state");
	}
	if(state2 != IL_TS_RUNNING)
	{
		ILUnitFailed("thread did not change to the `running' state");
	}
	if(state3 != IL_TS_STOPPED)
	{
		ILUnitFailed("thread did not end in the `stopped' state");
	}
}

/*
 * Test that we can destroy a newly created thread that isn't started yet.
 */
static void thread_create_destroy(void *arg)
{
	ILThread *thread;

	/* Create the thread */
	thread = ILThreadCreate(sleepThread, (void *)4);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Wait 1 time step to let the system settle */
	sleepFor(1);

	/* Destroy the thread */
	ILThreadDestroy(thread);
}

/*
 * Test that new threads are created as "foreground" threads.
 */
static void thread_create_foreground(void *arg)
{
	ILThread *thread;
	int isbg;
	int state;

	/* Create the thread */
	thread = ILThreadCreate(sleepThread, (void *)4);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Check the background flag and the state for the thread */
	isbg = ILThreadGetBackground(thread);
	state = ILThreadGetState(thread);

	/* Destroy the thread */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(isbg)
	{
		ILUnitFailed("new thread was not created as `foreground'");
	}
	if((state & IL_TS_BACKGROUND) != 0)
	{
		ILUnitFailed("new thread state is not consistent with background flag");
	}
}

/*
 * Test that we can suspend a running thread.
 */
static void thread_suspend(void *arg)
{
	ILThread *thread;
	int state1;
	int state2;

	/* Create the thread */
	thread = ILThreadCreate(sleepThread, (void *)4);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Start the thread */
	ILThreadStart(thread);

	/* Wait 1 time step and then suspend it */
	sleepFor(1);
	if(!ILThreadSuspend(thread))
	{
		ILUnitFailed("ILThreadSuspend returned zero");
	}

	/* Wait 4 more time steps - the thread will exit if
	   it was not properly suspended */
	sleepFor(4);

	/* Get the thread's current state (which should be "suspended") */
	state1 = ILThreadGetState(thread);

	/* Resume the thread to allow it to exit normally */
	ILThreadResume(thread);

	/* Wait another time step: the "sleepFor" in the thread
	   has now expired and should terminate the thread */
	sleepFor(1);

	/* Get the current state (which should be "stopped") */
	state2 = ILThreadGetState(thread);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(state1 != IL_TS_SUSPENDED)
	{
		ILUnitFailed("thread did not suspend when requested");
	}
	if(state2 != IL_TS_STOPPED)
	{
		ILUnitFailed("thread did not end in the `stopped' state");
	}
}

/*
 * A thread that suspends itself.
 */
static void suspendThread(void *arg)
{
	ILThreadSuspend(ILThreadSelf());
	sleepFor(2);
}

/*
 * Test that we can resume a thread that has suspended itself.
 */
static void thread_suspend_self(void *arg)
{
	ILThread *thread;
	int state1;
	int state2;
	int state3;

	/* Create the thread */
	thread = ILThreadCreate(suspendThread, 0);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Start the thread, which should immediately suspend */
	ILThreadStart(thread);

	/* Wait 4 time steps - if the suspend is ignored, the
	   thread will run to completion */
	sleepFor(4);

	/* Get the thread's current state (which should be "suspended") */
	state1 = ILThreadGetState(thread);

	/* Resume the thread to allow it to exit normally */
	ILThreadResume(thread);

	/* Wait another time step to allow the thread to resume */
	sleepFor(1);

	/* Get the current state (which should be "running") */
	state2 = ILThreadGetState(thread);

	/* Wait two more time steps to allow the thread to terminate */
	sleepFor(2);

	/* Get the current state (which should be "stopped") */
	state3 = ILThreadGetState(thread);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(state1 != IL_TS_SUSPENDED)
	{
		ILUnitFailed("thread did not suspend itself");
	}
	if(state2 != IL_TS_RUNNING)
	{
		ILUnitFailed("thread did not resume when requested");
	}
	if(state3 != IL_TS_STOPPED)
	{
		ILUnitFailed("thread did not end in the `stopped' state");
	}
}

/*
 * Test that we can destroy a suspended thread.
 */
static void thread_suspend_destroy(void *arg)
{
	ILThread *thread;

	/* Create the thread */
	thread = ILThreadCreate(suspendThread, 0);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Start the thread, which should immediately suspend */
	ILThreadStart(thread);

	/* Wait 1 time step to let the system settle */
	sleepFor(1);

	/* Destroy the thread */
	ILThreadDestroy(thread);
}

/*
 * Thread start function that holds a mutex for a period of time.
 */
static void mutexHold(void *arg)
{
	ILMutex *mutex = ILMutexCreate();
	ILMutexLock(mutex);
	sleepFor(2);
	globalFlag = 1;
	ILMutexUnlock(mutex);
	ILMutexDestroy(mutex);
	sleepFor(2);
}

/*
 * Test that a thread cannot be suspended while it holds
 * a mutex, but that it will suspend as soon as it gives
 * up the mutex.
 */
static void thread_suspend_mutex(void *arg)
{
	ILThread *thread;
	int savedFlag;

	/* Create the thread */
	thread = ILThreadCreate(mutexHold, 0);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the global flag */
	globalFlag = 0;

	/* Start the thread, which should immediately suspend */
	ILThreadStart(thread);

	/* Wait 1 time step */
	sleepFor(1);

	/* Attempt to suspend the thread: this should block for 1 time step */
	ILThreadSuspend(thread);

	/* Save the global flag at this point */
	savedFlag = globalFlag;

	/* Resume the thread */
	ILThreadResume(thread);

	/* Wait 4 more time steps for the thread to exit */
	sleepFor(4);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors: the flag must have been set */
	if(!savedFlag)
	{
		ILUnitFailed("thread suspended while holding a mutex");
	}
}

/*
 * Thread start function that holds a read lock for a period of time.
 */
static void rwlockHold(void *arg)
{
	ILRWLock *rwlock = ILRWLockCreate();
	if(arg)
	{
		ILRWLockReadLock(rwlock);
	}
	else
	{
		ILRWLockWriteLock(rwlock);
	}
	sleepFor(2);
	globalFlag = 1;
	ILRWLockUnlock(rwlock);
	ILRWLockDestroy(rwlock);
	sleepFor(2);
}

/*
 * Test that a thread cannot be suspended while it holds
 * a read/write lock, but that it will suspend as soon as
 * it gives up the lock.  Read lock if arg != 0, and write
 * lock otherwise.
 */
static void thread_suspend_rwlock(void *arg)
{
	ILThread *thread;
	int savedFlag;

	/* Create the thread */
	thread = ILThreadCreate(rwlockHold, arg);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the global flag */
	globalFlag = 0;

	/* Start the thread, which should immediately suspend */
	ILThreadStart(thread);

	/* Wait 1 time step */
	sleepFor(1);

	/* Attempt to suspend the thread: this should block for 1 time step */
	ILThreadSuspend(thread);

	/* Save the global flag at this point */
	savedFlag = globalFlag;

	/* Resume the thread */
	ILThreadResume(thread);

	/* Wait 4 more time steps for the thread to exit */
	sleepFor(4);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors: the flag must have been set */
	if(!savedFlag)
	{
		ILUnitFailed("thread suspended while holding the lock");
	}
}

static int volatile mainWasSuspended;

/*
 * Thread start function that suspends the main thread.
 */
static void suspendMainThread(void *arg)
{
	ILThread *thread = (ILThread *)arg;

	/* Wait 1 time step and then suspend the main thread */
	sleepFor(1);
	ILThreadSuspend(thread);

	/* Wait 4 more time steps - the main thread will continue if
	   it was not properly suspended */
	sleepFor(4);

	/* Determine if the main thread was suspended */
	mainWasSuspended = (ILThreadGetState(thread) == IL_TS_SUSPENDED);

	/* Resume the main thread to allow it to continue normally */
	ILThreadResume(thread);
}

/*
 * Test that the main thread can be suspended by another thread.
 */
static void thread_suspend_main(void *arg)
{
	ILThread *thread;
	int flag;
	int state;

	/* Create the thread that will attempt to suspend us */
	thread = ILThreadCreate(suspendMainThread, ILThreadSelf());
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the "mainWasSuspended" flag */
	mainWasSuspended = 0;

	/* Start the thread */
	ILThreadStart(thread);

	/* Wait 2 time steps - main should be suspended during this time */
	sleepFor(2);

	/* Save the value of "mainWasSuspended".  This will be zero if
	   the main thread was not properly suspended */
	flag = mainWasSuspended;

	/* Wait 5 extra time steps for the system to settle */
	sleepFor(5);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* The main thread should now be in the "running" state */
	state = ILThreadGetState(ILThreadSelf());

	/* Check for errors */
	if(!flag)
	{
		ILUnitFailed("the main thread was not suspended");
	}
	if(state != IL_TS_RUNNING)
	{
		ILUnitFailed("the main thread did not return to the `running' state");
	}
}

/*
 * Thread start function that resumes the main thread.
 */
static void resumeMainThread(void *arg)
{
	ILThread *thread = (ILThread *)arg;

	/* Wait 4 time steps - if the suspend is ignored, the
	   main thread will continue running */
	sleepFor(4);

	/* Determine if the main thread is suspended */
	mainWasSuspended = (ILThreadGetState(thread) == IL_TS_SUSPENDED);

	/* Resume the main thread to allow it to continue execution */
	ILThreadResume(thread);
}

/*
 * Test that we can resume the main thread after it has suspended itself.
 */
static void thread_suspend_main_self(void *arg)
{
	ILThread *thread;
	int flag;
	int state;

	/* Create the thread */
	thread = ILThreadCreate(resumeMainThread, ILThreadSelf());
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the "mainWasSuspended" flag */
	mainWasSuspended = 0;

	/* Start the thread */
	ILThreadStart(thread);

	/* Suspend the main thread and sleep for 2 time steps */
	ILThreadSuspend(ILThreadSelf());
	sleepFor(2);

	/* Save the "mainWasSuspended" flag, which will be zero if
	   the main thread didn't really suspend */
	flag = mainWasSuspended;

	/* Wait 3 extra time steps for the resumption thread to exit */
	sleepFor(3);

	/* Get the main thread's current state (which should be "running") */
	state = ILThreadGetState(ILThreadSelf());

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(!flag)
	{
		ILUnitFailed("the main thread did not suspend itself");
	}
	if(state != IL_TS_RUNNING)
	{
		ILUnitFailed("the main thread did not return to the `running' state");
	}
}

/*
 * Flags that are used by "sleepILThread".
 */
static int volatile sleepResult;
static int volatile sleepDone;

/*
 * A thread procedure that sleeps for a number of time steps
 * using the "ILThreadSleep" function.
 */
static void sleepILThread(void *arg)
{
	sleepResult = ILThreadSleep((ILUInt32)STEPS_TO_MS((int)(ILNativeInt)arg));
	sleepDone = 1;
}

/*
 * Test thread sleep functionality.
 */
static void thread_sleep(void *arg)
{
	ILThread *thread;
	int done1;

	/* Create the thread */
	thread = ILThreadCreate(sleepILThread, (void *)2);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the global flags */
	sleepResult = 0;
	sleepDone = 0;

	/* Start the thread */
	ILThreadStart(thread);

	/* Wait 1 time step */
	sleepFor(1);

	/* The sleep should not be done yet */
	done1 = sleepDone;

	/* Wait 2 more time steps for the thread to exit */
	sleepFor(2);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(done1)
	{
		ILUnitFailed("thread did not sleep for the required amount of time");
	}
	if(!sleepDone)
	{
		ILUnitFailed("sleep did not end when expected");
	}
	if(!sleepResult)
	{
		ILUnitFailed("sleep was interrupted");
	}
}

/*
 * Test thread sleep interrupt functionality.
 */
static void thread_sleep_interrupt(void *arg)
{
	ILThread *thread;
	int done1;
	int result1;

	/* Create the thread */
	thread = ILThreadCreate(sleepILThread, (void *)3);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the global flags */
	sleepResult = 0;
	sleepDone = 0;

	/* Start the thread */
	ILThreadStart(thread);

	/* Wait 1 time step */
	sleepFor(1);

	/* Interrupt the thread */
	ILThreadInterrupt(thread);

	/* Wait 1 time step for the interrupt to be processed */
	sleepFor(1);

	/* Check that the sleep is done */
	done1 = sleepDone;
	result1 = sleepResult;

	/* Wait 2 more time steps for the thread to exit */
	sleepFor(2);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(!done1)
	{
		ILUnitFailed("sleep was not interrupted");
	}
	if(sleepResult != IL_WAIT_INTERRUPTED)
	{
		ILUnitFailed("sleep should have returned IL_WAIT_INTERRUPTED");
	}
}

/*
 * Test thread sleep suspend functionality.
 */
static void thread_sleep_suspend(void *arg)
{
	ILThread *thread;
	int done1;
	int done2;

	/* Create the thread */
	thread = ILThreadCreate(sleepILThread, (void *)2);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the global flags */
	sleepResult = 0;
	sleepDone = 0;

	/* Start the thread */
	ILThreadStart(thread);

	/* Wait 1 time step */
	sleepFor(1);

	/* Suspend the thread.  The suspend will wait until the sleep finishes */
	ILThreadSuspend(thread);

	/* Wait 2 time steps for the sleep to finish */
	sleepFor(2);

	/* The "sleepDone" flag should not be set yet, because the
	   thread has been suspended before it can be set */
	done1 = sleepDone;

	/* Resume the thread */
	ILThreadResume(thread);

	/* Wait 1 more time step for the thread to exit */
	sleepFor(1);

	/* The "sleepDone" flag should now be set */
	done2 = sleepDone;

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(done1)
	{
		ILUnitFailed("thread did not suspend after the sleep");
	}
	if(!done2)
	{
		ILUnitFailed("thread did not resume");
	}
}

/*
 * Test thread sleep suspend functionality, when the suspend is
 * resumed before the sleep finishes.
 */
static void thread_sleep_suspend_ignore(void *arg)
{
	ILThread *thread;
	int done1;

	/* Create the thread */
	thread = ILThreadCreate(sleepILThread, (void *)3);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the global flags */
	sleepResult = 0;
	sleepDone = 0;

	/* Start the thread */
	ILThreadStart(thread);

	/* Wait 1 time step */
	sleepFor(1);

	/* Suspend the thread.  The suspend will not be processed just yet */
	ILThreadSuspend(thread);

	/* Wait 1 time step */
	sleepFor(1);

	/* Resume the thread */
	ILThreadResume(thread);

	/* Wait 2 more time steps for the thread to exit */
	sleepFor(2);

	/* The "sleepDone" flag should now be set */
	done1 = sleepDone;

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(!done1)
	{
		ILUnitFailed("resume was not ignored");
	}
}

/*
 * Flags that are used by "checkGetSetValue".
 */
static int volatile correctFlag1;
static int volatile correctFlag2;
static int volatile correctFlag3;

/*
 * Thread start function that checks that its argument is set correctly,
 * and then changes the object to something else.
 */
static void checkGetSetValue(void *arg)
{
	ILThread *thread = ILThreadSelf();

	/* Check that the argument and thread object are 0xBADBEEF3 */
	correctFlag1 = (arg == (void *)0xBADBEEF1);
	correctFlag2 = (ILThreadGetObject(thread) == 0xBADBEEF3);

	/* Change the object to 0xBADBEEF4 and re-test */
	ILThreadSetObject(thread, (void *)0xBADBEEF4);
	correctFlag3 = (ILThreadGetObject(thread) == (void *)0xBADBEEF4);
}

/*
 * Test setting and getting the object on some other thread.
 */
static void thread_other_object(void *arg)
{
	ILThread *thread;
	int correct1;
	int correct2;
	int correct3;

	/* Create the thread */
	thread = ILThreadCreate(checkGetSetValue, (void *)0xBADBEEF1);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Get the current object, which should be 0 */
	correct1 = (ILThreadGetObject(thread) == 0);

	/* Change the object to 0xBADBEEF2 and check */
	ILThreadSetObject(thread, (void *)0xBADBEEF2);
	correct2 = (ILThreadGetObject(thread) == (void *)0xBADBEEF2);

	/* Change the object to 0xBADBEEF3 */
	ILThreadSetObject(thread, (void *)0xBADBEEF3);

	/* Start the thread, which checks to see if its argument is 0xBADBEEF3 */
	ILThreadStart(thread);

	/* Wait 1 time step for the thread to exit */
	sleepFor(1);

	/* Check that the final object value is 0xBADBEEF4 */
	correct3 = (ILThreadGetObject(thread) == (void *)0xBADBEEF4);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors */
	if(!correct1)
	{
		ILUnitFailed("initial object not set correctly");
	}
	if(!correct2)
	{
		ILUnitFailed("object could not be changed by main thread");
	}
	if(!correctFlag1)
	{
		ILUnitFailed("thread start function got wrong argument");
	}
	if(!correctFlag2)
	{
		ILUnitFailed("thread object not set properly");
	}
	if(!correctFlag3)
	{
		ILUnitFailed("could not change object in thread start function");
	}
	if(!correct3)
	{
		ILUnitFailed("final object value incorrect");
	}
}

/*
 * Make sure that the thread counts have returned to the correct values.
 * This indirectly validates that the "thread_create_destroy" and
 * "thread_suspend_destroy" tests updated the thread counts correctly.
 * It also validates that normal thread exits update the thread counts
 * correctly.
 */
static void thread_counts(void *arg)
{
	unsigned long numForeground;
	unsigned long numBackground;
	ILThreadGetCounts(&numForeground, &numBackground);
	if(numForeground != 1)
	{
		ILUnitFailed("foreground thread count has not returned to 1");
	}
	if(numBackground > 1)
	{
		/* The GC thread doesn't start until needed so numBackground can be 0 */
		/* Currently there is one background thread (the finalizer thread) */
		ILUnitFailed("background thread count has not returned to 0 or 1");
	}
}

/*
 * Test wait mutex creation.
 */
static void mutex_create(void *arg)
{
	ILWaitHandle *handle;
	ILWaitHandle *handle2;
	int gotOwn;

	/* Create simple mutexes */
	handle = ILWaitMutexCreate(0);
	if(!handle)
	{
		ILUnitFailed("could not create a simple mutex");
	}
	ILWaitHandleClose(handle);
	handle = ILWaitMutexCreate(1);
	if(!handle)
	{
		ILUnitFailed("could not create a simple mutex that is owned");
	}
	ILWaitHandleClose(handle);

	/* Create a named mutex */
	gotOwn = -100;
	handle = ILWaitMutexNamedCreate("aaa", 0, &gotOwn);
	if(!handle)
	{
		ILUnitFailed("could not create a named mutex");
	}
	if(gotOwn == -100)
	{
		ILUnitFailed("gotOwnership was not changed");
	}
	if(gotOwn)
	{
		ILUnitFailed("gotOwnership was set when it should have been cleared");
	}

	/* Create the same named mutex and acquire it */
	gotOwn = -100;
	handle2 = ILWaitMutexNamedCreate("aaa", 1, &gotOwn);
	if(!handle2)
	{
		ILUnitFailed("could not create a named mutex (2)");
	}
	if(handle != handle2)
	{
		ILUnitFailed("did not reacquire the same mutex");
	}
	if(gotOwn == -100)
	{
		ILUnitFailed("gotOwnership was not changed (2)");
	}
	if(!gotOwn)
	{
		ILUnitFailed("gotOwnership was cleared when it "
					 "should have been set (2)");
	}

	/* Close the second copy of the named mutex */
	ILWaitMutexRelease(handle2);
	ILWaitHandleClose(handle2);

	/* Create a named mutex with a different name */
	handle2 = ILWaitMutexNamedCreate("bbb", 0, 0);
	if(!handle2)
	{
		ILUnitFailed("could not create a named mutex (3)");
	}
	if(handle == handle2)
	{
		ILUnitFailed("mutexes with different names have the same handle");
	}

	/* Clean up */
	ILWaitHandleClose(handle);
	ILWaitHandleClose(handle2);
}

/*
 * Test monitor creation.
 */
static void monitor_create(void *arg)
{
	ILWaitHandle *handle;
	handle = ILWaitMonitorCreate();
	if(!handle)
	{
		ILUnitFailed("could not create a monitor");
	}
	ILWaitHandleClose(handle);
}

/*
 * Test monitor acquire/release.
 */
static void monitor_acquire(void *arg)
{
	ILWaitHandle *handle;

	/* Create the monitor */
	handle = ILWaitMonitorCreate();
	if(!handle)
	{
		ILUnitFailed("could not create a monitor");
	}

	/* Acquire it: zero timeout but we should get it immediately */
	if(ILWaitMonitorTryEnter(handle, 0) != 0)
	{
		ILUnitFailed("could not acquire (1)");
	}

	/* Acquire it again */
	if(ILWaitMonitorEnter(handle) != 0)
	{
		ILUnitFailed("could not acquire (2)");
	}

	/* Release twice */
	if(!ILWaitMonitorLeave(handle))
	{
		ILUnitFailed("could not release (1)");
	}
	if(!ILWaitMonitorLeave(handle))
	{
		ILUnitFailed("could not release (2)");
	}

	/* Try to release again, which should fail */
	if(ILWaitMonitorLeave(handle) != 0)
	{
		ILUnitFailed("released a monitor that we don't own");
	}

	/* Clean up */
	ILWaitHandleClose(handle);
}

/*
 * Thread start function that holds a monitor for a period of time.
 */
static void monitorHold(void *arg)
{
	ILWaitHandle *monitor = ILWaitMonitorCreate();
	ILWaitMonitorEnter(monitor);
	sleepFor(2);
	globalFlag = 1;
	ILWaitMonitorLeave(monitor);
	ILWaitHandleClose(monitor);
	sleepFor(2);
}

/*
 * Test that a thread can be suspended while it holds a monitor.
 */
static void monitor_suspend(void *arg)
{
	ILThread *thread;
	int savedFlag;

	/* Create the thread */
	thread = ILThreadCreate(monitorHold, 0);
	if(!thread)
	{
		ILUnitOutOfMemory();
	}

	/* Clear the global flag */
	globalFlag = 0;

	/* Start the thread, which should immediately suspend */
	ILThreadStart(thread);

	/* Wait 1 time step */
	sleepFor(1);

	/* Suspend the thread */
	ILThreadSuspend(thread);

	/* Wait for 4 time steps (enough for the thread to exit
	   if it wasn't suspended) */
	sleepFor(4);

	/* Save the global flag at this point */
	savedFlag = globalFlag;

	/* Resume the thread */
	ILThreadResume(thread);

	/* Wait 4 more time steps for the thread to exit */
	sleepFor(4);

	/* Clean up the thread object (the thread itself is now dead) */
	ILThreadDestroy(thread);

	/* Check for errors: the flag must not have been set */
	if(savedFlag)
	{
		ILUnitFailed("thread holding the monitor did not suspend");
	}
	if(!globalFlag)
	{
		ILUnitFailed("thread holding the monitor did not finish");
	}
}

/*
 * Simple test registration macro.
 */
#define	RegisterSimple(name)	(ILUnitRegister(#name, name, 0))

/*
 * Register all unit tests.
 */
void ILUnitRegisterTests(void)
{
	/*
	 * Bail out if no thread support at all in the system.
	 */
	if(!ILHasThreads())
	{
		fputs("System does not support threads - skipping all tests\n", stdout);
		return;
	}

	/*
	 * Initialize the thread subsystem.
	 */
	ILThreadInit();

	/*
	 * Initialize the GC system (the GC is used to create threads).
	 */
	ILGCInit(0);	

	/*
	 * Test the properties of the "main" thread.
	 */
	ILUnitRegisterSuite("Main Thread Properties");
	RegisterSimple(thread_main_nonnull);
	RegisterSimple(thread_main_object);
	RegisterSimple(thread_main_running);
	RegisterSimple(thread_main_foreground);

	/*
	 * Test thread creation behaviours.
	 */
	ILUnitRegisterSuite("Thread Creation");
	RegisterSimple(thread_create_arg);
	RegisterSimple(thread_create_suspended);
	RegisterSimple(thread_create_state);
	RegisterSimple(thread_create_destroy);
	RegisterSimple(thread_create_foreground);

	/*
	 * Test thread suspend behaviours.
	 */
	ILUnitRegisterSuite("Thread Suspend");
	RegisterSimple(thread_suspend);
	RegisterSimple(thread_suspend_self);
	RegisterSimple(thread_suspend_destroy);
	RegisterSimple(thread_suspend_mutex);
	ILUnitRegister("thread_suspend_rdlock", thread_suspend_rwlock, (void *)1);
	ILUnitRegister("thread_suspend_wrlock", thread_suspend_rwlock, (void *)0);
	RegisterSimple(thread_suspend_main);
	RegisterSimple(thread_suspend_main_self);

	/*
	 * Test thread sleep and interrupt behaviours.
	 */
	ILUnitRegisterSuite("Thread Sleep");
	RegisterSimple(thread_sleep);
	RegisterSimple(thread_sleep_interrupt);
	RegisterSimple(thread_sleep_suspend);
	RegisterSimple(thread_sleep_suspend_ignore);

	/*
	 * Test miscellaneous thread behaviours.
	 */
	ILUnitRegisterSuite("Misc Thread Tests");
	RegisterSimple(thread_other_object);
	RegisterSimple(thread_counts);

	/*
	 * Test wait mutex behaviours.
	 */
	ILUnitRegisterSuite("Wait Mutex Tests");
	RegisterSimple(mutex_create);

	/*
	 * Test monitor behaviours.
	 */
	ILUnitRegisterSuite("Monitor Tests");
	RegisterSimple(monitor_create);
	RegisterSimple(monitor_acquire);
	RegisterSimple(monitor_suspend);
}

#ifdef	__cplusplus
};
#endif
