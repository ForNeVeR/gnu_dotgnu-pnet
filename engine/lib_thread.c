/*
 * lib_thread.c - Internalcall methods for "System.Threading.*".
 *
 * Copyright (C) 2001, 2002, 2003  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Thong Nguyen <tum@veridicus.com>
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

#include "engine.h"
#include "lib_defs.h"
#include "../support/interlocked.h"

#if HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#if HAVE_UNISTD_H
	#include <unistd.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Comments on terminology:
 *
 * Support Thread:	Refers to an ILThread instance.
 * Engine Thread:		Refers to an ILExecThread instance.
 * CLR Thread:		Refers to a System.Threading.Thread instance. 
 */

/*
 * Timeout value the managed WaitHandle uses.
 */
#define IL_CLR_WAIT_TIMEOUT (258L)

/*
 * Maximum number of wait handles that can be processed at once.
 */
#define	IL_MAX_WAIT_HANDLES		64


/*************************************/
/* Interlocked functions for ILInt32 */
/*************************************/

/*
 * public static int CompareExchange(ref int location1, int value,
 *									 int comparand);
 */
ILInt32 _IL_Interlocked_CompareExchange_Riii
						(ILExecThread *thread,
						 ILInt32 *location1,
						 ILInt32 value,
						 ILInt32 comparand)
{
	return ILInterlockedCompareAndExchange(location1, value, comparand);
}

/*
* public static int Increment(ref int location);
*/
ILInt32 _IL_Interlocked_Increment_Ri(ILExecThread *thread, ILInt32 *location)
{
	return ILInterlockedIncrement(location);
}


/*
* public static int Decrement(ref int location);
*/
ILInt32 _IL_Interlocked_Decrement_Ri(ILExecThread *thread, ILInt32 *location)
{
	return ILInterlockedDecrement(location);
}

/*
 * public static int Exchange(ref int location, int value);
 */
ILInt32 _IL_Interlocked_Exchange_Rii(ILExecThread *thread,
									 ILInt32 *location, ILInt32 value)
{
	return ILInterlockedExchange(location, value);
}

/*************************************/
/* Interlocked functions for ILInt64 */
/*************************************/

/*
 * public static long Increment(ref long location);
 */
