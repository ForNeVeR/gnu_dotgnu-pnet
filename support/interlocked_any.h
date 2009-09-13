/*
 * interlocked_any.h - Generic implementation of interlocked functions
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

#if !defined(IL_HAVE_INTERLOCKED_MEMORYBARRIER)
/*
 * Flush cache and set a memory barrier.
 */
static IL_INLINE void ILInterlockedMemoryBarrier()
{
	ILThreadAtomicStart();
	ILThreadAtomicEnd();
}
#endif /* !defined(IL_HAVE_INTERLOCKED_MEMORYBARRIER) */

/*
 * Exchange integers.
 */
static IL_INLINE ILInt32 _ILInterlockedExchange(volatile ILInt32 *dest,
												ILInt32 value)
{
	ILInt32 retval;

	ILThreadAtomicStart();

	retval = *dest;
	*dest = value;

	ILThreadAtomicEnd();

	return retval;
}
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGE)
#define ILInterlockedExchange(dest, value) \
		_ILInterlockedExchange((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE) */

/*
 * Exchange pointers.
 */
static IL_INLINE void *_ILInterlockedExchangePointers(void * volatile *dest,
													  void *value)
{
	void *retval;
		
	ILThreadAtomicStart();
	
	retval = *dest;
	*dest = value;
	
	ILThreadAtomicEnd();
	
	return retval;
}
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS)
#define ILInterlockedExchangePointers(dest, value) \
		_ILInterlockedExchangePointers((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS) */

/*
 * Compare and exchange two 32bit integers.
 */
static IL_INLINE ILInt32 _ILInterlockedCompareAndExchange(volatile ILInt32 *dest,
														  ILInt32 value,
														  ILInt32 comparand)
{
	ILInt32 retval;
	
	ILThreadAtomicStart();
	
	retval = *dest;
	
	if (retval == comparand)
	{
		*dest = value;
	}
	
	ILThreadAtomicEnd();
	
	return retval;
}
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE)
#define ILInterlockedCompareAndExchange(dest, value, comparand) \
		_ILInterlockedCompareAndExchange((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE) */

/*
 * Compare and exchange two pointers.
 */
static IL_INLINE void *_ILInterlockedCompareAndExchangePointers(void * volatile *dest,
																void *value,
																void *comparand)
{
	void *retval;
		
	ILThreadAtomicStart();
	
	retval = (void *)*dest;
	
	if (retval == comparand)
	{
		*dest = value;
	}
	
	ILThreadAtomicEnd();
	
	return retval;
}
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS)
#define ILInterlockedCompareAndExchangePointers(dest, value, comparand) \
		_ILInterlockedCompareAndExchangePointers((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS) */

/*
 * Add the two 32bit integers *dest and value and store the result at *dest.
 */
static IL_INLINE ILInt32 _ILInterlockedAdd(volatile ILInt32 *dest,
										   ILInt32 value)
{
	ILInt32 retval;

	ILThreadAtomicStart();

	retval = *dest + value;
	*dest = retval;

	ILThreadAtomicEnd();

	return retval;	
}
#if !defined(IL_HAVE_INTERLOCKED_ADD)
#define ILInterlockedAdd(dest, value)	_ILInterlockedAdd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD) */

/*
 * Subtract value from *dest and store the result at *dest.
 */
#define _ILInterlockedSub(dest, value)	_ILInterlockedAdd((dest), -(value))
#if !defined(IL_HAVE_INTERLOCKED_SUB)
#if defined(IL_HAVE_INTERLOCKED_ADD)
#define ILInterlockedSub(dest, value)	ILInterlockedAdd((dest), -(value))
#define IL_HAVE_INTERLOCKED_SUB 1
#else
#define ILInterlockedSub(dest, value)	_ILInterlockedSub((dest), (value))
#endif
#endif /* !defined(IL_HAVE_INTERLOCKED_SUB) */

/*
 * Increment a 32bit integer.
 */
#define _ILInterlockedIncrement(dest)	_ILInterlockedAdd((dest), 1)
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT)
#if defined(IL_HAVE_INTERLOCKED_ADD)
#define ILInterlockedIncrement(dest)	ILInterlockedAdd((dest), 1)
#define IL_HAVE_INTERLOCKED_INCREMENT 1
#else
#define ILInterlockedIncrement(dest)	_ILInterlockedIncrement((dest))
#endif
#endif /* !defined(IL_HAVE_INTERLOCKED_INCREMENT) */

/*
 * Decrement a 32bit integer.
 */
#define _ILInterlockedDecrement(dest)	_ILInterlockedSub((dest), 1)
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT)
#if defined(IL_HAVE_INTERLOCKED_SUB)
#define ILInterlockedDecrement(dest)	ILInterlockedSub((dest), 1)
#define IL_HAVE_INTERLOCKED_DECREMENT 1
#else
#define ILInterlockedDecrement(dest)	_ILInterlockedDecrement((dest))
#endif
#endif /* !defined(IL_HAVE_INTERLOCKED_DECREMENT) */

/*
 * Atomic bitwise AND of unsigned 32 bit values
 */
static IL_INLINE void _ILInterlockedAnd(volatile ILUInt32 *dest,
										ILUInt32 value)
{
	ILThreadAtomicStart();

	*dest &= value;

	ILThreadAtomicEnd();
}
#if !defined(IL_HAVE_INTERLOCKED_AND)
#define ILInterlockedAnd(dest, value)	_ILInterlockedAnd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_AND) */

/*
 * Atomic bitwise OR of unsigned 32 bit values
 */
static IL_INLINE void _ILInterlockedOr(volatile ILUInt32 *dest,
									   ILUInt32 value)
{
	ILThreadAtomicStart();

	*dest |= value;

	ILThreadAtomicEnd();
}
#if !defined(IL_HAVE_INTERLOCKED_OR)
#define ILInterlockedOr(dest, value)	_ILInterlockedOr((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_OR) */
