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
 * Thread start function that sets a global flag.
 */
static void setFlag(void *arg)
{
	globalFlag = 1;
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
	RegisterSimple(thread_create_suspended);
	RegisterSimple(thread_create_state);

	/*
	 * Test thread suspend and resume behaviours.
	 */
	ILUnitRegisterSuite("Thread Suspend/Resume");
	RegisterSimple(thread_suspend);
}

#ifdef	__cplusplus
};
#endif
