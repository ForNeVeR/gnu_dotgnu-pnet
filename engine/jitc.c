/*
 * jitc.c - Coder implementation for JIT output.
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
#include "il_coder.h"

#ifdef IL_USE_JIT

#include "il_opcodes.h"
#include "il_utils.h"
#ifndef IL_WITHOUT_TOOLS
#include "il_dumpasm.h"
#endif
#include "lib_defs.h"
#include "jitc.h"

#ifdef	__cplusplus
extern	"C" {
#endif

static struct _tagILJitTypes _ILJitType_VOID;
static struct _tagILJitTypes _ILJitType_BOOLEAN;
static struct _tagILJitTypes _ILJitType_BYTE;
static struct _tagILJitTypes _ILJitType_CHAR;
static struct _tagILJitTypes _ILJitType_I;
static struct _tagILJitTypes _ILJitType_I2;
static struct _tagILJitTypes _ILJitType_I4;
static struct _tagILJitTypes _ILJitType_I8;
static struct _tagILJitTypes _ILJitType_NFLOAT;
static struct _tagILJitTypes _ILJitType_R4;
static struct _tagILJitTypes _ILJitType_R8;
static struct _tagILJitTypes _ILJitType_SBYTE;
static struct _tagILJitTypes _ILJitType_U;
static struct _tagILJitTypes _ILJitType_U2;
static struct _tagILJitTypes _ILJitType_U4;
static struct _tagILJitTypes _ILJitType_U8;
static struct _tagILJitTypes _ILJitType_VPTR;

/*
 * Define the structure of a JIT coder's instance block.
 */
typedef struct _tagILJITCoder ILJITCoder;
struct _tagILJITCoder
{
	ILCoder			coder;
	ILExecProcess  *process;
	jit_context_t   context;

	ILMethod	   *currentMethod;
	int				debugEnabled;
	jit_value_t	   *jitStack;
	int				stackSize;
	int				stackTop;
};

/*
 * Convert a pointer to an ILCoder to a pointer to the ILJITVoder instance
 */
#define _ILCoderToILJITCoder(coder) ((ILJITCoder *)coder)

/*
 * Initialize a ILJitTypes base structure 
 */
#define _ILJitTypesInitBase(jitTypes, jitType) \
	{ \
		(jitTypes)->jitTypeBase = (jitType); \
		(jitTypes)->jitTypePtr = 0; \
		(jitTypes)->jitTypeRef = 0; \
		(jitTypes)->jitTypeOut = 0; \
	}

/*
 * Allocate enough space for "n" values on the stack.
 */
#define	ALLOC_STACK(coder, n)	\
			do { \
				if(n > (coder)->stackSize) \
				{ \
					ILJitValue *newStack = \
						(ILJitValue *)ILRealloc((coder)->stack, \
											  n * sizeof(ILJitValue)); \
					if(!newStack) \
					{ \
						return 0; \
					} \
					(coder)->stack = newStack; \
					(coder)->stackSize = n; \
				} \
			} while (0)


/*
 * Destroy every ILJitType in a ILJitTypes  structure 
 */
void ILJitTypesDestroy(ILJitTypes *jitTypes)
{
	if(jitTypes->jitTypeOut)
	{
		ILJitTypeDestroy(jitTypes->jitTypeOut);
		jitTypes->jitTypeOut = 0;
	}
	if(jitTypes->jitTypeRef)
	{
		ILJitTypeDestroy(jitTypes->jitTypeRef);
		jitTypes->jitTypeRef = 0;
	}
	if(jitTypes->jitTypePtr)
	{
		ILJitTypeDestroy(jitTypes->jitTypePtr);
		jitTypes->jitTypePtr = 0;
	}
	if(jitTypes->jitTypeBase)
	{
		ILJitTypeDestroy(jitTypes->jitTypeBase);
		jitTypes->jitTypeBase = 0;
	}
}

/*
 * Get the jit types for this ILType.
 */
