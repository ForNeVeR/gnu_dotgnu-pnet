/*
 * interlocked_any.h - Generic implementation of interlocked functions
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

#if !defined(IL_HAVE_INTERLOCK)

#define IL_HAVE_INTERLOCK 1

/*
* Flush cache and set a memory barrier.
*/
static IL_INLINE void ILInterlockedMemoryBarrier()
{
	ILThreadAtomicStart();
	ILThreadAtomicEnd();
}

/*
 * Compare and exchange two 32bit integers.
 */
static IL_INLINE ILInt32 ILInterlockedCompareAndExchange(ILInt32 *destination, ILInt32 value,
															ILInt32 comparand)
{
	ILInt32 retval;
	
	ILThreadAtomicStart();
	
	retval = *destination;
	
	if (retval == comparand)
	{
		*destination = value;
	}
	
	ILThreadAtomicEnd();
	
	return retval;
}

/*
 * Increment a 32bit integer.
 */
static IL_INLINE ILInt32 ILInterlockedIncrement(ILInt32 *destination)
{
	int retval;

	ILThreadAtomicStart();

	retval = ++(*destination);

	ILThreadAtomicEnd();

	return retval;	
}

/*
 * Decrement a 32bit integer.
 */
static IL_INLINE ILInt32 ILInterlockedDecrement(ILInt32 *destination)
{
	int retval;

	ILThreadAtomicStart();

	retval = --(*destination);

	ILThreadAtomicEnd();

	return retval;
}

/*
 * Exchange integers.
 */
static IL_INLINE ILInt32 ILInterlockedExchange(ILInt32 *destination, ILInt32 value)
{
	int retval;

	ILThreadAtomicStart();

	retval = *destination;
	*destination = value;

	ILThreadAtomicEnd();

	return retval;
}

/*
 * Compare and exchange two pointers.
 */
static IL_INLINE void *ILInterlockedCompareAndExchangePointers(void **destination, void *value,
																void *comparand)
{
	void *retval;
		
	ILThreadAtomicStart();
	
	retval = *destination;
	
	if (retval == comparand)
	{
		*destination = value;
	}
	
	ILThreadAtomicEnd();
	
	return retval;
}

/*
 * Exchange pointers.
 */
static IL_INLINE void *ILInterlockedExchangePointers(void **destination, void *value)
{
	void *retval;
		
	ILThreadAtomicStart();
	
	retval = *destination;	
	*destination = value;
	
	ILThreadAtomicEnd();
	
	return retval;
}

#endif