ILInt64 _IL_Interlocked_Increment_Rl(ILExecThread *thread, ILInt64 *location)
{
	ILInt64 value;
	ILThreadAtomicStart();
	ILMemCpy(&value, location, sizeof(ILInt64));
	++value;
	ILMemCpy(location, &value, sizeof(ILInt64));
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static long Decrement(ref long location);
 */
ILInt64 _IL_Interlocked_Decrement_Rl(ILExecThread *thread, ILInt64 *location)
{
	ILInt64 value;
	ILThreadAtomicStart();
	ILMemCpy(&value, location, sizeof(ILInt64));
	--value;
	ILMemCpy(location, &value, sizeof(ILInt64));
	ILThreadAtomicEnd();
	return value;
}

/*************************************/
/* Interlocked functions for ILFloat */
/*************************************/

/*
 * public static float Exchange(ref float location, float value);
 */
ILFloat _IL_Interlocked_Exchange_Rff(ILExecThread *thread,
								     ILFloat *location, ILFloat value)
{
#if (SIZEOF_FLOAT == 4)
	ILInt32 retval;

	/* memcpy to get rid of type-punned pointer warning */
	retval = ILInterlockedExchange((ILInt32 *)location, *(ILInt32 *)(void *)&value);

	return *(ILFloat *)(void *)&retval;
#else
	ILFloat orig;
	ILThreadAtomicStart();
	orig = *location;
	*location = value;
	ILThreadAtomicEnd();
	return orig;
#endif
}

/*
 * public static float CompareExchange(ref float location1, float value,
 *									   float comparand);
 */
ILFloat _IL_Interlocked_CompareExchange_Rfff(ILExecThread *thread, ILFloat *location1,
						ILFloat value, ILFloat comparand)
{
#if (SIZEOF_FLOAT == 4)
	ILInt32 retval;

	retval = ILInterlockedCompareAndExchange((ILInt32 *)location1, *((ILInt32 *)(void *)&value), *((ILInt32 *)(void *)&comparand));

	return *(ILFloat *)(void *)&retval;
#else
	ILFloat orig;
	ILThreadAtomicStart();
	if((orig = *location1) == comparand)
	{
		*location1 = value;
	}
	ILThreadAtomicEnd();

	return orig;
#endif
}

/***************************************/
/* Interlocked functions for ILObject* */
/***************************************/

/*
 * public static Object Exchange(ref Object location, Object value);
 */
ILObject *_IL_Interlocked_Exchange_RObjectObject
(ILExecThread *thread, ILObject **location, ILObject *value)
{
	return ILInterlockedExchangePointers((void **)location, (void *)value);
}

/*
 * public static Object CompareExchange(ref Object location1, Object value,
 *									    Object comparand);
 */
ILObject *_IL_Interlocked_CompareExchange_RObjectObjectObject
						(ILExecThread *thread,
						 ILObject **location1,
						 ILObject *value,
						 ILObject *comparand)
{
	return (ILObject *)ILInterlockedCompareAndExchangePointers((void **)location1, (void *)value, (void *)comparand);
}


/*
 *  Thread function for ILThread.
 *
 *  @param objectArg	The ILExecThread for the new thread.
 */
static void __PrivateThreadStart(void *objectArg)
{
	ILMethod *method;
	ILExecValue result;
	ILExecThread *thread;

	thread = (ILExecThread *)objectArg;

	/* Get the ThreadStart.Invoke method */
	method = ILExecThreadLookupMethod(thread, "System.Threading.ThreadStart", "Invoke", "(T)V");

	/* Invoke the ThreadStart delegate */
	ILExecThreadCall(thread, method, &result, ((System_Thread *)thread->clrThread)->start);

	/* Print out any uncaught exceptions */
	if (ILExecThreadHasException(thread)
		&& !ILExecThreadIsThreadAbortException(thread, ILExecThreadGetException(thread)))
	{				
		ILExecThreadPrintException(thread);
	}
}

/*
 * private void InitializeThread();
 */
void _IL_Thread_InitializeThread(ILExecThread *thread, ILObject *_this)
{
	ILThread *supportThread;

	if (((System_Thread *)_this)->privateData != NULL)
	{
		/* InitializeThread can't be called more than once */

		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", (const char *)0);

		return;
	}
	
	if (!ILHasThreads())
	{
		/* Threading isn't supported */

		ILExecThreadThrowSystem(thread, "System.NotSupportedException", 
			"Exception_ThreadsNotSupported");

		return;
	}

	/* Create a new support thread */

	if ((supportThread = ILThreadCreate(__PrivateThreadStart, 0)) == 0)
	{
		/* Threading is supported but creation failed.
			ThrowOutOfMemoryException */
			
		ILExecThreadThrowOutOfMemory(thread);
		
		return;
	}

	/* Associate the support thread with the CLR thread */
	((System_Thread *)_this)->privateData = supportThread;
}

/*
 * private void FinalizeThread();
 */
void _IL_Thread_FinalizeThread(ILExecThread *thread, ILObject *_this)
{
	ILThread *supportThread;
	System_Thread *clrThread;

	clrThread = (System_Thread *)_this;
	supportThread = clrThread->privateData;
	
	/* Only threads created from managed code should destroy the underlying
	   support thread.  Threads created outside of managed code but which
	   enter managed code are the reponsibility of the unmanaged code that
	   created them. */
	if (supportThread && clrThread->createdFromManagedCode)
	{
		clrThread->privateData = 0;

		ILThreadDestroy(supportThread);		
	}
}

/*
 * public void Abort();
 */
void _IL_Thread_Abort(ILExecThread *thread, ILObject *_this)
{
	ILThread *supportThread;
	ILExecThread *execThread;

	supportThread = ((System_Thread *)_this)->privateData;

	/* Lock the process to prevent the ILExecThread from being destroyed
	   while we are accessing it (This could happen if Abort is called on a
	   thread that has finished and is in the process of cleaning up itself) */
	
	ILMutexLock(thread->process->lock);

	execThread = ILExecThreadFromThread(supportThread);

	if (execThread == 0)
	{		
		ILMutexUnlock(thread->process->lock);
		return;
	}
	else
	{
		execThread->abortRequested = 1;
	}

	ILMutexUnlock(thread->process->lock);

	ILThreadAbort(supportThread);

	/* If the current thread is aborting itself then abort immediately */
	if (supportThread == thread->supportThread)
	{
		_ILAbortThread(thread);		
	}
}

/*
 * public void Interrupt();
 */
void _IL_Thread_Interrupt(ILExecThread *thread, ILObject *_this)
{
	ILThreadInterrupt(((System_Thread *)_this)->privateData);

	/* If the current thread is interrupting itself then it will
	   interrupt next time it enters a wait/sleep/join state */
}

/*
 * public void Suspend();
 */
void _IL_Thread_Suspend(ILExecThread *thread, ILObject *_this)
{	
	if (ILThreadSuspend(((System_Thread *)_this)->privateData) == 0)
	{
		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadStateException",
				(const char *)0
			);
	}
}

