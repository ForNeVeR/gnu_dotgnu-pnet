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
 * Load a 32 bit value from a location.
 */
static IL_INLINE ILInt32 _ILInterlockedLoad(const volatile ILInt32 *dest)
{
	/*
	 * Cast away the volatile because gcc will generate code with acquire
	 * semantics on IA64 otherwise.
	 */
	return *(ILInt32 *)dest;
}
#if !defined(IL_HAVE_INTERLOCKED_LOAD)
#define ILInterlockedLoad(dest)	_ILInterlockedLoad((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_LOAD) */

/*
 * Load a 32 bit value from a location with acquire semantics.
 */
static IL_INLINE ILInt32 _ILInterlockedLoad_Acquire(const volatile ILInt32 *dest)
{
	ILInt32 value;

	value = *dest;
	ILInterlockedCompilerBarrier;
	return value;
}
#if !defined(IL_HAVE_INTERLOCKED_LOAD_ACQUIRE)
#define ILInterlockedLoad_Acquire(dest)	_ILInterlockedLoad_Acquire((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_LOAD_ACQUIRE) */

/*
 * Load a pointer value from a location.
 */
static IL_INLINE void * _ILInterlockedLoadPointer(void * const volatile *dest)
{
	/*
	 * Cast away the volatile because gcc will generate code with acquire
	 * semantics on IA64 otherwise.
	 */
	return *(void **)dest;
}
#if !defined(IL_HAVE_INTERLOCKED_LOADPOINTER)
#define ILInterlockedLoadPointer(dest)	_ILInterlockedLoadPointer((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_LOADPOINTER) */

/*
 * Load a pointer value from a location with acquire semantics.
 */
static IL_INLINE void *_ILInterlockedLoadPointer_Acquire(void * const volatile *dest)
{
	void *value;

	value = *dest;
	ILInterlockedCompilerBarrier;
	return value;
}
#if !defined(IL_HAVE_INTERLOCKED_LOADPOINTER_ACQUIRE)
#define ILInterlockedLoadPointer_Acquire(dest)	_ILInterlockedLoadPointer_Acquire((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_LOADPOINTER_ACQUIRE) */

/*
 * Store a 32 bit value to a location.
 */
static IL_INLINE void _ILInterlockedStore(volatile ILInt32 *dest,
										  ILInt32 value)
{
	/*
	 * Cast away the volatile because gcc will generate code with release
	 * semantics on IA64 otherwise.
	 */
	*(ILInt32 *)dest = value;
}
#if !defined(IL_HAVE_INTERLOCKED_STORE)
#define ILInterlockedStore(dest, value)	_ILInterlockedStore((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_STORE) */

/*
 * Store a 32 bit value to a location with release semantics.
 */
static IL_INLINE void _ILInterlockedStore_Release(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInterlockedCompilerBarrier;
	*dest = value;
}
#if !defined(IL_HAVE_INTERLOCKED_STORE_RELEASE)
#define ILInterlockedStore_Release(dest, value) \
		_ILInterlockedStore_Release((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_STORE_RELEASE) */

/*
 * Store a pointer value to a location.
 */
static IL_INLINE void _ILInterlockedStorePointer(void * volatile *dest,
												 void *value)
{
	/*
	 * Cast away the volatile because gcc will generate code with release
	 * semantics on IA64 otherwise.
	 */
	*(void **)dest = value;
}
#if !defined(IL_HAVE_INTERLOCKED_STOREPOINTER)
#define ILInterlockedStorePointer(dest, value) \
		_ILInterlockedStorePointer((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_STOREPOINTER) */

/*
 * Store a pointer value to a location with release semantics.
 */
static IL_INLINE void _ILInterlockedStorePointer_Release(void * volatile *dest,
														 void *value)
{
	ILInterlockedCompilerBarrier;
	*dest = value;
}
#if !defined(IL_HAVE_INTERLOCKED_STOREPOINTER_RELEASE)
#define ILInterlockedStorePointer_Release(dest, value) \
		_ILInterlockedStorePointer_Release((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_STOREPOINTER_RELEASE) */

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
#if defined(IL_HAVE_INTERLOCKED_EXCHANGE_FULL)
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGE_ACQUIRE)
#define ILInterlockedExchange_Acquire(dest, value) \
		ILInterlockedExchange_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGE_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGE_RELEASE)
#define ILInterlockedExchange_Release(dest, value) \
		ILInterlockedExchange_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGE_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_EXCHANGE_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGE)
