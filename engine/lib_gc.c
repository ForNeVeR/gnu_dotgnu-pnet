/*
 * lib_gc.c - Internalcall methods for the classes "System.GC" and
 *					"System.Runtime.InteropServices.GCHandle".
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#include "engine_private.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static void KeepAlive(Object obj);
 */
void _IL_GC_KeepAlive(ILExecThread *_thread, ILObject *obj)
{
	/* This internalcall exists purely to ensure that the object
	   stays alive until this method is called, by keeping it
	   somewhere on the current thread's stack.  In other words,
	   once this method is called, we have nothing to do */
}

/*
 * public static void ReRegisterForFinalize(Object obj);
 */
void _IL_GC_ReRegisterForFinalize(ILExecThread *_thread, ILObject *obj)
{
	if(obj)
	{
   		if(((ILClassPrivate *)((GetObjectClass(obj))->userData))
				->hasFinalizer)
		{
			ILGCRegisterFinalizer
				(GetObjectGcBase(obj),
				 _ILFinalizeObject, (void *)0);
		}
	}
	else
	{
		ILExecThreadThrowArgNull(_thread, "obj");
	}
}

/*
 * public static void SuppressFinalize(Object obj);
 */
void _IL_GC_SuppressFinalize(ILExecThread *_thread, ILObject *obj)
{
	if(obj)
	{
		ILGCRegisterFinalizer
			(GetObjectGcBase(obj), (ILGCFinalizer)0, (void *)0);
	}
	else
	{
		ILExecThreadThrowArgNull(_thread, "obj");
	}
}

/*
 * public static void WaitForPendingFinalizers();
 */
void _IL_GC_WaitForPendingFinalizers(ILExecThread *_thread)
{	
	_ILExecThreadHandleWaitResult(_thread, ILGCInvokeFinalizers(-1));
}

/*
 * public static void Collect();
 */
void _IL_GC_Collect(ILExecThread *_thread)
{
	ILGCCollect();
}

/*
 * public static long GetTotalMemory();
 */
ILInt64 _IL_GC_GetTotalMemory(ILExecThread *_thread,
							  ILBool forceFullCollection)
{
	if(forceFullCollection)
	{
		ILGCCollect();
	}
	return (ILInt64)ILGCGetHeapSize();
}

#ifdef IL_CONFIG_RUNTIME_INFRA

/*
 * Handle types.  This list must be kept in sync with
 * System.Runtime.InteropServices.GCHandleType.
 */
#define	GCHandleType_Weak					0
#define	GCHandleType_WeakTrackResurrection	1
#define	GCHandleType_Normal					2
#define	GCHandleType_Pinned					3

/*
 * Structure of the GC handle table.
 */
typedef struct _tagILGCHandleTable
{
	/* Lock to serialize access to this object */
	ILMutex        *lock;

	void **regularHandles;
	ILInt32 numRegularHandles;
	void **weakHandles;
	ILInt32 numWeakHandles;

} ILGCHandleTable;

static void _ILGCHandleTableInit(ILGCHandleTable *table)
{
	/* Initialize all members even if that might be not needed. */
	table->regularHandles = 0;
	table->numRegularHandles = 0;
	table->weakHandles = 0;
	table->numWeakHandles = 0;

	/* Initialize the object lock */
	table->lock = ILMutexCreate();
}

/*
 * Get the GC handle table and lock it down.
 */
static ILGCHandleTable *GetGCHandleTable(ILExecThread *_thread)
{
	ILExecProcess *process = _thread->process;
	if(!(process->gcHandles))
	{
		ILMutexLock(process->lock);
		/* Check again because of race conditions. */
		if(!(process->gcHandles))
		{
			/* Disable finalizers here because we hold the process lock.
			 * Otherwise we might get a deadlock if finalizers are invoked
			 * the first time and the finalizer thread is created.
			 */
			ILGCDisableFinalizers(-1);
			process->gcHandles = (ILGCHandleTable *)
				ILGCAllocPersistent(sizeof(ILGCHandleTable));
			ILGCEnableFinalizers();
			if(process->gcHandles)
			{
				_ILGCHandleTableInit(process->gcHandles);
				ILMutexLock(process->gcHandles->lock);
			}
		}
		else
		{
			ILMutexLock(process->gcHandles->lock);
		}
		ILMutexUnlock(process->lock);
	}
	else
	{
		ILMutexLock(process->gcHandles->lock);
	}
	return process->gcHandles;
}

/*
 * Unlock the GC handle table.
 */
static void UnlockGCHandleTable(ILGCHandleTable *table)
{
	ILMutexUnlock(table->lock);
}

