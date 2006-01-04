/*
 * jitc.h - Definitions for the JIT coder.
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

#ifndef	_ENGINE_JITC_H
#define	_ENGINE_JITC_H

#ifdef IL_USE_JIT

#include "jit/jit.h"

/*
 * Map the native IL types to JIT types.
 */
#define _IL_JIT_TYPE_BYTE	jit_type_ubyte
#define _IL_JIT_TYPE_CHAR	jit_type_ushort
#define _IL_JIT_TYPE_DOUBLE	jit_type_float64
#define _IL_JIT_TYPE_INT16	jit_type_short
#define _IL_JIT_TYPE_INT32	jit_type_int
#define _IL_JIT_TYPE_INT64	jit_type_long
#define _IL_JIT_TYPE_NINT	jit_type_nint
#define _IL_JIT_TYPE_INTPTR	jit_type_void_ptr
#define _IL_JIT_TYPE_NFLOAT	jit_type_nfloat
#define _IL_JIT_TYPE_SBYTE	jit_type_sbyte
#define _IL_JIT_TYPE_SINGLE	jit_type_float32
#define _IL_JIT_TYPE_UINT16	jit_type_ushort
#define _IL_JIT_TYPE_UINT32	jit_type_uint
#define _IL_JIT_TYPE_UINT64	jit_type_ulong
#define _IL_JIT_TYPE_NUINT	jit_type_nuint
#define _IL_JIT_TYPE_VPTR	jit_type_void_ptr
 
/*
 * Definition of method metadata indexes.
 */
#define IL_JIT_META_METHOD 1


/*
 * Defaut calling convention for libjit.
 */
#define IL_JIT_CALLCONV_DEFAULT		jit_abi_cdecl


/*
 * Calling conventions for libjit.
 */
#define IL_JIT_CALLCONV_CDECL		jit_abi_cdecl
#define IL_JIT_CALLCONV_VARARG		jit_abi_vararg
#define IL_JIT_CALLCONV_STDCALL		jit_abi_stdcall
#define IL_JIT_CALLCONV_FASTCALL	jit_abi_fastcall

/*
 * Representation of a type representation for libjit.
 */
typedef jit_type_t		ILJitType;

/*
 * Representation of a method representation for libjit.
 */
typedef jit_function_t	ILJitFunction;

/*
 * Representation of a jit value.
 */
typedef jit_value_t		ILJitValue;

/*
 * Structure to hold the common types needed for jitc for a clr type.
 */
typedef struct _tagILJitTypes ILJitTypes;
struct _tagILJitTypes
{
	ILJitType	jitTypeBase;	/* the base type (class or struct) */
	ILJitType	jitTypePtr;		/* the pointer type pointing to the base type. */
	ILJitType	jitTypeRef;		/* the type to pass the base type as a byref arg. */
	ILJitType	jitTypeOut;		/* the type to pass the base type as an out arg. */
};

/*
 * Initialize a ILJitTypes structure 
 */
#define _ILJitTypesInit(jitTypes) \
	{ \
		(jitTypes)->jitTypeBase = 0; \
		(jitTypes)->jitTypePtr = 0; \
		(jitTypes)->jitTypeRef = 0; \
		(jitTypes)->jitTypeOut = 0; \
	}

/*
 * Initialize the libjit coder.
 * Returns 1 on success and 0 on failure.
 */
int ILJitInit();

/*
 * Create the jit function header for an ILMethod.
 * We allways pass the ILExecThread as arg 0.
 * Returns 1 on success and 0 on error.
 */
int ILJitFunctionCreate(ILCoder *_coder, ILMethod *method);

/*
 * Create all jitMethods for the given class.
 * Returns 1 on success and 0 on error.
 */
int ILJitCreateFunctionsForClass(ILCoder *_coder, ILClass *info);

/*
 * Get the ILJitFunction for an ILMethod.
 * Returns 0 if the jit function stub isn't created yet.
 */
ILJitFunction ILJitFunctionFromILMethod(ILMethod *method);

/*
 * Call the jit function for an ILMethod.
 * Returns 1 if an exception occured.
 */
int ILJitCallMethod(ILExecThread *thread, ILMethod *method,
					void**jitArgs, void *result);

/*
 * Create the class/struct representation of a clr type for libjit.
 * and store the type in classPrivate.
 * Returns the jit type on success else 0
 */
int ILJitTypeCreate(ILClassPrivate *classPrivate);

/*
 * Destroy the given type in libjit.
 */
void ILJitTypeDestroy(ILJitType type);

/*
 * Destroy every ILJitType in a ILJitTypes  structure 
 */
void ILJitTypesDestroy(ILJitTypes *jitTypes);

/*
 * Get the jit type for a primitive clr type.
 * Returns 0 if primitiveClrType is not a primitive clr type.
 */
ILJitTypes *ILJitPrimitiveClrTypeToJitTypes(int primitiveClrType);

/*
 * Get the alignment for a jit type.
 */
#define ILJitTypeGetAlignment(jitType) jit_type_get_alignment((jitType))

/*
 * Get the zize of a jit type in bytes.
 */
#define ILJitTypeGetSize(jitType)      jit_type_get_size((jitType))

#endif  /* IL_USE_JIT */

#endif	/* _ENGINE_JITC_H */