#if defined(IL_HAVE_INTERLOCKED_EXCHANGE_ACQUIRE)
#define ILInterlockedExchange(dest, value) \
		ILInterlockedExchange_Acquire((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGE 1
#elif defined(IL_HAVE_INTERLOCKED_EXCHANGE_RELEASE)
#define ILInterlockedExchange(dest, value) \
		ILInterlockedExchange_Release((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGE 1
#elif defined(IL_HAVE_INTERLOCKED_EXCHANGE_FULL)
#define ILInterlockedExchange(dest, value) \
		ILInterlockedExchange_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGE 1
#else /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE_FULL) */
#define ILInterlockedExchange(dest, value) \
		_ILInterlockedExchange((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE_FULL) */
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGE_ACQUIRE)
#define ILInterlockedExchange_Acquire(dest, value) \
		_ILInterlockedExchange((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGE_RELEASE)
#define ILInterlockedExchange_Release(dest, value) \
		_ILInterlockedExchange((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGE_FULL)
#define ILInterlockedExchange_Full(dest, value) \
		_ILInterlockedExchange((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE_FULL) */

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
#if defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL)
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_ACQUIRE)
#define ILInterlockedExchangePointers_Acquire(dest, value) \
		ILInterlockedExchangePointers_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_RELEASE)
#define ILInterlockedExchangePointers_Release(dest, value) \
		ILInterlockedExchangePointers_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS)
#if defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_ACQUIRE)
#define ILInterlockedExchangePointers(dest, value) \
		ILInterlockedExchangePointers_Acquire((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS 1
#elif defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_RELEASE)
#define ILInterlockedExchangePointers(dest, value) \
		ILInterlockedExchangePointers_Release((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS 1
#elif defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL)
#define ILInterlockedExchangePointers(dest, value) \
		ILInterlockedExchangePointers_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_EXCHANGE 1
#else /* !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL) */
#define ILInterlockedExchangePointers(dest, value) \
		_ILInterlockedExchangePointers((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL) */
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_ACQUIRE)
#define ILInterlockedExchangePointers_Acquire(dest, value) \
		_ILInterlockedExchangePointers((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_RELEASE)
#define ILInterlockedExchangePointers_Release(dest, value) \
		_ILInterlockedExchangePointers((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_EXCHANGEPOINTERS_FULL)
#define ILInterlockedExchangePointers_Full(dest, value) \
		_ILInterlockedExchangePointers((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_EXCHANGE_FULL) */

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
#if defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL)
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE)
#define ILInterlockedCompareAndExchange_Acquire(dest, value, comparand) \
		ILInterlockedCompareAndExchange_Full((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE)
#define ILInterlockedCompareAndExchange_Release(dest, value, comparand) \
		ILInterlockedCompareAndExchange_Full((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE)
#if defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE)
#define ILInterlockedCompareAndExchange(dest, value, comparand) \
		ILInterlockedCompareAndExchange_Acquire((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE 1
#elif defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE)
#define ILInterlockedCompareAndExchange(dest, value, comparand) \
		ILInterlockedCompareAndExchange_Release((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE 1
#elif defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL)
#define ILInterlockedCompareAndExchange(dest, value, comparand) \
		ILInterlockedCompareAndExchange_Full((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE 1
#else /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL) */
#define ILInterlockedCompareAndExchange(dest, value, comparand) \
		_ILInterlockedCompareAndExchange((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL) */
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE)
#define ILInterlockedCompareAndExchange_Acquire(dest, value, comparand) \
		_ILInterlockedCompareAndExchange((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE)
#define ILInterlockedCompareAndExchange_Release(dest, value, comparand) \
		_ILInterlockedCompareAndExchange((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL)
#define ILInterlockedCompareAndExchange_Full(dest, value, comparand) \
		_ILInterlockedCompareAndExchange((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL) */

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
#if defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL)
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_ACQUIRE)
#define ILInterlockedCompareAndExchangePointers_Acquire(dest, value, comparand) \
		ILInterlockedCompareAndExchangePointers_Full((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_RELEASE)
#define ILInterlockedCompareAndExchangePointers_Release(dest, value, comparand) \
		ILInterlockedCompareAndExchangePointers_Full((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS)
#if defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_ACQUIRE)
#define ILInterlockedCompareAndExchangePointers(dest, value, comparand) \
		ILInterlockedCompareAndExchangePointers_Acquire((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS 1
#elif defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_RELEASE)
#define ILInterlockedCompareAndExchangePointers(dest, value, comparand) \
		ILInterlockedCompareAndExchangePointers_Release((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS 1
#elif defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL)
#define ILInterlockedCompareAndExchangePointers(dest, value, comparand) \
		ILInterlockedCompareAndExchangePointers_Full((dest), (value), (comparand))
#define IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS 1
#else /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL) */
#define ILInterlockedCompareAndExchangePointers(dest, value, comparand) \
		_ILInterlockedCompareAndExchangePointers((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL) */
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_ACQUIRE)
#define ILInterlockedCompareAndExchangePointers_Acquire(dest, value, comparand) \
		_ILInterlockedCompareAndExchangePointers((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_RELEASE)
#define ILInterlockedCompareAndExchangePointers_Release(dest, value, comparand) \
		_ILInterlockedCompareAndExchangePointers((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL)
#define ILInterlockedCompareAndExchangePointers_Full(dest, value, comparand) \
		_ILInterlockedCompareAndExchangePointers((dest), (value), (comparand))
#endif /* !defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGEPOINTERS_FULL) */

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
#if defined(IL_HAVE_INTERLOCKED_ADD_FULL)
#if !defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE)
#define ILInterlockedAdd_Acquire(dest, value) \
		ILInterlockedAdd_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_ADD_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_ADD_RELEASE)
#define ILInterlockedAdd_Release(dest, value) \
		ILInterlockedAdd_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_ADD_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_ADD_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_ADD)
#if defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE)
#define ILInterlockedAdd(dest, value) \
		ILInterlockedAdd_Acquire((dest), (value))
#define IL_HAVE_INTERLOCKED_ADD 1
#elif defined(IL_HAVE_INTERLOCKED_ADD_RELEASE)
#define ILInterlockedAdd(dest, value) \
		ILInterlockedAdd_Release((dest), (value))
#define IL_HAVE_INTERLOCKED_ADD 1
#elif defined(IL_HAVE_INTERLOCKED_ADD_FULL)
#define ILInterlockedAdd(dest, value) \
		ILInterlockedAdd_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_ADD 1
#endif /* defined(IL_HAVE_INTERLOCKED_ADD_FULL) */
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD) */
#if !defined(IL_HAVE_INTERLOCKED_ADD) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE)
static IL_INLINE ILInt32 ILInterlockedAdd(volatile ILInt32 *dest,
										  ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval + value;
	} while(ILInterlockedCompareAndExchange(dest, retval, oldval) != oldVal);

	return retval;	
}
#define IL_HAVE_INTERLOCKED_ADD 1
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD) */
#if !defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE)
static IL_INLINE ILInt32 ILInterlockedAdd_Acquire(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval + value;
	} while(ILInterlockedCompareAndExchange_Acquire(dest, retval, oldval) != oldval);

	return retval;	
}
#define IL_HAVE_INTERLOCKED_ADD_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_ADD_RELEASE) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE)
static IL_INLINE ILInt32 ILInterlockedAdd_Release(volatile ILInt32 *dest,
												  ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval + value;
	} while(ILInterlockedCompareAndExchange_Release(dest, retval, oldval) != oldval);

	return retval;	
}
#define IL_HAVE_INTERLOCKED_ADD_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_ADD_FULL) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL)
static IL_INLINE ILInt32 ILInterlockedAdd_Full(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval + value;
	} while(ILInterlockedCompareAndExchange_Full(dest, retval, oldval) != oldval);

	return retval;	
}
#define IL_HAVE_INTERLOCKED_ADD_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_ADD)
#define ILInterlockedAdd(dest, value)	_ILInterlockedAdd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD) */
#if !defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE)
#define ILInterlockedAdd_Acquire(dest, value)	_ILInterlockedAdd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_ADD_RELEASE)
#define ILInterlockedAdd_Release(dest, value)	_ILInterlockedAdd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_ADD_FULL)
#define ILInterlockedAdd_Full(dest, value)	_ILInterlockedAdd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_FULL) */

/*
 * Subtract value from *dest and store the result at *dest.
 */
#define _ILInterlockedSub(dest, value)	_ILInterlockedAdd((dest), -(value))
#if defined(IL_HAVE_INTERLOCKED_SUB_FULL)
#if !defined(IL_HAVE_INTERLOCKED_SUB_ACQUIRE)
#define ILInterlockedSub_Acquire(dest, value) \
		ILInterlockedSub_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_SUB_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_SUB_RELEASE)
#define ILInterlockedSub_Release(dest, value) \
		ILInterlockedSub_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_SUB_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_SUB_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_SUB_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_SUB) && \
	defined(IL_HAVE_INTERLOCKED_ADD)
#define ILInterlockedSub(dest, value)	ILInterlockedAdd((dest), -(value))
#define IL_HAVE_INTERLOCKED_SUB 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_SUB_ACQUIRE) && \
	defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE)
#define ILInterlockedSub_Acquire(dest, value) \
		ILInterlockedAdd_Acquire((dest), -(value))
#define IL_HAVE_INTERLOCKED_SUB_ACQUIRE 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_SUB_RELEASE) && \
	defined(IL_HAVE_INTERLOCKED_ADD_RELEASE)
#define ILInterlockedSub_Release(dest, value) \
		ILInterlockedAdd_Release((dest), -(value))
#define IL_HAVE_INTERLOCKED_SUB_RELEASE 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_SUB_FULL) && \
	defined(IL_HAVE_INTERLOCKED_ADD_FULL)
#define ILInterlockedSub_Full(dest, value) \
		ILInterlockedAdd_Full((dest), -(value))
#define IL_HAVE_INTERLOCKED_SUB_FULL 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_SUB)
#define ILInterlockedSub(dest, value)	_ILInterlockedSub((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_SUB) */
#if !defined(IL_HAVE_INTERLOCKED_SUB_ACQUIRE)
#define ILInterlockedSub_Acquire(dest, value) \
		_ILInterlockedSub((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_SUB_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_SUB_RELEASE)
#define ILInterlockedSub_Release(dest, value) \
		_ILInterlockedSub((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_SUB_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_SUB_FULL)
#define ILInterlockedSub_Full(dest, value)	_ILInterlockedSub((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_SUB_FULL) */

/*
 * Increment a 32bit integer.
 */
#define _ILInterlockedIncrement(dest)	_ILInterlockedAdd((dest), 1)
#if defined(IL_HAVE_INTERLOCKED_INCREMENT_FULL)
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT_ACQUIRE)
#define ILInterlockedIncrement_Acquire(dest) \
		ILInterlockedIncrement_Full((dest))
#define IL_HAVE_INTERLOCKED_INCREMENT_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_INCREMENT_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT_RELEASE)
#define ILInterlockedIncrement_Release(dest) \
		ILInterlockedIncrement_Full((dest))
#define IL_HAVE_INTERLOCKED_INCREMENT_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_INCREMENT_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_INCREMENT_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT) && \
	defined(IL_HAVE_INTERLOCKED_ADD)
#define ILInterlockedIncrement(dest)	ILInterlockedAdd((dest), 1)
#define IL_HAVE_INTERLOCKED_INCREMENT 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT_ACQUIRE) && \
	defined(IL_HAVE_INTERLOCKED_ADD_ACQUIRE)
#define ILInterlockedIncrement_Acquire(dest) \
		ILInterlockedAdd_Acquire((dest), 1)
#define IL_HAVE_INTERLOCKED_INCREMENT_ACQUIRE 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT_RELEASE) && \
	defined(IL_HAVE_INTERLOCKED_ADD_RELEASE)
#define ILInterlockedIncrement_Release(dest) \
		ILInterlockedAdd_Release((dest), 1)
#define IL_HAVE_INTERLOCKED_INCREMENT_RELEASE 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT_FULL) && \
	defined(IL_HAVE_INTERLOCKED_ADD_FULL)
#define ILInterlockedIncrement_Full(dest) \
		ILInterlockedAdd_Full((dest), 1)
#define IL_HAVE_INTERLOCKED_INCREMENT_FULL 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT)
#define ILInterlockedIncrement(dest)	_ILInterlockedIncrement((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_INCREMENT) */
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT_ACQUIRE)
#define ILInterlockedIncrement_Acquire(dest)	_ILInterlockedIncrement((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_INCREMENT_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT_RELEASE)
#define ILInterlockedIncrement_Release(dest)	_ILInterlockedIncrement((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_INCREMENT_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_INCREMENT_FULL)
#define ILInterlockedIncrement_Full(dest)	_ILInterlockedIncrement((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_INCREMENT_FULL) */

/*
 * Decrement a 32bit integer.
 */
#define _ILInterlockedDecrement(dest)	_ILInterlockedSub((dest), 1)
#if defined(IL_HAVE_INTERLOCKED_DECREMENT_FULL)
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT_ACQUIRE)
#define ILInterlockedDecrement_Acquire(dest) \
		ILInterlockedDecrement_Full((dest))
#define IL_HAVE_INTERLOCKED_DECREMENT_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_DECREMENT_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT_RELEASE)
#define ILInterlockedDecrement_Release(dest) \
		ILInterlockedDecrement_Full((dest))
#define IL_HAVE_INTERLOCKED_DECREMENT_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_DECREMENT_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_DECREMENT_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT) && \
	defined(IL_HAVE_INTERLOCKED_SUB)
#define ILInterlockedDecrement(dest)	ILInterlockedSub((dest), 1)
#define IL_HAVE_INTERLOCKED_DECREMENT 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT_ACQUIRE) && \
	defined(IL_HAVE_INTERLOCKED_SUB_ACQUIRE)
#define ILInterlockedDecrement_Acquire(dest) \
		ILInterlockedSub_Acquire((dest), 1)
#define IL_HAVE_INTERLOCKED_DECREMENT_ACQUIRE 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT_RELEASE) && \
	defined(IL_HAVE_INTERLOCKED_SUB_RELEASE)
#define ILInterlockedDecrement_Release(dest) \
		ILInterlockedSub_Release((dest), 1)
#define IL_HAVE_INTERLOCKED_DECREMENT_RELEASE 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT_FULL) && \
	defined(IL_HAVE_INTERLOCKED_SUB_FULL)
#define ILInterlockedDecrement_Full(dest) \
		ILInterlockedSub_Full((dest), 1)
#define IL_HAVE_INTERLOCKED_DECREMENT_FULL 1
#endif
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT)
#define ILInterlockedDecrement(dest)	_ILInterlockedDecrement((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_DECREMENT) */
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT_ACQUIRE)
#define ILInterlockedDecrement_Acquire(dest)	_ILInterlockedDecrement((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_DECREMENT_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT_RELEASE)
#define ILInterlockedDecrement_Release(dest)	_ILInterlockedDecrement((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_DECREMENT_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_DECREMENT_FULL)
#define ILInterlockedDecrement_Full(dest)	_ILInterlockedDecrement((dest))
#endif /* !defined(IL_HAVE_INTERLOCKED_DECREMENT_FULL) */

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
#if defined(IL_HAVE_INTERLOCKED_AND_FULL)
#if !defined(IL_HAVE_INTERLOCKED_AND_ACQUIRE)
#define ILInterlockedAnd_Acquire(dest, value) \
		ILInterlockedAnd_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_AND_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_AND_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_AND_RELEASE)
#define ILInterlockedAnd_Release(dest, value) \
		ILInterlockedAnd_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_AND_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_AND_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_AND_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_AND)
#if defined(IL_HAVE_INTERLOCKED_AND_ACQUIRE)
#define ILInterlockedAnd(dest, value) \
		ILInterlockedAnd_Acquire((dest), (value))
#define IL_HAVE_INTERLOCKED_AND 1
#elif defined(IL_HAVE_INTERLOCKED_AND_RELEASE)
#define ILInterlockedAnd(dest, value) \
		ILInterlockedAnd_Release((dest), (value))
#define IL_HAVE_INTERLOCKED_AND 1
#elif defined(IL_HAVE_INTERLOCKED_AND_FULL)
#define ILInterlockedAnd(dest, value) \
		ILInterlockedAnd_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_AND 1
#endif /* defined(IL_HAVE_INTERLOCKED_AND_FULL) */
#endif /* !defined(IL_HAVE_INTERLOCKED_AND) */
#if !defined(IL_HAVE_INTERLOCKED_AND) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE)
static IL_INLINE void ILInterlockedAnd(volatile ILInt32 *dest,
									   ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval & value;
	} while(ILInterlockedCompareAndExchange(dest, retval, oldval) != oldVal);
}
#define IL_HAVE_INTERLOCKED_AND 1
#endif /* !defined(IL_HAVE_INTERLOCKED_AND) */
#if !defined(IL_HAVE_INTERLOCKED_AND_ACQUIRE) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE)
static IL_INLINE void ILInterlockedAnd_Acquire(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval & value;
	} while(ILInterlockedCompareAndExchange_Acquire(dest, retval, oldval) != oldval);
}
#define IL_HAVE_INTERLOCKED_AND_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_AND_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_AND_RELEASE) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE)
static IL_INLINE void ILInterlockedAnd_Release(volatile ILInt32 *dest,
											   ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval & value;
	} while(ILInterlockedCompareAndExchange_Release(dest, retval, oldval) != oldval);
}
#define IL_HAVE_INTERLOCKED_AND_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_AND_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_AND_FULL) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL)
static IL_INLINE void ILInterlockedAnd_Full(volatile ILInt32 *dest,
											ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval & value;
	} while(ILInterlockedCompareAndExchange_Full(dest, retval, oldval) != oldval);
}
#define IL_HAVE_INTERLOCKED_AND_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_AND_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_AND)
#define ILInterlockedAnd(dest, value)	_ILInterlockedAnd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_AND) */
#if !defined(IL_HAVE_INTERLOCKED_AND_ACQUIRE)
#define ILInterlockedAnd_Acquire(dest, value)	_ILInterlockedAnd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_AND_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_AND_RELEASE)
#define ILInterlockedAnd_Release(dest, value)	_ILInterlockedAnd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_AND_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_AND_FULL)
#define ILInterlockedAnd_Full(dest, value)	_ILInterlockedAnd((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_AND_FULL) */

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
#if defined(IL_HAVE_INTERLOCKED_OR_FULL)
#if !defined(IL_HAVE_INTERLOCKED_OR_ACQUIRE)
#define ILInterlockedOr_Acquire(dest, value) \
		ILInterlockedOr_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_OR_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_OR_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_OR_RELEASE)
#define ILInterlockedOr_Release(dest, value) \
		ILInterlockedOr_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_OR_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_OR_RELEASE) */
#endif /* defined(IL_HAVE_INTERLOCKED_OR_FULL) */
#if !defined(IL_HAVE_INTERLOCKED_OR)
#if defined(IL_HAVE_INTERLOCKED_OR_ACQUIRE)
#define ILInterlockedOr(dest, value) \
		ILInterlockedOr_Acquire((dest), (value))
#define IL_HAVE_INTERLOCKED_OR 1
#elif defined(IL_HAVE_INTERLOCKED_OR_RELEASE)
#define ILInterlockedOr(dest, value) \
		ILInterlockedOr_Release((dest), (value))
#define IL_HAVE_INTERLOCKED_OR 1
#elif defined(IL_HAVE_INTERLOCKED_OR_FULL)
#define ILInterlockedOr(dest, value) \
		ILInterlockedOr_Full((dest), (value))
#define IL_HAVE_INTERLOCKED_OR 1
#endif /* defined(IL_HAVE_INTERLOCKED_OR_FULL) */
#endif /* !defined(IL_HAVE_INTERLOCKED_OR) */
#if !defined(IL_HAVE_INTERLOCKED_OR) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE)
static IL_INLINE void ILInterlockedOr(volatile ILInt32 *dest,
									  ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval | value;
	} while(ILInterlockedCompareAndExchange(dest, retval, oldval) != oldVal);
}
#define IL_HAVE_INTERLOCKED_OR 1
#endif /* !defined(IL_HAVE_INTERLOCKED_OR) */
#if !defined(IL_HAVE_INTERLOCKED_OR_ACQUIRE) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_ACQUIRE)
static IL_INLINE void ILInterlockedOr_Acquire(volatile ILInt32 *dest,
											  ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval | value;
	} while(ILInterlockedCompareAndExchange_Acquire(dest, retval, oldval) != oldval);
}
#define IL_HAVE_INTERLOCKED_OR_ACQUIRE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_OR_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_OR_RELEASE) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_RELEASE)
static IL_INLINE void ILInterlockedOr_Release(volatile ILInt32 *dest,
											  ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval | value;
	} while(ILInterlockedCompareAndExchange_Release(dest, retval, oldval) != oldval);
}
#define IL_HAVE_INTERLOCKED_OR_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_OR_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_OR_FULL) && \
	defined(IL_HAVE_INTERLOCKED_COMPAREANDEXCHANGE_FULL)
static IL_INLINE void ILInterlockedOr_Full(volatile ILInt32 *dest,
										   ILInt32 value)
{
	ILInt32 oldval;
	ILInt32 retval;

	do
	{
		oldval = ILInterlockedLoad(dest);
		retval = oldval | value;
	} while(ILInterlockedCompareAndExchange_Full(dest, retval, oldval) != oldval);
}
#define IL_HAVE_INTERLOCKED_OR_RELEASE 1
#endif /* !defined(IL_HAVE_INTERLOCKED_OR_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_OR)
#define ILInterlockedOr(dest, value)	_ILInterlockedOr((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_OR) */
#if !defined(IL_HAVE_INTERLOCKED_OR_ACQUIRE)
#define ILInterlockedOr_Acquire(dest, value)	_ILInterlockedOr((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_OR_ACQUIRE) */
#if !defined(IL_HAVE_INTERLOCKED_OR_RELEASE)
#define ILInterlockedOr_Release(dest, value)	_ILInterlockedOr((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_OR_RELEASE) */
#if !defined(IL_HAVE_INTERLOCKED_OR_FULL)
#define ILInterlockedOr_Full(dest, value)	_ILInterlockedOr((dest), (value))
#endif /* !defined(IL_HAVE_INTERLOCKED_OR_FULL) */
