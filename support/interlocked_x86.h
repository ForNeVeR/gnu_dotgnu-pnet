/*
 * interlocked_x86.h - Implementation of interlocked functions for
 * intel processors.
 *
 * Copyright (C) 2002, 2009  Southern Storm Software, Pty Ltd.
 *
 * Authors: Thong Nguyen (tum@veridicus.com)
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

#if (defined(__i386) || defined(__i386__) || defined(__x86_64__))

#if defined(__GNUC__)

/*
 * Flush cache and set a memory barrier.
 */
static IL_INLINE void ILInterlockedMemoryBarrier()
{
#if defined(__SSE2__) || defined(__sse2__)
	__asm__ __volatile__
	(
		"mfence"
		:::
		"memory"
	);
#else
	__asm__ __volatile__
	(
		"lock; addl $0,0(%%esp)"
		:::
		"memory"
	);
#endif
}
#define IL_HAVE_INTERLOCKED_MEMORYBARRIER 1

/*
 * Exchange two 32 bit integers.
 */
static IL_INLINE ILInt32 ILInterlockedExchange(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:;"
		"lock;"
		"cmpxchgl %2, %0;"
		"jne 1b"
		: "=m" (*dest), "=a" (retval)
		: "r" (value), "m" (*dest), "a" (*dest)
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGE 1

/*
 * Exchange pointers.
 */
static IL_INLINE void *ILInterlockedExchangePointers(void * volatile *dest,
													 void *value)
{
	void *retval;

	__asm__ __volatile__ 
	(
		"1:;"
		"lock;"
#if defined(__x86_64__)
		"cmpxchgq %2, %0;"
#else
		"cmpxchgl %2, %0;"
#endif
		"jne 1b"
		: "=m" (*dest), "=a" (retval)
		: "r" (value), "m" (*dest), "a" (*dest)
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS 1

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
		"lock;"
		"cmpxchgl %2, %0"
		: "=m" (*dest), "=a" (retval)
		: "r" (value), "m" (*dest), "a" (comparand)
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE 1

/*
 * Compare and exchange two pointers.
 */
static IL_INLINE void *ILInterlockedCompareAndExchangePointers(void * volatile *dest,
															   void *value,
															   void *comparand)
{
	void *retval;

	__asm__ __volatile__
	(
		"lock;"
#if defined(__x86_64__)
		"cmpxchgq %2, %0;"
#else
		"cmpxchgl %2, %0;"
#endif
		: "=m" (*dest), "=a" (retval)
		: "r" (value), "m" (*dest), "a" (comparand)
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS 1

/*
 * Add two 32 bit integer values.
 */
static IL_INLINE ILInt32 ILInterlockedAdd(volatile ILInt32 *dest,
										 ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"lock;"
		"xaddl %0, %1"
		: "=r" (retval), "=m" (*dest)
		: "0" (value), "m" (*dest) : "memory"
	);

	return retval + value;
}
#define IL_HAVE_INTERLOCKED_ADD 1

/*
 * 32bit bitwise AND
 */
static IL_INLINE void ILInterlockedAnd(volatile ILUInt32 *dest, ILUInt32 value)
{
	__asm__ __volatile__ 
	(
		"lock;"
		"andl %1, %0"
		: "=m" (*dest)
		: "er" (value), "m" (*dest)
		: "memory");
}
#define IL_HAVE_INTERLOCKED_AND 1

/*
 * 32bit bitwise OR
 */
static IL_INLINE void ILInterlockedOr(volatile ILUInt32 *dest, ILUInt32 value)
{
	__asm__ __volatile__ 
	(
		"lock;"
		"orl %1, %0"
		: "=m" (*dest)
		: "er" (value), "m" (*dest)
		: "memory");
}
#define IL_HAVE_INTERLOCKED_OR 1

#endif /* defined(__GNUC__) */

#endif /* (defined(__i386) || defined(__i386__) || defined(__x86_64__)) */
