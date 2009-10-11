/*
 * interlocked_arm.h - Implementation of interlocked functions for
 * Arm processors.
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

#if defined(__arm) || defined(__arm__)

#if defined(__GNUC__)

/*
 * Define _IL_INTERLOCKED_ARM_MP to enable multiprocessor by default
 */
#define _IL_INTERLOCKED_ARM_MP 1

#if defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_7M__) || \
	defined(_IL_INTERLOCKED_ARM_MP)
/*
 * These are the multi core variants which will need an explicit memory barrier
 */
#define _IL_INTERLOCKED_ARM_MEMORYBARRIER	"\tmcr p15, 0, r0, c7, c10, 5\n"
#else /* !defined(__ARM_ARCH_6M__) && !defined(__ARM_ARCH_7M__) */
#define _IL_INTERLOCKED_ARM_MEMORYBARRIER	""
#endif /* !defined(__ARM_ARCH_6M__) && !defined(__ARM_ARCH_7M__) */

/*
 * Flush cache and set a memory barrier.
 */
static IL_INLINE void ILInterlockedMemoryBarrier()
{
	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		:
		:
		: "memory"
	);
}
#define IL_HAVE_INTERLOCKED_MEMORYBARRIER 1

/*
 * Load a 32 bit value from a location.
 */
static IL_INLINE ILInt32 ILInterlockedLoad(const volatile ILInt32 *dest)
{
	ILInt32 retval;

	__asm__ __volatile__
	(
		"\tldr	%0, %1\n"
		: "=r" (retval)
		: "m" (*dest)
	);
	return retval;
}
#define IL_HAVE_INTERLOCKED_LOAD 1

static IL_INLINE ILInt32 ILInterlockedLoad_Acquire(const volatile ILInt32 *dest)
{
	ILInt32 retval;

	__asm__ __volatile__
	(
		"\tldr	%0, %1\n"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=r" (retval)
		: "m" (*dest)
		: "memory"
	);
	return retval;
}
#define IL_HAVE_INTERLOCKED_LOAD_ACQUIRE 1

/*
 * Load a pointer value from a location.
 */
static IL_INLINE void *ILInterlockedLoadPointer(void * const volatile *dest)
{
	void *retval;

	__asm__ __volatile__
	(
		"\tldr	%0, %1\n"
		: "=r" (retval)
		: "m" (*dest)
	);
	return retval;
}
#define IL_HAVE_INTERLOCKED_LOADPOINTER 1

static IL_INLINE void *ILInterlockedLoadPointer_Acquire(void * const volatile *dest)
{
	void *retval;

	__asm__ __volatile__
	(
		"\tldr	%0, %1\n"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=r" (retval)
		: "m" (*dest)
		: "memory"
	);
	return retval;
}
#define IL_HAVE_INTERLOCKED_LOADPOINTER_ACQUIRE 1

/*
 * Store a 32 bit value to a location.
 */
static IL_INLINE void ILInterlockedStore(volatile ILInt32 *dest,
										 ILInt32 value)
{
	__asm__ __volatile__
	(
		"\tstr	%1, %0\n"
		: "=m" (*dest)
		: "r" (value)
	);
}
#define IL_HAVE_INTERLOCKED_STORE 1

static IL_INLINE void ILInterlockedStore_Release(volatile ILInt32 *dest,
												 ILInt32 value)
{
	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"\tstr	%1, %0\n"
		: "=m" (*dest)
		: "r" (value)
		: "memory"
	);
}
#define IL_HAVE_INTERLOCKED_STORE_RELEASE 1

/*
 * Store a pointer value to a location.
 */
static IL_INLINE void ILInterlockedStorePointer(void * volatile *dest,
												void *value)
{
	__asm__ __volatile__
	(
		"\tstr	%1, %0\n"
		: "=m" (*dest)
		: "r" (value)
	);
}
#define IL_HAVE_INTERLOCKED_STOREPOINTER 1

