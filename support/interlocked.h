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
 * Semantics:
 * All Functions without one of the siffixes _Acquire, _Release or
 * _Full have a relaxed memory access order constraint. This means it is not
 * guaranteed that the operation is visible to the public at the point where
 * the source code states with respect to preceding and following loads or
 * stores. This can happen because of possible processor load / store
 * reordering.
 *
 * _Acquire Suffix: Operations with this suffix guarantee that no operations
 * following this instruction are moved before this instruction.
 *
 * _Release Suffix: Operations with this suffix guarantee that no operations
 * preceeding this instruction are moved after this instruction.
 *
 * _Full suffix: The rules for _Acquire and _Release apply.
 *
 * NOTE: The identifier after define: has to be defined if an arch specific
 * version of this function is available.
 *
 * Implemented functions:
 *
 * Full memory barrier.
 * define: IL_HAVE_INTERLOCKED_MEMORYBARRIER
 *
 * void ILInterlockedMemoryBarrier(void)
 *
 * Load a 32 bit value from a location.
 * Returns the 32 bit value from the location specified.
 * define: IL_HAVE_INTERLOCKED_LOAD
 * define: IL_HAVE_INTERLOCKED_LOAD_ACQUIRE
 *
 * ILInt32 ILInterlockedLoad(const volatile ILInt32 *dest)
 *
 * Load a pointer value from a location.
 * Returns the pointer value from the location specified.
 * define: IL_HAVE_INTERLOCKED_LOADPOINTER
 * define: IL_HAVE_INTERLOCKED_LOADPOINTER_ACQUIRE
 *
 * void * ILInterlockedLoadPointer(void * const volatile *dest)
 *
 * Store a 32 bit value to a location.
 * define: IL_HAVE_INTERLOCKED_STORE
 * define: IL_HAVE_INTERLOCKED_STORE_RELEASE
 *
 * void ILInterlockedStore(volatile ILInt32 *dest, ILInt32 value)
 *
 * Store a pointer value to a location.
 * define: IL_HAVE_INTERLOCKED_STOREPOINTER
 * define: IL_HAVE_INTERLOCKED_STOREPOINTER_RELEASE
 *
 * void ILInterlockedStorePointer(void * volatile *dest, void *value)
 *
 * Exchange integers.
 * Returns the original value at *dest.
 * define: IL_HAVE_INTERLOCKED_EXCHANGE
 * define: IL_HAVE_INTERLOCKED_EXCHANGE_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_EXCHANGE_RELEASE
 * define: IL_HAVE_INTERLOCKED_EXCHANGE_FULL
 *
 * ILInt32 ILInterlockedExchange(volatile ILInt32 *dest, ILInt32 value)
 *
 * Exchange pointers.
 * Returns the original value at *dest 
 * define: IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS
 * define: IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_RELEASE
 * define: IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL
 *
 * void *ILInterlockedExchangePointers(void * volatile *dest, void *value)
 *
 * Compare and exchange two 32bit integers.
 * Returns the original value at *dest 
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL
 *
 * ILInt32 ILInterlockedCompareAndExchange(volatile ILInt32 *dest,
 *										   ILInt32 value,
 *										   ILInt32 comparand)
 *
 * Compare and exchange two pointers.
 * Returns the original value at *dest 
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_RELEASE
 * define: IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL
 * 
 * void *ILInterlockedCompareAndExchangePointers(void * volatile *dest,
 *												 void *value,
 *												 void *comparand)
 *
 * Add the 32 bit values *dest and value and store the result at *dest.
 * Returns the result od the addition (new value at *dest)
 * define: IL_HAVE_INTERLOCKED_ADD
 * define: IL_HAVE_INTERLOCKED_ADD_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_ADD_RELEASE
 * define: IL_HAVE_INTERLOCKED_ADD_FULL
 *
 * ILInt32 ILInterlockedAdd(volatile ILInt32 *dest, ILInt32 value)
 *
 * Substract the 32 bit values *dest and value and store the result at *dest.
 * Returns the result od the subtraction (new value at *dest)
 * define: IL_HAVE_INTERLOCKED_SUB
 * define: IL_HAVE_INTERLOCKED_SUB_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_SUB_RELEASE
 * define: IL_HAVE_INTERLOCKED_SUB_FULL
 *
 * ILInt32 ILInterlockedSub(volatile ILInt32 *dest, ILInt32 value)
 *
 * NOTE: If not defined by the arch specific definition it is a implemented
 * by a macro that is using ILInterlockedAdd with value negated.
 * (See interlocked_any.h)
 *
 * Increment a 32bit integer.
 * Returns the value of the incremented integer.
 * define: IL_HAVE_INTERLOCKED_INCREMENT
 * define: IL_HAVE_INTERLOCKED_INCREMENT_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_INCREMENT_RELEASE
 * define: IL_HAVE_INTERLOCKED_INCREMENT_FULL
 *
 * ILInt32 ILInterlockedIncrement(volatile ILInt32 *dest)
 *
 * NOTE: If not defined by the arch specific definition it is a implemented
 * by a macro that is using ILInterlockedAdd with a value of 1.
 * (See interlocked_any.h)
 *
 * Decrement a 32bit integer.
 * Returns the value of the decremented integer.
 * define: IL_HAVE_INTERLOCKED_DECREMENT
 * define: IL_HAVE_INTERLOCKED_DECREMENT_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_DECREMENT_RELEASE
 * define: IL_HAVE_INTERLOCKED_DECREMENT_FULL
 *
 * ILInt32 ILInterlockedDecrement(volatile ILInt32 *dest)
 *
 * NOTE: If not defined by the arch specific definition it is a implemented
 * by a macro that is using ILInterlockedSub with a value of 1.
 * (See interlocked_any.h)
 *
 * Bitwise AND of *dest and value and the result is stored at *dest
 * define: IL_HAVE_INTERLOCKED_AND
 * define: IL_HAVE_INTERLOCKED_AND_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_AND_RELEASE
 * define: IL_HAVE_INTERLOCKED_AND_FULL
 *
 * void ILInterlockedAnd(volatile ILUInt32 *dest, ILUInt32 value)
 *
 * Bitwise OR of *dest and value and the result is stored at *dest
 * define: IL_HAVE_INTERLOCKED_OR
 * define: IL_HAVE_INTERLOCKED_OR_ACQUIRE
 * define: IL_HAVE_INTERLOCKED_OR_RELEASE
 * define: IL_HAVE_INTERLOCKED_OR_FULL
 *
 * void ILInterlockedOr(volatile ILUInt32 *dest, ILUInt32 value)
 *
 * NOTE: If archdependent versions with the _Aquire or _Release suffix
 * are not defined they will be mapped to the version with the _Full suffix.
 *
 * NOTE: The versions using a mutex to protect the value are generally
 * available with the same name and a leading underscore.
 * This is to allow the usage of the version using the mutex even if a
 * native version is available if propper protection requires the
 * consistent availability of more than one interlocked function.
 */

/*
 * Define a barrier for the used compiler to prevent that the operation
 * in question is moved around by the compiler's optimizer.
 */
#if defined(__GNUC__)
#define ILInterlockedCompilerBarrier	__asm__ __volatile__ ("" : : : "memory")
#else
#define ILInterlockedCompilerBarrier
#endif

/* TODO: implement native interlocked functions for other processors */

#include "interlocked_x86.h"
#include "interlocked_arm.h"
#include "interlocked_ppc.h"
#include "interlocked_any.h"

#endif /* _INTERLOCKED_H_ */