static ILJitTypes *_ILJitGetTypes(ILType *type, ILExecProcess *process)
{

	type = ILTypeStripPrefixes(type);
	if(ILType_IsPrimitive(type))
	{
		return ILJitPrimitiveClrTypeToJitTypes(ILType_ToElement(type));
	}
	else
	{
		ILClass *classInfo;
		ILClassPrivate *classPrivate;

		if(ILType_IsValueType(type))
		{
			classInfo = ILClassResolve(ILType_ToValueType(type));
			ILType *synType = ILClassGetSynType(classInfo);

			if(synType != 0)
			{
				/* classInfo = ILClassResolve(ILType_ToClass(ILTypeStripPrefixes(synType))); */
				return _ILJitGetTypes(synType, process);
			}
		}
		else if(ILType_IsComplex(type) && type != 0)
		{
			switch(ILType_Kind(type))
			{
				case IL_TYPE_COMPLEX_PTR:
				{
					/* Unsafe pointers are represented as native integers */
					return &_ILJitType_I;
				}
				/* Not reached */

				case IL_TYPE_COMPLEX_BYREF:
				{
					/* Reference values are managed pointers */
					return &_ILJitType_VPTR;
				}
				/* Not reached */

				case IL_TYPE_COMPLEX_PINNED:
				{
					/* Pinned types are the same as their underlying type */
					return _ILJitGetTypes(ILType_Ref(type), process);
				}
				/* Not reached */

				case IL_TYPE_COMPLEX_CMOD_REQD:
				case IL_TYPE_COMPLEX_CMOD_OPT:
				{
					/* Strip the modifier and inspect the underlying type */
					return _ILJitGetTypes(type->un.modifier__.type__, process);
				}
				/* Not reached */

				case IL_TYPE_COMPLEX_METHOD:
				case IL_TYPE_COMPLEX_METHOD | IL_TYPE_COMPLEX_METHOD_SENTINEL:
				{
					/* Pass method pointers around the system as "I".  Higher
				   	   level code will also set the "typeInfo" field to reflect
					   the signature so that method pointers become verifiable */
					return &_ILJitType_I;
				}
				/* Not reached */

				default:
				{
					/* Everything else is a pointer type. */
					return &_ILJitType_VPTR;
				}
				/* Not reached */
			}
		} 
		else
		{
			classInfo = ILClassResolve(ILType_ToClass(type));
		}
		classPrivate = classInfo->userData;

		/* check if the class is layouted. */
		if(!classPrivate)
		{
			if(!_ILLayoutClass(process, classInfo))
			{
				return 0;
			}
			classPrivate = classInfo->userData;
		}
		return &(classPrivate->jitTypes);
	}
}

/*
 * Get the pointer to base type from the JitTypes.
 * The pointer type is created on demand if not allready present.
 * Returns 0 when out of memory.
 */
static ILJitType _ILJitGetPointerTypeFromJitTypes(ILJitTypes *types)
{
	if(!types->jitTypePtr)
	{
		if(!(types->jitTypePtr = jit_type_create_pointer(types->jitTypeBase, 1)))
		{
			return 0;
		}
	}
	return types->jitTypePtr;
}

/*
 * Get the jit type representing the this pointer for the given ILType.
 */
static ILJitType _ILJitGetThisType(ILType *type, ILExecProcess *process)
{
	ILJitTypes *types = _ILJitGetTypes(type, process);

	if(!types)
	{
		return 0;
	}
	return _ILJitGetPointerTypeFromJitTypes(types);
}

/*
 * Get the jit type representing the argument type for the given ILType.
 * TODO: Handle ref and out args.
 */
static ILJitType _ILJitGetArgType(ILType *type, ILExecProcess *process)
{
	ILJitTypes *types = _ILJitGetTypes(type, process);

	if(!types)
	{
		return 0;
	}
	if(ILType_IsClass(type))
	{
		return _ILJitGetPointerTypeFromJitTypes(types);
	}
	else
	{
		return types->jitTypeBase;
	}
}

