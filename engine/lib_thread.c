/*
 * lib_thread.c - Internalcall methods for "System.Threading.*".
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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

#ifdef	__cplusplus
extern	"C" {
#endif

#define WIN32_WAIT_TIMEOUT (258L)

/*
 * public static void Enter(Object obj);
 */
void _IL_Monitor_Enter(ILExecThread *thread, ILObject *obj)
{
	if(obj)
	{
		/* The current implementation is single-threaded,
		   so we have nothing to do here */
	}
	else
	{
		ILExecThreadThrowArgNull(thread, "obj");
	}
}

/*
 * public static bool InternalTryEnter(Object obj, int timeout);
 */
ILBool _IL_Monitor_InternalTryEnter(ILExecThread *thread,
					   	    		ILObject *obj, ILInt32 timeout)
{
	/* The current implementation is single-threaded,
	   so we assume that we can always enter the monitor */
	return 1;
}

/*
 * public static void Exit(Object obj);
 */
void _IL_Monitor_Exit(ILExecThread *thread, ILObject *obj)
{
	if(obj)
	{
		/* The current implementation is single-threaded,
		   so we have nothing to do here */
	}
	else
	{
		ILExecThreadThrowArgNull(thread, "obj");
	}
}

/*
 * public static bool InternalWait(Object obj, int timeout);
 */
ILBool _IL_Monitor_InternalWait(ILExecThread *thread,
							    ILObject *obj, ILInt32 timeout)
{
	/* The current implementation is single-threaded,
	   so we can never wait on a monitor, as there are
	   no other threads to wake us up! */
	return 0;
}

/*
 * public static void Pulse(Object obj);
 */
void _IL_Monitor_Pulse(ILExecThread *thread, ILObject *obj)
{
	if(obj)
	{
		/* The current implementation is single-threaded,
		   so we have nothing to do here */
	}
	else
	{
		ILExecThreadThrowArgNull(thread, "obj");
	}
}

/*
 * public static void PulseAll(Object obj);
 */
void _IL_Monitor_PulseAll(ILExecThread *thread, ILObject *obj)
{
	if(obj)
	{
		/* The current implementation is single-threaded,
		   so we have nothing to do here */
	}
	else
	{
		ILExecThreadThrowArgNull(thread, "obj");
	}
}

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
	ILInt32 orig;
	ILThreadAtomicStart();
	if((orig = *location1) == comparand)
	{
		*location1 = value;
	}
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static float CompareExchange(ref float location1, float value,
 *									   float comparand);
 */
