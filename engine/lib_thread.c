/*
 * lib_thread.c - Internalcall methods for "System.Threading.*".
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

/*
 * Start and end atomic operations.  Single-threaded systems do nothing.
 */
#define	ILThreadAtomicStart()
#define	ILThreadAtomicEnd()

/*
 * Process a memory barrier.  Single-threaded systems do nothing.
 */
#define	ILThreadMemoryBarrier()

/*
 * public static void Enter(Object obj);
 */
static void System_Threading_Monitor_Enter(ILExecThread *thread,
										   ILObject *obj)
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
static ILBool System_Threading_Monitor_InternalTryEnter(ILExecThread *thread,
										   	    		ILObject *obj,
														ILInt32 timeout)
{
	/* The current implementation is single-threaded,
	   so we assume that we can always enter the monitor */
	return 1;
}

/*
 * public static void Exit(Object obj);
 */
static void System_Threading_Monitor_Exit(ILExecThread *thread,
										  ILObject *obj)
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
static ILBool System_Threading_Monitor_InternalWait(ILExecThread *thread,
										   	    	ILObject *obj,
													ILInt32 timeout)
{
	/* The current implementation is single-threaded,
	   so we can never wait on a monitor, as there are
	   no other threads to wake us up! */
	return 0;
}

/*
 * public static void Pulse(Object obj);
 */
static void System_Threading_Monitor_Pulse(ILExecThread *thread,
										   ILObject *obj)
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
static void System_Threading_Monitor_PulseAll(ILExecThread *thread,
										      ILObject *obj)
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
static ILInt32 System_Threading_Interlocked_CompareExchange_1
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
static ILFloat System_Threading_Interlocked_CompareExchange_2
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
static ILObject *System_Threading_Interlocked_CompareExchange_3
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
static ILInt32 System_Threading_Interlocked_Decrement_1(ILExecThread *thread,
														ILInt32 *location)
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
static ILInt64 System_Threading_Interlocked_Decrement_2(ILExecThread *thread,
														ILInt64 *location)
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
static ILInt32 System_Threading_Interlocked_Exchange_1(ILExecThread *thread,
													   ILInt32 *location,
													   ILInt32 value)
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
static ILFloat System_Threading_Interlocked_Exchange_2(ILExecThread *thread,
													   ILFloat *location,
													   ILFloat value)
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
static ILObject *System_Threading_Interlocked_Exchange_3(ILExecThread *thread,
													     ILObject **location,
													     ILObject *value)
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
static ILInt32 System_Threading_Interlocked_Increment_1(ILExecThread *thread,
														ILInt32 *location)
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
static ILInt64 System_Threading_Interlocked_Increment_2(ILExecThread *thread,
														ILInt64 *location)
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
 * private void InternalFinalize();
 */
static void System_Threading_Thread_InternalFinalize(ILExecThread *thread,
													 System_Thread *_this)
{
	/* TODO */
}

/*
 * public void Abort();
 */
static void System_Threading_Thread_Abort(ILExecThread *thread,
										  System_Thread *_this)
{
	/* TODO */
}

/*
 * public void Join();
 */
static void System_Threading_Thread_Join(ILExecThread *thread,
										 System_Thread *_this)
{
	/* TODO */
}

/*
 * public bool InternalJoin(int timeout);
 */
static ILBool System_Threading_Thread_InternalJoin(ILExecThread *thread,
										 		   System_Thread *_this,
												   ILInt32 timeout)
{
	/* TODO */
	return 0;
}

/*
 * public static void MemoryBarrier();
 */
static void System_Threading_Thread_MemoryBarrier(ILExecThread *thread)
{
	ILThreadMemoryBarrier();
}

/*
 * public static void ResetAbort();
 */
static void System_Threading_Thread_ResetAbort(ILExecThread *thread)
{
	/* TODO */
}

/*
 * public static void InternalSleep(int timeout);
 */
static void System_Threading_Thread_InternalSleep(ILExecThread *thread,
												  ILInt32 timeout)
{
	/* TODO */
}

/*
 * public void Start();
 */
static void System_Threading_Thread_Start(ILExecThread *thread,
										  System_Thread *_this)
{
	/* TODO */
}

/*
 * public static Thread InternalCurrentThread();
 */