ILNativeInt _IL_GCHandle_GCAddrOfPinnedObject(ILExecThread *_thread,
											  ILInt32 handle)
{
	ILGCHandleTable *table;
	void *ptr;
	ILObject *object = 0;

	/* Get the handle table and look up the handle.  We assume that
	   the caller has already checked that the handle is pinned */
	handle >>= 2;
	table = GetGCHandleTable(_thread);
	if(table)
	{
		if(handle > 0 && handle <= table->numRegularHandles)
		{
			ptr = table->regularHandles[handle - 1];
			if(ptr == ((void *)(ILNativeInt)(-1)) || !ptr)
			{
				object = 0;
			}
			else
			{
				object = GetObjectFromGcBase(ptr);
				if(_ILIsSArray((System_Array *)object))
				{
					object = ((void *)(((System_Array *)(object)) + 1));
				}
			}
		}
		/* Unlock the handle table */
		UnlockGCHandleTable(table);
	}

	/* None of the objects in our implementation move about
	   memory, so the object pointer is the pinned address */
	return (ILNativeInt)(void *)object;
}

ILInt32 _IL_GCHandle_GCAlloc(ILExecThread *_thread, ILObject *value,
							 ILInt32 type)
{
	ILGCHandleTable *table;
	void *ptr;
	ILInt32 handle = 0;
	void **newArray;
	ILInt32 index;

	/* Convert the object into a pointer to the object's GC base */
	if(value)
	{
		ptr = GetObjectGcBase(value);
	}
	else
	{
		ptr = 0;
	}

	/* Get the GC handle table */
	table = GetGCHandleTable(_thread);
	if(table)
	{
		/* Allocate a new handle slot */
		switch(type)
		{
			case GCHandleType_Weak:
			case GCHandleType_WeakTrackResurrection:
			{
				for( index = 0; index < table->numWeakHandles; index++ ) {
					if(table->weakHandles[index] == (void *)(ILNativeInt)(-1)) {
						table->weakHandles[index] = ptr;
						if( 0 != table->weakHandles[index] ) {
							ILGCRegisterGeneralWeak(&(table->weakHandles[index]), table->weakHandles[index] );
						}
						handle = (((++(index)) << 2) | type);
						break;
					}
				}
				if( handle != 0 ) break;
				
				if((table->numWeakHandles & 7) == 0)
				{
					/* Extend the size of the weak handle table */
					/* for weak refs, do use AllocAtomic so that the ref will
					 * not be scanned.by the gc during collection.
 					 */
					newArray = (void **)ILGCAllocAtomic
						((table->numWeakHandles + 8) * sizeof(void *));
					if(!newArray)
					{
						ILExecThreadThrowOutOfMemory(_thread);
						break;
					}
					if(table->weakHandles != 0)
					{
						for(index = 0; index < table->numWeakHandles; ++index)
						{
							newArray[index] = table->weakHandles[index];
							if(table->weakHandles[index] !=
									(void *)(ILNativeInt)(-1))
							{
								/* use RegisterGeneralWeak to monitor 
								 * disappearing links
								 */
								if( 0 != table->weakHandles[index] ) {
									ILGCUnregisterWeak(&(table->weakHandles[index]));
								}
								if( 0 != newArray[index] ) {
									ILGCRegisterGeneralWeak(&(newArray[index]), newArray[index] ); 
								}
							}
						}
						ILGCFreePersistent(table->weakHandles);
					}
					table->weakHandles = newArray;
				}
				table->weakHandles[table->numWeakHandles] = ptr;
				
				if( 0 != table->weakHandles[table->numWeakHandles] ) {
					ILGCRegisterGeneralWeak(&(table->weakHandles[table->numWeakHandles]), table->weakHandles[table->numWeakHandles] );
				}
				handle = (((++(table->numWeakHandles)) << 2) | type);
			}
			break;

			case GCHandleType_Normal:
			case GCHandleType_Pinned:
			{
				for( index = 0; index < table->numRegularHandles; index++ ) {
					if(table->regularHandles[index] == (void *)(ILNativeInt)(-1)) {
						table->regularHandles[index] = ptr;
						handle = (((++(index)) << 2) | type);
						break;
					}
				}
				if( handle != 0 ) break;
				
				if((table->numRegularHandles & 7) == 0)
				{
					/* Extend the size of the regular handle table */
					newArray = (void **)ILGCAllocPersistent
						((table->numRegularHandles + 8) * sizeof(void *));
					if(!newArray)
					{
						ILExecThreadThrowOutOfMemory(_thread);
						break;
					}
					if(table->regularHandles != 0)
					{
						ILMemCpy(newArray, table->regularHandles,
								 table->numRegularHandles * sizeof(void *));
						ILGCFreePersistent(table->regularHandles);
					}
					table->regularHandles = newArray;
				}
				table->regularHandles[table->numRegularHandles] = ptr;
				handle = (((++(table->numRegularHandles)) << 2) | type);
			}
			break;
		}
		/* Unlock the GC handle table and return */
		UnlockGCHandleTable(table);
	}

	return handle;
}

