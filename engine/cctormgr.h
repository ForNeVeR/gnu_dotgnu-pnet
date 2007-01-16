/*
 * cctormgr.h - Header file for the cctor manager.
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

#ifndef	_ENGINE_CCTORMGR_H
#define	_ENGINE_CCTORMGR_H

#include "engine_private.h"


#ifdef	__cplusplus
extern	"C" {
#endif

/* Forward declarations. */
typedef struct _tagILClassEntry	ILClassEntry;
typedef struct _tagILCCtorMgr	ILCCtorMgr;

struct _tagILClassEntry
{
	ILClass		   *classInfo;	/* The class for which the cctor has to be executed. */
	ILClassEntry   *prevClass;	/* The previous class entry. */
};

struct _tagILCCtorMgr
{
	/* Mutex to synchronize the cctor execution. */
	ILMutex		   *lock;
	
	/* The thread holding the lock. */
	ILThread	   *thread;

	/* The currently compiled method. */
	ILMethod	   *currentMethod;
	/* Flag if the current method is a cctor. */
	ILInt32			isStaticConstructor;

	/* Pool for the class entries. */
	ILMemPool		classPool;
	/* The last classentry  in the memory pool. */
	ILClassEntry   *lastClass;

};

int ILCCtorMgr_Init(ILCCtorMgr *cctorMgr,
						   ILInt32 numCCtorEntries);

void ILCCtorMgr_Destroy(ILCCtorMgr *cctorMgr);

/*
 * Call this method before any non virtual method call is done.
 * It checks if the static constructor for the method owner has to be invoked
 * before the call is done.
 */
int ILCCtorMgr_OnCallMethod(ILCCtorMgr *cctorMgr,
							ILMethod *method);
/*
 * Call this method before accessing any static field.
 * It checks if the static constructor for the field's owner has to be invoked
 * before the field is accessed.
 */
int ILCCtorMgr_OnStaticFieldAccess(ILCCtorMgr *cctorMgr,
								   ILField *field);

/*
 * Run the cctor for the given class.
 * Returns != 0 on success or 0 if an exception was thrown.
 */
int ILCCtorMgr_RunCCtor(ILCCtorMgr *cctorMgr, ILClass *classInfo);

/*
 * Run the queued cctors.
 * Returns != 0 on success or 0 if an exception was thrown.
 */
int ILCCtorMgr_RunCCtors(ILCCtorMgr *cctorMgr);

#ifdef	__cplusplus
};
#endif

#endif	/* !_ENGINE_CCTORMGR_H */

