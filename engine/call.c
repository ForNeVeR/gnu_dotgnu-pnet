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
#include "lib_defs.h"
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
					thread->thrownException = _ILSystemException \
						(thread, "System.StackOverflowException"); \
					return 1; \
				} \
			} while (0)

int _ILCallPackVaParams(ILExecThread *thread, ILMethod *method,
					    int isCtor, void *_this, void *userData)
{
	VA_LIST va;
	ILType *signature = ILMethod_Signature(method);
	CVMWord *stacktop, *stacklimit;
	ILUInt32 param, numParams;
	ILType *paramType;
	void *ptr;
	ILUInt32 size, sizeInWords;
	ILInt64 int64Value;
	ILNativeFloat fValue;

	/* Copy the incoming "va_list" value */
	ILMemCpy(&va, userData, sizeof(VA_LIST));

	/* Get the top and extent of the stack */
	stacktop = thread->stackTop;
	stacklimit = thread->stackLimit;

	/* Push the arguments onto the evaluation stack */
	if(ILType_HasThis(signature) && !isCtor)
	{
		/* Push the "this" argument */
		CHECK_SPACE(1);
		if(_this)
		{
			/* Use the supplied "this" parameter */
			stacktop->ptrValue = _this;
		}
		else
		{
			/* Get the "this" parameter from the varargs list */
			stacktop->ptrValue = (void *)(VA_ARG(va, ILObject *));
		}
		++stacktop;
	}
	numParams = ILTypeNumParams(signature);
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
					ILMemCpy(stacktop, ptr, sizeof(ILTypedRef));
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
				ILType_Kind(paramType) == IL_TYPE_COMPLEX_BYREF)
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

	/* Update the stack top */
	thread->stackTop = stacktop;
	return 0;
}

int _ILCallPackVParams(ILExecThread *thread, ILMethod *method,
					   int isCtor, void *_this, void *userData)
{
	ILExecValue *args = (ILExecValue *)userData;
	ILType *signature = ILMethod_Signature(method);
	CVMWord *stacktop, *stacklimit;
	ILUInt32 param, numParams;
	ILType *paramType;
	void *ptr;
	ILUInt32 size, sizeInWords;

	/* Get the top and extent of the stack */
	stacktop = thread->stackTop;
	stacklimit = thread->stackLimit;

	/* Push the arguments onto the evaluation stack */
	if(ILType_HasThis(signature) && !isCtor)
	{
		/* Push the "this" argument */
		CHECK_SPACE(1);
		if(_this)
		{
			/* Use the supplied "this" parameter */
			stacktop->ptrValue = _this;
		}
		else
		{
			/* Get the "this" parameter from the argument list */
			stacktop->ptrValue = args->objValue;
			++args;
		}
		++stacktop;
	}
	numParams = ILTypeNumParams(signature);
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
					stacktop->intValue = args->int32Value;
					++args;
					++stacktop;
				}
				break;

				case IL_META_ELEMTYPE_U4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_U:
			#endif
				{
					CHECK_SPACE(1);
					stacktop->uintValue = args->uint32Value;
					++args;
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
					ILMemCpy(stacktop, &(args->int64Value), sizeof(ILInt64));
					++args;
					stacktop += CVM_WORDS_PER_LONG;
				}
				break;

				case IL_META_ELEMTYPE_R4:
				case IL_META_ELEMTYPE_R8:
				case IL_META_ELEMTYPE_R:
				{
					CHECK_SPACE(CVM_WORDS_PER_NATIVE_FLOAT);
					ILMemCpy(stacktop, &(args->floatValue),
							 sizeof(ILNativeFloat));
					++args;
					stacktop += CVM_WORDS_PER_NATIVE_FLOAT;
				}
				break;

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					CHECK_SPACE(CVM_WORDS_PER_TYPED_REF);
					ILMemCpy(stacktop, &(args->typedRefValue),
							 sizeof(ILTypedRef));
					++args;
					stacktop += CVM_WORDS_PER_TYPED_REF;
				}
				break;
			}
		}
		else if(ILType_IsClass(paramType))
		{
			/* Process an object reference */
			CHECK_SPACE(1);
			stacktop->ptrValue = args->objValue;
			++args;
			++stacktop;
		}
		else if(ILType_IsValueType(paramType))
		{
			/* Process a value type: we assume that the caller has
			   put the value into a temporary location and then
			   passed a pointer to the temporary to us */
			ptr = args->ptrValue;
			++args;
			size = ILSizeOfType(thread, paramType);
			sizeInWords = ((size + sizeof(CVMWord) - 1) / sizeof(CVMWord));
			CHECK_SPACE(sizeInWords);
			ILMemCpy(stacktop, ptr, size);
			stacktop += sizeInWords;
		}
		else if(paramType != 0 && ILType_IsComplex(paramType) &&
				ILType_Kind(paramType) == IL_TYPE_COMPLEX_BYREF)
		{
			/* Process a value that is being passed by reference */
			CHECK_SPACE(1);
			stacktop->ptrValue = args->ptrValue;
			++args;
			++stacktop;
		}
		else
		{
			/* Assume that everything else is an object reference */
			CHECK_SPACE(1);
			stacktop->ptrValue = args->objValue;
			++args;
			++stacktop;
		}
	}

	/* Update the stack top */
	thread->stackTop = stacktop;
	return 0;
}

