/*
 * lib_object.c - Internalcall methods for "System.Object".
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * Authors:  Thong Nguyen (tum@veridicus.com)
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

#include "engine.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILExecMonitor *ILExecMonitorCreate(void)
{
	ILExecMonitor *monitor;

	/* Allocate memory for the ILExecMonitor */
	if((monitor = (ILExecMonitor *)ILMalloc(sizeof(ILExecMonitor))) == 0)
	{
		return 0;
	}

	/* Allocate memory for the wait monitor */
	if((monitor->supportMonitor = ILWaitMonitorCreate()) == 0)
	{
		return 0;
	}

	monitor->waiters = 0;
	monitor->next = 0;

	return monitor;
}

void ILExecMonitorDestroy(ILExecMonitor *monitor)
{
	ILWaitHandleClose(monitor->supportMonitor);

	ILFree(monitor);
}

#ifdef USE_HASHING_MONITORS

#define DEFAULT_HASHTABLE_SIZE		(523)

/*
 *	An entry in the monitor Hashtable.
 */
typedef struct _tagILMonitorEntry ILMonitorEntry;
struct _tagILMonitorEntry
{
	ILObject *obj;
	ILLockWord lockWord;
	ILMonitorEntry *next;
};

/*
 *	Gets a pointer to the WaitHandle object used by the object.
 */
static ILLockWord *GetObjectLockWordPtr(ILExecThread *thread, ILObject *obj)
{
	ILInt32 x;
	ILMonitorEntry **table, *entry, *ghost;

	ILMutexLock(thread->process->monitorSystemLock);

	table = thread->process->monitorHash;

	if (table == 0)
	{
		/* Create the monitor hashtable */

		table = (ILMonitorEntry **)ILGCAlloc(sizeof(ILMonitorEntry *)
																* DEFAULT_HASHTABLE_SIZE);

		if (table == 0)
		{
			ILExecThreadThrowOutOfMemory(thread);

			ILMutexUnlock(thread->process->monitorSystemLock);

			return 0;
		}

		thread->process->monitorHash = table;
	}

	/* Get the hashtable index */

	x = (ILInt32)obj;
	x = ((x >> 16) ^ (x & 0xffff)) % DEFAULT_HASHTABLE_SIZE;
	
	entry = table[x];
	
	if (entry == 0)
	{
		/* The bucket is empty */
		/* Start a new bucket by creating a new hashtable entry */

		if ((entry = (ILMonitorEntry *)ILGCAlloc(sizeof(ILMonitorEntry))) == 0)
		{
			ILExecThreadThrowOutOfMemory(thread);

			ILMutexUnlock(thread->process->monitorSystemLock);

			return 0;
		}

		entry->lockWord = 0;
		entry->obj = obj;
		entry->next = 0;
		table[x] = entry;

		ILGCRegisterGeneralWeak(&entry->obj, obj);

		ILMutexUnlock(thread->process->monitorSystemLock);

		return &entry->lockWord;
	}
	else
	{
		/* The bucket isn't empty.  Search for the right entry. */

		ghost = 0;

		for (;;)
		{
			if (entry->obj == obj)
			{
				/* Entry was found.  Return it */

				ILMutexUnlock(thread->process->monitorSystemLock);

				return &entry->lockWord;
			}
			else if (entry->obj == 0)
			{
				/* Found an entry pointing to a dead object */

				if (ghost == 0)
				{
					/* The ghost will be GCed so no need to free it if we don't 
										end up using it */

					ghost = entry;

					ghost->obj = obj;
					ghost->next =	 0;					
				}
			}
			
			entry = entry->next;

			if (entry == 0)
			{
				/* No entry was found */

				if (ghost == 0)
				{					
					/* No ghost found.  Create a whole new entry. */

					if ((entry = (ILMonitorEntry *)ILGCAlloc(sizeof(ILMonitorEntry))) == 0)
					{
						ILExecThreadThrowOutOfMemory(thread);

						ILMutexUnlock(thread->process->monitorSystemLock);

						return 0;
					}

					entry->lockWord = 0;
				}
				else
				{
					/* We found a ghost!  Use the ghost */

					entry = ghost;
				}

				/* Setup the new entry */

				entry->obj = obj;
				entry->next = table[x];
				table[x] = entry;

				/* Make the entry's obj pointer weak and let it be set to 0 
								if the object is collected */
				ILGCRegisterGeneralWeak(&entry->obj, obj);

				ILMutexUnlock(thread->process->monitorSystemLock);

				return &entry->lockWord;
			}
		}
	}

	ILMutexUnlock(thread->process->monitorSystemLock);
	
	return 0;
}

/*
 *	Implementation of GetObjectLockWord using hashtables.
 */
ILLockWord	GetObjectLockWord(ILExecThread *thread, ILObject *obj)
{
	return *GetObjectLockWordPtr(thread, obj);
}

/*
 *	Implementation of CompareAndExchangeObjectLockWord using hashtables.
 */
ILLockWord CompareAndExchangeObjectLockWord(ILExecThread *thread, 
							ILObject *obj, ILLockWord value, ILLockWord comparand)
{
	ILLockWord oldValue;
	ILLockWord *lockWordPtr;

	/* Lockdown the monitor hashtable */
	ILMutexLock(thread->process->monitorSystemLock);
	
	lockWordPtr = GetObjectLockWordPtr(thread, obj);

	/* Do the compare & exchange */
	if ((oldValue = *lockWordPtr ) == comparand)
	{
		*lockWordPtr  = value;
	}

	/* Unlock the monitor hashtable */
	ILMutexUnlock(thread->process->monitorSystemLock);

	return oldValue;
}

#endif

#ifdef	__cplusplus
};
#endif
