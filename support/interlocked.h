/*
 * interlocked.h - Implementation of interlocked functions.
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

#ifndef _INTERLOCKED_H_
#define _INTERLOCKED_H_

#ifndef IL_INLINE
	#ifdef __GNUC__
		#define IL_INLINE       __inline__
	#elif defined(_MSC_VER)
		#define IL_INLINE	__forceinline
	#else
		#define IL_INLINE
	#endif
#endif

/*
 * NOTE: The identifier after define: has to be defined if an arch specific
 * version of this function is available.
 *
 * Implemented functions:
 *
 * Flush cache and set a memory barrier.
 * define: IL_HAVE_INTERLOCKED_MEMORYBARRIER
 *
 * void ILInterlockedMemoryBarrier(void)
 *
 * Exchange integers.
 * Returns the original value at *dest.
 * define: IL_HAVE_INTERLOCKED_EXCHANGE
 *
 * ILInt32 ILInterlockedExchange(ILInt32 *dest, ILInt32 value)
 *
 * Exchange pointers.
 * Returns the original value at *dest 
 * define: IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS
 *
 * void *ILInterlockedExchangePointers(void **dest, void *value)
 *
 * Compare and exchange two 32bit integers.
 * Returns the original value at *dest 
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE
 *
 * ILInt32 ILInterlockedCompareAndExchange(ILInt32 *dest, ILInt32 value,
 *										   ILInt32 comparand)
 *
 * Compare and exchange two pointers.
 * Returns the original value at *dest 
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS
 * 
 * void *ILInterlockedCompareAndExchangePointers(void **dest, void *value,
 *												 void *comparand)
 *
 * Add the 32 bit values *dest and value and store the result at *dest.
 * Returns the result od the addition (new value at *dest)
 * define: IL_HAVE_INTERLOCKED_ADD
 *
 * ILInt32 ILInterlockedAdd(ILInt32 *dest, ILInt32 value)
 *
 * Substract the 32 bit values *dest and value and store the result at *dest.
 * Returns the result od the subtraction (new value at *dest)
 * define: IL_HAVE_INTERLOCKED_SUB
 *
 * ILInt32 ILInterlockedSub(ILInt32 *dest, ILInt32 value)
 *
 * NOTE: If not defined by the arch specific definition it is a implemented
 * by a macro that is using ILInterlockedAdd with value negated.
 * (See interlocked_any.h)
 *
 * Increment a 32bit integer.
 * Returns the value of the incremented integer.
 * define: IL_HAVE_INTERLOCKED_INCREMENT
 *
 * ILInt32 ILInterlockedIncrement(ILInt32 *dest)
 *
 * NOTE: If not defined by the arch specific definition it is a implemented
 * by a macro that is using ILInterlockedAdd with a value of 1.
 * (See interlocked_any.h)
 *
 * Decrement a 32bit integer.
 * Returns the value of the decremented integer.
 * define: IL_HAVE_INTERLOCKED_DECREMENT
 *
 * ILInt32 ILInterlockedDecrement(ILInt32 *dest)
 *
 * NOTE: If not defined by the arch specific definition it is a implemented
 * by a macro that is using ILInterlockedSub with a value of 1.
 * (See interlocked_any.h)
 *
 * Bitwise AND of *dest and value and the result is stored at *dest
 * define: IL_HAVE_INTERLOCKED_AND
 *
 * void ILInterlockedAnd(volatile ILUInt32 *dest, ILUInt32 value)
 *
 * Bitwise OR of *dest and value and the result is stored at *dest
 * define: IL_HAVE_INTERLOCKED_OR
 *
 * void ILInterlockedAnd(volatile ILUInt32 *dest, ILUInt32 value)
 */

/* TODO: implement native interlocked functions for other processors */

#include "interlocked_x86.h"
#include "interlocked_any.h"

#endif /* _INTERLOCKED_H_ */