void _ILCallUnpackDirectResult(ILExecThread *thread, ILMethod *method,
					           int isCtor, void *result, void *userData)
{
	ILType *signature = ILMethod_Signature(method);
	ILType *paramType;
	ILUInt32 size, sizeInWords;
	ILNativeFloat fValue;

	if(isCtor)
	{
		/* Copy the returned object value */
		*((void **)result) = thread->stackTop[-1].ptrValue;
		--(thread->stackTop);
	}
	else
	{
		/* Copy the return value into place */
		paramType = ILTypeGetEnumType(ILTypeGetReturn(signature));
		if(ILType_IsPrimitive(paramType))
		{
			/* Process a primitive value */
			switch(ILType_ToElement(paramType))
			{
				case IL_META_ELEMTYPE_VOID:		break;

				case IL_META_ELEMTYPE_BOOLEAN:
				case IL_META_ELEMTYPE_I1:
				case IL_META_ELEMTYPE_U1:
				{
					*((ILInt8 *)result) =
						(ILInt8)(thread->stackTop[-1].intValue);
					--(thread->stackTop);
				}
				break;

				case IL_META_ELEMTYPE_I2:
				case IL_META_ELEMTYPE_U2:
				case IL_META_ELEMTYPE_CHAR:
				{
					*((ILInt16 *)result) =
						(ILInt16)(thread->stackTop[-1].intValue);
					--(thread->stackTop);
				}
				break;

				case IL_META_ELEMTYPE_I4:
				case IL_META_ELEMTYPE_U4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					*((ILInt32 *)result) = thread->stackTop[-1].intValue;
					--(thread->stackTop);
				}
				break;

				case IL_META_ELEMTYPE_I8:
				case IL_META_ELEMTYPE_U8:
			#ifdef IL_NATIVE_INT64
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					ILMemCpy(result, thread->stackTop - CVM_WORDS_PER_LONG,
							 sizeof(ILInt64));
					thread->stackTop -= CVM_WORDS_PER_LONG;
				}
				break;

				case IL_META_ELEMTYPE_R4:
				{
					ILMemCpy(&fValue,
							 thread->stackTop - CVM_WORDS_PER_NATIVE_FLOAT,
							 sizeof(ILNativeFloat));
					*((ILFloat *)result) = (ILFloat)fValue;
					thread->stackTop -= CVM_WORDS_PER_NATIVE_FLOAT;
				}
				break;

				case IL_META_ELEMTYPE_R8:
				{
					ILMemCpy(&fValue,
							 thread->stackTop - CVM_WORDS_PER_NATIVE_FLOAT,
							 sizeof(ILNativeFloat));
					*((ILDouble *)result) = (ILDouble)fValue;
					thread->stackTop -= CVM_WORDS_PER_NATIVE_FLOAT;
				}
				break;

				case IL_META_ELEMTYPE_R:
				{
					ILMemCpy(result,
							 thread->stackTop - CVM_WORDS_PER_NATIVE_FLOAT,
							 sizeof(ILNativeFloat));
					thread->stackTop -= CVM_WORDS_PER_NATIVE_FLOAT;
				}
				break;

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					ILMemCpy(result,
							 thread->stackTop - CVM_WORDS_PER_TYPED_REF,
							 sizeof(ILTypedRef));
					thread->stackTop -= CVM_WORDS_PER_TYPED_REF;
				}
				break;
			}
		}
		else if(ILType_IsClass(paramType))
		{
			/* Process an object reference */
			*((void **)result) = thread->stackTop[-1].ptrValue;
			--(thread->stackTop);
		}
		else if(ILType_IsValueType(paramType))
		{
			/* Process a value type */
			size = ILSizeOfType(thread, paramType);
			sizeInWords = ((size + sizeof(CVMWord) - 1) / sizeof(CVMWord));
			ILMemCpy(result, thread->stackTop - sizeInWords, size);
			thread->stackTop -= sizeInWords;
		}
		else
		{
			/* Assume that everything else is an object reference */
			*((void **)result) = thread->stackTop[-1].ptrValue;
			--(thread->stackTop);
		}
	}
}