ILFloat _IL_Interlocked_CompareExchange_Rfff
						(ILExecThread *thread,
						 ILFloat *location1,
						 ILFloat value,
						 ILFloat comparand)
{
	ILFloat orig;
	ILThreadAtomicStart();
	if((orig = *location1) == comparand)
	{
		*location1 = value;
	}
	ILThreadAtomicEnd();
	return orig;
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
	ILObject *orig;
	ILThreadAtomicStart();
	if((orig = *location1) == comparand)
	{
		*location1 = value;
	}
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static int Decrement(ref int location);
 */
ILInt32 _IL_Interlocked_Decrement_Ri(ILExecThread *thread, ILInt32 *location)
{
	ILInt32 value;
	ILThreadAtomicStart();
	value = --(*location);
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

/*
 * public static int Exchange(ref int location, int value);
 */
ILInt32 _IL_Interlocked_Exchange_Rii(ILExecThread *thread,
								     ILInt32 *location, ILInt32 value)
{
	ILInt32 orig;
	ILThreadAtomicStart();
	orig = *location;
	*location = value;
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static float Exchange(ref float location, float value);
 */
ILFloat _IL_Interlocked_Exchange_Rff(ILExecThread *thread,
								     ILFloat *location, ILFloat value)
{
	ILFloat orig;
	ILThreadAtomicStart();
	orig = *location;
	*location = value;
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static Object Exchange(ref Object location, Object value);
 */
ILObject *_IL_Interlocked_Exchange_RObjectObject
				(ILExecThread *thread, ILObject **location, ILObject *value)
{
	ILObject *orig;
	ILThreadAtomicStart();
	orig = *location;
	*location = value;
	ILThreadAtomicEnd();
	return orig;
}

/*
 * public static int Increment(ref int location);
 */
ILInt32 _IL_Interlocked_Increment_Ri(ILExecThread *thread, ILInt32 *location)
{
	ILInt32 value;
	ILThreadAtomicStart();
	value = ++(*location);
	ILThreadAtomicEnd();
	return value;
}

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
 * private void FinalizeThread();
 */
void _IL_Thread_FinalizeThread(ILExecThread *thread, ILObject *_this)
{
	/* TODO */
}

/*
 * public void Abort();
 */
void _IL_Thread_Abort(ILExecThread *thread, ILObject *_this)
{
	/* TODO */
}

/*
 * private bool InternalJoin(int timeout);
 */
ILBool _IL_Thread_InternalJoin(ILExecThread *thread, ILObject *_this,
							   ILInt32 timeout)
{
	/* TODO */
	return 0;
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
	/* TODO */
}

/*
 * public static void InternalSleep(int timeout);
 */
void _IL_Thread_InternalSleep(ILExecThread *thread, ILInt32 timeout)
{
	ILThreadSleep((ILUInt32)timeout);
}

/*
 * public void Start();
 */
void _IL_Thread_Start(ILExecThread *thread, ILObject *_this)
{
	/* TODO */
}

/*
 * public static Thread InternalCurrentThread();
 */
ILObject *_IL_Thread_InternalCurrentThread(ILExecThread *thread)
{
	/* TODO */
	return 0;
}

/*
 * public void InternalSetBackground(bool value);
 */
void _IL_Thread_InternalSetBackground(ILExecThread *thread,
									  ILObject *_this, ILBool value)
{
	/* TODO */
}

/*
 * public ThreadPriority InternalGetPriority();
 */
ILInt32 _IL_Thread_InternalGetPriority(ILExecThread *thread, ILObject *_this)
{
	/* TODO */
	return 2;	/* Normal */
}

/*
 * public void InternalSetPriority(ThreadPriority priority);
 */
void _IL_Thread_InternalSetPriority(ILExecThread *thread, ILObject *_this,
									ILInt32 priority)
{
	/* TODO */
}

/*
 * public ThreadState InternalGetState();
 */
ILInt32 _IL_Thread_InternalGetState(ILExecThread *thread, ILObject *_this)
{
	/* TODO */
	return 0;
}

/*
 * internal static bool CanStartThreads();
 */
ILBool _IL_Thread_CanStartThreads(ILExecThread *_thread)
{
	/* TODO */
	return 0;
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
	/* TODO */
	return 0;
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

/*
 * Handle the result from an "ILWait*" function call.
 */
static void HandleWaitResult(ILExecThread *thread, int result)
{
	if(result == IL_WAIT_INTERRUPTED)
	{
		ILExecThreadThrowSystem
			(thread, "System.Threading.ThreadInterruptedException",
			 (const char *)0);
	}
	else if(result == IL_WAIT_ABORTED)
	{
		/* Determine if we currently have an abort in progress,
		   or if we need to throw a new abort exception */
		if(ILThreadIsAborting())
		{
			if(ILThreadAbort(ILThreadSelf()))
			{
				/* Allocate an instance of "ThreadAbortException" and throw */
				/* TODO */
			}
		}
	}
}

/*
 * Maximum number of wait handles that can be processed at once.
 */
#define	IL_MAX_WAIT_HANDLES		64

/*
 * Convert a C# array of wait handles into a C array.
 * Returns zero if something is wrong.
 */
static int ConvertWaitHandleArray(ILExecThread *_thread,
								  System_Array *waitHandles,
								  ILWaitHandle **handles)
{
	ILInt32 index;

	/* Validate the length of the incoming array */
	if(waitHandles->length > IL_MAX_WAIT_HANDLES)
	{
		ILExecThreadThrowSystem
			(_thread, "System.NotSupportedException",
			 (const char *)0);
		return 0;
	}

	/* Convert the WaitHandle objects into system wait handles */
	for(index = 0; index < waitHandles->length; ++index)
	{
		handles[index] = *((ILWaitHandle **)
			(((ILObject **)(ArrayToBuffer(waitHandles)))[index]));
		if(handles[index] == 0)
		{
			ILExecThreadThrowArgNull(_thread, "waitHandles");
			return 0;
		}
	}

	/* Ready to go */
	return 1;
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
	ILWaitHandle *handles[IL_MAX_WAIT_HANDLES];
	int result;

	/* Return immediately if there are no handles */
	if(!(waitHandles->length))
	{
		return 1;
	}

	/* Convert the WaitHandle objects into system wait handles */
	if(!ConvertWaitHandleArray(_thread, waitHandles, handles))
	{
		return 0;
	}

	/* Perform the wait */
	result = ILWaitAll(handles, (ILUInt32)(waitHandles->length), timeout);
	HandleWaitResult(_thread, result);
	return (result == 0);
}

/*
 * private static int InternalWaitAny(WaitHandle[] waitHandles,
 *									  int timeout, bool exitContext);
 */
ILInt32 _IL_WaitHandle_InternalWaitAny(ILExecThread *_thread,
									   System_Array *waitHandles,
									   ILInt32 timeout, ILBool exitContext)
{
	ILWaitHandle *handles[IL_MAX_WAIT_HANDLES];
	int result;

	/* Return immediately if there are no handles */
	if(!(waitHandles->length))
	{
		return 1;
	}

	/* Convert the WaitHandle objects into system wait handles */
	if(!ConvertWaitHandleArray(_thread, waitHandles, handles))
	{
		return 0;
	}

	/* Perform the wait */
	result = ILWaitAny(handles, (ILUInt32)(waitHandles->length), timeout);
	HandleWaitResult(_thread, result);
	/*return (result == 0);*/
	
	/* Now returns index of the handle that was set and proper timeout value */

	if (result == IL_WAIT_TIMEOUT)
	{
		result = WIN32_WAIT_TIMEOUT;
	}
	else if (result < 0)
	{
		/* TODO: FIXME: Docs don't state how to report an error! */
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
		HandleWaitResult(_thread, result);
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
