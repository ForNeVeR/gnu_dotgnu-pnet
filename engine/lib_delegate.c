/*
 * lib_delegate.c - Delegate handling for the runtime engine.
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
		_ILMakeClosureForDelegate(delegate, methodInfo);
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
	/* TODO */
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
 * Pack the parameters for a delegate invocation.
 */
static int PackDelegateInvokeParams(ILExecThread *thread, ILMethod *method,
					                int isCtor, void *_this, void *userData)
{
	DelegateInvokeParams *params = (DelegateInvokeParams *)userData;
	ILType *signature = ILMethod_Signature(method);
	CVMWord *stacktop = thread->stackTop;

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
	ILMemCpy(stacktop, params->words, params->numWords * sizeof(CVMWord));
	stacktop += params->numWords;

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

#else /* HAVE_LIBFFI */

/*
 * Wrap "Delegate_Invoke" for the various return types.
 */
static void Delegate_Invoke_void(ILExecThread *thread,
								 ILObject *_this)
{
	Delegate_Invoke(thread, (void *)0, _this);
} 
#define	Delegate_Invoke_type(iltype,type)	\
static type Delegate_Invoke_##iltype(ILExecThread *thread, \
									 ILObject *_this) \
{ \
	type result; \
	Delegate_Invoke(thread, &result, _this); \
	return result; \
} 
Delegate_Invoke_type(byte, ILUInt8)
Delegate_Invoke_type(sbyte, ILInt8)
Delegate_Invoke_type(short, ILInt16)
Delegate_Invoke_type(ushort, ILUInt16)
Delegate_Invoke_type(int, ILInt32)
Delegate_Invoke_type(uint, ILUInt32)
Delegate_Invoke_type(long, ILInt64)
Delegate_Invoke_type(ulong, ILUInt64)
Delegate_Invoke_type(float, ILFloat)
Delegate_Invoke_type(double, ILDouble)
Delegate_Invoke_type(nativeFloat, ILNativeFloat)
Delegate_Invoke_type(typedref, ILTypedRef)
Delegate_Invoke_type(ref, void *)
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

#endif /* HAVE_LIBFFI */

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
		type = ILTypeGetEnumType(ILTypeGetReturn(type));
		if(ILType_IsPrimitive(type))
		{
			/* The delegate returns a primitive type */
			switch(ILType_ToElement(type))
			{
				case IL_META_ELEMTYPE_VOID:
				{
					info->func = (void *)Delegate_Invoke_void;
					info->marshal = (void *)Delegate_Invoke_void_marshal;
				}
				break;

				case IL_META_ELEMTYPE_BOOLEAN:
				case IL_META_ELEMTYPE_I1:
				{
					info->func = (void *)Delegate_Invoke_sbyte;
					info->marshal = (void *)Delegate_Invoke_sbyte_marshal;
				}
				break;

				case IL_META_ELEMTYPE_U1:
				{
					info->func = (void *)Delegate_Invoke_byte;
					info->marshal = (void *)Delegate_Invoke_byte_marshal;
				}
				break;

				case IL_META_ELEMTYPE_I2:
				{
					info->func = (void *)Delegate_Invoke_short;
					info->marshal = (void *)Delegate_Invoke_short_marshal;
				}
				break;

				case IL_META_ELEMTYPE_U2:
				case IL_META_ELEMTYPE_CHAR:
				{
					info->func = (void *)Delegate_Invoke_ushort;
					info->marshal = (void *)Delegate_Invoke_ushort_marshal;
				}
				break;

				case IL_META_ELEMTYPE_I4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_I:
			#endif
				{
					info->func = (void *)Delegate_Invoke_int;
					info->marshal = (void *)Delegate_Invoke_int_marshal;
				}
				break;

				case IL_META_ELEMTYPE_U4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_U:
			#endif
				{
					info->func = (void *)Delegate_Invoke_uint;
					info->marshal = (void *)Delegate_Invoke_uint_marshal;
				}
				break;

				case IL_META_ELEMTYPE_I8:
			#ifdef IL_NATIVE_INT64
				case IL_META_ELEMTYPE_I:
			#endif
				{
					info->func = (void *)Delegate_Invoke_long;
					info->marshal = (void *)Delegate_Invoke_long_marshal;
				}
				break;

				case IL_META_ELEMTYPE_U8:
			#ifdef IL_NATIVE_INT64
				case IL_META_ELEMTYPE_U:
			#endif
				{
					info->func = (void *)Delegate_Invoke_ulong;
					info->marshal = (void *)Delegate_Invoke_ulong_marshal;
				}
				break;

				case IL_META_ELEMTYPE_R4:
				{
					info->func = (void *)Delegate_Invoke_float;
					info->marshal = (void *)Delegate_Invoke_float_marshal;
				}
				break;

				case IL_META_ELEMTYPE_R8:
				{
					info->func = (void *)Delegate_Invoke_double;
					info->marshal = (void *)Delegate_Invoke_double_marshal;
				}
				break;

				case IL_META_ELEMTYPE_R:
				{
					info->func = (void *)Delegate_Invoke_nativeFloat;
					info->marshal =
						(void *)Delegate_Invoke_nativeFloat_marshal;
				}
				break;

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					info->func = (void *)Delegate_Invoke_typedref;
					info->marshal = (void *)Delegate_Invoke_typedref_marshal;
				}
				break;

				default:	return 0;
			}
		}
		else if(ILType_IsValueType(type))
		{
			/* The delegate returns a managed value */
			info->func = (void *)Delegate_Invoke_managedValue;
			info->marshal = (void *)Delegate_Invoke_managedValue_marshal;
		}
		else
		{
			/* Everything else is assumed to be a pointer */
			info->func = (void *)Delegate_Invoke_ref;
			info->marshal = (void *)Delegate_Invoke_ref_marshal;
		}
		return 1;
	}
	else if(!strcmp(name, "BeginInvoke"))
	{
		/* TODO: asynchronous delegates */
	}
	else if(!strcmp(name, "EndInvoke"))
	{
		/* TODO: asynchronous delegates */
	}
	return 0;
}

#ifdef	__cplusplus
};
#endif