void _ILCallUnpackVResult(ILExecThread *thread, ILMethod *method,
				          int isCtor, void *_result, void *userData)
{
	ILExecValue *result = (ILExecValue *)_result;
	ILType *signature = ILMethod_Signature(method);
	ILType *paramType;
	ILUInt32 size, sizeInWords;

	if(isCtor)
	{
		/* Copy the returned object value */
		result->objValue = (ILObject *)(thread->stackTop[-1].ptrValue);
		--(thread->stackTop);
	}
	else
	{
		/* Copy the return value into place */
		paramType = ILTypeGetEnumType(ILTypeGetReturn(signature));
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
				case IL_META_ELEMTYPE_U4:
			#ifdef IL_NATIVE_INT32
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					result->int32Value = thread->stackTop[-1].intValue;
					--(thread->stackTop);
				}
				break;

				case IL_META_ELEMTYPE_I8:
				case IL_META_ELEMTYPE_U8:
			#ifdef IL_NATIVE_INT64
				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
			#endif
				{
					ILMemCpy(&(result->int64Value),
							 thread->stackTop - CVM_WORDS_PER_LONG,
							 sizeof(ILInt64));
					thread->stackTop -= CVM_WORDS_PER_LONG;
				}
				break;

				case IL_META_ELEMTYPE_R4:
				case IL_META_ELEMTYPE_R8:
				case IL_META_ELEMTYPE_R:
				{
					ILMemCpy(&(result->floatValue),
							 thread->stackTop - CVM_WORDS_PER_NATIVE_FLOAT,
							 sizeof(ILNativeFloat));
					thread->stackTop -= CVM_WORDS_PER_NATIVE_FLOAT;
				}
				break;

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					ILMemCpy(&(result->typedRefValue),
							 thread->stackTop - CVM_WORDS_PER_TYPED_REF,
							 sizeof(ILTypedRef));
					thread->stackTop -= CVM_WORDS_PER_TYPED_REF;
				}
				break;
			}
		}
		else if(ILType_IsClass(paramType))
		{
			/* Process an object reference */
			result->ptrValue = thread->stackTop[-1].ptrValue;
			--(thread->stackTop);
		}
		else if(ILType_IsValueType(paramType))
		{
			/* Process a value type */
			size = ILSizeOfType(thread, paramType);
			sizeInWords = ((size + sizeof(CVMWord) - 1) / sizeof(CVMWord));
			ILMemCpy(result->ptrValue, thread->stackTop - sizeInWords, size);
			thread->stackTop -= sizeInWords;
		}
		else
		{
			/* Assume that everything else is an object reference */
			result->ptrValue = thread->stackTop[-1].ptrValue;
			--(thread->stackTop);
		}
	}
}

int _ILCallMethod(ILExecThread *thread, ILMethod *method,
				  ILCallUnpackFunc unpack, void *result,
				  int isCtor, void *_this,
				  ILCallPackFunc pack, void *userData)
{
	ILCallFrame *frame;
	unsigned char *savePC;
	int threwException;
	unsigned char *pcstart;

	/* Push the arguments onto the evaluation stack */
	if((*pack)(thread, method, isCtor, _this, userData))
	{
		return 1;
	}

	/* Clear the pending exception on entry to the method */
	thread->thrownException = 0;

	/* Convert the method into CVM bytecode */
	pcstart = _ILConvertMethod(thread, method);
	if(!pcstart)
	{
		/* "_ILConvertMethod" threw an exception */
		return 1;
	}

	/* Create a call frame for the method */
	if(thread->numFrames >= thread->maxFrames)
	{
	    if((frame = _ILAllocCallFrame(thread)) == 0)
		{
			thread->thrownException = _ILSystemException
				(thread, "System.StackOverflowException");
			return 1;
	    }
	}
	else
	{
		frame = &(thread->frameStack[(thread->numFrames)++]);
	}
	savePC = thread->pc;
	frame->method = thread->method;
	frame->pc = IL_INVALID_PC;
	frame->frame = thread->frame;
	frame->exceptHeight = thread->exceptHeight;
	frame->permissions = 0;

	/* Call the method */
	if(isCtor)
	{
		/* We are calling the allocation constructor, which starts
		   several bytes before the actual method entry point */
		thread->pc = pcstart - ILCoderCtorOffset(thread->process->coder);
	}
	else
	{
		thread->pc = pcstart;
	}
	thread->exceptHeight = 0;
	thread->method = method;
	threwException = _ILCVMInterpreter(thread);
	if(threwException)
	{
		/* An exception occurred, which is already stored in the thread */
	}
	else
	{
		/* Unpack the return value */
		(*unpack)(thread, method, isCtor, result, userData);
	}

	/* Restore the original PC: everything else was restored
	   by the "return" instruction within the interpreter */
	thread->pc = savePC;

	/* Done */
	return threwException;
}

