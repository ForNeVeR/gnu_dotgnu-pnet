/*
 * box.c - Box and unbox operations.
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

#include "engine.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILObject *ILExecThreadBox(ILExecThread *thread, ILType *type, void *ptr)
{
	ILClass *classInfo;
	ILObject *object;
	ILUInt32 typeSize;
	if(ILType_IsPrimitive(type) || ILType_IsValueType(type))
	{
		classInfo = ILClassFromType
			(ILContextNextImage(thread->process->context, 0),
			 0, type, 0);
		if(!classInfo)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		typeSize = ILSizeOfType(thread, type);
		object = (ILObject *)_ILEngineAlloc(thread, classInfo, typeSize);
		if(object)
		{
			ILMemCpy(object, ptr, typeSize);
		}
		return object;
	}
	else
	{
		return 0;
	}
}

ILObject *ILExecThreadBoxFloat(ILExecThread *thread, ILType *type, void *ptr)
{
	ILClass *classInfo;
	ILObject *object;
	ILUInt32 typeSize;
	if(ILType_IsPrimitive(type) || ILType_IsValueType(type))
	{
		classInfo = ILClassFromType
			(ILContextNextImage(thread->process->context, 0),
			 0, type, 0);
		if(!classInfo)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		typeSize = ILSizeOfType(thread, type);
		object = (ILObject *)_ILEngineAlloc(thread, classInfo, typeSize);
		if(object)
		{
			if(type == ILType_Float32)
			{
				*((ILFloat *)object) = (ILFloat)(*((ILNativeFloat *)ptr));
			}
			else if(type == ILType_Float64)
			{
				*((ILDouble *)object) = (ILDouble)(*((ILNativeFloat *)ptr));
			}
			else
			{
				ILMemCpy(object, ptr, typeSize);
			}
		}
		return object;
	}
	else
	{
		return 0;
	}
}

int ILExecThreadUnbox(ILExecThread *thread, ILType *type,
					  ILObject *object, void *ptr)
{
	if(object && ptr &&
	   (ILType_IsPrimitive(type) || ILType_IsValueType(type)) &&
	   ILTypeIdentical(type, ILClassToType(GetObjectClass(object))))
	{
		ILMemCpy(ptr, object, ILSizeOfType(thread, type));
		return 1;
	}
	else
	{
		return 0;
	}
}

int ILExecThreadUnboxFloat(ILExecThread *thread, ILType *type,
					       ILObject *object, void *ptr)
{
	if(object && ptr &&
	   (ILType_IsPrimitive(type) || ILType_IsValueType(type)) &&
	   ILTypeIdentical(type, ILClassToType(GetObjectClass(object))))
	{
		if(type == ILType_Float32)
		{
			*((ILNativeFloat *)ptr) = (ILNativeFloat)(*((ILFloat *)object));
		}
		else if(type == ILType_Float64)
		{
			*((ILNativeFloat *)ptr) = (ILNativeFloat)(*((ILDouble *)object));
		}
		else
		{
			ILMemCpy(ptr, object, ILSizeOfType(thread, type));
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif
