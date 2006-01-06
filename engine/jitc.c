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
#include "jitc_gen.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Mapping of the native clr types to the corresponing jit types.
 */
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
 * Definition of signatures of internal functions used by jitted code.
 * They have to be kept in sync wirh the corresponding engine funcions.
 */

/*
 * ILObject *_ILEngineAlloc(ILExecThread *thread, ILClass *classInfo,
 *						 	ILUInt32 size)
 */
static ILJitType _ILJitSignature_ILEngineAlloc = 0;

/*
 * ILInt32 ILRuntimeCanCastClass(ILExecThread *thread, ILObject *object, ILClass *toClass)
 *
 */
static ILJitType _ILJitSignature_ILRuntimeCanCastClass = 0;

/*
 * ILInt32 ILRuntimeClassImplements(ILObject *object, ILClass *toClass)
 */
static ILJitType _ILJitSignature_ILRuntimeClassImplements = 0;

/*
 * void *ILRuntimeGetThreadStatic(ILExecThread *thread,
 *							   ILUInt32 slot, ILUInt32 size)
 */
static ILJitType _ILJitSignature_ILRuntimeGetThreadStatic = 0;

/*
 * Define offsetof macro if not present.
 */
#ifndef offsetof
#define offsetof(struct_type, member) \
          (size_t) &(((struct_type *)0)->member)
#endif

/*
 * Define the structure of a JIT label.
 */
typedef struct _tagILJITLabel ILJITLabel;
struct _tagILJITLabel
{
	ILUInt32	address;		/* Address in the IL code */
	jit_label_t	label;			/* the libjit label */
	ILJITLabel *next;			/* Next label block */

};

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
	int				flags;

	/* Members to manage the evaluation stack. */
	jit_value_t	   *jitStack;
	int				stackSize;
	int				stackTop;

	/* Members to manage the local variables. */
	jit_value_t	   *jitLocals;
	int				numLocals;
	int				maxLocals;

	/* Handle the labels. */
	ILMemPool		labelPool;
	ILJITLabel     *labelList;
	int				labelOutOfMemory;

	/* The current jitted function. */
	ILJitFunction	jitFunction;
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
						(ILJitValue *)ILRealloc((coder)->jitStack, \
											  n * sizeof(ILJitValue)); \
					if(!newStack) \
					{ \
						return 0; \
					} \
					(coder)->jitStack = newStack; \
					(coder)->stackSize = n; \
				} \
			} while (0)

/*
 * Acquire and release the metadata lock during layouting a class.
 */
#define	METADATA_WRLOCK(thread)	\
			do { \
				IL_METADATA_WRLOCK(_ILExecThreadProcess(thread)); \
			} while (0)
#define	METADATA_UNLOCK(thread)	\
			do { \
				IL_METADATA_UNLOCK(_ILExecThreadProcess(thread)); \
			} while (0)

/*
 * Perform a class layout.
 */
static int _LayoutClass(ILExecThread *thread, ILClass *info)
{
	int result = 0;

	METADATA_WRLOCK(thread);
	result = _ILLayoutClass(_ILExecThreadProcess(thread), info);
	METADATA_UNLOCK(thread);
	return result; 
}

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
 * Perform a class cast, taking arrays into account.
 */
ILInt32 ILRuntimeCanCastClass(ILExecThread *thread, ILObject *object, ILClass *toClass)
{
	ILImage *image = ILProgramItem_Image(thread->method);
	ILClass *fromClass = GetObjectClass(object);
	ILType *fromType = ILClassGetSynType(fromClass);
	ILType *toType = ILClassGetSynType(toClass);
	if(fromType && toType)
	{
		if(ILType_IsArray(fromType) && ILType_IsArray(toType) &&
		   ILTypeGetRank(fromType) == ILTypeGetRank(toType))
		{
			return ILTypeAssignCompatibleNonBoxing
			  (image, ILTypeGetElemType(fromType), ILTypeGetElemType(toType));
		}
		else
		{
			return 0;
		}
	}
	else
	{
		fromType=ILTypeGetEnumType(ILClassToType(fromClass));
		toType=ILTypeGetEnumType(ILClassToType(toClass));
		
		if(ILTypeIdentical(fromType,toType))
		{
			return 1;
		}

	   	return ILClassInheritsFrom(fromClass, toClass);
	}
}

/*
 * Perform a check if a class implements a given interface.
 */
ILInt32 ILRuntimeClassImplements(ILObject *object, ILClass *toClass)
{
	ILClass *info = GetObjectClass(object);

	return ILClassImplements(info, toClass);
}

/*
 * Get a thread-static value from the current thread.
 */