/*
 * Get the jit type representing the return value for the given ILType.
 */
static ILJitType _ILJitGetReturnType(ILType *type, ILExecProcess *process)
{
	ILJitTypes *types = _ILJitGetTypes(type, process);

	if(!types)
	{
		return 0;
	}
	if(ILType_IsClass(type))
	{
		return _ILJitGetPointerTypeFromJitTypes(types);
	}
	else
	{
		return types->jitTypeBase;
	}
}

/*
 * Initialize the libjit coder.
 * Returns 1 on success and 0 on failure.
 */
int ILJitInit()
{
	/* Initialize libjit */
	jit_init();

	_ILJitTypesInitBase(&_ILJitType_VOID, jit_type_void);
	_ILJitTypesInitBase(&_ILJitType_BOOLEAN, jit_type_ubyte);
	_ILJitTypesInitBase(&_ILJitType_BYTE, jit_type_ubyte);
	_ILJitTypesInitBase(&_ILJitType_CHAR, jit_type_ushort);
#ifdef IL_NATIVE_INT32
	_ILJitTypesInitBase(&_ILJitType_I, jit_type_int);
#else
#ifdef IL_NATIVE_INT64
	_ILJitTypesInitBase(&_ILJitType_I, jit_type_long);
#endif
#endif
	_ILJitTypesInitBase(&_ILJitType_I2, jit_type_short);
	_ILJitTypesInitBase(&_ILJitType_I4, jit_type_int);
	_ILJitTypesInitBase(&_ILJitType_I8, jit_type_long);
	_ILJitTypesInitBase(&_ILJitType_NFLOAT, jit_type_nfloat);
	_ILJitTypesInitBase(&_ILJitType_R4, jit_type_float32);
	_ILJitTypesInitBase(&_ILJitType_R8, jit_type_float64);
	_ILJitTypesInitBase(&_ILJitType_SBYTE, jit_type_sbyte);
#ifdef IL_NATIVE_INT32
	_ILJitTypesInitBase(&_ILJitType_U, jit_type_uint);
#else
#ifdef IL_NATIVE_INT64
	_ILJitTypesInitBase(&_ILJitType_U, jit_type_ulong);
#endif
#endif
	_ILJitTypesInitBase(&_ILJitType_U2, jit_type_ushort);
	_ILJitTypesInitBase(&_ILJitType_U4, jit_type_uint);
	_ILJitTypesInitBase(&_ILJitType_U8, jit_type_ulong);
	_ILJitTypesInitBase(&_ILJitType_VPTR, jit_type_void_ptr);

	return 1;
}
/*
 * Create a new JIT coder instance.
 */
static ILCoder *JITCoder_Create(ILExecProcess *process, ILUInt32 size,
								unsigned long cachePageSize)
{
	ILJITCoder *coder;
	if((coder = (ILJITCoder *)ILMalloc(sizeof(ILJITCoder))) == 0)
	{
		return 0;
	}
	coder->coder.classInfo = &_ILJITCoderClass;
	coder->process = process;
	if(!(coder->context = jit_context_create()))
	{
		ILFree(coder);
		return 0;
	}
	coder->debugEnabled = 0;
	/* Initialize the stack management. */
	coder->jitStack = 0;
	coder->stackTop = -1;
	coder->stackSize = 0;
	/* Ready to go */
	return &(coder->coder);
}

/*
 * Enable debug mode in a JIT coder instance.
 */
static void JITCoder_EnableDebug(ILCoder *coder)
{
	(_ILCoderToILJITCoder(coder))->debugEnabled = 1;
}

/*
 * Allocate memory within a JIT coder instance.
 */
static void *JITCoder_Alloc(ILCoder *_coder, ILUInt32 size)
{
	/* this might be a noop for the JIT coder */
	/* ILJITCoder *coder = _ILCoderToILJITCoder(_coder);
	return ILCacheAllocNoMethod(coder->cache, size); */
	return 0;
}

