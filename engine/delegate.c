/*
 * delegate.c - Delegate handling for the runtime engine.
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

#include "engine.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

int _ILDelegateSignatureMatch(ILClass *delegateClass, ILMethod *method)
{
	ILMethod *invoke;
	ILType *methodSignature;
	ILType *invokeSignature;
	unsigned long numParams;
	unsigned long param;

	/* Constructors can never be used in delegate invocation */
	if(ILMethod_IsConstructor(method) || ILMethod_IsStaticConstructor(method))
	{
		return 0;
	}

	/* Find the delegate invocation method.  This also checks
	   that the class is truly a delegate */
	invoke = ILTypeGetDelegateMethod(ILType_FromClass(delegateClass));
	if(!invoke)
	{
		return 0;
	}

	/* Check that the delegate signatures match */
	methodSignature = ILMethod_Signature(method);
	invokeSignature = ILMethod_Signature(invoke);
	numParams = ILTypeNumParams(methodSignature);
	if(numParams != ILTypeNumParams(invokeSignature))
	{
		return 0;
	}
	if(!ILTypeIdentical(ILTypeGetReturn(methodSignature),
						ILTypeGetReturn(invokeSignature)))
	{
		return 0;
	}
	for(param = 1; param <= numParams; ++param)
	{
		if(!ILTypeIdentical(ILTypeGetParam(methodSignature, param),
							ILTypeGetParam(invokeSignature, param)))
		{
			return 0;
		}
	}
	return 1;
}

void *_ILDelegateGetClosure(ILObject *delegate)
{
#if defined(HAVE_LIBFFI)
	ILObject *methodInfo;
	ILMethod *method;

	/* See if we have a cached closure from last time */
	if(!delegate)
	{
		return 0;
	}
	if(((System_Delegate *)delegate)->closure)
	{
		return ((System_Delegate *)delegate)->closure;
	}

	/* If we don't have a method, then the delegate is invalid */
	methodInfo = ((System_Delegate *)delegate)->methodInfo;
	if(!methodInfo)
	{
		return 0;
	}
	method = ((System_Reflection *)methodInfo)->privateData;
	if(!method)
	{
		return 0;
	}

	/* Make a native closure and cache it for next time */
	((System_Delegate *)delegate)->closure =
		_ILMakeClosureForDelegate(delegate, method);
	return ((System_Delegate *)delegate)->closure;
#else
	/* We don't have support for creating closures on this system */
	return 0;
#endif
}

/*
 * private static Delegate CreateBlankDelegate(Type type, ClrMethod method);
 */
ILObject *_IL_Delegate_CreateBlankDelegate(ILExecThread *_thread,
										   ILObject *type,
										   ILObject *method)
{
	ILClass *classInfo;
	ILMethod *methodInfo;

	/* Convert the type into a delegate class descriptor */
	classInfo = _ILGetClrClass(_thread, type);
	if(!classInfo)
	{
		return 0;
	}

	/* Convert the "ClrMethod" instance into a method descriptor */
	if(!method)
	{
		return 0;
	}
	methodInfo = ((System_Reflection *)method)->privateData;
	if(!methodInfo)
	{
		return 0;
	}

	/* Check that the delegate signatures match */
	if(!_ILDelegateSignatureMatch(classInfo, methodInfo))
	{
		return 0;
	}

	/* Create the delegate object and return */
	return _ILEngineAllocObject(_thread, classInfo);
}

#ifdef	__cplusplus
};
#endif
