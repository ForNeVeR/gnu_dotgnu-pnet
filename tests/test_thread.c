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
#if HAVE_UNISTD_H
	#include <unistd.h>
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
#else
	/* Time steps are 1s in length */
	sleep(steps);
#endif
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
	sleepFor((int)arg);
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
	correctFlag1 = (arg == (void *)0xBADBEEF3);
	correctFlag2 = (ILThreadGetObject(thread) == arg);

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

	/* Get the current object, which should be 0xBADBEEF1 */
	correct1 = (ILThreadGetObject(thread) == (void *)0xBADBEEF1);

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
		ILUnitFailed("thread object not the same as thread argument");
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
	if(numBackground != 0)
	{
		ILUnitFailed("background thread count has not returned to 0");
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
	 * Test thread creation behaviours.
	 */
	ILUnitRegisterSuite("Thread Creation");
	RegisterSimple(thread_create_arg);
	RegisterSimple(thread_create_suspended);
	RegisterSimple(thread_create_state);
	RegisterSimple(thread_create_destroy);

	/*
	 * Test thread suspend and resume behaviours.
	 */
	ILUnitRegisterSuite("Thread Suspend/Resume");
	RegisterSimple(thread_suspend);
	RegisterSimple(thread_suspend_self);
	RegisterSimple(thread_suspend_destroy);

	/*
	 * Test the properties of the "main" thread.
	 */
	ILUnitRegisterSuite("Main Thread Properties");
	RegisterSimple(thread_main_nonnull);
	RegisterSimple(thread_main_object);
	RegisterSimple(thread_main_running);

	/*
	 * Test miscellaneous thread behaviours.
	 */
	ILUnitRegisterSuite("Misc Thread Tests");
	RegisterSimple(thread_other_object);
	RegisterSimple(thread_counts);
}

#ifdef	__cplusplus
};
#endif