static ILObject *System_Threading_Thread_InternalCurrentThread
				(ILExecThread *thread)
{
	/* TODO */
	return 0;
}

/*
 * public void InternalSetBackground(bool value);
 */
static void System_Threading_Thread_InternalSetBackground
				(ILExecThread *thread, System_Thread *_this, ILBool value)
{
	/* TODO */
}

/*
 * public ThreadPriority InternalGetPriority();
 */
static ILInt32 System_Threading_Thread_InternalGetPriority
				(ILExecThread *thread, System_Thread *_this)
{
	/* TODO */
	return 2;	/* Normal */
}

/*
 * public void InternalSetPriority(ThreadPriority priority);
 */
static void System_Threading_Thread_InternalSetPriority
				(ILExecThread *thread, System_Thread *_this, ILInt32 priority)
{
	/* TODO */
}

/*
 * public ThreadState InternalGetState();
 */
static ILInt32 System_Threading_Thread_InternalGetState
				(ILExecThread *thread, System_Thread *_this)
{
	/* TODO */
	return 0;
}

/*
 * public static sbyte VolatileRead(ref sbyte address);
 */
static ILInt8 System_Threading_Thread_VolatileRead_sbyte(ILExecThread *thread,
														 ILInt8 *address)
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
static ILUInt8 System_Threading_Thread_VolatileRead_byte(ILExecThread *thread,
														 ILUInt8 *address)
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
static ILInt16 System_Threading_Thread_VolatileRead_short(ILExecThread *thread,
														  ILInt16 *address)
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
static ILUInt16 System_Threading_Thread_VolatileRead_ushort
					(ILExecThread *thread, ILUInt16 *address)
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
static ILInt32 System_Threading_Thread_VolatileRead_int(ILExecThread *thread,
														ILInt32 *address)
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
static ILUInt32 System_Threading_Thread_VolatileRead_uint(ILExecThread *thread,
														  ILUInt32 *address)
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
static ILInt64 System_Threading_Thread_VolatileRead_long(ILExecThread *thread,
														 ILInt64 *address)
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
static ILUInt64 System_Threading_Thread_VolatileRead_ulong(ILExecThread *thread,
														   ILUInt64 *address)
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
static ILNativeInt System_Threading_Thread_VolatileRead_nint
			(ILExecThread *thread, ILNativeInt *address)
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
static ILNativeUInt System_Threading_Thread_VolatileRead_nuint
			(ILExecThread *thread, ILNativeUInt *address)
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
static ILFloat System_Threading_Thread_VolatileRead_float
			(ILExecThread *thread, ILFloat *address)
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
static ILDouble System_Threading_Thread_VolatileRead_double
			(ILExecThread *thread, ILDouble *address)
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
static ILObject *System_Threading_Thread_VolatileRead_Object
			(ILExecThread *thread, ILObject **address)
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
static void System_Threading_Thread_VolatileWrite_sbyte
				(ILExecThread *thread, ILInt8 *address, ILInt8 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref byte address, byte value);
 */
static void System_Threading_Thread_VolatileWrite_byte
				(ILExecThread *thread, ILUInt8 *address, ILUInt8 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref short address, short value);
 */
static void System_Threading_Thread_VolatileWrite_short
				(ILExecThread *thread, ILInt16 *address, ILInt16 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref ushort address, ushort value);
 */
static void System_Threading_Thread_VolatileWrite_ushort
				(ILExecThread *thread, ILUInt16 *address, ILUInt16 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref int address, int value);
 */
static void System_Threading_Thread_VolatileWrite_int
				(ILExecThread *thread, ILInt32 *address, ILInt32 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref uint address, uint value);
 */
static void System_Threading_Thread_VolatileWrite_uint
				(ILExecThread *thread, ILUInt32 *address, ILUInt32 value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref long address, long value);
 */
static void System_Threading_Thread_VolatileWrite_long
				(ILExecThread *thread, ILInt64 *address, ILInt64 value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILInt64));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref ulong address, ulong value);
 */
static void System_Threading_Thread_VolatileWrite_ulong
				(ILExecThread *thread, ILUInt64 *address, ILUInt64 value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILUInt64));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref IntPtr address, IntPtr value);
 */
