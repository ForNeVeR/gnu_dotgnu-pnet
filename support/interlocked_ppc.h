/*
 * interlocked_ppc.h - Implementation of interlocked functions for
 * powerpc processors.
 *
 * Copyright (C) 2009  Southern Storm Software, Pty Ltd.
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

#if defined(__powerpc__) || defined(powerpc) || \
	defined(__powerpc) || defined(PPC) || defined(__ppc__)

#if defined(__GNUC__)

/*
 * References:
 * http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2009.02.22a.html
 */

/*
 * Use the leight weight sync for release semantics if available.
 */
#ifdef __NO_LWSYNC__
#define _IL_INTERLOCKED_PPC_LWSYNC	"\tsync\n"
#else
#define _IL_INTERLOCKED_PPC_LWSYNC	"\tlwsync\n"
#endif

/*
 * Flush cache and set a memory barrier.
 */
static IL_INLINE void ILInterlockedMemoryBarrier()
{
	__asm__ __volatile__
	(
		"sync"
		:
		:
		: "memory"
	);
}
#define IL_HAVE_INTERLOCKED_MEMORYBARRIER 1

/*
 * Load a 32 bit value from a location with acquire semantics.
 */
static IL_INLINE ILInt32 ILInterlockedLoad_Acquire(const volatile ILInt32 *dest)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
	    "\tlwz%U1%X1 %0, %1\n"
	    "\tcmpw		%0, %0\n"
	    "\tbne-		1f\n"
    	"1:"
		"\tisync\n"
	    : "=r" (retval)
	    : "m" (*dest)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_LOAD_ACQUIRE

/*
 * Load a pointer value from a location with acquire semantics.
 */
static IL_INLINE void *ILInterlockedLoadPointer_Acquire(void * const volatile *dest)
{
	void *retval;

	__asm__ __volatile__ 
	(
	    "\tlwz%U1%X1 %0, %1\n"
	    "\tcmpw		%0, %0\n"
	    "\tbne-		1f\n"
    	"1:"
		"\tisync\n"
	    : "=r" (retval)
	    : "m" (*dest)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_LOADPOINTER_ACQUIRE

/*
 * Store a 32 bit value to a location with release semantics.
 */
static IL_INLINE void ILInterlockedStore_Release(volatile ILInt32 *dest,
												 ILInt32 value)
{
	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"\tstw	%1, %0\n"
		: "=m" (*dest)
		: "r" (value)
		: "memory"
	);
}
#define IL_HAVE_INTERLOCKED_STORE_RELEASE 1

/*
 * Store a pointer value to a location with release semantics.
 * TODO: Add support for ppc64
 */
static IL_INLINE void ILInterlockedStorePointer_Release(void * volatile *dest,
														void *value)
{
	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"\tstw	%1, %0\n"
		: "=m" (*dest)
		: "r" (value)
		: "memory"
	);
}
#define IL_HAVE_INTERLOCKED_STOREPOINTER_RELEASE 1

/*
 * Exchange two 32 bit integers.
 */
