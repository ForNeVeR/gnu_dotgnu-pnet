/*
 * interlocked_x86.h - Implementation of interlocked functions for
 * intel processors.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#if !defined(IL_HAVE_INTERLOCK) \
	&& ((defined(__i386) || defined(__i386__) || defined(__x86_64__)) \
	&& defined(__GNUC__))

#define IL_HAVE_INTERLOCK 1

/*
 * Compare and exchange two 32bit integers.
 */

static IL_INLINE ILInt32 ILInterlockedCompareAndExchange(ILInt32 *destination, ILInt32 value,
										ILInt32 comparand)
{
	ILInt32 retval;

	__asm__ __volatile__
	(
		"lock;"
		"cmpxchgl %2, %0"
		: "=m" (*destination), "=a" (retval)
		: "r" (value), "m" (*destination), "a" (comparand)
	);

	return retval;
}

/*
 * Increment a 32bit integer.
 */
static IL_INLINE ILInt32 ILInterlockedIncrement(ILInt32 *destination)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"lock;"
		"xaddl %0, %1"
		: "=r" (retval), "=m" (*destination)
		: "0" (1), "m" (*destination)
	);

	return retval + 1;
}

/*
 * Decrement a 32bit integer.
 */
static IL_INLINE ILInt32 ILInterlockedDecrement(ILInt32 *destination)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"lock;"
		"xaddl %0, %1"
		: "=r" (retval), "=m" (*destination)
		: "0" (-1), "m" (*destination)
	);

	return retval - 1;
}

/*
 * Exchange integers.
 */
static IL_INLINE ILInt32 ILInterlockedExchange(ILInt32 *destination, ILInt32 value)
{
	ILInt32 retval;

	__asm__ __volatile__ 
	(
		"1:;"
		"lock;"
		"cmpxchgl %2, %0;"
		"jne 1b"
		: "=m" (*destination), "=a" (retval)
		: "r" (value), "m" (*destination), "a" (*destination)
	);

	return retval;
}

/*
 * Compare and exchange two pointers.
 */
static IL_INLINE void *ILInterlockedCompareAndExchangePointers(void **destination, void *value,
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
		: "=m" (*destination), "=a" (retval)
		: "r" (value), "m" (*destination), "a" (comparand)
	);

	return retval;
}

/*
 * Exchange pointers.
 */
static IL_INLINE void *ILInterlockedExchangePointers(void **destination, void *value)
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
		: "=m" (*destination), "=a" (retval)
		: "r" (value), "m" (*destination), "a" (*destination)
	);

	return retval;
}

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

#endif