static void System_Threading_Thread_VolatileWrite_nint
				(ILExecThread *thread, ILNativeInt *address, ILNativeInt value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref UIntPtr address, UIntPtr value);
 */
static void System_Threading_Thread_VolatileWrite_nuint
				(ILExecThread *thread, ILNativeUInt *address,
				 ILNativeUInt value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref float address, float value);
 */
static void System_Threading_Thread_VolatileWrite_float
				(ILExecThread *thread, ILFloat *address, ILFloat value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref double address, double value);
 */
static void System_Threading_Thread_VolatileWrite_double
				(ILExecThread *thread, ILDouble *address, ILDouble value)
{
	ILThreadAtomicStart();
	ILMemCpy(address, &value, sizeof(ILDouble));
	ILThreadAtomicEnd();
}

/*
 * public static void VolatileWrite(ref Object address, Object value);
 */
static void System_Threading_Thread_VolatileWrite_Object
				(ILExecThread *thread, ILObject **address, ILObject *value)
{
	ILThreadAtomicStart();
	*address = value;
	ILThreadAtomicEnd();
}

/*
 * Method table for the "System.Threading.Monitor" class.
 */
IL_METHOD_BEGIN(_ILSystemThreadingMonitorMethods)
	IL_METHOD("Enter",
			  "(oSystem.Object;)V",
			  System_Threading_Monitor_Enter)
	IL_METHOD("InternalTryEnter",
			  "(oSystem.Object;i)Z",
			  System_Threading_Monitor_InternalTryEnter)
	IL_METHOD("Exit",
			  "(oSystem.Object;)V",
			  System_Threading_Monitor_Exit)
	IL_METHOD("InternalWait",
			  "(oSystem.Object;i)Z",
			  System_Threading_Monitor_InternalWait)
	IL_METHOD("Pulse",
			  "(oSystem.Object;)V",
			  System_Threading_Monitor_Pulse)
	IL_METHOD("PulseAll",
			  "(oSystem.Object;)V",
			  System_Threading_Monitor_PulseAll)
IL_METHOD_END

/*
 * Method table for the "System.Threading.Interlocked" class.
 */
IL_METHOD_BEGIN(_ILSystemThreadingInterlockedMethods)
	IL_METHOD("CompareExchange", "(&iii)i",
			  System_Threading_Interlocked_CompareExchange_1)
	IL_METHOD("CompareExchange", "(&fff)f",
			  System_Threading_Interlocked_CompareExchange_2)
	IL_METHOD("CompareExchange",
			  "(&oSystem.Object;oSystem.Object;oSystem.Object;)oSystem.Object;",
			  System_Threading_Interlocked_CompareExchange_3)
	IL_METHOD("Decrement", "(&i)i",
			  System_Threading_Interlocked_Decrement_1)
	IL_METHOD("Decrement", "(&l)l",
			  System_Threading_Interlocked_Decrement_2)
	IL_METHOD("Exchange", "(&ii)i",
			  System_Threading_Interlocked_Exchange_1)
	IL_METHOD("Exchange", "(&ff)f",
			  System_Threading_Interlocked_Exchange_2)
	IL_METHOD("Exchange",
			  "(&oSystem.Object;oSystem.Object;)oSystem.Object;",
			  System_Threading_Interlocked_Exchange_3)
	IL_METHOD("Increment", "(&i)i",
			  System_Threading_Interlocked_Increment_1)
	IL_METHOD("Increment", "(&l)l",
			  System_Threading_Interlocked_Increment_2)
IL_METHOD_END

/*
 * Method table for the "System.Threading.Thread" class.
 */
