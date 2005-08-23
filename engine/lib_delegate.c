/*
 * lib_delegate.c - Delegate handling for the runtime engine.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * Contributions:  Thong Nguyen (tum@veridicus.com)
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

void *_ILDelegateGetClosure(ILExecThread *thread, ILObject *delegate)
{
#if defined(HAVE_LIBFFI) && defined(IL_CONFIG_RUNTIME_INFRA)
	ILMethod *methodInfo;

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

	/* Nail down the delegate object, to protect it from garbage collection */
	_IL_GCHandle_GCAlloc(thread, delegate, 2 /* Normal */);

	/* Make a native closure and cache it for next time */
	((System_Delegate *)delegate)->closure =
		_ILMakeClosureForDelegate(_ILExecThreadProcess(thread), delegate,
									methodInfo);
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
	if(!ILTypeDelegateSignatureMatch(ILType_FromClass(classInfo), methodInfo))
	{
		return 0;
	}

	/* Create the delegate object and return */
	return _ILEngineAllocObject(_thread, classInfo);
}

/*
 * private static void SetOutParams(Delegate del, Object[] args,
 *									Object[] outParams);
 */
void _IL_AsyncResult_SetOutParams(ILExecThread *_thread, ILObject *del,
								  System_Array *args, System_Array *outParams)
{
	int i, j, paramcount;
	ILObject **argsArray, **outArray;
	ILType *invokeSignature, *paramType;

	invokeSignature = ILMethod_Signature(((System_Delegate *)del)->methodInfo);

	paramcount = ILTypeNumParams(invokeSignature);

	if (args->length < paramcount || outParams->length < paramcount)
	{
		return;
	}

	j = 0;

	argsArray = ((ILObject **)ArrayToBuffer(args));
	outArray = ((ILObject **)ArrayToBuffer(outParams));

	for (i = 1; i <= paramcount; i++)
	{
		paramType = ILTypeGetParam(invokeSignature, i);

		if (ILType_IsComplex(paramType) 
			&& ILType_Kind(paramType) == IL_TYPE_COMPLEX_BYREF)
		{
			paramType = ILType_Ref(paramType);

			if (j >= outParams->length)
			{
				break;
			}

			outArray[j++] = argsArray[i - 1];
		}
	}

}

/*
 * public Delegate(Object target, IntPtr method);
 */
static ILObject *Delegate_ctor(ILExecThread *thread,
							   ILObject *target,
							   ILNativeInt method)
{
	ILClass *classInfo;
	ILObject *_this;

	/* Allocate space for the delegate object */
	classInfo = ILMethod_Owner(thread->method);
	_this = _ILEngineAllocObject(thread, classInfo);

	if(!_this)
	{
		return 0;
	}

	/* Set the delegate's fields */
	((System_Delegate *)_this)->target = target;
	((System_Delegate *)_this)->methodInfo = (ILMethod *)method;
	((System_Delegate *)_this)->closure = 0;

	/* Done */
	return _this;
}

/*
 * Parameter information for delegate invocation.
 */
typedef struct
{
	CVMWord *words;
	ILUInt32 numWords;

} DelegateInvokeParams;

/*
 * Read a double value from a stack position.
 */
static IL_INLINE ILDouble DelegateReadDouble(CVMWord *stack)
{
#ifdef CVM_DOUBLES_ALIGNED_WORD
	return *((ILDouble *)stack);
#else
	ILDouble temp;
	ILMemCpy(&temp, stack, sizeof(ILDouble));
	return temp;
#endif
}

/*
 * Pack the parameters for a delegate invocation.
 */