static IL_INLINE void ILInterlockedStorePointer_Release(void * volatile *dest,
														void *value)
{
	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"\tstr	%1, %0\n"
		: "=m" (*dest)
		: "r" (value)
		: "memory"
	);
}
#define IL_HAVE_INTERLOCKED_STOREPOINTER_RELEASE 1

#if defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || \
	defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6ZK__) || \
	defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
	defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7R__)

/*
 * The versions for the interlocked operations available for all arm cores
 * version 6 and later (except Armv6_M).
 */

/*
 * Exchange two 32 bit integers.
 */
static IL_INLINE ILInt32 ILInterlockedExchange(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex  %0, [%3];"
		"strex  %1, %4, [%3];"
		"teq            %1, #0;"
		"bne            1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex  %0, [%3];"
		"strex  %1, %4, [%3];"
		"teq            %1, #0;"
		"bne            1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex  %0, [%3];"
		"strex  %1, %4, [%3];"
		"teq            %1, #0;"
		"bne            1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex  %0, [%3];"
		"strex  %1, %4, [%3];"
		"teq            %1, #0;"
		"bne            1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value)
		: "memory", "cc"
	);
	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGE_FULL 1

/*
 * Exchange pointers.
 */
static IL_INLINE void *ILInterlockedExchangePointers(void * volatile *dest,
													 void *value)
{
	void *retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex  %0, [%3];"
		"strex  %1, %4, [%3];"
		"teq        %1, #0;"
		"bne        1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex  %0, [%3];"
		"strex  %1, %4, [%3];"
		"teq        %1, #0;"
		"bne        1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex  %0, [%3];"
		"strex  %1, %4, [%3];"
		"teq        %1, #0;"
		"bne        1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex  %0, [%3];"
		"strex  %1, %4, [%3];"
		"teq        %1, #0;"
		"bne        1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex			%0, [%3];"
		"teq			%0, %5;"
		"strexeq	%1, %4, [%3];"
		"teq			%1, #0;"
		"bne			1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value), "Jr" (comparand)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex			%0, [%3];"
		"teq			%0, %5;"
		"strexeq	%1, %4, [%3];"
		"teq			%1, #0;"
		"bne			1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value), "Jr" (comparand)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex			%0, [%3];"
		"teq			%0, %5;"
		"strexeq	%1, %4, [%3];"
		"teq			%1, #0;"
		"bne			1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value), "Jr" (comparand)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex			%0, [%3];"
		"teq			%0, %5;"
		"strexeq	%1, %4, [%3];"
		"teq			%1, #0;"
		"bne			1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value), "Jr" (comparand)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL 1

/*
 * Compare and exchange two pointers.
 */
static IL_INLINE void *ILInterlockedCompareAndExchangePointers(void * volatile *dest,
															   void *value,
															   void *comparand)
{
	void *retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex			%0, [%3];"
		"teq			%0, %5;"
		"strexeq	%1, %4, [%3];"
		"teq			%1, #0;"
		"bne			1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value), "Jr" (comparand)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex			%0, [%3];"
		"teq			%0, %5;"
		"strexeq	%1, %4, [%3];"
		"teq			%1, #0;"
		"bne			1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value), "Jr" (comparand)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex			%0, [%3];"
		"teq			%0, %5;"
		"strexeq	%1, %4, [%3];"
		"teq			%1, #0;"
		"bne			1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value), "Jr" (comparand)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex			%0, [%3];"
		"teq			%0, %5;"
		"strexeq	%1, %4, [%3];"
		"teq			%1, #0;"
		"bne			1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "r" (value), "Jr" (comparand)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex		%0, [%3];"
		"add		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_ADD 1

static IL_INLINE ILInt32 ILInterlockedAdd_Acquire(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex		%0, [%3];"
		"add		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_ADD_ACQUIRE 1

static IL_INLINE ILInt32 ILInterlockedAdd_Release(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex		%0, [%3];"
		"add		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_ADD_RELEASE 1

static IL_INLINE ILInt32 ILInterlockedAdd_Full(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex		%0, [%3];"
		"add		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
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
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex		%0, [%3];"
		"sub		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_SUB 1

static IL_INLINE ILInt32 ILInterlockedSub_Acquire(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		"1:"
		"ldrex		%0, [%3];"
		"sub		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_SUB_ACQUIRE 1

static IL_INLINE ILInt32 ILInterlockedSub_Release(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex		%0, [%3];"
		"sub		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_SUB_RELEASE 1

static IL_INLINE ILInt32 ILInterlockedSub_Full(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex		%0, [%3];"
		"sub		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
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
	ILInt32 state;

	__asm__ __volatile__ 
	(
		"1:"
		"ldrex		%0, [%3];"
		"and		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "cc"
	);
}
#define IL_HAVE_INTERLOCKED_AND 1

static IL_INLINE void ILInterlockedAnd_Acquire(volatile ILUInt32 *dest,
											   ILUInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__ 
	(
		"1:"
		"ldrex		%0, [%3];"
		"and		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);
}
#define IL_HAVE_INTERLOCKED_AND_ACQUIRE 1

static IL_INLINE void ILInterlockedAnd_Release(volatile ILUInt32 *dest,
											   ILUInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex		%0, [%3];"
		"and		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);
}
#define IL_HAVE_INTERLOCKED_AND_RELEASE 1

static IL_INLINE void ILInterlockedAnd_Full(volatile ILUInt32 *dest,
											ILUInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex		%0, [%3];"
		"and		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
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
	ILInt32 state;

	__asm__ __volatile__ 
	(
		"1:"
		"ldrex		%0, [%3];"
		"orr		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "cc"
	);
}
#define IL_HAVE_INTERLOCKED_OR 1

static IL_INLINE void ILInterlockedOr_Acquire(volatile ILUInt32 *dest, ILUInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__ 
	(
		"1:"
		"ldrex		%0, [%3];"
		"orr		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);
}
#define IL_HAVE_INTERLOCKED_OR_ACQUIRE 1

static IL_INLINE void ILInterlockedOr_Release(volatile ILUInt32 *dest, ILUInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex		%0, [%3];"
		"orr		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);
}
#define IL_HAVE_INTERLOCKED_OR_RELEASE 1

static IL_INLINE void ILInterlockedOr_Full(volatile ILUInt32 *dest, ILUInt32 value)
{
	ILInt32 retval;
	ILInt32 state;

	__asm__ __volatile__ 
	(
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		"1:"
		"ldrex		%0, [%3];"
		"orr		%0,	%0, %4;"
		"strex	%1, %0, [%3];"
		"teq		%1, #0;"
		"bne		1b;"
		_IL_INTERLOCKED_ARM_MEMORYBARRIER
		: "=&r" (retval), "=&r" (state), "+m" (*dest)
		: "r" (dest), "Jr" (value)
		: "memory", "cc"
	);
}
#define IL_HAVE_INTERLOCKED_OR_FULL 1

#else /* __ARM_ARCH__ <= 5 || __ARM_ARCH_6M__ */

/*
 * Exchange two 32 bit integers.
 */
static IL_INLINE ILInt32 ILInterlockedExchange_Full(volatile ILInt32 *dest,
													ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__
	(
		"swp %0, %2, [%3]"
		: "=&r" (retval), "=&r" (dest)
		: "r" (value), "1" (dest)
		: "memory"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGE_FULL 1

/*
 * Exchange pointers.
 */
static IL_INLINE void *ILInterlockedExchangePointers_Full(void * volatile *dest,
														  void *value)
{
	void *retval;

	__asm__ __volatile__
	(
		"swp %0, %2, [%3]"
		: "=&r" (retval), "=&r" (dest)
		: "r" (value), "1" (dest)
		: "memory"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL 1

#endif /* __ARM_ARCH__ <= 5  || __ARM_ARCH_6M__ */

#endif /* defined(__GNUC__) */

#endif /* defined(__arm) || defined(__arm__) */