static IL_INLINE ILInt32 ILInterlockedExchange(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGE 1

static IL_INLINE ILInt32 ILInterlockedExchange_Acquire(volatile ILInt32 *dest,
													   ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGE_ACQUIRE 1

static IL_INLINE ILInt32 ILInterlockedExchange_Release(volatile ILInt32 *dest,
													   ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGE_RELEASE 1

static IL_INLINE ILInt32 ILInterlockedExchange_Full(volatile ILInt32 *dest,
													ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGE_FULL 1

/*
 * Exchange pointers.
 *
 * FIXME: Add support for the 64bit powerpc
 */
static IL_INLINE void *ILInterlockedExchangePointers(void * volatile *dest,
													 void *value)
{
	void *retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS 1

static IL_INLINE void *ILInterlockedExchangePointers_Acquire(void * volatile *dest,
															 void *value)
{
	void *retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_ACQUIRE 1

static IL_INLINE void *ILInterlockedExchangePointers_Release(void * volatile *dest,
															 void *value)
{
	void *retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_RELEASE 1

static IL_INLINE void *ILInterlockedExchangePointers_Full(void * volatile *dest,
														  void *value)
{
	void *retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL 1

/*
 * Compare and exchange two 32bit integers.
 */
static IL_INLINE ILInt32 ILInterlockedCompareAndExchange(volatile ILInt32 *dest,
														 ILInt32 value,
														 ILInt32 comparand)
{
	ILInt32 retval;

	__asm__ __volatile__
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tcmpw		%0, %4\n"
		"\tbne		2f\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		"2:"
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value), "r" (comparand)
		: "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE 1

static IL_INLINE ILInt32 ILInterlockedCompareAndExchange_Acquire(volatile ILInt32 *dest,
																 ILInt32 value,
																 ILInt32 comparand)
{
	ILInt32 retval;

	__asm__ __volatile__
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tcmpw		%0, %4\n"
		"\tbne		2f\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		"2:"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value), "r" (comparand)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE 1

static IL_INLINE ILInt32 ILInterlockedCompareAndExchange_Release(volatile ILInt32 *dest,
																 ILInt32 value,
																 ILInt32 comparand)
{
	ILInt32 retval;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tcmpw		%0, %4\n"
		"\tbne		2f\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		"2:"
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value), "r" (comparand)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE 1

static IL_INLINE ILInt32 ILInterlockedCompareAndExchange_Full(volatile ILInt32 *dest,
															  ILInt32 value,
															  ILInt32 comparand)
{
	ILInt32 retval;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tcmpw		%0, %4\n"
		"\tbne		2f\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		"2:"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value), "r" (comparand)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL 1

/*
 * Compare and exchange two pointers.
 * TODO: Add ppc64 support
 */
static IL_INLINE void *ILInterlockedCompareAndExchangePointers(void * volatile *dest,
															   void *value,
															   void *comparand)
{
	void *retval;

	__asm__ __volatile__
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tcmpw		%0, %4\n"
		"\tbne		2f\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		"2:"
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value), "r" (comparand)
		: "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS 1

static IL_INLINE void *ILInterlockedCompareAndExchangePointers_Acquire(void * volatile *dest,
																	   void *value,
																	   void *comparand)
{
	void *retval;

	__asm__ __volatile__
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tcmpw		%0, %4\n"
		"\tbne		2f\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		"2:"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value), "r" (comparand)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_ACQUIRE 1

static IL_INLINE void *ILInterlockedCompareAndExchangePointers_Release(void * volatile *dest,
																	   void *value,
																	   void *comparand)
{
	void *retval;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tcmpw		%0, %4\n"
		"\tbne		2f\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		"2:"
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value), "r" (comparand)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_RELEASE 1

static IL_INLINE void *ILInterlockedCompareAndExchangePointers_Full(void * volatile *dest,
																	void *value,
																	void *comparand)
{
	void *retval;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tcmpw		%0, %4\n"
		"\tbne		2f\n"
		"\tstwcx.	%3, 0, %2\n"
		"\tbne-		1b\n"
		"2:"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=&r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value), "r" (comparand)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL 1

/*
 * Add two 32 bit integer values.
 */
static IL_INLINE ILInt32 ILInterlockedAdd(volatile ILInt32 *dest,
										  ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tadd		%0, %0, %3\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_ADD 1

static IL_INLINE ILInt32 ILInterlockedAdd_Acquire(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tadd		%0, %0, %3\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_ADD_ACQUIRE 1

static IL_INLINE ILInt32 ILInterlockedAdd_Release(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tadd		%0, %0, %3\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_ADD_RELEASE 1

static IL_INLINE ILInt32 ILInterlockedAdd_Full(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tadd		%0, %0, %3\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_ADD_FULL 1

/*
 * Subtract two 32 bit integer values.
 */
static IL_INLINE ILInt32 ILInterlockedSub(volatile ILInt32 *dest,
										  ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tsubf		%0, %3, %0\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_SUB 1

static IL_INLINE ILInt32 ILInterlockedSub_Acquire(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tsubf		%0, %3, %0\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_SUB_ACQUIRE 1

static IL_INLINE ILInt32 ILInterlockedSub_Release(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tsubf		%0, %3, %0\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_SUB_RELEASE 1

static IL_INLINE ILInt32 ILInterlockedSub_Full(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tsubf		%0, %3, %0\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_SUB_FULL 1

/*
 * 32bit bitwise AND
 */
static IL_INLINE void ILInterlockedAnd(volatile ILUInt32 *dest, ILUInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tand		%0, %0, %3\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "cc"
	);
}
#define IL_HAVE_INTERLOCKED_AND 1

static IL_INLINE void ILInterlockedAnd_Full(volatile ILUInt32 *dest, ILUInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tand		%0, %0, %3\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);
}
#define IL_HAVE_INTERLOCKED_AND_FULL 1

/*
 * 32bit bitwise OR
 */
static IL_INLINE void ILInterlockedOr(volatile ILUInt32 *dest, ILUInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tor		%0, %0, %3\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "cc"
	);
}
#define IL_HAVE_INTERLOCKED_OR 1

static IL_INLINE void ILInterlockedOr_Full(volatile ILUInt32 *dest, ILUInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_PPC_LWSYNC
		"1:"
		"\tlwarx	%0, 0, %2\n"
		"\tor		%0, %0, %3\n"
		"\tstwcx.	%0, 0, %2\n"
		"\tbne-		1b\n"
		_IL_INTERLOCKED_PPC_LWSYNC
		: "=r" (retval), "=m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);
}
#define IL_HAVE_INTERLOCKED_OR_FULL 1

#endif /* defined(__GNUC__) */

#endif /* defined(__powerpc__) || defined(powerpc) || defined(__powerpc) || defined(PPC) || defined(__ppc__) */