static int PackDelegateInvokeParams(ILExecThread *thread, ILMethod *method,
					                int isCtor, void *_this, void *userData)
{
	DelegateInvokeParams *params = (DelegateInvokeParams *)userData;
	ILType *signature = ILMethod_Signature(method);
	CVMWord *stacktop = thread->stackTop;
	ILType *type;
	unsigned long numParams;
	unsigned long paramNum;
	ILNativeFloat nativeFloat;
	CVMWord *words;
	ILUInt32 size;

	/* Push the "this" pointer if necessary */
	if(ILType_HasThis(signature))
	{
		if(stacktop >= thread->stackLimit)
		{
			thread->thrownException = _ILSystemException
				(thread, "System.StackOverflowException");
			return 1;
		}
		stacktop->ptrValue = _this;
		++stacktop;
	}

	/* Push the parameter words */
	if((stacktop + params->numWords) >= thread->stackLimit)
	{
		thread->thrownException = _ILSystemException
			(thread, "System.StackOverflowException");
		return 1;
	}
	/* Expand "float" and "double" parameters, because the frame variables
	   are in "fixed up" form, rather than native float form */
	numParams = ILTypeNumParams(signature);
	words = params->words;
	for(paramNum = 1; paramNum <= numParams; ++paramNum)
	{
		void *ptr;

		type = ILTypeGetParam(signature, paramNum);
		if(type == ILType_Float32)
		{
			nativeFloat = (ILNativeFloat)(*((ILFloat *)words));
			ptr = (void *)&nativeFloat;
			size = CVM_WORDS_PER_NATIVE_FLOAT;
		}
		else if(type == ILType_Float64)
		{
			nativeFloat = (ILNativeFloat)DelegateReadDouble(words);
			ptr = (void *)&nativeFloat;
			size = CVM_WORDS_PER_NATIVE_FLOAT;
		}
		else
		{
			ptr = (void *)words;
			size = ((ILSizeOfType(thread, type) + sizeof(CVMWord) - 1)
						/ sizeof(CVMWord));
		}
		ILMemCpy(stacktop, ptr, size * sizeof(CVMWord));
		words += size;
		stacktop += size;
	}

	/* Update the stack top */
	thread->stackTop = stacktop;
	return 0;
}

/*
 * public Type Invoke(...);
 */
static void Delegate_Invoke(ILExecThread *thread,
							void *result,
							ILObject *_this)
{
	ILObject *target;
	ILMethod *method;
	DelegateInvokeParams params;

	/* If this is a multicast delegate, then execute "prev" first */
	if(((System_Delegate *)_this)->prev)
	{
		Delegate_Invoke(thread, result, ((System_Delegate *)_this)->prev);
		if(ILExecThreadHasException(thread))
		{
			return;
		}
	}

	/* Extract the fields from the delegate and validate them */
	target = ((System_Delegate *)_this)->target;
	method = ((System_Delegate *)_this)->methodInfo;
	if(!method)
	{
		ILExecThreadThrowSystem(thread, "System.MissingMethodException",
								(const char *)0);
		return;
	}

	/* Locate the start and end of the parameters to "Invoke",
	   excluding the "this" value that represents the delegate */
	params.words = thread->frame + 1;
	params.numWords = _ILGetMethodParamCount(thread, method, 1);

	/* Call the method */
	_ILCallMethod(thread, method,
		_ILCallUnpackDirectResult, result,
		0, target,
		PackDelegateInvokeParams, &params);
}

static ILObject *Delegate_BeginInvoke(ILExecThread *thread, ILObject *_this)
{	
	void *array;		
	ILClass *classInfo;
	ILObject *obj;
	ILObject *result = 0;
	ILType *beginInvokeMethodSignature;
	ILMethod *beginInvokeMethodInfo, *async_ctor;	
	CVMWord *stackTop = thread->stackTop;
	int paramWords, paramCount;

	/* Get the AsyncResult classs */
	classInfo = ILExecThreadLookupClass
		(
			thread,
			"System.Runtime.Remoting.Messaging.AsyncResult"
		);

	if (classInfo == 0)
	{
		ILExecThreadThrowSystem(thread, "System.MissingMethodException", (const char *)0);

		return 0;
	}

	/* Get the constructor for AsyncResult */
	async_ctor = ILExecThreadLookupMethod
		(
			thread,
			"System.Runtime.Remoting.Messaging.AsyncResult",
			".ctor",
			"(ToSystem.Delegate;[oSystem.Object;oSystem.AsyncCallback;oSystem.Object;)V"
		);

	if (async_ctor == 0)
	{
		ILExecThreadThrowSystem(thread, "System.MissingMethodException", (const char *)0);

		return 0;
	}

	/* Get the BeginInvoke method for the delegate */
	beginInvokeMethodInfo = (ILMethod *)ILTypeGetDelegateBeginInvokeMethod
		(
			ILType_FromClass(GetObjectClassPrivate(_this)->classInfo)
		);

	if(!beginInvokeMethodInfo)
	{
		ILExecThreadThrowSystem(thread, "System.MissingMethodException",
			(const char *)0);

		return 0;
	}

	/* Get the signature for the BeginInvoke method */
	beginInvokeMethodSignature = ILMethod_Signature(beginInvokeMethodInfo);

	/* The number of paramters for the BeginInvoke method */
	paramCount = ILTypeNumParams(beginInvokeMethodSignature);

	/* Get the number of CVM words the delegate method requires  */
	paramWords = _ILGetMethodParamCount
		(
			thread,
			beginInvokeMethodInfo,
			0
		);

	/* Pack the parameters into a managed object array */
	_ILPackCVMStackArgs
		(
			thread,
			 /* stackTop is the part "just below" the IAsyncResult parameter */
			thread->frame + paramWords - 2,
			/* 0 is return param */
			1,
			/* Number of parameters in BeginInvoke not including the AsyncResult & state */
			paramCount - 2,
			/* Can use BeginInvoke signature because the first parameters are the same */
			beginInvokeMethodSignature,
			&array
		); 

	obj = _ILEngineAllocObject(thread, classInfo);

	/* Call AsyncResult constructor */
	ILExecThreadCall
		(
			thread,
			async_ctor,
			&result,
			obj,
			(System_Delegate *)_this,
			array /* Delegate method arguments */,
			(thread->frame + paramWords - 2)->ptrValue /* AsyncCallback */,
			(thread->frame + paramWords - 1)->ptrValue /* AsyncState */
		);

	thread->stackTop = stackTop;

	return obj;
}

