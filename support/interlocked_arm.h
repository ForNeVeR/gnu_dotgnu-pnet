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

#if defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || \
	defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6ZK__) || \
	defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
	defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7R__)

/*
 * The versions for the interlocked operations available for all arm cores
 * version 6 and later.
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

#else /* __ARM_ARCH__ <= 5 */

/*
 * Exchange two 32 bit integers.
 */
static IL_INLINE ILInt32 ILInterlockedExchange(volatile ILInt32 *dest,
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
		"swp %0, %2, [%3]"
		: "=&r" (retval), "=&r" (dest)
		: "r" (value), "1" (dest)
		: "memory"
	);

	return retval;
}
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS 1

#endif /* __ARM_ARCH__ <= 5 */

#endif /* defined(__GNUC__) */

#endif /* defined(__arm) || defined(__arm__) */