/*
 * public void Resume();
 */
void _IL_Thread_Resume(ILExecThread *thread, ILObject *_this)
{	
	ILThreadResume(((System_Thread *)_this)->privateData);
}

/*
 * public static void SpinWait(int iterations);
 */
void _IL_Thread_SpinWait(ILExecThread *_thread, ILInt32 iterations)
{
	/*
	 * Spin wait is used to yield CPU time from one logical
	 * CPU to another logical CPU in a hyper-threaded CPU.
	 * see: http://blogs.msdn.com/cbrumme/archive/2003/04/15/51321.aspx
	 */

	if(iterations > 0)
	{		
		/*
		 * Can't use ILThreadSleep because it will catch interrupts
		 * and we don't want it to do that.
		 *
		 * Also, usleep() isn't thread safe on some platforms.
		 */
		ILThreadYield();		
	}
}

/*
 * private bool InternalJoin(int timeout);
 */
ILBool _IL_Thread_InternalJoin(ILExecThread *thread, ILObject *_this,
							   ILInt32 timeout)
{	
	ILThread *supportThread;

	supportThread = ((System_Thread *)_this)->privateData;
	
	switch (ILThreadJoin(supportThread, timeout))
	{	
	case IL_JOIN_OK:
	case IL_JOIN_SELF:		
		return 1;
	case IL_JOIN_TIMEOUT:		
		return 0;
	case IL_JOIN_MEMORY:		
		ILExecThreadThrowOutOfMemory(thread);
		return 0;
	case IL_JOIN_UNSTARTED:
		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadStateException",
				(const char *)0
			);
		return 0;
	case IL_JOIN_INTERRUPTED:
		
		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadInterruptedException",
				(const char *)0
			);
		
		return 0;

	case IL_JOIN_ABORTED:

		_ILAbortThread(thread);

		return 0;

	default:	

		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadStateException",
				(const char *)0
			);

		return 0;
	}
}

/*
 * public static void MemoryBarrier();
 */
void _IL_Thread_MemoryBarrier(ILExecThread *thread)
{
	ILThreadMemoryBarrier();
}

/*
 * public static void ResetAbort();
 */
void _IL_Thread_ResetAbort(ILExecThread *thread)
{
	if (!ILThreadIsAborting())
	{
		/* No abort has been requested */

		ILExecThreadThrowSystem(thread,
			"System.Threading.ThreadStateException", (const char *)0);

		return;
	}

	if (ILThreadAbortReset())
	{
		thread->aborting = 0;
		thread->abortRequested = 0;
		thread->abortHandlerEndPC = 0;
		thread->abortHandlerFrame = 0;
		thread->threadAbortException = 0;
	}
}

/*
 * public static void InternalSleep(int timeout);
 */