static void Delegate_EndInvoke(ILExecThread *thread,
							void *result,
							ILObject *_this)
{	
	ILObject *retValue;
	ILObject *array;
	ILObject **arrayBuffer;
	ILClass *classInfo;
	ILType *retType;	
	ILMethod *endInvokeMethodInfo;
	ILType *endInvokeMethodSignature;
	ILMethod *asyncEndInvokeMethodInfo;	
	CVMWord *stackTop = thread->stackTop;
	int i, paramWords, paramCount;
	CVMWord *frame;

	classInfo = GetObjectClass(_this);

	/* Get the "EndInvoke" method */
	endInvokeMethodInfo = (ILMethod *)ILTypeGetDelegateEndInvokeMethod
		(
			ILType_FromClass(GetObjectClassPrivate(_this)->classInfo)
		);

	if (endInvokeMethodInfo == 0)
	{
		ILExecThreadThrowSystem(thread, "System.MissingMethodException", (const char *)0);		

		return;
	}

	/* Get the return type of the delegate */
	retType = ILTypeGetReturn(ILMethod_Signature(endInvokeMethodInfo));
	
	endInvokeMethodSignature = ILMethod_Signature(endInvokeMethodInfo);

	/* Number of parameters for the EndInvoke method */
	paramCount = ILTypeNumParams(endInvokeMethodSignature);

	/* Get the number of CVM words the parameters take up */
	paramWords = _ILGetMethodParamCount
		(
			thread,
			endInvokeMethodInfo,
			0
		);

	/* Get the AsyncResult.EndInvoke method */
	asyncEndInvokeMethodInfo = ILExecThreadLookupMethod
		(
			thread,
			"System.Runtime.Remoting.Messaging.AsyncResult",
			"EndInvoke",
			"(T[oSystem.Object;)oSystem.Object;"
		);

	if (asyncEndInvokeMethodInfo == 0)
	{
		ILExecThreadThrowSystem(thread, "System.MissingMethodException", (const char *)0);		

		return;
	}

	/* Create an array to store the out params */
	array = ILExecThreadNew(thread, "[oSystem.Object;", "(Ti)V",
		(ILVaUInt)paramCount - 1 /* Number of out-params is (number of params) - (1 for AsyncResult) */);

	arrayBuffer = ((ILObject **)ArrayToBuffer(array));

	if (array == 0)
	{
		ILExecThreadThrowOutOfMemory(thread);

		return;
	}

	/* Call AsyncResult.EndInvoke. */
	ILExecThreadCall
		(
			thread,
			asyncEndInvokeMethodInfo,
			&retValue,
			/* IAsyncResult */
			(thread->frame + paramWords - 1)->ptrValue,
			/* Array for out-params */
			array			
		);

	/*
	 * Pack "out" values back onto the stack.
	 */

	/* Skip the "this" pointer */
	frame = thread->frame + 1;

	for (i = 1; i <= paramCount - 1 /* -1 to skip IAsyncResult */; i++)
	{
		ILUInt32 paramSize;
		ILType *paramType;

		paramType = ILTypeGetParam(endInvokeMethodSignature, i);
		paramSize = _ILStackWordsForType(thread, paramType);
		paramType = ILType_Ref(paramType);

		if (ILType_IsPrimitive(paramType) || ILType_IsValueType(paramType))
		{
			/* If the param is a value type then unbox directly into the argument */
			ILExecThreadUnbox(thread, paramType, arrayBuffer[i - 1], frame->ptrValue);
		}
		else if (ILType_IsClass(paramType))
		{
			/* If the param is a class reference then set the new class reference */
			*(ILObject **)frame->ptrValue = arrayBuffer[i - 1];
		}
		else
		{
			/* Don't know how to return this type of out param */
		}

		frame += paramSize;
	}
	
	/* Set the return value */
	if (ILTypeIsValue(retType))
	{
		ILExecThreadUnbox(thread, retType, retValue, result);
	}
	else
	{
		*((ILObject **)result) = retValue;
	}	

	thread->stackTop = stackTop;
}

