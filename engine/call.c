/*
 * call.c - External interface for calling methods using the engine.
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

/*

The following calling conventions are used with "ILExecThreadCall"
and the other va-based API functions:

	bool, sbyte, byte, short, ushort, char, int
			- Passed as the larger of "int" and "ILInt32".  The helper
			  type "ILVaInt" is recommended for the cast.
	uint
			- Passed as the larger of "unsigned int" and "ILUInt32".
			  The type "ILVaUInt" is recommended for the cast.
	long, ulong
			- Passed as "ILInt64" and "ILUInt64".
	float, double, native float
			- Passed as "double".  The type "ILVaDouble" is recommended.
	typedref
			- Passed as a pointer to a "ILTypedRef" structure.
	class types
			- Passed as an object reference handle: "ILObject *".
	value types
			- Passed as a pointer to a temporary copy of the
			  structure: "void *".
	byref values
			- Passed as a native pointer: "void *".
	other types
			- Passed as an object reference handle: "ILObject *".

Care should be taken to cast all arguments to the correct type so that
the values can be unpacked successfully using the va macros.  e.g.,
when calling "void m(float x, int y)", use the following:

	ILExecThreadCall(thread, m, &result, (ILVaDouble)x, (ILVaInt)y);

The C compiler's default promotion rules will normally take care of
this automatically, but the explicit casts are recommended.

*/

#include "engine_private.h"
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#define	VA_LIST				va_list
#define	VA_START(arg)		va_list va; va_start(va, arg)
#define	VA_END				va_end(va)
#define	VA_ARG(va,type)		va_arg(va, type)
#define	VA_GET_LIST			va
#else
#ifdef HAVE_VARARGS_H
#include <varargs.h>
#define	VA_LIST				va_list
#define	VA_START(arg)		va_list va; va_start(va)
#define	VA_END				va_end(va)
#define	VA_ARG(va,type)		va_arg(va, type)
#define	VA_GET_LIST			va
#else
#define	VA_LIST				int
#define	VA_START
#define	VA_END
#define	VA_ARG(va,type)		((type)0)
#define	VA_GET_LIST			0
#endif
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Check that we have enough space to push an argument.
 */
#define	CHECK_SPACE(nwords)	\
			do { \
				if((stacktop + (nwords)) > stacklimit) \
				{ \
					/* TODO: throw a stack overflow exception */ \
				} \
			} while (0)

static int CallMethod(ILExecThread *thread, ILMethod *method,
					  void *result, VA_LIST va)
{
	ILType *signature = ILMethod_Signature(method);
	CVMWord *stacktop, *stacklimit;
	ILUInt32 param, numParams;
	ILType *paramType;
	void *ptr;
	ILUInt32 size, sizeInWords;
	ILInt64 int64Value;
	ILNativeFloat fValue;
	ILCallFrame *frame;
	ILUInt32 savePC;
	int threwException;

	/* Get the top and extent of the stack */
	stacktop = thread->stackTop;
	stacklimit = thread->stackLimit;

	/* Push the arguments onto the evaluation stack */
	if((signature->kind & (IL_META_CALLCONV_HASTHIS << 8)) != 0 &&
	   (signature->kind & (IL_META_CALLCONV_EXPLICITTHIS << 8)) == 0)
	{
		/* Push the "this" argument */
		CHECK_SPACE(1);
		stacktop->ptrValue = (void *)(VA_ARG(va, ILObject *));
		++stacktop;
	}
	numParams = signature->num;
	for(param = 1; param <= numParams; ++param)
	{
		paramType = ILTypeGetEnumType(ILTypeGetParam(signature, param));
		if(ILType_IsPrimitive(paramType))
		{
			/* Process a primitive value */
			switch(ILType_ToElement(paramType))
			{
				case IL_META_ELEMTYPE_VOID:		break;

				case IL_META_ELEMTYPE_BOOLEAN:
				case IL_META_ELEMTYPE_I1:
				case IL_META_ELEMTYPE_U1:
				case IL_META_ELEMTYPE_I2:
				case IL_META_ELEMTYPE_U2:
				case IL_META_ELEMTYPE_CHAR:
				case IL_META_ELEMTYPE_I4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_I:
			#endif
				{
					CHECK_SPACE(1);
					stacktop->intValue = (ILInt32)(VA_ARG(va, ILVaInt));
					++stacktop;
				}
				break;

				case IL_META_ELEMTYPE_U4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_U:
			#endif
				{
					CHECK_SPACE(1);
					stacktop->uintValue = (ILUInt32)(VA_ARG(va, ILVaUInt));
					++stacktop;
				}
				break;

				case IL_META_ELEMTYPE_I8:
				case IL_META_ELEMTYPE_U8:
			#ifdef IL_NATIVE_INT64
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					CHECK_SPACE(CVM_WORDS_PER_LONG);
					int64Value = VA_ARG(va, ILInt64);
					ILMemCpy(stacktop, &int64Value, sizeof(int64Value));
					stacktop += CVM_WORDS_PER_LONG;
				}
				break;

				case IL_META_ELEMTYPE_R4:
				case IL_META_ELEMTYPE_R8:
				case IL_META_ELEMTYPE_R:
				{
					CHECK_SPACE(CVM_WORDS_PER_NATIVE_FLOAT);
					fValue = (ILNativeFloat)(VA_ARG(va, ILVaDouble));
					ILMemCpy(stacktop, &fValue, sizeof(fValue));
					stacktop += CVM_WORDS_PER_NATIVE_FLOAT;
				}
				break;

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					/* We assume that typed references are passed to us
					   as a pointer to a temporary typedref structure */
					CHECK_SPACE(CVM_WORDS_PER_TYPED_REF);
					ptr = (void *)(VA_ARG(va, void *));
					stacktop += CVM_WORDS_PER_TYPED_REF;
				}
				break;
			}
		}
		else if(ILType_IsClass(paramType))
		{
			/* Process an object reference */
			CHECK_SPACE(1);
			stacktop->ptrValue = (void *)(VA_ARG(va, ILObject *));
			++stacktop;
		}
		else if(ILType_IsValueType(paramType))
		{
			/* Process a value type: we assume that the caller has
			   put the value into a temporary location and then
			   passed a pointer to the temporary to us */
			ptr = (void *)(VA_ARG(va, void *));
			size = ILSizeOfType(thread, paramType);
			sizeInWords = ((size + sizeof(CVMWord) - 1) / sizeof(CVMWord));
			CHECK_SPACE(sizeInWords);
			ILMemCpy(stacktop, ptr, size);
			stacktop += sizeInWords;
		}
		else if(paramType != 0 && ILType_IsComplex(paramType) &&
				paramType->kind == IL_TYPE_COMPLEX_BYREF)
		{
			/* Process a value that is being passed by reference */
			CHECK_SPACE(1);
			stacktop->ptrValue = (void *)(VA_ARG(va, void *));
			++stacktop;
		}
		else
		{
			/* Assume that everything else is an object reference */
			CHECK_SPACE(1);
			stacktop->ptrValue = (void *)(VA_ARG(va, ILObject *));
			++stacktop;
		}
	}

	/* Record the number of arguments for popping later */
	sizeInWords = thread->stackTop - thread->stackBase;
	thread->stackTop = stacktop;

	/* Clear the pending exception on entry to the method */
	thread->thrownException = 0;

	/* Create a call frame for the method */
	if(thread->numFrames >= thread->maxFrames)
	{
		/* TODO: throw a stack overflow exception */
	}
	frame = &(thread->frameStack[(thread->numFrames)++]);
	savePC = thread->pc;
	frame->method = thread->method;
	frame->pc = IL_MAX_UINT32;
	frame->frame = thread->frame;
	frame->except = IL_MAX_UINT32;

	/* Call the method */
	threwException = _ILCVMInterpreter(thread);
	if(threwException)
	{
		/* Pop the exception from the stack */
		thread->thrownException = thread->stackTop[-1].ptrValue;
		--(thread->stackTop);
	}

	/* Pop the call frame */
	thread->pc = savePC;
	thread->stackTop = thread->stackBase + sizeInWords;

	/* Done */
	return threwException;
}