void _IL_Thread_InternalSleep(ILExecThread *thread, ILInt32 timeout)
{
	int result;
	
	if (timeout < -1)
	{
		ILExecThreadThrowSystem(thread,
			"System.ArgumentOutOfRangeException", (const char *)0);
	}
	
	result = ILThreadSleep((ILUInt32)timeout);
	
	_ILHandleWaitResult(thread, result);
}

/*
 * public void Start();
 */
void _IL_Thread_Start(ILExecThread *thread, ILObject *_this)
{
	ILThread *supportThread;
	ILExecThread *execThread;

	_IL_Monitor_Enter(thread, _this);

	/* Get the support thread stored inside the first field of the 
	CLR thread by _IL_Thread_InitializeThread */

	supportThread = ((System_Thread *)_this)->privateData;
	
	if (supportThread == 0
		|| (ILThreadGetState(supportThread) & IL_TS_UNSTARTED) == 0)
	{
		/* Thread has already been started or has ended. */

		_IL_Monitor_Exit(thread, _this);

		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", 
			"Exception_ThreadAlreadyStarted");
		
		return;
	}

	/* Register the support thread for managed code execution */

	if ((execThread = ILThreadRegisterForManagedExecution(thread->process, supportThread)) == 0)
	{
		/* Runtime isn't fully initialized.  This should never happen */

		_IL_Monitor_Exit(thread, _this);

		ILExecThreadThrowSystem(thread, "System.ExecutionEngineException",
			"Exception_UnexpectedEngineState");
		
		return;
	}
	
	/* Associate the CLR thread with the engine thread */

	execThread->clrThread = _this;

	/* Start the support thread */

	if (ILThreadStart(supportThread) == 0)
	{
		/* Start unsuccessul.  Do a GC then try again */

		ILGCCollect();
		ILGCInvokeFinalizers();
	
		if (ILThreadStart(supportThread) == 0)
		{
			/* Start unsuccessful.  Destroy the engine thread */
			/* The support thread will linger as long as the CLR thread does */

			ILThreadUnregisterForManagedExecution(supportThread);
			
			/* Throw an OutOfMemoryException */

			ILExecThreadThrowOutOfMemory(thread);
		}
	}
	
	_IL_Monitor_Exit(thread, _this);
}

/*
 * public static Thread InternalCurrentThread();
 */
ILObject *_IL_Thread_InternalCurrentThread(ILExecThread *thread)
{
	return ILExecThreadGetClrThread(thread);
}

/*
 * public void InternalSetBackground(bool value);
 */
void _IL_Thread_InternalSetBackground(ILExecThread *thread,
									  ILObject *_this, ILBool value)
{
	if (ILThreadGetState(((System_Thread *)_this)->privateData) == IL_TS_STOPPED)
	{
		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", (const char *)0);

		return;
	}

	ILThreadSetBackground(((System_Thread *)_this)->privateData, value);
}

/*
 * public ThreadPriority InternalGetPriority();
 */
ILInt32 _IL_Thread_InternalGetPriority(ILExecThread *thread, ILObject *_this)
{
	ILThread *supportThread;

	supportThread = ((System_Thread *)_this)->privateData;

	if ((ILThreadGetState(supportThread) 
			& (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED | IL_TS_STOPPED)) != 0)
	{
		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", (const char *)0);

		return 0;
	}

	return ILThreadGetPriority(supportThread);
}

/*
 * public void InternalSetPriority(ThreadPriority priority);
 */
void _IL_Thread_InternalSetPriority(ILExecThread *thread, ILObject *_this,
									ILInt32 priority)
{
	ILThread *supportThread;
	
	supportThread = ((System_Thread *)_this)->privateData;

	if ((ILThreadGetState(supportThread)
			& (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED | IL_TS_STOPPED)) != 0)
	{
		ILExecThreadThrowSystem(thread, "System.Threading.ThreadStateException", (const char *)0);

		return;
	}

	ILThreadSetPriority(supportThread, priority);
}

/*
 * public ThreadState InternalGetState();
 */
ILInt32 _IL_Thread_InternalGetState(ILExecThread *thread, ILObject *_this)
{
	return ILThreadGetState(((System_Thread *)_this)->privateData);
}