ILMethod *_ILLookupInterfaceMethod(ILClassPrivate *objectClassPrivate,
								   ILClass *interfaceClass,
								   ILUInt32 index)
{
	ILImplPrivate *implements;
	ILClassPrivate *searchClass = objectClassPrivate;
	ILClass *parent;

	/* Locate the interface table within the class hierarchy for the object */
	while(searchClass != 0)
	{
		implements = searchClass->implements;
		while(implements != 0)
		{
			if(implements->interface == interfaceClass)
			{
				/* We've found the interface, so look in the interface
				   table to find the vtable slot, which is then used to
				   look in the class's vtable for the actual method */
				index = (ILUInt32)((ILImplPrivate_Table(implements))[index]);
				if(index != (ILUInt32)(ILUInt16)0xFFFF)
				{
					return objectClassPrivate->vtable[index];
				}
				else
				{
					/* The interface slot is abstract.  This shouldn't
					   happen in practice, but let's be paranoid anyway */
					return 0;
				}
			}
			implements = implements->next;
		}
		parent = ILClassGetParent(searchClass->classInfo);
		if(!parent)
		{
			break;
		}
		searchClass = (ILClassPrivate *)(parent->userData);
	}

	/* The interface implementation was not found */
	return 0;
}

/*
 * Throw a missing method exception.
 */
static void ThrowMethodMissing(ILExecThread *thread)
{
	ILExecThreadThrowSystem(thread, "System.MissingMethodException",
							(const char *)0);
}

static int CallVirtualMethod(ILExecThread *thread, ILMethod *method,
					  		 void *result, void *_this, VA_LIST va)
{
	ILClass *classInfo;
	ILClass *objectClass;

	/* Throw a "NullReferenceException" if "this" is NULL */
	if(!_this)
	{
		ILExecThreadThrowSystem(thread, "System.NullReferenceException",
								(const char *)0);
		return 1;
	}

	/* The calling sequence is handled differently depending
	   upon whether the method is normal, interface, or virtual */
	if((method->member.attributes & IL_META_METHODDEF_VIRTUAL) == 0)
	{
		/* This is a normal method which has been called incorrectly */
		return _ILCallMethod(thread, method,
							 _ILCallUnpackDirectResult, result,
							 0, _this,
							 _ILCallPackVaParams, &va);
	}
	classInfo = method->member.owner;
	objectClass = GetObjectClass(_this);
	if((classInfo->attributes & IL_META_TYPEDEF_CLASS_SEMANTICS_MASK)
			== IL_META_TYPEDEF_INTERFACE)
	{
		/* This is an interface method call */
		if(ILClassImplements(objectClass, classInfo))
		{
			method = _ILLookupInterfaceMethod
				(((ILClassPrivate *)(objectClass->userData)),
				 classInfo, method->index);
			if(method)
			{
				return _ILCallMethod(thread, method,
									 _ILCallUnpackDirectResult, result,
									 0, _this,
									 _ILCallPackVaParams, &va);
			}
		}
	}
	else
	{
		/* This is an ordinary virtual method call */
		if(ILClassInheritsFrom(objectClass, classInfo))
		{
			method = ((ILClassPrivate *)(objectClass->userData))->
							vtable[method->index];
			if(method)
			{
				return _ILCallMethod(thread, method,
									 _ILCallUnpackDirectResult, result,
									 0, _this,
									 _ILCallPackVaParams, &va);
			}
		}
	}

	/* If we get here, then we could not resolve the virtual */
	ThrowMethodMissing(thread);
	return 1;
}