void _IL_GCHandle_GCFree(ILExecThread *_thread, ILInt32 handle)
{
	ILGCHandleTable *table;
	ILInt32 index = (handle >> 2);

	/* Get the GC handle table */
	table = GetGCHandleTable(_thread);
	if(table)
	{
		switch(handle & 3)
		{
			case GCHandleType_Weak:
			case GCHandleType_WeakTrackResurrection:
			{
				if(index > 0 && index <= table->numWeakHandles)
				{
					--index;
					if(table->weakHandles[index] != (void *)(ILNativeInt)(-1))
					{
						ILGCUnregisterWeak(&(table->weakHandles[index]));
						table->weakHandles[index] = (void *)(ILNativeInt)(-1);
					}
				}
			}
			break;

			case GCHandleType_Normal:
			case GCHandleType_Pinned:
			{
				if(index > 0 && index <= table->numRegularHandles)
				{
					table->regularHandles[index - 1]
						= (void *)(ILNativeInt)(-1);
				}
			}
			break;
		}
		/* Unlock the GC handle table and return */
		UnlockGCHandleTable(table);
	}
}

ILBool _IL_GCHandle_GCValidate(ILExecThread *_thread, ILInt32 handle)
{
	ILGCHandleTable *table;
	ILBool valid = 0;
	ILInt32 index = (handle >> 2);

	/* Get the GC handle table */
	table = GetGCHandleTable(_thread);
	if(table)
	{
		switch(handle & 3)
		{
			case GCHandleType_Weak:
			case GCHandleType_WeakTrackResurrection:
			{
				valid = (index > 0 && index <= table->numWeakHandles);
			}
			break;

			case GCHandleType_Normal:
			case GCHandleType_Pinned:
			{
				valid = (index > 0 && index <= table->numRegularHandles);
			}
			break;
		}
		/* Unlock the handle table and return */
		UnlockGCHandleTable(table);
	}

	return valid;
}

ILObject *_IL_GCHandle_GCGetTarget(ILExecThread *_thread, ILInt32 handle)
{
	ILGCHandleTable *table;
	void *ptr;
	ILObject *object = 0;
	ILInt32 index = (handle >> 2);

	/* Get the handle table and look up the handle */
	table = GetGCHandleTable(_thread);
	if(table)
	{
		ptr = 0;
		switch(handle & 3)
		{
			case GCHandleType_Weak:
			case GCHandleType_WeakTrackResurrection:
			{
				if(index > 0 && index <= table->numWeakHandles)
				{
					ptr = table->weakHandles[index - 1];
				}
			}
			break;

			case GCHandleType_Normal:
			case GCHandleType_Pinned:
			{
				if(index > 0 && index <= table->numRegularHandles)
				{
					ptr = table->regularHandles[index - 1];
				}
			}
			break;
		}
		if(ptr == ((void *)(ILNativeInt)(-1)) || !ptr)
		{
			object = 0;
		}
		else
		{
			object = GetObjectFromGcBase(ptr);
		}
		/* Unlock the handle table and return */
		UnlockGCHandleTable(table);
	}

	return object;
}

void _IL_GCHandle_GCSetTarget(ILExecThread *_thread, ILInt32 handle,
							  ILObject *value)
{
	ILGCHandleTable *table;
	void *ptr;
	ILInt32 index = (handle >> 2);

	/* Convert the object into a pointer to the object's GC base */
	if(value)
	{
		ptr = GetObjectGcBase(value);
	}
	else
	{
		ptr = 0;
	}

	/* Get the handle table and look up the handle */
	table = GetGCHandleTable(_thread);
	if(table)
	{
		switch(handle & 3)
		{
			case GCHandleType_Weak:
			case GCHandleType_WeakTrackResurrection:
			{
				if(index > 0 && index <= table->numWeakHandles)
				{
					if(table->weakHandles[index - 1] !=
							(void *)(ILNativeInt)(-1))
					{
						/* unregister old Target and Register new target */
						if( 0 != table->weakHandles[index - 1] )
						{
							ILGCUnregisterWeak(&(table->weakHandles[index-1]));
						}
						
						table->weakHandles[index - 1] = ptr;
						
						if( 0 != ptr )
						{
							ILGCRegisterGeneralWeak(&(table->weakHandles[index-1]), table->weakHandles[index-1] ); 
						}
					}
				}
			}
			break;

			case GCHandleType_Normal:
			case GCHandleType_Pinned:
			{
				if(index > 0 && index <= table->numRegularHandles)
				{
					if(table->regularHandles[index - 1] !=
							(void *)(ILNativeInt)(-1))
					{
						table->regularHandles[index - 1] = ptr;
					}
				}
			}
			break;
		}
		/* Unlock the handle table and return */
		UnlockGCHandleTable(table);
	}
}

void _ILGCHandleTableFree(ILGCHandleTable *table)
{
	if(table->regularHandles)
	{
		ILGCFreePersistent(table->regularHandles);
	}
	if(table->weakHandles)
	{
		ILInt32 index;
		for(index = 0; index < table->numWeakHandles; ++index)
		{
			ILGCUnregisterWeak(&(table->weakHandles[index]));
		}
	}

	if(table->lock)
	{
		/* Destroy the object lock */
		ILMutexDestroy(table->lock);
	}
	ILGCFreePersistent(table);
}

#endif /* IL_CONFIG_RUNTIME_INFRA */

#ifdef	__cplusplus
};
#endif