/*
 * internal static bool CanStartThreads();
 */
ILBool _IL_Thread_CanStartThreads(ILExecThread *_thread)
{
	return ILHasThreads();
}

/*
 * public static sbyte VolatileRead(ref sbyte address);
 */
ILInt8 _IL_Thread_VolatileRead_Rb(ILExecThread *thread, ILInt8 *address)
{
	ILInt8 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static byte VolatileRead(ref byte address);
 */
ILUInt8 _IL_Thread_VolatileRead_RB(ILExecThread *thread, ILUInt8 *address)
{
	ILUInt8 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static short VolatileRead(ref short address);
 */
ILInt16 _IL_Thread_VolatileRead_Rs(ILExecThread *thread, ILInt16 *address)
{
	ILInt16 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static ushort VolatileRead(ref ushort address);
 */
ILUInt16 _IL_Thread_VolatileRead_RS(ILExecThread *thread, ILUInt16 *address)
{
	ILUInt16 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static int VolatileRead(ref int address);
 */
ILInt32 _IL_Thread_VolatileRead_Ri(ILExecThread *thread, ILInt32 *address)
{
	ILInt32 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static uint VolatileRead(ref uint address);
 */
ILUInt32 _IL_Thread_VolatileRead_RI(ILExecThread *thread, ILUInt32 *address)
{
	ILUInt32 value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static long VolatileRead(ref long address);
 */
ILInt64 _IL_Thread_VolatileRead_Rl(ILExecThread *thread, ILInt64 *address)
{
	ILInt64 value;
	ILThreadAtomicStart();
	ILMemCpy(&value, address, sizeof(ILInt64));
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static ulong VolatileRead(ref ulong address);
 */
ILUInt64 _IL_Thread_VolatileRead_RL(ILExecThread *thread, ILUInt64 *address)
{
	ILUInt64 value;
	ILThreadAtomicStart();
	ILMemCpy(&value, address, sizeof(ILUInt64));
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static IntPtr VolatileRead(ref IntPtr address);
 */
ILNativeInt _IL_Thread_VolatileRead_Rj(ILExecThread *thread,
									   ILNativeInt *address)
{
	ILNativeInt value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static UIntPtr VolatileRead(ref UIntPtr address);
 */
ILNativeUInt _IL_Thread_VolatileRead_RJ(ILExecThread *thread,
										ILNativeUInt *address)
{
	ILNativeUInt value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static float VolatileRead(ref float address);
 */
ILFloat _IL_Thread_VolatileRead_Rf(ILExecThread *thread, ILFloat *address)
{
	ILFloat value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static double VolatileRead(ref double address);
 */
ILDouble _IL_Thread_VolatileRead_Rd(ILExecThread *thread, ILDouble *address)
{
	ILDouble value;
	ILThreadAtomicStart();
	ILMemCpy(&value, address, sizeof(ILDouble));
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static Object VolatileRead(ref Object address);
 */
ILObject *_IL_Thread_VolatileRead_RObject(ILExecThread *thread,
										  ILObject **address)
{
	ILObject *value;
	ILThreadAtomicStart();
	value = *address;
	ILThreadAtomicEnd();
	return value;
}

/*
 * public static void VolatileWrite(ref sbyte address, sbyte value);
 */
void _IL_Thread_VolatileWrite_Rbb(ILExecThread *thread,
								  ILInt8 *address, ILInt8 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref byte address, byte value);
 */
void _IL_Thread_VolatileWrite_RBB(ILExecThread *thread,
								  ILUInt8 *address, ILUInt8 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref short address, short value);
 */
void _IL_Thread_VolatileWrite_Rss(ILExecThread *thread,
								  ILInt16 *address, ILInt16 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref ushort address, ushort value);
 */
void _IL_Thread_VolatileWrite_RSS(ILExecThread *thread,
								  ILUInt16 *address, ILUInt16 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref int address, int value);
 */
void _IL_Thread_VolatileWrite_Rii(ILExecThread *thread,
								  ILInt32 *address, ILInt32 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref uint address, uint value);
 */
void _IL_Thread_VolatileWrite_RII(ILExecThread *thread,
								  ILUInt32 *address, ILUInt32 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref long address, long value);
 */
void _IL_Thread_VolatileWrite_Rll(ILExecThread *thread,
								  ILInt64 *address, ILInt64 value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILInt64));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref ulong address, ulong value);
 */
void _IL_Thread_VolatileWrite_RLL(ILExecThread *thread,
								  ILUInt64 *address, ILUInt64 value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILUInt64));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref IntPtr address, IntPtr value);
 */
void _IL_Thread_VolatileWrite_Rjj(ILExecThread *thread,
								  ILNativeInt *address, ILNativeInt value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref UIntPtr address, UIntPtr value);
 */
void _IL_Thread_VolatileWrite_RJJ(ILExecThread *thread,
								  ILNativeUInt *address, ILNativeUInt value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref float address, float value);
 */
void _IL_Thread_VolatileWrite_Rff(ILExecThread *thread,
								  ILFloat *address, ILFloat value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref double address, double value);
 */
void _IL_Thread_VolatileWrite_Rdd(ILExecThread *thread,
								  ILDouble *address, ILDouble value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILDouble));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref Object address, Object value);
 */
void _IL_Thread_VolatileWrite_RObjectObject(ILExecThread *thread,
											ILObject **address, ILObject *value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * internal static int InternalGetThreadId();
 */
ILInt32 _IL_Thread_InternalGetThreadId(ILExecThread *_thread)
{
	return (ILInt32)(*((ILThread **)_thread));
}

/*
 * private static void InternalClose(IntPtr privateData);
 */
void _IL_WaitHandle_InternalClose(ILExecThread *_thread,
								  ILNativeInt privateData)
{
	if(privateData)
	{
		ILWaitHandleClose((ILWaitHandle *)privateData);
	}
}

void _ILAbortThread(ILExecThread *thread)
{
	/* Determine if we currently have an abort in progress,
	or if we need to throw a new abort exception */
	if(!ILThreadIsAborting())
	{
		if(ILThreadSelfAborting())
		{
			/* Allocate an instance of "ThreadAbortException" and throw */

			ILObject *exception;

			exception = _ILExecThreadNewThreadAbortException(thread, 0);
			thread->aborting = 1;
			thread->abortRequested = 0;
			thread->abortHandlerEndPC = 0;
			thread->threadAbortException = 0;
			thread->abortHandlerFrame = 0;

			ILExecThreadThrow(thread, exception);
		}
	}
}

/*
 * Handle the result from an "ILWait*" function call.
 */
void _ILHandleWaitResult(ILExecThread *thread, int result)
{
	switch (result)
	{
		case IL_WAIT_INTERRUPTED:
		{
			ILExecThreadThrowSystem
				(
					thread,
					"System.Threading.ThreadInterruptedException",
					(const char *)0
				);
		}
		break;
		case IL_WAIT_ABORTED:
		{
			_ILAbortThread(thread);			
		}
		break;
		case IL_WAIT_FAILED:
		{
			ILExecThreadThrowSystem
				(
					thread,
					"System.Threading.SystemException",
					(const char *)0
				);
		}
	}
}

/*
 * private static bool InternalWaitAll(WaitHandle[] waitHandles,
 *									   int timeout, bool exitContext);
 */
ILBool _IL_WaitHandle_InternalWaitAll(ILExecThread *_thread,
									  System_Array *waitHandles,
									  ILInt32 timeout,
									  ILBool exitContext)
{
	int result;
	ILWaitHandle **handles;
	
	if(waitHandles == 0)
	{
		ILExecThreadThrowArgNull(_thread, "waitHandles");

		return 0;
	}

	handles = (ILWaitHandle **)ArrayToBuffer(waitHandles);

	/* Perform the wait */
	result = ILWaitAll(handles, (ILUInt32)(waitHandles->length), timeout);

	if (result == IL_WAIT_TIMEOUT)
	{
		return exitContext;
	}
	else if (result < 0)
	{
		_ILHandleWaitResult(_thread, result);

		return 0;
	}
	else
	{
		return exitContext;
	}
}

/*
 * private static int InternalWaitAny(WaitHandle[] waitHandles,
 *									  int timeout, bool exitContext);
 */
ILInt32 _IL_WaitHandle_InternalWaitAny(ILExecThread *_thread,
									   System_Array *waitHandles,
									   ILInt32 timeout, ILBool exitContext)
{
	int result;
	ILWaitHandle **handles;
	
	if(waitHandles == 0)
	{
		ILExecThreadThrowArgNull(_thread, "waitHandles");

		return 0;
	}

	handles = (ILWaitHandle **)ArrayToBuffer(waitHandles);

	/* Perform the wait */
	result = ILWaitAny(handles, (ILUInt32)(waitHandles->length), timeout);

	if (result == IL_WAIT_TIMEOUT)
	{
		result = IL_CLR_WAIT_TIMEOUT;
	}
	else if (result < 0)
	{
		_ILHandleWaitResult(_thread, result);

		result = 0;
	}

	return result;
}

/*
 * private static bool InternalWaitOne(IntPtr privateData, int timeout);
 */
ILBool _IL_WaitHandle_InternalWaitOne(ILExecThread *_thread,
									  ILNativeInt privateData,
									  ILInt32 timeout)
{
	if(privateData)
	{
		int result = ILWaitOne((ILWaitHandle *)privateData, timeout);
		_ILHandleWaitResult(_thread, result);
		return (result == 0);
	}
	return 0;
}

/*
 * private static IntPtr InternalCreateMutex(bool initiallyOwned,
 *											 String name,
 *                                           out bool gotOwnership);
 */
ILNativeInt _IL_Mutex_InternalCreateMutex(ILExecThread *_thread,
										  ILBool initiallyOwned,
										  ILString *name,
										  ILBool *gotOwnership)
{
	ILWaitHandle *handle;
	if(!name)
	{
		/* Create an ordinary mutex */
		handle = ILWaitMutexCreate(initiallyOwned);
		*gotOwnership = initiallyOwned;
	}
	else
	{
		/* Create a named mutex */
		char *nameStr = ILStringToUTF8(_thread, name);
		if(nameStr)
		{
			int gotOwn = 0;
			handle = ILWaitMutexNamedCreate(nameStr, initiallyOwned, &gotOwn);
			*gotOwnership = (ILBool)gotOwn;
		}
		else
		{
			handle = 0;
		}
	}
	if(handle)
	{
		return (ILNativeInt)handle;
	}
	else
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return 0;
	}
}

/*
 * private static void InternalReleaseMutex(IntPtr mutex);
 */
void _IL_Mutex_InternalReleaseMutex(ILExecThread *_thread, ILNativeInt mutex)
{
	if(mutex != 0)
	{
		ILWaitMutexRelease((ILWaitHandle *)mutex);
	}
}

/*
 * Internal WaitEvent methods.
 */

/*
 * internal static extern IntPtr InternalCreateEvent(bool manualReset, bool initialState);
 */
ILNativeInt _IL_WaitEvent_InternalCreateEvent(ILExecThread *_thread, ILBool manualReset, ILBool initialState)
{
	ILWaitHandle *event;

	event = ILWaitEventCreate((int)manualReset, (int)initialState);

	if (event == 0)
	{
		ILExecThreadThrowOutOfMemory(_thread);
	}

	return (ILNativeInt)event;
}

/*
 * internal static extern bool InternalSetEvent(IntPtr handle);
 */
ILBool _IL_WaitEvent_InternalResetEvent(ILExecThread *_thread, ILNativeInt event)
{
	return (ILBool)ILWaitEventReset((ILWaitHandle *)event);
}

/*
 * internal static extern bool InternalResetEvent(IntPtr handle);
 */
ILBool _IL_WaitEvent_InternalSetEvent(ILExecThread *_thread, ILNativeInt event)
{
	return (ILBool)ILWaitEventSet((ILWaitHandle *)event);
}

#ifdef	__cplusplus
};
#endif