/*
 * Get the size of the method cache.
 */
static unsigned long JITCoder_GetCacheSize(ILCoder *_coder)
{
	/* this might be a noop for the JIT coder */
	/* return ILCacheGetSize((_ILCoderToILJITCoder(_coder))->cache); */
	return 0;
}

/*
 * Destroy a JIT coder instance.
 */
static void JITCoder_Destroy(ILCoder *_coder)
{
	ILJITCoder *coder = _ILCoderToILJITCoder(_coder);
	if(coder->jitStack)
	{
		ILFree(coder->jitStack);
		coder->jitStack = 0;
	}
	if(coder->context)
	{
		jit_context_destroy(coder->context);
		coder->context = 0;
	}
	ILFree(coder);
}

/*
 * Get an IL offset from a native offset within a method.
 */
static ILUInt32 JITCoder_GetILOffset(ILCoder *_coder, void *start,
									 ILUInt32 offset, int exact)
{
	/* return ILCacheGetBytecode(((ILCVMCoder *)_coder)->cache, start,
							  offset, exact); */
	/* TODO */
	return 0;
}

/*
 * Get a native offset from an IL offset within a method.
 */
static ILUInt32 JITCoder_GetNativeOffset(ILCoder *_coder, void *start,
									     ILUInt32 offset, int exact)
{
	/* return ILCacheGetNative(((ILCVMCoder *)_coder)->cache, start,
							offset, exact); */
	/* TODO */
	return 0;
}

/*
 * Mark the current position with a bytecode offset.
 */
static void JITCoder_MarkBytecode(ILCoder *coder, ILUInt32 offset)
{
	/* 
	ILCacheMarkBytecode(&(((ILCVMCoder *)coder)->codePosn), offset);
	if(((ILCVMCoder *)coder)->debugEnabled)
	{
		CVM_OUT_BREAK(IL_BREAK_DEBUG_LINE);
	}
	*/
}

/*
 * Get a block of method cache memory for use in code unrolling.
 */
/*
int _ILJITStartUnrollBlock(ILCoder *_coder, int align, ILCachePosn *posn)
{
	ILJITCoder *coder = (ILJITCoder *)_coder;
	return (ILCacheStartMethod(coder->cache, posn, align, 0) != 0);
	return 0;
}
*/

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)


#endif /* !IL_CONFIG_REDUCE_CODE */

/*
 * On demand code generator.
 */
static int _ILJitOnDemandFunc(jit_function_t func)
{
	/* TODO */
	ILExecThread *thread = ILExecThreadCurrent();
	ILMethod *method = (ILMethod *)jit_function_get_meta(func, IL_JIT_META_METHOD);
	ILMethodCode code;

	if(!method)
	{
		return JIT_RESULT_COMPILE_ERROR;
	}

	/* Get the method code */
	if(!ILMethodGetCode(method, &code))
	{
		code.code = 0;
	}
	
	/* We have to handle pinvokes too. */
	if(code.code)
	{
		if(!_ILConvertMethod(thread, method))
		{
			return JIT_RESULT_COMPILE_ERROR;
		}
		return JIT_RESULT_OK;
	}
	else
	{
		/* This is a "PInvoke", "internalcall", or "runtime" method */
		ILPInvoke *pinv;

		pinv = ILPInvokeFind(method);

		return JIT_RESULT_OK;
	}
}

/*
 * Create the jit function header for an ILMethod.
 * We allways pass the ILExecThread as arg 0.
 */
