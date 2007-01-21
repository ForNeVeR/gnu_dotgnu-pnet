/*
 * cctormgr.c - Queue and execute class initializers (static constructors).
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

#include "cctormgr.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Reenable finalizers, unlock the metadata lock and run finalizers.
 * Must be kept in sync with convert.c
 */
#define	METADATA_UNLOCK(thread)	\
			do { \
				ILGCEnableFinalizers(); \
				IL_METADATA_UNLOCK(_ILExecThreadProcess(thread)); \
				ILGCInvokeFinalizers(0); \
			} while (0)
/*
 * Get the number of queued cctors.
 */
static ILInt32 _ILCCtorMgr_GetNumQueuedCCtors(ILCCtorMgr *cctorMgr)
{
	ILInt32 numQueuedCCtors = 0;
	ILClassEntry *classEntry = cctorMgr->lastClass;

	while(classEntry)
	{
		numQueuedCCtors++;
		classEntry = classEntry->prevClass;
	}
	return numQueuedCCtors;
}

/*
 * Execute the cctor of the given class.
 * This function must be executed with the current thread holding the
 * cctor manager's lock.
 */
static int _ILCCtorMgr_RunCCtor(ILCCtorMgr *cctorMgr,
								ILExecThread *thread,
								ILClass *classInfo)
{
	if((classInfo->attributes &
		(IL_META_TYPEDEF_CCTOR_RUNNING | IL_META_TYPEDEF_CCTOR_ONCE)) == 0)
	{
		/* The cctor was not allready executed and is not currently running. */
		/* So find the cctor now. */
		ILMethod *cctor = 0;

		IL_METADATA_RDLOCK(_ILExecThreadProcess(thread));
		while((cctor = (ILMethod *)ILClassNextMemberByKind(classInfo,
														  (ILMember *)cctor,
														  IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if(ILMethod_IsStaticConstructor(cctor))
			{
				break;
			}
		}
		IL_METADATA_UNLOCK(_ILExecThreadProcess(thread));
		if(cctor)
		{
			/* We found the cctor so we can execute it now. */
			classInfo->attributes |= IL_META_TYPEDEF_CCTOR_RUNNING;
			if(ILExecThreadCall(thread, cctor, 0))
			{
				classInfo->attributes &= ~IL_META_TYPEDEF_CCTOR_RUNNING;
				return 0;
			}
		}
		classInfo->attributes |= IL_META_TYPEDEF_CCTOR_ONCE;
	}
	return 1;
}

/*
 * Execute the queued cctors.
 * This function must be executed with the current thread holding the
 * cctor manager's lock.
 */
static int _ILCCtorMgr_RunCCtors(ILCCtorMgr *cctorMgr,
								 ILExecThread *thread,
								 ILClass *classes[],
								 ILInt32 numClasses)
{
	ILInt32 currentCCtor;

	/* And run the cctors now. */
	for(currentCCtor = 0; currentCCtor < numClasses; currentCCtor++)
	{
		if(!(_ILCCtorMgr_RunCCtor(cctorMgr,
								  thread,
								  classes[currentCCtor])))
		{
			return 0;
		}
	}
	return 1;
}

/*
 * Lookup the class entry for the given class in the cctor manager and return
 * the pointer to the class entry or add a new class entry and return the
 * pointer to the newly created entry.
 * Returns 0 on out of memory.
 */
static ILClassEntry *ILCCtorMgr_FindOrAddClass(ILCCtorMgr *cctorMgr,
											   ILClass *classInfo)
{
	ILClassEntry *currentClass = cctorMgr->lastClass;

	while(currentClass)
	{
		if(currentClass->classInfo == classInfo)
		{
			return currentClass;
		}
		currentClass = currentClass->prevClass;
	}

	/* Add the new class to the queue. */
	if(!(currentClass = ILMemPoolAlloc(&(cctorMgr->classPool),
									   ILClassEntry)))
	{
		return 0;
	}
	/* Now fill the class entry with the current class. */
	currentClass->classInfo = classInfo;
	currentClass->prevClass = cctorMgr->lastClass;
	cctorMgr->lastClass = currentClass;

	return currentClass;
}

/*
 * Add a class to the queue of the classes which have to be initialized.
 */
static int _ILCCtorMgr_QueueClass(ILCCtorMgr *cctorMgr,
						  ILClass *classInfo)
{
	ILMethod *cctor = 0;
	ILClassEntry *currentClass = 0;

	if(!cctorMgr)
	{
		return 0;
	}

	if(!classInfo)
	{
		return 0;
	}

	if((classInfo->attributes & IL_META_TYPEDEF_CCTOR_ONCE) != 0)
	{
		/* We already know that the static constructor has been executed, */
		return 1;
	}

	while((cctor = (ILMethod *)ILClassNextMemberByKind(classInfo,
													  (ILMember *)cctor,
													  IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if(ILMethod_IsStaticConstructor(cctor))
		{
			break;
		}
	}
	if(cctor == 0)
	{
		/* We couldn't find a cctor. */
		/* So we flag the class as if the cctor was executed. */
		classInfo->attributes |= IL_META_TYPEDEF_CCTOR_ONCE;
		return 1;
	}

	/* Check if the class is allready in the queue or add it. */
	if(!(currentClass = ILCCtorMgr_FindOrAddClass(cctorMgr, classInfo)))
	{
		return 0;
	}

	/* And exit with success. */
	return 1;
}

/*
 * Throw a System.TypeInitializationException.
 */
static void _ILCCtorMgr_ThrowTypeInitializationException(ILExecThread *thread)
{
	ILObject *exception = ILExecThreadGetException(thread);

	if(exception)
	{
		ILObject *typeInitializationException = 0;

		ILExecThreadClearException(thread);

		ILExecThreadThrowSystem(thread,
								"System.TypeInitializationException",
								 0);

		typeInitializationException = ILExecThreadGetException(thread);
		if(typeInitializationException)
		{
			/* Set the inner exception thrown by the class initializer. */
			((System_Exception *)typeInitializationException)->innerException = (System_Exception *)exception;
		}
	}
}

/*
 * Initialize a cctor manager instance.
 */
int ILCCtorMgr_Init(ILCCtorMgr *cctorMgr,
						   ILInt32 numCCtorEntries)
{
	if(cctorMgr)
	{
		/* Initialize the lock mutex. */
		if(!(cctorMgr->lock = ILMutexCreate()))
		{
			return 0;
		}
		cctorMgr->thread = (ILThread *)0;;
		cctorMgr->currentMethod = (ILMethod *)0;
		cctorMgr->isStaticConstructor = 0;
		cctorMgr->isConstructor = 0;

		cctorMgr->lastClass = (ILClassEntry *)0;

		/* Intialize the pool for the class infos. */
		ILMemPoolInit(&(cctorMgr->classPool),
					  sizeof(ILClassEntry),
					  numCCtorEntries);

		return 1;
	}
	return 0;
}

/*
 * Destroy a cctor manager instance.
 */
void ILCCtorMgr_Destroy(ILCCtorMgr *cctorMgr)
{
	if(cctorMgr)
	{
		ILMemPoolDestroy(&(cctorMgr->classPool));

		if(cctorMgr->lock)
		{
			/* Destroy the lock mutex. */
			ILMutexDestroy(cctorMgr->lock);
		}
	}
}

/*
 * Set the current method to be compiled.
 * This checks if the class initializer of the class owning the method has
 * to be executed prior to executing the method.
 */
void ILCCtorMgr_SetCurrentMethod(ILCCtorMgr *cctorMgr,
								 ILMethod *method)
{
	if(cctorMgr)
	{
		if(method)
		{
			ILClass *methodOwner = ILMethod_Owner(method);

			/* Setup the information for the current method. */
			cctorMgr->currentMethod = method;
			if(ILMethod_IsConstructor(method))
			{
				cctorMgr->isConstructor = 1;
				cctorMgr->isStaticConstructor = 0;
			}
			else
			{
				cctorMgr->isConstructor = 0;
				if(ILMethod_IsStaticConstructor(method))
				{
					cctorMgr->isStaticConstructor = 1;
				}
				else
				{
					cctorMgr->isStaticConstructor = 0;
				}
			}

			if((methodOwner->attributes & IL_META_TYPEDEF_CCTOR_ONCE) != 0)
			{
				/* We already know that the static constructor has been called, */
				return;
			}

			/* Now check if the class initializer of the method's owner */
			/* needs to be executed first and queue the class */
			/* if neccessairy. */
			if((methodOwner->attributes & IL_META_TYPEDEF_BEFORE_FIELD_INIT) == 0)
			{
				if(ILMethod_IsStatic(method))
				{
					/* We have to call the cctor before calling any static method */
					/* of this type */
					_ILCCtorMgr_QueueClass(cctorMgr, methodOwner);
					return;
				}
				if(cctorMgr->isConstructor)
				{
					/* We have to call the cctor before calling a */
					/* constructor of this type. */
					_ILCCtorMgr_QueueClass(cctorMgr, methodOwner);
					return;
				}
			}
		}
		else
		{
			cctorMgr->currentMethod = (ILMethod *)0;
			cctorMgr->isStaticConstructor = 0;
			cctorMgr->isConstructor = 0;
		}
	}
}

/*
 * Call this method before any non virtual method call is done.
 * It checks if the static constructor for the method owner has to be invoked
 * before the call is done.
 */
int ILCCtorMgr_OnCallMethod(ILCCtorMgr *cctorMgr,
							ILMethod *method)
{
	ILClass *methodOwner = ILMethod_Owner(method);

	if((methodOwner->attributes & IL_META_TYPEDEF_CCTOR_ONCE) != 0)
	{
		/* We already know that the static constructor has been called, */
		return 1;
	}

	if((methodOwner->attributes & IL_META_TYPEDEF_BEFORE_FIELD_INIT) == 0)
	{
		/* We have to call the cctor before calling any static method */
		/* or constructor of this type. */
		if(ILMethod_IsStatic(method) || ILMethod_IsConstructor(method))
		{
			if(cctorMgr->isStaticConstructor)
			{
				ILClass *cctorOwner = ILMethod_Owner(cctorMgr->currentMethod);

				if(cctorOwner == methodOwner)
				{
					/* We are in the static constructor of the class owning */
					/* the method. So we don't need to call the cctor again. */
					return 1;
				}
			}
			return _ILCCtorMgr_QueueClass(cctorMgr, methodOwner);
		}
	}
	return 1;
}

/*
 * Call this method before accessing any static field.
 * It checks if the static constructor for the field's owner has to be invoked
 * before the field is accessed.
 */
int ILCCtorMgr_OnStaticFieldAccess(ILCCtorMgr *cctorMgr,
								   ILField *field)
{
	ILClass *fieldOwner = ILField_Owner(field);

	if((fieldOwner->attributes & IL_META_TYPEDEF_CCTOR_ONCE) != 0)
	{
		/* We already know that the static constructor has been called, */
		return 1;
	}

	if(ILField_IsStatic(field))
	{
		/* We have to call the cctor before accessing any static field */
		/* of this type */
		if(cctorMgr->isStaticConstructor)
		{
			ILClass *methodOwner = ILMethod_Owner(cctorMgr->currentMethod);

			if(methodOwner == fieldOwner)
			{
				/* We are in the static constructor of the class owning */
				/* the field. So we don't need to call the cctor again. */
				return 1;
			}
		}
		return _ILCCtorMgr_QueueClass(cctorMgr, fieldOwner);
	}
	return 1;
}

/*
 * Run the cctor for the given class.
 */
int ILCCtorMgr_RunCCtor(ILCCtorMgr *cctorMgr, ILClass *classInfo)
{
	if((cctorMgr != 0) && (classInfo != 0))
	{
		if((classInfo->attributes & IL_META_TYPEDEF_CCTOR_ONCE) != 0)
		{
			/* We already know that the static constructor has been called, */
			return 1;
		}
		else
		{
			ILExecThread *thread = ILExecThreadCurrent();
			int result;

			if(cctorMgr->thread == thread->supportThread)
			{
				/* We allready hold the cctor lock. */
				result = _ILCCtorMgr_RunCCtor(cctorMgr,
									 		  thread,
									 		  classInfo);
			}
			else
			{
				/* Lock the cctor manager. */
				ILMutexLock(cctorMgr->lock);

				/* And set the thread holding the lock. */
				cctorMgr->thread = thread->supportThread;

				result = _ILCCtorMgr_RunCCtor(cctorMgr,
									 		  thread,
									 		  classInfo);

				/* reset the thread holding the lock. */
				cctorMgr->thread = (ILThread *)0;
				/* and unlock the cctor manager. */
				ILMutexUnlock(cctorMgr->lock);
			}
			if(!result)
			{
				_ILCCtorMgr_ThrowTypeInitializationException(thread);
			}
			return result;
		}
	}
	return 0;
}

/*
 * Run the queued cctors.
 */
int ILCCtorMgr_RunCCtors(ILCCtorMgr *cctorMgr)
{
	if(cctorMgr)
	{
		ILExecThread *thread = ILExecThreadCurrent();

		if(!(cctorMgr->lastClass))
		{
			/* There are no cctors queued. */
			/* So unlock the metadata. */
			METADATA_UNLOCK(thread);
			/* and return with success. */
			return 1;
		}
		else
		{
			int result;
			ILInt32 numQueuedCCtors = _ILCCtorMgr_GetNumQueuedCCtors(cctorMgr);
			ILClass *classes[numQueuedCCtors];
			ILClassEntry *classEntry = cctorMgr->lastClass;
			ILInt32 currentCCtor;

			/* Copy the cctors to execute to the class array. */
			for(currentCCtor = 0; currentCCtor < numQueuedCCtors; currentCCtor++)
			{
				classes[currentCCtor] = classEntry->classInfo;
				classEntry = classEntry->prevClass;
			}

			/* and reset the queue in the cctor manager. */
			cctorMgr->lastClass = (ILClassEntry *)0;
			ILMemPoolClear(&(cctorMgr->classPool));

			/* Now unlock the metadata. */
			METADATA_UNLOCK(thread);

			if(cctorMgr->thread == thread->supportThread)
			{
				/* We allready hold the cctor lock. */
				result = _ILCCtorMgr_RunCCtors(cctorMgr,
									 		   thread,
									 		   classes,
									 		   numQueuedCCtors);
			}
			else
			{
				/* Lock the cctor manager. */
				ILMutexLock(cctorMgr->lock);

				/* And set the thread holding the lock. */
				cctorMgr->thread = thread->supportThread;

				result = _ILCCtorMgr_RunCCtors(cctorMgr,
									 		   thread,
									 		   classes,
									 		   numQueuedCCtors);

				/* reset the thread holding the lock. */
				cctorMgr->thread = (ILThread *)0;
				/* and unlock the cctor manager. */
				ILMutexUnlock(cctorMgr->lock);
			}
			if(!result)
			{
				_ILCCtorMgr_ThrowTypeInitializationException(thread);
			}
			return result;
		}
	}
	return 0;
}

#ifdef	__cplusplus
};
#endif
