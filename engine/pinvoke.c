/*
 * pinvoke.c - Handle PInvoke and "internalcall" methods within the engine.
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
#include "ffi.h"
#include "ffi_private.h"

/*
 * The hard work is done using "libffi", which is a generic library
 * for packing up the arguments for an arbitrary function call,
 * making the call, and then getting the return value.
 *
 * The main difference between "PInvoke" and "internalcall" is the
 * calling conventions.  All "internalcall" methods are passed an
 * extra first parameter which points to the "ILExecThread" that
 * the method was invoked within.  This is necessary because many
 * "internalcall" functions deal with runtime data structures.
 *
 * PInvoke methods are assumed to be external to the runtime engine
 * and so don't need "ILExecThread" passed to them.
 *
 * Further information on how this works can be found in "doc/pinvoke.html".
 */

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Structure type for passing typed references on the stack.
 */
static ffi_type *typedref_members[] =
{
	&ffi_type_pointer,
	&ffi_type_pointer,
	0
};
static ffi_type ffi_type_typedref =
{
	0,					/* size */
	0,					/* alignment */
	FFI_TYPE_STRUCT,	/* type */
	typedref_members,	/* elements */
};

/*
 * Convert an IL type into an "ffi" type descriptor.
 */
static ffi_type *TypeToFFI(ILType *type)
{
	if(ILType_IsPrimitive(type))
	{
		/* Determine how to marshal a primitive type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:		return &ffi_type_void;
			case IL_META_ELEMTYPE_BOOLEAN:	return &ffi_type_sint8;
			case IL_META_ELEMTYPE_I1:		return &ffi_type_sint8;
			case IL_META_ELEMTYPE_U1:		return &ffi_type_uint8;
			case IL_META_ELEMTYPE_I2:		return &ffi_type_sint16;
			case IL_META_ELEMTYPE_U2:		return &ffi_type_uint16;
			case IL_META_ELEMTYPE_CHAR:		return &ffi_type_uint16;
			case IL_META_ELEMTYPE_I4:		return &ffi_type_sint32;
			case IL_META_ELEMTYPE_U4:		return &ffi_type_uint32;
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_I:		return &ffi_type_sint32;
			case IL_META_ELEMTYPE_U:		return &ffi_type_uint32;
		#else
			case IL_META_ELEMTYPE_I:		return &ffi_type_sint64;
			case IL_META_ELEMTYPE_U:		return &ffi_type_uint64;
		#endif
			case IL_META_ELEMTYPE_I8:		return &ffi_type_sint64;
			case IL_META_ELEMTYPE_U8:		return &ffi_type_uint64;
			case IL_META_ELEMTYPE_R4:		return &ffi_type_float;
			case IL_META_ELEMTYPE_R8:		return &ffi_type_double;
		#ifdef IL_NATIVE_FLOAT
			case IL_META_ELEMTYPE_R:		return &ffi_type_longdouble;
		#else
			case IL_META_ELEMTYPE_R:		return &ffi_type_double;
		#endif
			case IL_META_ELEMTYPE_TYPEDBYREF: return &ffi_type_typedref;
		}
		return &ffi_type_pointer;
	}
	else
	{
		/* Everything else is passed as a pointer */
		return &ffi_type_pointer;
	}
}

void *_ILMakeCifForMethod(ILCoder *coder, ILMethod *method, int isInternal)
{
	ILType *signature = ILMethod_Signature(method);
	ILType *returnType = ILTypeGetEnumType(ILTypeGetReturn(signature));
	ILUInt32 numArgs;
	ILUInt32 numParams;
	ffi_cif *cif;
	ffi_type **args;
	ffi_type *rtype;
	ILUInt32 arg;
	ILUInt32 param;

	/* Determine the number of argument blocks that we need */
	numArgs = numParams = ILTypeNumParams(signature);
	if(ILType_HasThis(signature))
	{
		++numArgs;
	}
	if(isInternal)
	{
		/* This is an "internalcall" or "runtime" method
		   which needs an extra argument for the thread */
		++numArgs;
	}
	if(ILType_IsValueType(returnType))
	{
		/* We need an extra argument to pass a pointer to
		   the buffer to use to return the result */
		++numArgs;
	}

	/* Allocate space for the cif within the coder's memory space */
	cif = ILCoderAlloc(coder, sizeof(ffi_cif) +
							  sizeof(ffi_type *) * numArgs);
	if(!cif)
	{
		return 0;
	}
	args = ((ffi_type **)(cif + 1));

	/* Convert the return type */
	rtype = TypeToFFI(returnType);

	/* Convert the argument types */
	arg = 0;
	if(isInternal)
	{
		/* Pointer argument for the thread */
		args[arg++] = &ffi_type_pointer;
	}
	if(ILType_IsValueType(returnType))
	{
		/* Pointer argument for value type returns */
		args[arg++] = &ffi_type_pointer;
	}
	if(ILType_HasThis(signature))
	{
		/* Pointer argument for "this" */
		args[arg++] = &ffi_type_pointer;
	}
	for(param = 1; param <= numParams; ++param)
	{
		args[arg++] = TypeToFFI(ILTypeGetEnumType
									(ILTypeGetParam(signature, param)));
	}

	/* Prepare the "ffi_cif" structure for the call */
	ffi_prep_cif(cif, FFI_DEFAULT_ABI, numArgs, rtype, args);

	/* Ready to go */
	return (void *)cif;
}

void *_ILMakeCifForConstructor(ILCoder *coder, ILMethod *method,
							   int isInternal)
{
	ILType *signature = ILMethod_Signature(method);
	ILUInt32 numArgs;
	ILUInt32 numParams;
	ffi_cif *cif;
	ffi_type **args;
	ffi_type *rtype;
	ILUInt32 arg;
	ILUInt32 param;

	/* Determine the number of argument blocks that we need */
	numArgs = numParams = ILTypeNumParams(signature);
	if(isInternal)
	{
		/* This is an "internalcall" or "runtime" method
		   which needs an extra argument for the thread */
		++numArgs;
	}

	/* Allocate space for the cif within the coder's memory space */
	cif = ILCoderAlloc(coder, sizeof(ffi_cif) +
							  sizeof(ffi_type *) * numArgs);
	if(!cif)
	{
		return 0;
	}
	args = ((ffi_type **)(cif + 1));

	/* The return value is always a pointer, indicating the object
	   that was just allocated by the constructor */
	rtype = &ffi_type_pointer;

	/* Convert the argument types */
	arg = 0;
	if(isInternal)
	{
		/* Pointer argument for the thread */
		args[arg++] = &ffi_type_pointer;
	}
	for(param = 1; param <= numParams; ++param)
	{
		args[arg++] = TypeToFFI(ILTypeGetEnumType
									(ILTypeGetParam(signature, param)));
	}

	/* Prepare the "ffi_cif" structure for the call */
	ffi_prep_cif(cif, FFI_DEFAULT_ABI, numArgs, rtype, args);

	/* Ready to go */
	return (void *)cif;
}

#ifdef	__cplusplus
};
#endif