int ILJitFunctionCreate(ILCoder *_coder, ILMethod *method)
{
	ILJITCoder *coder = ((ILJITCoder *)_coder);
	ILType *signature = ILMethod_Signature(method);
	ILType *type;
	/* number of args in the bytecode */
	/* Argument 0 is the type of the return value. */
	ILUInt32 num = ILTypeNumParams(signature);
	/* total number of args */
	/* because we pass the ILExecThread as first arg we have to add one */
	ILUInt32 total = num + 1;
	ILUInt32 current;
	/* We set jitArgc to 1 because we allways pass the current ILExecThread */
	/* in jitArgs[0]. */
	ILUInt32 jitArgc = 1;
	/* We use the C calling convention by default for jitted functions. */
	jit_abi_t jitAbi = jit_abi_cdecl;
	/* JitType to hold the return type */
	ILJitType jitReturnType;
	/* calling convention for this function. */
	/* The type of the jit signature for this function. */
	ILJitType jitSignature;
	/* The new created function. */
	ILJitFunction jitFunction;

	/* Don't create the jit function twice. */
	if(method->userData)
	{
		return 1;
	}
	if(ILType_HasThis(signature))
	{
		/* we need an other arg for this */
		total++;
	}

	ILJitType jitArgs[total];

	/* Get the return type for this function */
	type = ILTypeGetEnumType(ILTypeGetParam(signature, 0));
	if(!(jitReturnType = _ILJitGetReturnType(type, coder->process)))
	{
		return 0;
	}

	/* arg 0 is allways the ILExecThread */
	jitArgs[0] = _IL_JIT_TYPE_VPTR;

	if(ILType_HasThis(signature))
	{
		/* We need to setup the this arg */
		/* determine the type of this arg */
		if(!(type = (ILType *)ILMethod_Owner(method)))
		{
			return 0;
		}
		/* at this time the type must be layouted or at least partially layouted */
		if(!(jitArgs[1] = _ILJitGetThisType(type, coder->process)))
		{
			return 0;
		}
		jitArgc++;
	}

	/* Get the jit types for the regular arguments */
	for(current = 1; current <= num; ++current)
	{
		type = ILTypeGetEnumType(ILTypeGetParam(signature, current));
		if(!(jitArgs[jitArgc] = _ILJitGetArgType(type, coder->process)))
		{
			return 0;
		}
		jitArgc++;
	}

#ifdef IL_CONFIG_VARARGS
	/* Vararg methods can have additional arguments not specified in the signature. */
	if((ILType_CallConv(signature) & IL_META_CALLCONV_MASK) ==
			IL_META_CALLCONV_VARARG)
	{
		jitAbi = jit_abi_vararg;
	}
#endif

	if(!(jitSignature = jit_type_create_signature(jitAbi, jitReturnType,
													jitArgs, jitArgc, 1)))
	{
		return 0;
	}

	/* Now we can create the jit function itself. */
	/* We must be able to create jit function prototypes while an other */
	/* function is on demand compiled. */
	if(!(jitFunction = jit_function_create(coder->context, jitSignature)))
	{
		ILJitTypeDestroy(jitSignature);
		return 0;
	}

	/* Set the ILMethod in the new functions metadata. */
	/* Because there has nothing to be freed we can use 0 for the free_func. */
	if(!jit_function_set_meta(jitFunction, IL_JIT_META_METHOD, method, 0, 0))
	{
		ILJitTypeDestroy(jitSignature);
		return 0;
	}
	
	/* now set the on demand compiler function */
	jit_function_set_on_demand_compiler(jitFunction, _ILJitOnDemandFunc);

	/* and link the new jitFunction to the method. */
	method->userData = jitFunction;

	/* are we ready now ? */

	return 1;
}


/*
 * Create all jitMethods for the given class.
 * Returns 0 on error.
 */