static int CallVirtualMethod(ILExecThread *thread, ILMethod *method,
					  		 void *result, VA_LIST va)
{
	/* TODO */
	return CallMethod(thread, method, result, va);
}

int ILExecThreadCall(ILExecThread *thread, ILMethod *method,
					 void *result, ...)
{
	int threwException;
	VA_START(result);
	threwException = CallMethod(thread, method, result, VA_GET_LIST);
	VA_END;
	return threwException;
}

int ILExecThreadCallVirtual(ILExecThread *thread, ILMethod *method,
							void *result, ...)
{
	int threwException;
	VA_START(result);
	if(ILMethod_IsVirtual(method))
	{
		threwException = CallVirtualMethod(thread, method, result, VA_GET_LIST);
	}
	else
	{
		threwException = CallMethod(thread, method, result, VA_GET_LIST);
	}
	VA_END;
	return threwException;
}

int ILExecThreadCallNamed(ILExecThread *thread, const char *typeName,
						  const char *methodName, const char *signature,
						  void *result, ...)
{
	int threwException;
	ILMethod *method;
	VA_START(result);
	method = ILExecThreadLookupMethod(thread, typeName,
									  methodName, signature);
	if(!method)
	{
		/* End argument processing */
		VA_END;

		/* Construct and throw a "MissingMethodException" object */
		/* TODO */

		/* There is a pending exception waiting for the caller */
		return 1;
	}
	threwException = CallMethod(thread, method, result, VA_GET_LIST);
	VA_END;
	return threwException;
}

int ILExecThreadCallNamedVirtual(ILExecThread *thread, const char *typeName,
						         const char *methodName, const char *signature,
						         void *result, ...)
{
	int threwException;
	ILMethod *method;
	VA_START(result);
	method = ILExecThreadLookupMethod(thread, typeName,
									  methodName, signature);
	if(!method)
	{
		/* End argument processing */
		VA_END;

		/* Construct and throw a "MissingMethodException" object */
		/* TODO */

		/* There is a pending exception waiting for the caller */
		return 1;
	}
	if(ILMethod_IsVirtual(method))
	{
		threwException = CallVirtualMethod(thread, method, result, VA_GET_LIST);
	}
	else
	{
		threwException = CallMethod(thread, method, result, VA_GET_LIST);
	}
	VA_END;
	return threwException;
}

int ILExecThreadHasException(ILExecThread *thread)
{
	return (thread->thrownException != 0);
}

ILObject *ILExecThreadGetException(ILExecThread *thread)
{
	return thread->thrownException;
}

void ILExecThreadClearException(ILExecThread *thread)
{
	thread->thrownException = 0;
}

void ILExecThreadSetException(ILExecThread *thread, ILObject *obj)
{
	thread->thrownException = obj;
}

#ifdef	__cplusplus
};
#endif