#if !defined(HAVE_LIBFFI)

/*
 * Marshal a delegate constructor.
 */
static void Delegate_ctor_marshal
	(void (*fn)(), void *rvalue, void **avalue)
{
	*((ILObject **)rvalue) =
		Delegate_ctor(*((ILExecThread **)(avalue[0])),
		              *((ILObject **)(avalue[1])),
		              *((ILNativeInt *)(avalue[2])));
}

/*
 * Marshal a delegate "Invoke" method for the various return types.
 */
static void Delegate_Invoke_marshal
	(void (*fn)(), void *rvalue, void **avalue)
{
	Delegate_Invoke(*((ILExecThread **)(avalue[0])), rvalue,
		            *((ILObject **)(avalue[1])));
}
static void Delegate_Invoke_sbyte_marshal
	(void (*fn)(), void *rvalue, void **avalue)
{
	ILInt8 result;
	Delegate_Invoke(*((ILExecThread **)(avalue[0])), &result,
		            *((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_Invoke_byte_marshal
	(void (*fn)(), void *rvalue, void **avalue)
{
	ILUInt8 result;
	Delegate_Invoke(*((ILExecThread **)(avalue[0])), &result,
		            *((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_Invoke_short_marshal
	(void (*fn)(), void *rvalue, void **avalue)
{
	ILInt16 result;
	Delegate_Invoke(*((ILExecThread **)(avalue[0])), &result,
		            *((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_Invoke_ushort_marshal
	(void (*fn)(), void *rvalue, void **avalue)
{
	ILUInt16 result;
	Delegate_Invoke(*((ILExecThread **)(avalue[0])), &result,
		            *((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_Invoke_int_marshal
	(void (*fn)(), void *rvalue, void **avalue)
{
	ILInt32 result;
	Delegate_Invoke(*((ILExecThread **)(avalue[0])), &result,
		            *((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_Invoke_uint_marshal
	(void (*fn)(), void *rvalue, void **avalue)
{
	ILUInt32 result;
	Delegate_Invoke(*((ILExecThread **)(avalue[0])), &result,
		            *((ILObject **)(avalue[1])));
	*((ILNativeUInt *)rvalue) = (ILNativeUInt)result;
}

#define	Delegate_Invoke_void_marshal			Delegate_Invoke_marshal
#define	Delegate_Invoke_long_marshal			Delegate_Invoke_marshal
#define	Delegate_Invoke_ulong_marshal			Delegate_Invoke_marshal
#define	Delegate_Invoke_float_marshal			Delegate_Invoke_marshal
#define	Delegate_Invoke_double_marshal			Delegate_Invoke_marshal
#define	Delegate_Invoke_nativeFloat_marshal		Delegate_Invoke_marshal
#define	Delegate_Invoke_typedref_marshal		Delegate_Invoke_marshal
#define	Delegate_Invoke_ref_marshal				Delegate_Invoke_marshal
#define	Delegate_Invoke_managedValue_marshal	Delegate_Invoke_marshal

/*
* Marshal a delegate "EndInvoke" method for the various return types.
*/
static void Delegate_EndInvoke_marshal
(void (*fn)(), void *rvalue, void **avalue)
{
	Delegate_EndInvoke(*((ILExecThread **)(avalue[0])), rvalue,
		*((ILObject **)(avalue[1])));
}
static void Delegate_EndInvoke_sbyte_marshal
(void (*fn)(), void *rvalue, void **avalue)
{
	ILInt8 result;
	Delegate_EndInvoke(*((ILExecThread **)(avalue[0])), &result,
		*((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_EndInvoke_byte_marshal
(void (*fn)(), void *rvalue, void **avalue)
{
	ILUInt8 result;
	Delegate_EndInvoke(*((ILExecThread **)(avalue[0])), &result,
		*((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_EndInvoke_short_marshal
(void (*fn)(), void *rvalue, void **avalue)
{
	ILInt16 result;
	Delegate_EndInvoke(*((ILExecThread **)(avalue[0])), &result,
		*((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_EndInvoke_ushort_marshal
(void (*fn)(), void *rvalue, void **avalue)
{
	ILUInt16 result;
	Delegate_EndInvoke(*((ILExecThread **)(avalue[0])), &result,
		*((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_EndInvoke_int_marshal
(void (*fn)(), void *rvalue, void **avalue)
{
	ILInt32 result;
	Delegate_EndInvoke(*((ILExecThread **)(avalue[0])), &result,
		*((ILObject **)(avalue[1])));
	*((ILNativeInt *)rvalue) = (ILNativeInt)result;
}
static void Delegate_EndInvoke_uint_marshal
(void (*fn)(), void *rvalue, void **avalue)
{
	ILUInt32 result;
	Delegate_EndInvoke(*((ILExecThread **)(avalue[0])), &result,
		*((ILObject **)(avalue[1])));
	*((ILNativeUInt *)rvalue) = (ILNativeUInt)result;
}

#define	Delegate_EndInvoke_void_marshal			Delegate_EndInvoke_marshal
#define	Delegate_EndInvoke_long_marshal			Delegate_EndInvoke_marshal
#define	Delegate_EndInvoke_ulong_marshal			Delegate_EndInvoke_marshal
#define	Delegate_EndInvoke_float_marshal			Delegate_EndInvoke_marshal
#define	Delegate_EndInvoke_double_marshal			Delegate_EndInvoke_marshal
#define	Delegate_EndInvoke_nativeFloat_marshal		Delegate_EndInvoke_marshal
#define	Delegate_EndInvoke_typedref_marshal		Delegate_EndInvoke_marshal
#define	Delegate_EndInvoke_ref_marshal				Delegate_EndInvoke_marshal
#define	Delegate_EndInvoke_managedValue_marshal	Delegate_EndInvoke_marshal

/*
 * Point all invoke types at the common function (we'll be
 * calling it through the marshalling stub so it doesn't
 * matter what this value is).
 */
#define	Delegate_Invoke_void					Delegate_Invoke
#define	Delegate_Invoke_sbyte					Delegate_Invoke
#define	Delegate_Invoke_byte					Delegate_Invoke
#define	Delegate_Invoke_short					Delegate_Invoke
#define	Delegate_Invoke_ushort					Delegate_Invoke
#define	Delegate_Invoke_int						Delegate_Invoke
#define	Delegate_Invoke_uint					Delegate_Invoke
#define	Delegate_Invoke_long					Delegate_Invoke
#define	Delegate_Invoke_ulong					Delegate_Invoke
#define	Delegate_Invoke_float					Delegate_Invoke
#define	Delegate_Invoke_double					Delegate_Invoke
#define	Delegate_Invoke_nativeFloat				Delegate_Invoke
#define	Delegate_Invoke_typedref				Delegate_Invoke
#define	Delegate_Invoke_ref						Delegate_Invoke
#define	Delegate_Invoke_managedValue			Delegate_Invoke


#define	Delegate_EndInvoke_void					Delegate_EndInvoke
#define	Delegate_EndInvoke_sbyte				Delegate_EndInvoke
#define	Delegate_EndInvoke_byte					Delegate_EndInvoke
#define	Delegate_EndInvoke_short				Delegate_EndInvoke
#define	Delegate_EndInvoke_ushort				Delegate_EndInvoke
#define	Delegate_EndInvoke_int					Delegate_EndInvoke
#define	Delegate_EndInvoke_uint					Delegate_EndInvoke
#define	Delegate_EndInvoke_long					Delegate_EndInvoke
#define	Delegate_EndInvoke_ulong				Delegate_EndInvoke
#define	Delegate_EndInvoke_float				Delegate_EndInvoke
#define	Delegate_EndInvoke_double				Delegate_EndInvoke
#define	Delegate_EndInvoke_nativeFloat			Delegate_EndInvoke
#define	Delegate_EndInvoke_typedref				Delegate_EndInvoke
#define	Delegate_EndInvoke_ref					Delegate_EndInvoke
#define	Delegate_EndInvoke_managedValue			Delegate_EndInvoke

#else /* HAVE_LIBFFI */

/*
 * Wrap "Delegate_Invoke" for the various return types.
 */
static void Delegate_Invoke_void(ILExecThread *thread,
								 ILObject *_this)
{
	Delegate_Invoke(thread, (void *)0, _this);
} 
#define	Delegate_X_type(x, iltype,type)	\
static type Delegate_##x##_##iltype(ILExecThread *thread, \
									 ILObject *_this) \
{ \
	type result; \
	Delegate_##x(thread, &result, _this); \
	return result; \
} 
Delegate_X_type(Invoke, byte, ILUInt8)
Delegate_X_type(Invoke, sbyte, ILInt8)
Delegate_X_type(Invoke, short, ILInt16)
Delegate_X_type(Invoke, ushort, ILUInt16)
Delegate_X_type(Invoke, int, ILInt32)
Delegate_X_type(Invoke, uint, ILUInt32)
Delegate_X_type(Invoke, long, ILInt64)
Delegate_X_type(Invoke, ulong, ILUInt64)
Delegate_X_type(Invoke, float, ILFloat)
Delegate_X_type(Invoke, double, ILDouble)
Delegate_X_type(Invoke, nativeFloat, ILNativeFloat)
Delegate_X_type(Invoke, typedref, ILTypedRef)
Delegate_X_type(Invoke, ref, void *)
#define	Delegate_Invoke_managedValue			Delegate_Invoke

/*
* Marshalling stubs for libffi (not required).
*/
#define	Delegate_ctor_marshal					0
#define	Delegate_Invoke_void_marshal			0
#define	Delegate_Invoke_sbyte_marshal			0
#define	Delegate_Invoke_byte_marshal			0
#define	Delegate_Invoke_short_marshal			0
#define	Delegate_Invoke_ushort_marshal			0
#define	Delegate_Invoke_int_marshal				0
#define	Delegate_Invoke_uint_marshal			0
#define	Delegate_Invoke_long_marshal			0
#define	Delegate_Invoke_ulong_marshal			0
#define	Delegate_Invoke_float_marshal			0
#define	Delegate_Invoke_double_marshal			0
#define	Delegate_Invoke_nativeFloat_marshal		0
#define	Delegate_Invoke_typedref_marshal		0
#define	Delegate_Invoke_ref_marshal				0
#define	Delegate_Invoke_managedValue_marshal	0

/*
 * "Delegate_EndInvoke" for various return types.
 */
static void Delegate_EndInvoke_void(ILExecThread *thread,
 ILObject *_this)
{
	Delegate_EndInvoke(thread, (void *)0, _this);
} 
Delegate_X_type(EndInvoke, byte, ILUInt8)
Delegate_X_type(EndInvoke, sbyte, ILInt8)
Delegate_X_type(EndInvoke, short, ILInt16)
Delegate_X_type(EndInvoke, ushort, ILUInt16)
Delegate_X_type(EndInvoke, int, ILInt32)
Delegate_X_type(EndInvoke, uint, ILUInt32)
Delegate_X_type(EndInvoke, long, ILInt64)
Delegate_X_type(EndInvoke, ulong, ILUInt64)
Delegate_X_type(EndInvoke, float, ILFloat)
Delegate_X_type(EndInvoke, double, ILDouble)
Delegate_X_type(EndInvoke, nativeFloat, ILNativeFloat)
Delegate_X_type(EndInvoke, typedref, ILTypedRef)
Delegate_X_type(EndInvoke, ref, void *)
#define	Delegate_EndInvoke_managedValue			Delegate_EndInvoke

/*
 * Marshalling stubs for "Delegate_EndInvoke" for libffi (not required).
 */
#define	Delegate_ctor_marshal						0
#define	Delegate_EndInvoke_void_marshal				0
#define	Delegate_EndInvoke_sbyte_marshal			0
#define	Delegate_EndInvoke_byte_marshal				0
#define	Delegate_EndInvoke_short_marshal			0
#define	Delegate_EndInvoke_ushort_marshal			0
#define	Delegate_EndInvoke_int_marshal				0
#define	Delegate_EndInvoke_uint_marshal				0
#define	Delegate_EndInvoke_long_marshal				0
#define	Delegate_EndInvoke_ulong_marshal			0
#define	Delegate_EndInvoke_float_marshal			0
#define	Delegate_EndInvoke_double_marshal			0
#define	Delegate_EndInvoke_nativeFloat_marshal		0
#define	Delegate_EndInvoke_typedref_marshal			0
#define	Delegate_EndInvoke_ref_marshal				0
#define	Delegate_EndInvoke_managedValue_marshal		0

#endif /* HAVE_LIBFFI */
 
 /*
  * Table of functions pointers for each possible return type.
  */
typedef struct _tagInternalFuncs
{
	void *void_func;
	void *sbyte_func;
	void *byte_func;
	void *short_func;
	void *ushort_func;
	void *int_func;
	void *uint_func;
	void *long_func;
	void *ulong_func;
	void *float_func;
	void *double_func;
	void *nativeFloat_func;
	void *typedref_Func;
	void *ref_func;
	void *managedValue_func;
}
InternalFuncs;

#define DEFINE_FUNCS_TABLE(prefix)			\
	static InternalFuncs FuncsFor_Delegate_##prefix =\
	{										\
		Delegate_##prefix##_void,					\
		Delegate_##prefix##_sbyte,					\
		Delegate_##prefix##_byte,					\
		Delegate_##prefix##_short,					\
		Delegate_##prefix##_ushort,					\
		Delegate_##prefix##_int,						\
		Delegate_##prefix##_uint,					\
		Delegate_##prefix##_long,					\
		Delegate_##prefix##_ulong,					\
		Delegate_##prefix##_float,					\
		Delegate_##prefix##_double,					\
		Delegate_##prefix##_nativeFloat,				\
		Delegate_##prefix##_typedref,				\
		Delegate_##prefix##_ref,						\
		Delegate_##prefix##_managedValue				\
	};

#define DEFINE_MARSHAL_FUNCS_TABLE(prefix)	\
	static InternalFuncs MarshalFuncsFor_Delegate_##prefix =\
	{										\
		Delegate_##prefix##_void_marshal,			\
		Delegate_##prefix##_sbyte_marshal,			\
		Delegate_##prefix##_byte_marshal,			\
		Delegate_##prefix##_short_marshal,			\
		Delegate_##prefix##_ushort_marshal,			\
		Delegate_##prefix##_int_marshal,				\
		Delegate_##prefix##_uint_marshal,			\
		Delegate_##prefix##_long_marshal,			\
		Delegate_##prefix##_ulong_marshal,			\
		Delegate_##prefix##_float_marshal,			\
		Delegate_##prefix##_double_marshal,			\
		Delegate_##prefix##_nativeFloat_marshal,		\
		Delegate_##prefix##_typedref_marshal,		\
		Delegate_##prefix##_ref_marshal,				\
		Delegate_##prefix##_managedValue_marshal		\
	};

/*
 * Define function table for each return type for Delegate.Invoke.
 */
DEFINE_FUNCS_TABLE(Invoke)

/*
 * Define marshalling function table for each return type for Delegate.Invoke.
 */
DEFINE_MARSHAL_FUNCS_TABLE(Invoke)

/*
 * Define function table for each return type for Delegate.EndInvoke.
 */
DEFINE_FUNCS_TABLE(EndInvoke)

/*
 * Define marshalling function table for each return type for Delegate.EndInvoke.
 */
DEFINE_MARSHAL_FUNCS_TABLE(EndInvoke)

static int _ILGetInternalDelegateFunc(ILInternalInfo *info, ILType *returnType,
	InternalFuncs *internalFuncs, InternalFuncs *internalMarshalFuncs)
{
	ILType *type;

	type = ILTypeGetEnumType(returnType);

	if(ILType_IsPrimitive(type))
	{
		/* The delegate returns a primitive type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			{
				info->func = internalFuncs->void_func;
				info->marshal = internalMarshalFuncs->void_func;
			}
			break;

			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			{
				info->func = internalFuncs->sbyte_func;
				info->marshal = internalMarshalFuncs->sbyte_func;
			}
			break;

			case IL_META_ELEMTYPE_U1:
			{
				info->func = internalFuncs->byte_func;
				info->marshal = internalMarshalFuncs->byte_func;
			}
			break;

			case IL_META_ELEMTYPE_I2:
			{
				info->func = internalFuncs->short_func;
				info->marshal = internalMarshalFuncs->short_func;
			}
			break;

			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			{
				info->func = internalFuncs->ushort_func;
				info->marshal = internalMarshalFuncs->ushort_func;
			}
			break;

			case IL_META_ELEMTYPE_I4:
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_I:
		#endif
			{
				info->func = internalFuncs->int_func;
				info->marshal = internalMarshalFuncs->int_func;
			}
			break;

			case IL_META_ELEMTYPE_U4:
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_U:
		#endif
			{
				info->func = internalFuncs->uint_func;
				info->marshal = internalMarshalFuncs->uint_func;
			}
			break;

			case IL_META_ELEMTYPE_I8:
		#ifdef IL_NATIVE_INT64
			case IL_META_ELEMTYPE_I:
		#endif
			{
				info->func = internalFuncs->long_func;
				info->marshal = internalMarshalFuncs->long_func;
			}
			break;

			case IL_META_ELEMTYPE_U8:
		#ifdef IL_NATIVE_INT64
			case IL_META_ELEMTYPE_U:
		#endif
			{
				info->func = internalFuncs->ulong_func;
				info->marshal = internalMarshalFuncs->ulong_func;
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
				info->func = internalFuncs->float_func;
				info->marshal = internalMarshalFuncs->float_func;
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				info->func = internalFuncs->double_func;
				info->marshal = internalMarshalFuncs->double_func;
			}
			break;

			case IL_META_ELEMTYPE_R:
			{
				info->func = internalFuncs->nativeFloat_func;
				info->marshal = internalMarshalFuncs->nativeFloat_func;
			}
			break;

			case IL_META_ELEMTYPE_TYPEDBYREF:
			{
				info->func = internalFuncs->typedref_Func;
				info->marshal = internalMarshalFuncs->typedref_Func;
			}
			break;

			default:	return 0;
		}
	}
	else if(ILType_IsValueType(type))
	{
		/* The delegate returns a managed value */
		info->func = internalFuncs->managedValue_func;
		info->marshal = internalMarshalFuncs->managedValue_func;
	}
	else
	{
		/* Everything else is assumed to be a pointer */
		info->func = internalFuncs->ref_func;
		info->marshal = internalMarshalFuncs->ref_func;;
	}

	return 1;
}

int _ILGetInternalDelegate(ILMethod *method, int *isCtor,
						   ILInternalInfo *info)
{
	ILClass *classInfo;
	const char *name;
	ILType *type;

	/* Determine if the method's class is indeed a delegate */
	classInfo = ILMethod_Owner(method);
	if(!classInfo)
	{
		return 0;
	}
	if(!ILTypeIsDelegate(ILType_FromClass(classInfo)))
	{
		return 0;
	}

	/* Determine which method we are looking for */
	name = ILMethod_Name(method);
	type = ILMethod_Signature(method);
	if(!strcmp(name, ".ctor"))
	{
		/* Check that the constructor has the correct signature */
		if(_ILLookupTypeMatch(type, "(ToSystem.Object;j)V"))
		{
			*isCtor = 1;
			info->func = (void *)Delegate_ctor;
			info->marshal = (void *)Delegate_ctor_marshal;
			return 1;
		}
	}
	else if(!strcmp(name, "Invoke") && ILType_HasThis(type))
	{
		/* This is the delegate invocation method */
		*isCtor = 0;

		return _ILGetInternalDelegateFunc
			(
				info,
				ILTypeGetReturn(type),
				&FuncsFor_Delegate_Invoke,
				&MarshalFuncsFor_Delegate_Invoke
			);
	}
	else if(!strcmp(name, "BeginInvoke"))
	{
		*isCtor = 0;

		info->func = (void *)Delegate_BeginInvoke;
		info->marshal = (void *)0;

		return 1;
	}
	else if(!strcmp(name, "EndInvoke"))
	{
		/* This is the delegate invocation method */
		*isCtor = 0;
		
		return _ILGetInternalDelegateFunc
			(
				info,
				ILTypeGetReturn(type),
				&FuncsFor_Delegate_EndInvoke,
				&MarshalFuncsFor_Delegate_EndInvoke
			);
	}

	return 0;
}

#ifdef	__cplusplus
};
#endif