int ILJitCreateFunctionsForClass(ILCoder *_coder, ILClass *info)
{
	int result = 1;

	/* we do not need to create functions for interfaces. */
	if((info->attributes & IL_META_TYPEDEF_CLASS_SEMANTICS_MASK) !=
				IL_META_TYPEDEF_INTERFACE)
	{
		ILMethod *method;

		method = 0;
		while((method = (ILMethod *)ILClassNextMemberByKind
				(info, (ILMember *)method, IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if(!ILJitFunctionCreate(_coder, method))
			{
				result = 0;
			}
		}
	}
	return result;
}

/*
 * Create the class/struct representation of a clr type for libjit.
 * and store the type in classPrivate.
 * Returns the 1 on success else 0
 */
int ILJitTypeCreate(ILClassPrivate *classPrivate)
{
	ILJitType type;

	if(classPrivate->size >= 0)
	{
		if(!(type = jit_type_create_struct(0, 0, 0)))
		{
			return 0;
		}
		jit_type_set_size_and_alignment(type,
										classPrivate->size,
										classPrivate->alignment);
		classPrivate->jitTypes.jitTypeBase = type;
		if(!(classPrivate->jitTypes.jitTypePtr = jit_type_create_pointer(type, 1)))
		{
			jit_type_free(type);
			classPrivate->jitTypes.jitTypeBase = 0;
			return 0;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}

/*
 * Destroy the given type in libjit.
 */
void ILJitTypeDestroy(ILJitType type)
{
	jit_type_free(type);	
}

/*
 * Get the jit type for a primitive clr type.
 * Returns 0 if the type is no primitive clr type.
 */
ILJitTypes *ILJitPrimitiveClrTypeToJitTypes(int primitiveClrType)
{
	switch(primitiveClrType)
	{
		case IL_META_ELEMTYPE_VOID:
		{
			return &_ILJitType_VOID;
		}
		case IL_META_ELEMTYPE_BOOLEAN:
		{
			return &_ILJitType_BOOLEAN;
		}
		case IL_META_ELEMTYPE_I1:
		{
			return &_ILJitType_SBYTE;
		}
		case IL_META_ELEMTYPE_U1:
		{
			return &_ILJitType_BYTE;
		}
		case IL_META_ELEMTYPE_CHAR:
		{
			return &_ILJitType_CHAR;
		}
		case IL_META_ELEMTYPE_I2:
		{
			return &_ILJitType_I2;
		}
		case IL_META_ELEMTYPE_U2:
		{
			return &_ILJitType_U2;
		}
		case IL_META_ELEMTYPE_I4:
		{
			return &_ILJitType_I4;
		}
		case IL_META_ELEMTYPE_U4:
		{
			return &_ILJitType_U4;
		}
		case IL_META_ELEMTYPE_I8:
		{
			return &_ILJitType_I8;
		}
		case IL_META_ELEMTYPE_U8:
		{
			return &_ILJitType_U8;
		}
		case IL_META_ELEMTYPE_I:
		{
			return &_ILJitType_I;
		}
		case IL_META_ELEMTYPE_U:
		{
			return &_ILJitType_U;
		}
		case IL_META_ELEMTYPE_R4:
		{
			return &_ILJitType_R4;
		}
		case IL_META_ELEMTYPE_R8:
		{
			return &_ILJitType_R8;
		}
		case IL_META_ELEMTYPE_R:
		{
			return &_ILJitType_NFLOAT;
		}
		case IL_META_ELEMTYPE_TYPEDBYREF:
		{
			return &_ILJitType_VPTR;
		}
	}
	return 0;
}

/*
 * Include the rest of the JIT conversion routines from other files.
 * We split the implementation to make it easier to maintain the code.
 */
#define	IL_JITC_CODE
#include "jitc_setup.c"
#include "jitc_const.c"
#include "jitc_arith.c"
#include "jitc_var.c"
#include "jitc_stack.c"
#include "jitc_ptr.c"
#include "jitc_branch.c"
#include "jitc_except.c"
#include "jitc_conv.c"
#include "jitc_obj.c"
#include "jitc_call.c"
#undef	IL_JITC_CODE

/*
 * Define the JIT coder class.
 */
ILCoderClass const _ILJITCoderClass =
{
	JITCoder_Create,
	JITCoder_EnableDebug,
	JITCoder_Alloc,
	JITCoder_GetCacheSize,
	JITCoder_Setup,
	JITCoder_SetupExtern,
	JITCoder_SetupExternCtor,
	JITCoder_CtorOffset,
	JITCoder_Destroy,
	JITCoder_Finish,
	JITCoder_Label,
	JITCoder_StackRefresh,
	JITCoder_Constant,
	JITCoder_StringConstant,
	JITCoder_Binary,
	JITCoder_BinaryPtr,
	JITCoder_Shift,
	JITCoder_Unary,
	JITCoder_LoadArg,
	JITCoder_StoreArg,
	JITCoder_AddrOfArg,
	JITCoder_LoadLocal,
	JITCoder_StoreLocal,
	JITCoder_AddrOfLocal,
	JITCoder_Dup,
	JITCoder_Pop,
	JITCoder_ArrayAccess,
	JITCoder_PtrAccess,
	JITCoder_PtrAccessManaged,
	JITCoder_Branch,
	JITCoder_SwitchStart,
	JITCoder_SwitchEntry,
	JITCoder_Compare,
	JITCoder_Conv,
	JITCoder_ToPointer,
	JITCoder_PtrPrefix,
	JITCoder_ArrayLength,
	JITCoder_NewArray,
	JITCoder_LocalAlloc,
	JITCoder_CastClass,
	JITCoder_LoadField,
	JITCoder_LoadStaticField,
	JITCoder_LoadThisField,
	JITCoder_LoadFieldAddr,
	JITCoder_LoadStaticFieldAddr,
	JITCoder_StoreField,
	JITCoder_StoreStaticField,
	JITCoder_CopyObject,
	JITCoder_CopyBlock,
	JITCoder_InitObject,
	JITCoder_InitBlock,
	JITCoder_Box,
	JITCoder_BoxSmaller,
	JITCoder_Unbox,
	JITCoder_MakeTypedRef,
	JITCoder_RefAnyVal,
	JITCoder_RefAnyType,
	JITCoder_PushToken,
	JITCoder_SizeOf,
	JITCoder_ArgList,
	JITCoder_UpConvertArg,
	JITCoder_DownConvertArg,
	JITCoder_PackVarArgs,
	JITCoder_ValueCtorArgs,
	JITCoder_CheckCallNull,
	JITCoder_CallMethod,
	JITCoder_CallIndirect,
	JITCoder_CallCtor,
	JITCoder_CallVirtual,
	JITCoder_CallInterface,
	JITCoder_CallInlineable,
	JITCoder_JumpMethod,
	JITCoder_ReturnInsn,
	JITCoder_LoadFuncAddr,
	JITCoder_LoadVirtualAddr,
	JITCoder_LoadInterfaceAddr,
	JITCoder_SetupExceptions,
	JITCoder_Throw,
	JITCoder_SetStackTrace,
	JITCoder_Rethrow,
	JITCoder_Jsr,
	JITCoder_RetFromJsr,
	JITCoder_TryHandlerStart,
	JITCoder_TryHandlerEnd,
	JITCoder_Catch,
	JITCoder_EndCatchFinally,
	JITCoder_Finally,
	JITCoder_PCToHandler,
	JITCoder_PCToMethod,
	JITCoder_GetILOffset,
	JITCoder_GetNativeOffset,
	JITCoder_MarkBytecode,
	JITCoder_MarkEnd,
	JITCoder_SetFlags,
	JITCoder_GetFlags,
	JITCoder_AllocExtraLocal,
	JITCoder_PushThread,
	JITCoder_LoadNativeArgAddr,
	JITCoder_LoadNativeLocalAddr,
	JITCoder_StartFfiArgs,
	JITCoder_PushRawArgPointer,
	JITCoder_CallFfi,
	JITCoder_CheckNull,
	JITCoder_Convert,
	JITCoder_ConvertCustom,
	"sentinel"
};
#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_JIT */
