/*
 * lib_object.c - Internalcall methods for "System.Object".
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

#include "engine_private.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Get the private data attached to a class.
 */
#define	IL_CLASS_PRIVATE(classInfo)	\
				((ILClassPrivate *)((classInfo)->userData))

static ILObject *System_Object_GetType(ILExecThread *thread, ILObject *_this)
{
	ILObject *obj;

	/* Does the class already have a "ClrType" instance? */
	if(IL_CLASS_PRIVATE(GetObjectClass(_this))->clrType)
	{
		return IL_CLASS_PRIVATE(GetObjectClass(_this))->clrType;
	}

	/* Create a new "ClrType" instance for the "ILClass" structure */
	obj = _ILGetClrType(thread, GetObjectClass(_this));
	if(!obj)
	{
		return 0;
	}

	/* Return the object to the caller */
	return obj;
}

static ILInt32 System_Object_GetHashCode(ILExecThread *thread,
										 ILObject *_this)
{
	return (ILInt32)_this;
}

static ILBool System_Object_Equals(ILExecThread *thread,
								   ILObject *_this,
								   ILObject *obj)
{
	return (_this == obj);
}

static ILObject *System_Object_MemberwiseClone(ILExecThread *thread,
											   ILObject *_this)
{
	ILObject *obj;

	/* TODO: array cloning */

	/* Allocate a new object of the same class */
	obj = _ILEngineAllocObject(thread, GetObjectClass(_this));
	if(!obj)
	{
		return 0;
	}

	/* Copy the contents of "this" into the new object */
	if(IL_CLASS_PRIVATE(GetObjectClass(_this))->size != 0)
	{
		ILMemCpy(obj, _this, IL_CLASS_PRIVATE(GetObjectClass(_this))->size);
	}

	/* Return the cloned object to the caller */
	return obj;
}

/*
 * Method table for the "System.Object" class.
 */
IL_METHOD_BEGIN(_ILSystemObjectMethods)
	IL_METHOD("GetType",		 "(T)oSystem.Type;", System_Object_GetType)
	IL_METHOD("GetHashCode",	 "(T)i",			 System_Object_GetHashCode)
	IL_METHOD("Equals",			 "(ToSystem/Object;)Z", System_Object_Equals)
	IL_METHOD("MemberwiseClone", "(T)oSystem/Object;",
			  System_Object_MemberwiseClone)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