static int CallVirtualMethodV(ILExecThread *thread, ILMethod *method,
					  		  ILExecValue *result, void *_this,
							  ILExecValue *args)
{
	ILClass *classInfo;
	ILClass *objectClass;

	/* Throw a "NullReferenceException" if "this" is NULL */
	if(!_this)
	{
		ILExecThreadThrowSystem(thread, "System.NullReferenceException",
								(const char *)0);
		return 1;
	}

	/* The calling sequence is handled differently depending
	   upon whether the method is normal, interface, or virtual */
	if((method->member.attributes & IL_META_METHODDEF_VIRTUAL) == 0)
	{
		/* This is a normal method which has been called incorrectly */
		return _ILCallMethod(thread, method,
							 _ILCallUnpackVResult, result,
							 0, _this,
							 _ILCallPackVParams, args);
	}
	classInfo = method->member.owner;
	objectClass = GetObjectClass(_this);
	if((classInfo->attributes & IL_META_TYPEDEF_CLASS_SEMANTICS_MASK)
			== IL_META_TYPEDEF_INTERFACE)
	{
		/* This is an interface method call */
		if(ILClassImplements(objectClass, classInfo))
		{
			method = _ILLookupInterfaceMethod
					(((ILClassPrivate *)(objectClass->userData)),
					 classInfo, method->index);
			if(method)
			{
				return _ILCallMethod(thread, method,
									 _ILCallUnpackVResult, result,
									 0, _this,
									 _ILCallPackVParams, args);
			}
		}
	}
	else
	{
		/* This is an ordinary virtual method call */
		if(ILClassInheritsFrom(objectClass, classInfo))
		{
			method = ((ILClassPrivate *)(classInfo->userData))->
							vtable[method->index];
			if(method)
			{
				return _ILCallMethod(thread, method,
									 _ILCallUnpackVResult, result,
									 0, _this,
									 _ILCallPackVParams, args);
			}
		}
	}

	/* If we get here, then we could not resolve the virtual */
	ThrowMethodMissing(thread);
	return 1;
}

int ILExecThreadCall(ILExecThread *thread, ILMethod *method,
					 void *result, ...)
{
	int threwException;
	VA_START(result);
	threwException = _ILCallMethod(thread, method,
								   _ILCallUnpackDirectResult, result,
								   0, 0,
								   _ILCallPackVaParams, &VA_GET_LIST);
	VA_END;
	return threwException;
}

int ILExecThreadCallV(ILExecThread *thread, ILMethod *method,
					  ILExecValue *result, ILExecValue *args)
{
	return _ILCallMethod(thread, method,
						 _ILCallUnpackVResult, result,
						 0, 0,
						 _ILCallPackVParams, args);
}

ILObject *ILExecThreadCallCtorV(ILExecThread *thread, ILMethod *ctor,
                                ILExecValue *args)
{
	ILObject *result = 0;
	if(_ILCallMethod(thread, ctor,
					 _ILCallUnpackDirectResult, &result,
					 1, 0,
					 _ILCallPackVParams, args))
	{
		return 0;
	}
	else
	{
		return result;
	}
}

int ILExecThreadCallVirtual(ILExecThread *thread, ILMethod *method,
							void *result, void *_this, ...)
{
	int threwException;
	VA_START(_this);
	threwException = CallVirtualMethod
						(thread, method, result, _this, VA_GET_LIST);
	VA_END;
	return threwException;
}

int ILExecThreadCallVirtualV(ILExecThread *thread, ILMethod *method,
							 ILExecValue *result, void *_this,
							 ILExecValue *args)
{
	return CallVirtualMethodV(thread, method, result, _this, args);
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
		ThrowMethodMissing(thread);

		/* There is a pending exception waiting for the caller */
		return 1;
	}
	threwException = _ILCallMethod(thread, method,
								   _ILCallUnpackDirectResult, result,
								   0, 0,
								   _ILCallPackVaParams, &VA_GET_LIST);
	VA_END;
	return threwException;
}

int ILExecThreadCallNamedV(ILExecThread *thread, const char *typeName,
						   const char *methodName, const char *signature,
						   ILExecValue *result, ILExecValue *args)
{
	ILMethod *method;
	method = ILExecThreadLookupMethod(thread, typeName,
									  methodName, signature);
	if(!method)
	{
		/* Construct and throw a "MissingMethodException" object */
		ThrowMethodMissing(thread);

		/* There is a pending exception waiting for the caller */
		return 1;
	}
	return _ILCallMethod(thread, method,
						 _ILCallUnpackVResult, result,
						 0, 0,
						 _ILCallPackVParams, args);
}