IL_METHOD_BEGIN(_ILSystemThreadingThreadMethods)
	IL_METHOD("InternalFinalize", "(T)V",
			  System_Threading_Thread_InternalFinalize)
	IL_METHOD("Abort", "(T)V",
			  System_Threading_Thread_Abort)
	IL_METHOD("Join", "(T)V",
			  System_Threading_Thread_Join)
	IL_METHOD("InternalJoin", "(Ti)Z",
			  System_Threading_Thread_InternalJoin)
	IL_METHOD("MemoryBarrier", "()V",
			  System_Threading_Thread_MemoryBarrier)
	IL_METHOD("ResetAbort", "()V",
			  System_Threading_Thread_ResetAbort)
	IL_METHOD("InternalSleep", "(i)V",
			  System_Threading_Thread_InternalSleep)
	IL_METHOD("Start", "(T)V",
			  System_Threading_Thread_Start)
	IL_METHOD("InternalCurrentThread", "()oSystem.Threading.Thread;",
			  System_Threading_Thread_InternalCurrentThread)
	IL_METHOD("InternalSetBackground", "(TZ)V",
			  System_Threading_Thread_InternalSetBackground)
	IL_METHOD("InternalGetPriority", "(T)vSystem.Threading.ThreadPriority;",
			  System_Threading_Thread_InternalGetPriority)
	IL_METHOD("InternalSetPriority", "(TvSystem.Threading.ThreadPriority;)V",
			  System_Threading_Thread_InternalSetPriority)
	IL_METHOD("InternalGetState", "(T)vSystem.Threading.ThreadState;",
			  System_Threading_Thread_InternalGetState)
	IL_METHOD("VolatileRead", "(&b)b",
			  System_Threading_Thread_VolatileRead_sbyte)
	IL_METHOD("VolatileRead", "(&B)B",
			  System_Threading_Thread_VolatileRead_byte)
	IL_METHOD("VolatileRead", "(&s)s",
			  System_Threading_Thread_VolatileRead_short)
	IL_METHOD("VolatileRead", "(&S)S",
			  System_Threading_Thread_VolatileRead_ushort)
	IL_METHOD("VolatileRead", "(&i)i",
			  System_Threading_Thread_VolatileRead_int)
	IL_METHOD("VolatileRead", "(&I)I",
			  System_Threading_Thread_VolatileRead_uint)
	IL_METHOD("VolatileRead", "(&l)l",
			  System_Threading_Thread_VolatileRead_long)
	IL_METHOD("VolatileRead", "(&L)L",
			  System_Threading_Thread_VolatileRead_ulong)
	IL_METHOD("VolatileRead", "(&j)j",
			  System_Threading_Thread_VolatileRead_nint)
	IL_METHOD("VolatileRead", "(&J)J",
			  System_Threading_Thread_VolatileRead_nuint)
	IL_METHOD("VolatileRead", "(&f)f",
			  System_Threading_Thread_VolatileRead_float)
	IL_METHOD("VolatileRead", "(&d)d",
			  System_Threading_Thread_VolatileRead_double)
	IL_METHOD("VolatileRead", "(&oSystem.Object;)oSystem.Object;",
			  System_Threading_Thread_VolatileRead_Object)
	IL_METHOD("VolatileWrite", "(&bb)V",
			  System_Threading_Thread_VolatileWrite_sbyte)
	IL_METHOD("VolatileWrite", "(&BB)V",
			  System_Threading_Thread_VolatileWrite_byte)
	IL_METHOD("VolatileWrite", "(&ss)V",
			  System_Threading_Thread_VolatileWrite_short)
	IL_METHOD("VolatileWrite", "(&SS)V",
			  System_Threading_Thread_VolatileWrite_ushort)
	IL_METHOD("VolatileWrite", "(&ii)V",
			  System_Threading_Thread_VolatileWrite_int)
	IL_METHOD("VolatileWrite", "(&II)V",
			  System_Threading_Thread_VolatileWrite_uint)
	IL_METHOD("VolatileWrite", "(&ll)V",
			  System_Threading_Thread_VolatileWrite_long)
	IL_METHOD("VolatileWrite", "(&LL)V",
			  System_Threading_Thread_VolatileWrite_ulong)
	IL_METHOD("VolatileWrite", "(&jj)V",
			  System_Threading_Thread_VolatileWrite_nint)
	IL_METHOD("VolatileWrite", "(&JJ)V",
			  System_Threading_Thread_VolatileWrite_nuint)
	IL_METHOD("VolatileWrite", "(&ff)V",
			  System_Threading_Thread_VolatileWrite_float)
	IL_METHOD("VolatileWrite", "(&dd)V",
			  System_Threading_Thread_VolatileWrite_double)
	IL_METHOD("VolatileWrite", "(&oSystem.Object;oSystem.Object;)V",
			  System_Threading_Thread_VolatileWrite_Object)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