void *ILRuntimeGetThreadStatic(ILExecThread *thread,
							   ILUInt32 slot, ILUInt32 size)
{
	void **array;
	ILUInt32 used;
	void *ptr;

	/* Determine if we need to allocate space for a new slot */
	if(slot >= thread->threadStaticSlotsUsed)
	{
		used = (slot + 8) & ~7;
		array = (void **)ILGCAlloc(sizeof(void *) * used);
		if(!array)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		if(thread->threadStaticSlotsUsed > 0)
		{
			ILMemMove(array, thread->threadStaticSlots,
					  sizeof(void *) * thread->threadStaticSlotsUsed);
		}
		thread->threadStaticSlots = array;
		thread->threadStaticSlotsUsed = used;
	}

	/* Fetch the current value in the slot */
	ptr = thread->threadStaticSlots[slot];
	if(ptr)
	{
		return ptr;
	}

	/* Allocate a new value and write it to the slot */
	if(!size)
	{
		/* Sanity check, just in case */
		size = sizeof(unsigned long);
	}
	ptr = ILGCAlloc((unsigned long)size);
	if(!ptr)
	{
		ILExecThreadThrowOutOfMemory(thread);
		return 0;
	}
	thread->threadStaticSlots[slot] = ptr;
	return ptr;
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
 * Returns 0 whne the type could not be found or out of memory.
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
 * Returns 0 whne the type could not be found or out of memory.
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
 * Get the jit type representing the local type for the given ILType.
 * Returns 0 when the type could not be found or out of memory.
 */
static ILJitType _ILJitGetLocalsType(ILType *type, ILExecProcess *process)
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
 * Generate the code to throw the current exception in the thread in libjit.
 */
static void _ILJitThrowCurrentException(ILJITCoder *coder)
{
	ILJitValue thread = jit_value_get_param(coder->jitFunction,0);
	ILJitValue currentException = jit_insn_load_relative(coder->jitFunction, thread,
									offsetof(ILExecThread, currentException), 
									jit_type_void_ptr);
	jit_insn_throw(coder->jitFunction, currentException);
}

/*
 * Initialize the libjit coder.
 * Returns 1 on success and 0 on failure.
 */
int ILJitInit()
{
	ILJitType	returnType;
	ILJitType	args[3];

	/* Initialize libjit */
	jit_init();

	/* Initialize the nattive types. */
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

	/* Initialize the native method signatures. */
	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	args[2] = jit_type_int;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILEngineAlloc = 
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 3, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	args[2] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_INT32;
	if(!(_ILJitSignature_ILRuntimeCanCastClass = 
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 3, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_INT32;
	if(!(_ILJitSignature_ILRuntimeClassImplements = 
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 2, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_UINT32;
	args[2] = _IL_JIT_TYPE_UINT32;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILRuntimeGetThreadStatic = 
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 3, 1)))
	{
		return 0;
	}

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
	coder->flags = 0;
	/* Initialize the stack management. */
	coder->jitStack = 0;
	coder->stackTop = -1;
	coder->stackSize = 0;
	/* Initialize the locals management. */
	coder->jitLocals = 0;
	coder->numLocals = 0;
	coder->maxLocals = 0;

	/* Init the current jitted function. */
	coder->jitFunction = 0;

	/* Init the label stuff. */
	ILMemPoolInit(&(coder->labelPool), sizeof(ILJITLabel), 8);
	coder->labelList = 0;
	coder->labelOutOfMemory = 0;

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
	if(coder->jitLocals)
	{
		ILFree(coder->jitLocals);
		coder->jitLocals = 0;
	}
	if(coder->context)
	{
		jit_context_destroy(coder->context);
		coder->context = 0;
	}
	ILMemPoolDestroy(&(coder->labelPool));

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
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	if(offset != 0)
	{
		jit_insn_mark_offset(jitCoder->jitFunction, (jit_int)offset);
	}
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

#ifdef IL_CONFIG_PINVOKE

/*
 * Locate or load an external module that is being referenced via "PInvoke".
 * Returns the system module pointer, or NULL if it could not be loaded.
 */
static void *LocateExternalModule(ILExecProcess *process, const char *name,
								  ILPInvoke *pinvoke)
{
	ILLoadedModule *loaded;
	char *pathname;

	/* Search for an already-loaded module with the same name */
	loaded = process->loadedModules;
	while(loaded != 0)
	{
		if(!ILStrICmp(loaded->name, name))
		{
			return loaded->handle;
		}
		loaded = loaded->next;
	}

	/* Create a new module structure.  We keep this structure even
	   if we cannot load the actual module.  This ensures that
	   future requests for the same module will be rejected without
	   re-trying the open */
	loaded = (ILLoadedModule *)ILMalloc(sizeof(ILLoadedModule) + strlen(name));
	if(!loaded)
	{
		return 0;
	}
	loaded->next = process->loadedModules;
	loaded->handle = 0;
	strcpy(loaded->name, name);
	process->loadedModules = loaded;

	/* Resolve the module name to a library name */
	pathname = ILPInvokeResolveModule(pinvoke);
	if(!pathname)
	{
		return 0;
	}

	/* Attempt to open the module */
	loaded->handle = ILDynLibraryOpen(pathname);
	ILFree(pathname);
	return loaded->handle;
}

#endif /* IL_CONFIG_PINVOKE */

/*
 * Set the method member in the ILExecThread instance.
 */
static void _ILJitSetMethodInThread(ILJitFunction func, ILJitValue thread, ILMethod *method)
{
	ILJitValue methodPtr = jit_value_create_nint_constant(func, _IL_JIT_TYPE_VPTR, (jit_nint)method);

	jit_insn_store_relative(func, thread, offsetof(ILExecThread, method), methodPtr);
}

/*
 * Get the classPrivate pointer from an object reference.
 */
static ILJitValue _ILJitGetObjectClassPrivate(ILJitFunction func, ILJitValue object)
{
	ILJitValue classPrivate = 
		jit_insn_load_relative(func, object, 
		    				   -IL_OBJECT_HEADER_SIZE +
							   offsetof(ILObjectHeader, classPrivate),
							   _IL_JIT_TYPE_VPTR);
	return classPrivate;	
}

/*
 * Get the evaluation stack type for the given ILJitType.
 */
static ILJitType _ILJitTypeToStackType(ILJitType type)
{
	ILJitType  stackType = jit_type_promote_int(type);;

	if(type == stackType)
	{
		if((type == _IL_JIT_TYPE_SINGLE) || (type == _IL_JIT_TYPE_DOUBLE))
		{
			stackType = _IL_JIT_TYPE_NFLOAT;
		}
	}
	return stackType;
}

/*
 * Convert the given ILJitValue to the type needed on the evaluation stack.
 * When no conversion is needed the value is returned as it is.
 */
static ILJitValue _ILJitValueConvertToStackType(ILJitFunction func,
												ILJitValue value)
{
	ILJitType type = jit_value_get_type(value);
	ILJitType stackType = _ILJitTypeToStackType(type);

	if(type != stackType)
	{
		value = jit_insn_convert(func, value, stackType, 0);
	}
	return value;
}

/*
 * Generate the stub for calling an internal function.
 */
static int _ILJitCompileInternal(jit_function_t func, ILMethod *method, void *nativeFunction)
{
	ILJitType signature = jit_function_get_signature(func);
	unsigned int numParams = jit_type_num_params(signature);
	ILJitValue returnValue;

	/* We need to set the method member in the ILExecThread == arg[0]. */
	if(numParams > 0)
	{
		ILJitValue jitParams[numParams];
		ILUInt32 current;
		
		for(current = 0; current < numParams; current++)
		{
			if(!(jitParams[current] = jit_value_get_param(func, current)))
			{
				return JIT_RESULT_OUT_OF_MEMORY;
			}
		}
		_ILJitSetMethodInThread(func, jitParams[0], method);
		returnValue = jit_insn_call_native(func, 0, nativeFunction, signature, jitParams, numParams, 0);
	}
	else
	{
		returnValue = jit_insn_call_native(func, 0, nativeFunction, signature, 0, 0, 0);
	}
	jit_insn_return(func, returnValue);	
	return JIT_RESULT_OK;
}

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
		ILPInvoke *pinv = ILPInvokeFind(method);
		ILInternalInfo fnInfo;
		int isConstructor = ILMethod_IsConstructor(method);;
	#ifdef IL_CONFIG_PINVOKE
		ILModule *module;
		const char *name;
		void *moduleHandle;
	#endif

		switch(method->implementAttrs &
					(IL_META_METHODIMPL_CODE_TYPE_MASK |
					 IL_META_METHODIMPL_INTERNAL_CALL |
					 IL_META_METHODIMPL_JAVA))
		{
			case IL_META_METHODIMPL_IL:
			case IL_META_METHODIMPL_OPTIL:
			{
				/* If we don't have a PInvoke record, then we don't
				   know what to map this method call to */
				if(!pinv)
				{
					return JIT_RESULT_COMPILE_ERROR;
				}

			#ifdef IL_CONFIG_PINVOKE
				/* Find the module for the PInvoke record */
				module = ILPInvoke_Module(pinv);
				if(!module)
				{
					return JIT_RESULT_COMPILE_ERROR;
				}
				name = ILModule_Name(module);
				if(!name || *name == '\0')
				{
					return JIT_RESULT_COMPILE_ERROR;
				}
				moduleHandle = LocateExternalModule
									(ILExecThreadGetProcess(thread), name, pinv);
				if(!moduleHandle)
				{
					return JIT_RESULT_COMPILE_ERROR;
				}

				/* Get the name of the function within the module */
				name = ILPInvoke_Alias(pinv);
				if(!name || *name == '\0')
				{
					name = ILMethod_Name(method);
				}

				/* Look up the method within the module */
				fnInfo.func = ILDynLibraryGetSymbol(moduleHandle, name);
			#else /* !IL_CONFIG_PINVOKE */
				return JIT_RESULT_COMPILE_ERROR;
			#endif /* IL_CONFIG_PINVOKE */
			}
			break;

			case IL_META_METHODIMPL_RUNTIME:
			case IL_META_METHODIMPL_IL | IL_META_METHODIMPL_INTERNAL_CALL:
			{
				/* "internalcall" and "runtime" methods must not
				   have PInvoke records associated with them */
				if(pinv)
				{
					return JIT_RESULT_COMPILE_ERROR;
				}

				/* Look up the internalcall function details */
				if(!_ILFindInternalCall(_ILExecThreadProcess(thread),
										method, 0, &fnInfo))
				{
					if(isConstructor)
					{
						if(!_ILFindInternalCall(_ILExecThreadProcess(thread),
												method, 1, &fnInfo))
						{
							return JIT_RESULT_COMPILE_ERROR;
						}
					}
					else
					{
						return JIT_RESULT_COMPILE_ERROR;
					}
				}
				else if(isConstructor)
				{
					_ILFindInternalCall(ILExecThreadGetProcess(thread),
										method, 1, &fnInfo);
				}
			}
			break;

			default:
			{
				/* No idea how to invoke this method */
				return JIT_RESULT_COMPILE_ERROR;
			}
			/* Not reached */
		}

		/* Bail out if we did not find the underlying native method */
		if(!(fnInfo.func))
		{
			return JIT_RESULT_COMPILE_ERROR;
		}

		switch(method->implementAttrs &
					(IL_META_METHODIMPL_CODE_TYPE_MASK |
					 IL_META_METHODIMPL_INTERNAL_CALL |
					 IL_META_METHODIMPL_JAVA))
		{
			case IL_META_METHODIMPL_RUNTIME:
			case IL_META_METHODIMPL_IL | IL_META_METHODIMPL_INTERNAL_CALL:
			{
				return _ILJitCompileInternal(func, method, fnInfo.func);
			}
		}
		return JIT_RESULT_COMPILE_ERROR;
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
	/* Some infos that we'll need later. */
	ILClass *info = ILMethod_Owner(method);
	/* Flag if this is an array or string constructor. */
	int isArrayOrString = 0;
	/* Flag if the method is a ctor. */
	int isCtor = ILMethodIsConstructor(method);

	/* Don't create the jit function twice. */
	if(method->userData)
	{
		return 1;
	}
	if(ILType_HasThis(signature))
	{
		if(!isCtor)
		{
			/* we need an other arg for this */
			total++;
		}
		else
		{
			ILType *ownerType = ILType_FromClass(info);
			ILType *synType = type = ILClassGetSynType(info);

			if(!(synType && ILType_IsArray(synType)) && !ILTypeIsStringClass(ownerType))
			{
				/* we need an other arg for this */
				total++;
			}
			else
			{
				isArrayOrString = 1;
			}
		}
	}

	ILJitType jitArgs[total];

	/* Get the return type for this function */
	if(isCtor && isArrayOrString)
	{
		type = ILType_FromClass(info);
	}
	else
	{
		type = ILTypeGetEnumType(ILTypeGetParam(signature, 0));
	}
	if(!(jitReturnType = _ILJitGetReturnType(type, coder->process)))
	{
		return 0;
	}

	/* arg 0 is allways the ILExecThread */
	jitArgs[0] = _IL_JIT_TYPE_VPTR;

	if(ILType_HasThis(signature))
	{
		if(!isCtor || !isArrayOrString)
		{
			/* We need to setup the this arg */
			/* determine the type of this arg */
			if(!(type = ILType_FromClass(info)))
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
 * Call the jit function for a ILMethod.
 * Returns 1 if an exception occured.
 */
int ILJitCallMethod(ILExecThread *thread, ILMethod *method, void **jitArgs, void *result)
{
	ILJitFunction jitFunction = method->userData;

	if(!jitFunction)
	{
		/* We have to layout the class. */
		if(!_LayoutClass(thread, ILMethod_Owner(method)))
		{
			return 1;
		}
		jitFunction = method->userData;
	}

	if(!jit_function_apply(jitFunction, jitArgs, result))
	{
		return 1;
	}
	return 0;
}

/*
 * Get the ILJitFunction for an ILMethod.
 * Returns 0 if the jit function stub isn't created yet.
 */
ILJitFunction ILJitFunctionFromILMethod(ILMethod *method)
{
	if(method)
	{
		return (ILJitFunction)(method->userData);
	}
	return 0;
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