int ILExecThreadCallNamedVirtual(ILExecThread *thread, const char *typeName,
						         const char *methodName, const char *signature,
						         void *result, void *_this, ...)
{
	int threwException;
	ILMethod *method;
	VA_START(_this);
	method = ILExecThreadLookupMethod(thread, typeName,
									  methodName, signature);
	if(!method)
	{
		/* End argument processing */
		VA_END;

		/* Construct and throw a "MissingMethodException" object */
		ThrowMethodMissing(thread);

		/* There is a pending exception waiting for the caller */
		return 1;
	}
	threwException = CallVirtualMethod
						(thread, method, result, _this, VA_GET_LIST);
	VA_END;
	return threwException;
}

int ILExecThreadCallNamedVirtualV(ILExecThread *thread, const char *typeName,
						          const char *methodName, const char *signature,
						          ILExecValue *result, void *_this,
								  ILExecValue *args)
{
	ILMethod *method;
	method = ILExecThreadLookupMethod(thread, typeName,
									  methodName, signature);
	if(!method)
	{
		/* Construct and throw a "MissingMethodException" object */
		ThrowMethodMissing(thread);

		/* There is a pending exception waiting for the caller */
		return 1;
	}
	return CallVirtualMethodV(thread, method, result, _this, args);
}

ILObject *ILExecThreadNew(ILExecThread *thread, const char *typeName,
						  const char *signature, ...)
{
	ILMethod *ctor;
	ILClass *classInfo;
	ILObject *result;
	VA_START(signature);

	/* Find the constructor */
	ctor = ILExecThreadLookupMethod(thread, typeName, ".ctor", signature);
	if(!ctor)
	{
		/* Throw a "MissingMethodException" */
		VA_END;
		ThrowMethodMissing(thread);
		return 0;
	}

	/* Make sure that the class has been initialized */
	classInfo = ILMethod_Owner(ctor);
	IL_METADATA_WRLOCK(_ILExecThreadProcess(thread));
	if(!_ILLayoutClass(_ILExecThreadProcess(thread), classInfo))
	{
		/* Throw a "TypeLoadException" */
		IL_METADATA_UNLOCK(_ILExecThreadProcess(thread));
		VA_END;
		ILExecThreadThrowSystem(thread, "System.TypeLoadException",
								(const char *)0);
		return 0;
	}
	IL_METADATA_UNLOCK(_ILExecThreadProcess(thread));

	/* Call the constructor */
	result = 0;
	if(_ILCallMethod(thread, ctor,
					 _ILCallUnpackDirectResult, &result,
					 1, 0,
					 _ILCallPackVaParams, &VA_GET_LIST))
	{
		/* The constructor threw an exception */
		VA_END;
		return 0;
	}
	VA_END;
	return result;
}

ILObject *ILExecThreadNewV(ILExecThread *thread, const char *typeName,
						   const char *signature, ILExecValue *args)
{
	ILMethod *ctor;
	ILClass *classInfo;
	ILObject *result;

	/* Find the constructor */
	ctor = ILExecThreadLookupMethod(thread, typeName, ".ctor", signature);
	if(!ctor)
	{
		/* Throw a "MissingMethodException" */
		ThrowMethodMissing(thread);
		return 0;
	}

	/* Make sure that the class has been initialized */
	classInfo = ILMethod_Owner(ctor);
	IL_METADATA_WRLOCK(_ILExecThreadProcess(thread));
	if(!_ILLayoutClass(_ILExecThreadProcess(thread), classInfo))
	{
		/* Throw a "TypeLoadException" */
		IL_METADATA_UNLOCK(_ILExecThreadProcess(thread));
		ILExecThreadThrowSystem(thread, "System.TypeLoadException",
								(const char *)0);
		return 0;
	}
	IL_METADATA_UNLOCK(_ILExecThreadProcess(thread));

	/* Call the constructor */
	result = 0;
	if(_ILCallMethod(thread, ctor,
					 _ILCallUnpackDirectResult, &result,
					 1, 0,
					 _ILCallPackVParams, args))
	{
		/* The constructor threw an exception */
		return 0;
	}
	return result;
}

#ifdef	__cplusplus
};
#endif
