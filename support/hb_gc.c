/*
 * hb_gc.c - Interface to the Hans-Boehm garbage collector.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * Contributions by Thong Nguyen <tum@veridicus.com>
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
#include "il_thread.h"
#include "thr_defs.h"
#include "stdio.h"

#ifdef HAVE_LIBGC

#include "../libgc/include/gc.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Set to non-zero if finalization has been temporarily disabled.
 */
static int volatile FinalizersDisabled = 0;

static volatile int g_Deinit = 0;
static ILThread *g_FinalizerThread;
static ILWaitHandle *g_FinalizerSignal;
static ILWaitHandle *g_FinalizerResponse;

#ifdef GC_TRACE_ENABLE
	#define GC_TRACE(a, b)		printf(a, b)
#else
	#define GC_TRACE(a, b)
#endif

/*
 *	Main entry point for the finalizer thread.
 */
static void FinalizerThreadFunc(void *data)
{
	GC_TRACE("GC:FinalizerThread: Finalizer thread started [thread:%d]\n", (int)ILThreadSelf());

	for (;;)
	{
		ILWaitOne(g_FinalizerSignal, -1);

		GC_TRACE("GC:FinalizerThread: Signal [thread:%d]\n", (int)ILThreadSelf());

		if (GC_should_invoke_finalizers())
		{
			GC_TRACE("GC:FinalizerThread: Finalizers running [thread:%d]\n", (int)ILThreadSelf());

			GC_invoke_finalizers();

			GC_TRACE("GC:FinalizerThread: Finalizers finished [thread:%d]\n", (int)ILThreadSelf());
		}
		
		if (g_Deinit)
		{
			/* Exit finalizer thread after having invoked finalizers one last time */

			GC_TRACE("GC:FinalizerThread: Finalizer thread finished [thread:%d]\n", (int)ILThreadSelf());

			ILWaitEventReset(g_FinalizerSignal);
			ILWaitEventSet(g_FinalizerResponse);
			
			return;
		}

		GC_TRACE("GC:FinalizerThread: Response [thread:%d]\n", (int)ILThreadSelf());

		ILWaitEventReset(g_FinalizerSignal);
		ILWaitEventSet(g_FinalizerResponse);
	}
}

/*
 * Notify the finalization thread that there is work to do.
 */
static int PrivateGCNotifyFinalize(void)
{
	ILExecThread *thread;

	if (!ILHasThreads())
	{
		GC_invoke_finalizers();

		return;
	}

	if(!FinalizersDisabled)
	{
		/* Register the finalizer thread for managed code execution */

		ILThreadAtomicStart();

		if (ILThreadGetObject(g_FinalizerThread) == 0)
		{			
			/* Make sure the the finalizer thread is registered for managed execution. */

			/* This can't be done in ILGCInit cause the runtime isn't fully initialized in that function */

			thread = (ILExecThread *)ILThreadGetObject(ILThreadSelf());

			ILThreadRegisterForManagedExecution(ILExecThreadGetProcess(thread), g_FinalizerThread, 0);
		}

		ILThreadAtomicEnd();

		/* Signal the finalizer thread */

		ILWaitEventSet(g_FinalizerSignal);

		return 1;
	}
	else
	{
		return 0;
	}
}

static void GCNotifyFinalize(void)
{
	PrivateGCNotifyFinalize();
}

void ILGCInit(unsigned long maxSize)
{
	GC_INIT();		/* For shared library initialization on sparc */
	GC_init();		/* For Cygwin and Win32 threads */
	GC_set_max_heap_size((size_t)maxSize);
	
	/* Set up the finalization system the way we want it */
	GC_finalize_on_demand = 1;
	GC_java_finalization = 1;
	GC_finalizer_notifier = GCNotifyFinalize;
	FinalizersDisabled = 0;

	/* Create the finalizer thread */
	g_Deinit = 0;
	g_FinalizerThread = ILThreadCreate(FinalizerThreadFunc, 0);
	
	if (g_FinalizerThread)
	{
		g_FinalizerSignal = ILWaitEventCreate(1, 0);
		g_FinalizerResponse = ILWaitEventCreate(1, 0);

		ILThreadStart(g_FinalizerThread);
	}
}

void ILGCDeinit()
{
	g_Deinit = 1;

	if (g_FinalizerThread)
	{
		/* Notify the finalizer thread */
		ILWaitEventSet(g_FinalizerSignal);

		/* Wait 10(!?) seconds for the finalizers to finish */
		ILWaitOne(g_FinalizerResponse, 10000);
	}
}

void *ILGCAlloc(unsigned long size)
{
	/* The Hans-Boehm routines guarantee to zero the block */
	return GC_MALLOC((size_t)size);
}

void *ILGCAllocAtomic(unsigned long size)
{
	void *block = GC_MALLOC_ATOMIC((size_t)size);
	if(block)
	{
		/* The Hans-Boehm routines don't guarantee to zero the block */
		ILMemZero(block, size);
	}
	return block;
}

void *ILGCAllocPersistent(unsigned long size)
{
	/* The Hans-Boehm routines guarantee to zero the block */
	return GC_MALLOC_UNCOLLECTABLE((size_t)size);
}

void ILGCFreePersistent(void *block)
{
	if(block)
	{
		GC_FREE(block);
	}
}

void ILGCRegisterFinalizer(void *block, ILGCFinalizer func, void *data)
{
	/* We use the Java-style finalization algorithm, which
	   ignores cycles in the object structure */
	GC_REGISTER_FINALIZER_NO_ORDER(block, func, data, 0, 0);	
}

void ILGCMarkNoPointers(void *start, unsigned long size)
{
	GC_exclude_static_roots(start, (void *)(((unsigned char *)start) + size));
}

void ILGCCollect(void)
{
	GC_gcollect();
}

void ILGCInvokeFinalizers(void)
{
	if (!ILHasThreads())
	{
		PrivateGCNotifyFinalize();

		return;
	}

	if(!FinalizersDisabled)
	{
		/* Signal the finalizers to run (returns 1 if successful) */
		if (PrivateGCNotifyFinalize())
		{
			GC_TRACE("ILGCInvokeFinalizers: Invoked finalizers and waiting [thread: %d]\n", (int)ILThreadSelf());
			
			/* Wait until finalizers have finished */
			ILWaitOne(g_FinalizerResponse, -1);

			GC_TRACE("ILGCInvokeFinalizers: Finalizers finished[thread: %d]\n", (int)ILThreadSelf());
		}
	}
}

void ILGCDisableFinalizers(void)
{
	FinalizersDisabled = 1;
}

void ILGCEnableFinalizers(void)
{
	FinalizersDisabled = 0;
}

long ILGCGetHeapSize(void)
{
	return (long)GC_get_heap_size();
}

void ILGCRegisterWeak(void *ptr)
{
	GC_register_disappearing_link(ptr);
}

void ILGCUnregisterWeak(void *ptr)
{
	GC_unregister_disappearing_link(ptr);
}

void ILGCRegisterGeneralWeak(void *ptr, void *obj)
{
	GC_general_register_disappearing_link(ptr, obj);
}

#ifdef	__cplusplus
};
#endif

#endif /* HAVE_LIBGC */
