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
 * For enabling extended debugging output uncomment the following define.
 */
/* #define _IL_JIT_ENABLE_DEBUG 1 */

/*
 * For dumping the jitted function before it is compiled uncomment the
 * following define.
 */
/* #define _IL_JIT_DUMP_FUNCTION 1 */

/*
 * For dumping the disassembled jitted function before after  it is compiled
 *  uncomment the following define.
 */
/* #define _IL_JIT_DISASSEMBLE_FUNCTION 1 */

/*
 * To enable parameter / locals optimizations uncomment the following define.
 */
/* #define _IL_JIT_OPTIMIZE_LOCALS 1 */

/*
 * To enable null check optimizations uncomment the following define.
 */
/* #define _IL_JIT_OPTIMIZE_NULLCHECKS 1 */

/*
 * To defer the initialization of the locals uncomment the following define.
 */
/* #define _IL_JIT_OPTIMIZE_INIT_LOCALS 1 */

#ifdef _IL_JIT_DUMP_FUNCTION
#ifndef _IL_JIT_ENABLE_DEBUG
#define _IL_JIT_ENABLE_DEBUG 1
#endif
#endif

#ifdef _IL_JIT_DISASSEMBLE_FUNCTION
#ifndef _IL_JIT_ENABLE_DEBUG
#define _IL_JIT_ENABLE_DEBUG 1
#endif
#endif

#ifdef _IL_JIT_ENABLE_DEBUG
#include "jit/jit-dump.h"
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
static struct _tagILJitTypes _ILJitType_TYPEDREF;

/*
 * Jit type for typed references.
 */
static ILJitType _ILJitTypedRef = 0;

/*
 * Allocate memory for an object that contains object references.
 */
static ILObject *_ILJitAlloc(ILClass *classInfo, ILUInt32 size);

/*
 * Allocate memory for an object that does not contain any object references.
 */
static ILObject *_ILJitAllocAtomic(ILClass *classInfo, ILUInt32 size);

/*
 * Definition of signatures of internal functions used by jitted code.
 * They have to be kept in sync wirh the corresponding engine funcions.
 */

#ifndef IL_JIT_THREAD_IN_SIGNATURE
/*
 * ILExecThread *ILExecThreadCurrent()
 */
static ILJitType _ILJitSignature_ILExecThreadCurrent = 0;
#endif

/*
 * ILObject *_ILJitAlloc(ILClass *classInfo, ILUInt32 size)
 */
static ILJitType _ILJitSignature_ILJitAlloc = 0;

/*
 * System_Array *_ILJitGetExceptionStackTrace(ILExecThread *thread)
 */
static ILJitType _ILJitSignature_ILJitGetExceptionStackTrace = 0;

/*
 * void ILRuntimeExceptionRethrow(ILObject *exception)
 */
static ILJitType _ILJitSignature_ILRuntimeExceptionRethrow = 0;

/*
 * void ILRuntimeExceptionThrow(ILObject *exception)
 */
static ILJitType _ILJitSignature_ILRuntimeExceptionThrow = 0;

/*
 * void ILRuntimeExceptionThrowClass(ILClass *classInfo)
 */
static ILJitType _ILJitSignature_ILRuntimeExceptionThrowClass = 0;

/*
 * void ILRuntimeExceptionThrowOutOfMemory()
 */
static ILJitType _ILJitSignature_ILRuntimeExceptionThrowOutOfMemory = 0;


/*
 * static void *_ILRuntimeLookupInterfaceMethod(ILClassPrivate *objectClassPrivate,
 *												ILClass *interfaceClass,
 *												ILUInt32 index)
 */
static ILJitType _ILJitSignature_ILRuntimeLookupInterfaceMethod = 0;

/*
 * ILInt32 ILRuntimeCanCastClass(ILMethod *method, ILObject *object, ILClass *toClass)
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
 * void jit_exception_clear_last()
 */
static ILJitType _ILJitSignature_JitExceptionClearLast = 0;

/*
 * void *malloc(size_t nbytes)
 * This signature is used for GCAlloc and GCAllocAtomic too.
 */
static ILJitType _ILJitSignature_malloc = 0;

#ifdef IL_JIT_FNPTR_ILMETHOD
/*
 * void *ILRuntimeMethodToVtablePointer(ILMethod *method)
 */
static ILJitType _ILJitSignature_ILRuntimeMethodToVtablePointer = 0;
#endif

/*
 * Definition of the signatures for inlined calls of native runtime functions.
 */

/*
 * void _IL_Monitor_Enter(ILExecThread *thread, ILObject *obj)
 */
static ILJitType _ILJitSignature_ILMonitorEnter = 0;

/*
 * void _IL_Monitor_Exit(ILExecThread *thread, ILObject *obj)
 */
static ILJitType _ILJitSignature_ILMonitorExit = 0;

/*
 * void _ILGetClrType(ILExecThread *thread, ILClass *info)
 */
static ILJitType _ILJitSignature_ILGetClrType = 0;

/*
 * void ILRuntimeHandleManagedSafePointFlags(ILExecThread *thread)
 */
static ILJitType _ILJitSignature_ILRuntimeHandleManagedSafePointFlags = 0;

/*
 * char *ILStringToUTF8(ILExecThread *thread, ILString *str)
 */
static ILJitType _ILJitSignature_ILStringToUTF8 = 0;

/* 
 * ILString *ILStringCreate(ILExecThread *thread, const char *str)
 */
static ILJitType _ILJitSignature_ILStringCreate = 0;

/*
 * Define offsetof macro if not present.
 */
#ifndef offsetof
#define offsetof(struct_type, member) \
          (size_t) &(((struct_type *)0)->member)
#endif

/*
 * declaration of the different label types.
 */
#define _IL_JIT_LABEL_NORMAL 1
#define _IL_JIT_LABEL_STARTCATCH 2
#define _IL_JIT_LABEL_STARTFINALLY 4

/*
 * Define the structure of a JIT label.
 */
typedef struct _tagILJITLabel ILJITLabel;
struct _tagILJITLabel
{
	ILUInt32	address;		/* Address in the IL code */
	jit_label_t	label;			/* the libjit label */
	ILJITLabel *next;			/* Next label block */
	int			labelType;      /* type of the label. */
	int			stackSize;		/* No. of elements on the stack. */
	ILJitValue *jitStack;		/* Values on the stack. */
};

/*
 * Define the structure of a local/argument slot.
 */
typedef struct _tagILJitLocalSlot ILJitLocalSlot;
struct _tagILJitLocalSlot
{
	ILJitValue	value;			/* the ILJitValue */
	ILUInt32	flags;			/* State of the local/arg. */
};

/*
 * Define the structure for managing the local slots.
 */
typedef struct _tagILJitLocalSlots ILJitLocalSlots;
struct _tagILJitLocalSlots
{
	ILJitLocalSlot *slots;		/* Pointer to the slots. */
	int				numSlots;	/* Number of used slots. */
	int				maxSlots;	/* Number of allocated slots. */
};

#define _ILJitLocalSlotsInit(s) \
	do { \
		(s).slots = 0;		\
		(s).numSlots = 0;	\
		(s).maxSlots = 0;	\
	} while (0);

#define _ILJitLocalSlotsDestroy(s) \
	do { \
		if((s).slots)			\
		{						\
			ILFree((s).slots);	\
		}						\
		(s).slots = 0;			\
		(s).numSlots = 0;		\
		(s).maxSlots = 0;		\
	} while (0);

/*
 * Private method information for the jit coder.
 */
typedef struct _tagILJitMethodInfo ILJitMethodInfo;
struct _tagILJitMethodInfo
{
	ILJitFunction jitFunction;		/* Implementation of the method. */
	ILUInt32 implementationType;	/* Flag how the method is implemented. */
	ILInternalInfo fnInfo;			/* Information for internal calls or pinvokes. */
};

#define _IL_JIT_IMPL_DEFAULT		0x0
#define _IL_JIT_IMPL_INTERNAL		0x1
#define _IL_JIT_IMPL_INTERNALALLOC	0x2
#define _IL_JIT_IMPL_INTERNALMASK	0x3
#define _IL_JIT_IMPL_PINVOKE		0x4

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

	/* Pool for the method infos. */
	ILMemPool		methodPool;

	/* Members to manage the evaluation stack. */
	ILJitValue	   *jitStack;
	int				stackSize;
	int				stackTop;

	/* Members to manage the fixed arguments. */
	ILJitLocalSlots	jitParams;

	/* Members to manage the local variables. */
	ILJitLocalSlots jitLocals;
#ifdef _IL_JIT_OPTIMIZE_INIT_LOCALS
	int				localsInitialized;
#endif

	/* Handle the labels. */
	ILMemPool		labelPool;
	ILJITLabel     *labelList;
	int				labelOutOfMemory;
	ILMemStack		stackStates;

	/* Handle the switch table. */
	ILJitValue		switchValue;
	int				numSwitch;
	int				maxSwitch;

	/* The current jitted function. */
	ILJitFunction	jitFunction;

	/* Flag if the catcher is started. */
	int				isInCatcher;
	jit_label_t     nextBlock;
	jit_label_t     rethrowBlock;

#ifndef IL_JIT_THREAD_IN_SIGNATURE
	/* cache for the current thread. */
	ILJitValue		thread;
#endif
};

/*
 * Convert a pointer to an ILCoder to a pointer to the ILJITVoder instance
 */
#define _ILCoderToILJITCoder(coder) ((ILJITCoder *)coder)

/*
 * Generate the code to allocate the memory for an object.
 * Returns the ILJitValue with the pointer to the new object.
 */
static ILJitValue _ILJitAllocGen(ILJitFunction jitFunction, ILClass *classInfo,
								 ILUInt32 size);

/*
 * Generate the code to allocate the memory for an object.
 * Returns the ILJitValue with the pointer to the new object.
 */
static ILJitValue _ILJitAllocObjectGen(ILJitFunction jitFunction, ILClass *classInfo);

#ifdef IL_JIT_THREAD_IN_SIGNATURE
#define _ILJitCoderGetThread(coder)		jit_value_get_param((coder)->jitFunction, 0)
#define _ILJitFunctionGetThread(func)	jit_value_get_param((func), 0)
#else
static ILJitValue _ILJitFunctionGetThread(ILJitFunction func)
{
	return jit_insn_call_native(func, "ILExecThreadCurrent",
										ILExecThreadCurrent,
										_ILJitSignature_ILExecThreadCurrent,
										0, 0, JIT_CALL_NOTHROW);
}

static ILJitValue _ILJitCoderGetThread(ILJITCoder *jitCoder)
{
	if(!(jitCoder->thread))
	{
		ILJitValue thread;

		if(!(jitCoder->isInCatcher))
		{
			jit_label_t startLabel = jit_label_undefined;
			jit_label_t endLabel = jit_label_undefined;
			
			jit_insn_label(jitCoder->jitFunction, &startLabel);
			thread = _ILJitFunctionGetThread(jitCoder->jitFunction);
			jit_insn_label(jitCoder->jitFunction, &endLabel);
			jit_insn_move_blocks_to_start(jitCoder->jitFunction, startLabel,
																 endLabel);
		}
		else
		{
			thread = _ILJitFunctionGetThread(jitCoder->jitFunction);
		}
		jitCoder->thread = thread;
	}
	return jitCoder->thread;
}
#endif

/*
 * Declaration of the engine internal exceptions.
 */
#define _IL_JIT_OK					0
#define _IL_JIT_OUT_OF_MEMORY		1
#define _IL_JIT_INVALID_CAST		2
#define _IL_JIT_INDEX_OUT_OF_RANGE	3
#define _IL_JIT_METHOD_INVOKATION	4

/*
 * Emit the code to throw a system exception.
 */
static void _ILJitThrowSystem(ILJITCoder *jitCoder, ILUInt32 exception);

/*
 * Initialize a ILJitTypes base structure 
 */
#define _ILJitTypesInitBase(jitTypes, jitType) \
	{ \
		(jitTypes)->jitTypeBase = (jitType); \
		(jitTypes)->jitTypeKind = 0; \
	}

/*
 * Allocate enough space for "n" values on the stack.
 */
#define	ALLOC_STACK(coder, n)	\
			do { \
				if((n) > (coder)->stackSize) \
				{ \
					ILJitValue *newStack = \
						(ILJitValue *)ILRealloc((coder)->jitStack, \
											  (n) * sizeof(ILJitValue)); \
					if(!newStack) \
					{ \
						return 0; \
					} \
					(coder)->jitStack = newStack; \
					(coder)->stackSize = (n); \
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
 * Check if the typeKind is a floating point number.
 */
#define _JIT_TYPEKIND_IS_FLOAT(typeKind) \
((typeKind == JIT_TYPE_FLOAT32)	|| \
 (typeKind == JIT_TYPE_FLOAT64)	|| \
 (typeKind == JIT_TYPE_NFLOAT))

/*
 * Check if the typeKind is an int (<=32 bit value).
 */
#ifdef IL_NATIVE_INT64
#define _JIT_TYPEKIND_IS_INT(typeKind) \
((typeKind == JIT_TYPE_INT)		|| \
 (typeKind == JIT_TYPE_UINT)	|| \
 (typeKind == JIT_TYPE_SHORT)	|| \
 (typeKind == JIT_TYPE_USHORT)	|| \
 (typeKind == JIT_TYPE_SBYTE)	|| \
 (typeKind == JIT_TYPE_UBYTE))
#else
#define _JIT_TYPEKIND_IS_INT(typeKind) \
((typeKind == JIT_TYPE_NINT)	|| \
 (typeKind == JIT_TYPE_NUINT)	|| \
 (typeKind == JIT_TYPE_INT)		|| \
 (typeKind == JIT_TYPE_UINT)	|| \
 (typeKind == JIT_TYPE_SHORT)	|| \
 (typeKind == JIT_TYPE_USHORT)	|| \
 (typeKind == JIT_TYPE_SBYTE)	|| \
 (typeKind == JIT_TYPE_UBYTE)	|| \
 (typeKind == JIT_TYPE_PTR))
#endif

/*
 * Check if the typeKind is a long (64 bit value).
 */
#ifdef IL_NATIVE_INT64
#define _JIT_TYPEKIND_IS_LONG(typeKind) \
((typeKind == JIT_TYPE_LONG)	|| \
 (typeKind == JIT_TYPE_ULONG)	|| \
 (typeKind == JIT_TYPE_NINT)	|| \
 (typeKind == JIT_TYPE_NUINT)	|| \
 (typeKind == JIT_TYPE_PTR))
#else
#define _JIT_TYPEKIND_IS_LONG(typeKind) \
((typeKind == JIT_TYPE_LONG)	|| \
 (typeKind == JIT_TYPE_ULONG))
#endif

/*
 * Check if the typeKind is unsigned.
 */
#define _JIT_TYPEKIND_IS_UNSIGNED(typeKind) \
((typeKind == JIT_TYPE_ULONG)	|| \
 (typeKind == JIT_TYPE_NUINT)	|| \
 (typeKind == JIT_TYPE_UINT)	|| \
 (typeKind == JIT_TYPE_USHORT)	|| \
 (typeKind == JIT_TYPE_UBYTE))

/*
 * Check if the typeKind is signed.
 */
#define _JIT_TYPEKIND_IS_SIGNED(typeKind) \
((typeKind == JIT_TYPE_LONG)	|| \
 (typeKind == JIT_TYPE_NINT)	|| \
 (typeKind == JIT_TYPE_INT)	|| \
 (typeKind == JIT_TYPE_SHORT)	|| \
 (typeKind == JIT_TYPE_SBYTE))

/*
 * Check if the typeKind is a pointer type.
 */
#define _JIT_TYPEKIND_IS_POINTER(typeKind) \
(typeKind == JIT_TYPE_PTR)

/*
 * Convert a value to the corresponding signed/unsigned type.
 * Returns 1 if the value is converted 0 otherwise.
 */
static int AdjustSign(ILJitFunction func, ILJitValue *value, int toUnsigned,
															  int checkOverflow)
{
	ILJitType type = jit_value_get_type(*value);
	int typeKind = jit_type_get_kind(type);
	
	if(_JIT_TYPEKIND_IS_SIGNED(typeKind) && toUnsigned)
	{
		switch(typeKind)
		{
			case JIT_TYPE_SBYTE:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_BYTE,
														checkOverflow);
				return 1;
			}
			break;

			case JIT_TYPE_SHORT:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_UINT16,
														checkOverflow);
				return 1;
			}
			break;

			case JIT_TYPE_INT:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_UINT32,
														checkOverflow);
				return 1;
			}
			break;

			case JIT_TYPE_NINT:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_NUINT,
														checkOverflow);
				return 1;
			}
			break;

			case JIT_TYPE_LONG:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_UINT64,
														checkOverflow);
				return 1;
			}
			break;
		}
	}
	else if(_JIT_TYPEKIND_IS_UNSIGNED(typeKind) && !toUnsigned)
	{
		switch(typeKind)
		{
			case JIT_TYPE_UBYTE:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_SBYTE,
														checkOverflow);
				return 1;
			}
			break;

			case JIT_TYPE_USHORT:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_INT16,
														checkOverflow);
				return 1;
			}
			break;

			case JIT_TYPE_UINT:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_INT32,
														checkOverflow);
				return 1;
			}
			break;

			case JIT_TYPE_NUINT:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_NINT,
														checkOverflow);
				return 1;
			}
			break;

			case JIT_TYPE_ULONG:
			{
				*value = jit_insn_convert(func, *value, _IL_JIT_TYPE_INT64,
														checkOverflow);
				return 1;
			}
			break;
		}
	}
	return 0;
}

/*
 * Do the explicit conversion of an ILJitValue to the given target type.
 * The value to convert is on the stack in it's source type. This means that no
 * implicit conversion to the stacktype (either INT32, INT64 or a pointer type
 * was done.
 * Because of this we have to take into account that unsigned values with a
 * length < 4 should have been zero extended to a size of an INT32 and signed
 * values would have been sign extended.
 */
static ILJitValue _ILJitValueConvertExplicit(ILJitFunction func,
											 ILJitValue value,
											 ILJitType targetType,
											 int isUnsigned,
											 int overflowCheck)
{
	ILJitType sourceType = jit_value_get_type(value);
	int sourceSize;
	int sourceTypeKind;
	int targetSize;
	int targetTypeKind;

	if(sourceType == targetType)
	{
		/* Nothing to do here. */
		return value;
	}
	if(jit_type_is_struct(sourceType) || jit_type_is_union(sourceType))
	{
		/* something is wrong here. */
		return value;
	}
	sourceTypeKind = jit_type_get_kind(sourceType);
	targetTypeKind = jit_type_get_kind(targetType);
	if(_JIT_TYPEKIND_IS_FLOAT(sourceTypeKind))
	{
		/* We can convert these values directly */
		return jit_insn_convert(func, value, targetType, overflowCheck);
	}
	else
	{
		if(_JIT_TYPEKIND_IS_FLOAT(targetTypeKind))
		{
			if(_JIT_TYPEKIND_IS_SIGNED(sourceTypeKind))
			{
				if(isUnsigned)
				{
					AdjustSign(func, &value, 1, overflowCheck);
				}
			}
			else if(_JIT_TYPEKIND_IS_UNSIGNED(sourceTypeKind))
			{
				if(!isUnsigned)
				{
					AdjustSign(func, &value, 0, overflowCheck);
				}
			}
			else if(_JIT_TYPEKIND_IS_POINTER(sourceTypeKind))
			{
				if(isUnsigned)
				{
					value = jit_insn_convert(func, value, _IL_JIT_TYPE_NUINT,
														  overflowCheck);
				}
				else
				{
						value = jit_insn_convert(func, value, _IL_JIT_TYPE_NINT,
													  overflowCheck);
				}
			}
			value = jit_insn_convert(func, value, targetType, overflowCheck);
		}
		else
		{
			sourceSize = jit_type_get_size(sourceType);
			targetSize = jit_type_get_size(targetType);

			if(targetSize <= sourceSize)
			{
				/* The value is truncated or the sign is changed. */
			}
			else
			{
				if(_JIT_TYPEKIND_IS_LONG(targetTypeKind))
				{
					if(_JIT_TYPEKIND_IS_SIGNED(targetTypeKind))
					{
						/* In this case we have to zero extend unsigned source */
						/* values to the size of an INT32 first. */
						if(_JIT_TYPEKIND_IS_UNSIGNED(sourceTypeKind))
						{
							/* We have to zero extend the value to the */
							/* size an INT32 first. */
							/* I assume that unsigned source values will be  */
							/* zero extended to the target size. */
							value = jit_insn_convert(func, value,
														 _IL_JIT_TYPE_INT32,
														 overflowCheck);
						}
						else if(_JIT_TYPEKIND_IS_POINTER(sourceTypeKind))
						{
							/* Pointers have to be sign extended in this case. */
							/* So we convert to a signed NINT first. */
							value = jit_insn_convert(func, value,
														 _IL_JIT_TYPE_NINT,
														 overflowCheck);
						}
					}
					else if(_JIT_TYPEKIND_IS_UNSIGNED(targetTypeKind))
					{

						/* In this case we have to sign extend signed source */
						/* values to the size of an INT32 first. */
						if(_JIT_TYPEKIND_IS_SIGNED(sourceTypeKind))
						{
							/* We have to sign extend the value to the */
							/* size an INT32 first. */
							/* I assume that signed source values will be  */
							/* sign extended to the target size. */
							value = jit_insn_convert(func, value,
														 _IL_JIT_TYPE_UINT32,
														 overflowCheck);
						}
						else if(_JIT_TYPEKIND_IS_POINTER(sourceTypeKind))
						{
							/* Pointers have to be zero extended in this case. */
							/* So we convert to a unsigned NUINT first. */
							value = jit_insn_convert(func, value,
														 _IL_JIT_TYPE_NUINT,
														 overflowCheck);
						}
					}
#ifdef IL_NATIVE_INT64
					else if(_JIT_TYPEKIND_IS_POINTER(targetTypeKind))
					{
						/* This is not allowed in explicit conversion. */
						/* But we treat pointers like unsigned values. */
						if(_JIT_TYPEKIND_IS_SIGNED(sourceTypeKind))
						{
							AdjustSign(func, &value, 1, overflowCheck);
						}
					}
#endif
				}
				else
				{
					/* The value would have been zero- or sign extended to */
					/* the size of an INT32 depending on the signedness of */
					/* the value. Now we have the less or the same size so */
					/* we can safely simply convert to the target type.    */
				}
			}
			value = jit_insn_convert(func, value, targetType, overflowCheck);
		}
	}
	return value;
}

/*
 * Do the implicit conversion of an ILJitValue to the given target type.
 * The value to convert is on the stack in it's source type. This means that no
 * implicit conversion to the stacktype (either INT32, INT64 or a pointer type
 * was done.
 * Because of this we have to take into account that unsigned values with a
 * length < 4 should have been zero extended to a size of an INT32 and signed
 * values would have been sign extended.
 */
static ILJitValue _ILJitValueConvertImplicit(ILJitFunction func,
											 ILJitValue value,
											 ILJitType targetType)
{
	ILJitType sourceType = jit_value_get_type(value);
	int sourceTypeKind;
	int targetTypeKind;

	if(sourceType == targetType)
	{
		/* Nothing to do here. */
		return value;
	}
	if(jit_type_is_struct(sourceType) || jit_type_is_union(sourceType))
	{
		/* something is wrong here. */
		return value;
	}
	sourceTypeKind = jit_type_get_kind(sourceType);
	targetTypeKind = jit_type_get_kind(targetType);
	if(_JIT_TYPEKIND_IS_FLOAT(sourceTypeKind))
	{
		/* We can convert these values directly */
		return jit_insn_convert(func, value, targetType, 0);
	}
	else
	{
		if(_JIT_TYPEKIND_IS_FLOAT(targetTypeKind))
		{
			value = jit_insn_convert(func, value, targetType, 0);
		}
		else
		{
			int sourceSize = jit_type_get_size(sourceType);
			int targetSize = jit_type_get_size(targetType);

			if(targetSize <= sourceSize)
			{
				/* The value is truncated or the sign is changed. */
			}
			else
			{
				if(_JIT_TYPEKIND_IS_LONG(targetTypeKind))
				{
					if(_JIT_TYPEKIND_IS_SIGNED(targetTypeKind))
					{
						/* In this case we have to zero extend unsigned source */
						/* values to the size of an INT32 first. */
						if(_JIT_TYPEKIND_IS_UNSIGNED(sourceTypeKind))
						{
							/* We have to zero extend the value to the */
							/* size an INT32 first. */
							/* I assume that unsigned source values will be  */
							/* zero extended to the target size. */
							value = jit_insn_convert(func, value,
														 _IL_JIT_TYPE_INT32,
														 0);
						}
						else if(_JIT_TYPEKIND_IS_POINTER(sourceTypeKind))
						{
							/* Pointers have to be sign extended in this case. */
							/* So we convert to a signed NINT first. */
							value = jit_insn_convert(func, value,
														 _IL_JIT_TYPE_NINT,
														 0);
						}
					}
					else if(_JIT_TYPEKIND_IS_UNSIGNED(targetTypeKind))
					{

						/* In this case we have to sign extend signed source */
						/* values to the size of an INT32 first. */
						if(_JIT_TYPEKIND_IS_SIGNED(sourceTypeKind))
						{
							/* We have to sign extend the value to the */
							/* size an INT32 first. */
							/* I assume that signed source values will be  */
							/* sign extended to the target size. */
							value = jit_insn_convert(func, value,
														 _IL_JIT_TYPE_UINT32,
														 0);
						}
						else if(_JIT_TYPEKIND_IS_POINTER(sourceTypeKind))
						{
							/* Pointers have to be zero extended in this case. */
							/* So we convert to a unsigned NUINT first. */
							value = jit_insn_convert(func, value,
														 _IL_JIT_TYPE_NUINT,
														 0);
						}
					}
#ifdef IL_NATIVE_INT64
					else if(_JIT_TYPEKIND_IS_POINTER(targetTypeKind))
					{
						/* This is not allowed in explicit conversion. */
						/* But we treat pointers like unsigned values. */
						if(_JIT_TYPEKIND_IS_SIGNED(sourceTypeKind))
						{
							AdjustSign(func, &value, 1, 0);
						}
					}
#endif
				}
				else
				{
					/* The value would have been zero- or sign extended to */
					/* the size of an INT32 depending on the signedness of */
					/* the value. Now we have the less or the same size so */
					/* we can safely simply convert to the target type.    */
				}
			}
			value = jit_insn_convert(func, value, targetType, 0);
		}
	}
	return value;
}

/*
 * Readjust the stack to normalize binary operands when
 * I and I4 are mixed together.  Also determine which of
 * I4 or I8 to use if the operation involves I.
 */
static void AdjustMixedBinary(ILJITCoder *coder, int isUnsigned,
							  ILJitValue *value1, ILJitValue *value2)
{
	ILJitType type1 = jit_value_get_type(*value1);
	ILJitType newType1 = type1;
	ILJitType type2 = jit_value_get_type(*value2);
	ILJitType newType2 = type2;
	int type1Kind = jit_type_get_kind(type1);
	int type2Kind = jit_type_get_kind(type2);
	int type1IsFloat = _JIT_TYPEKIND_IS_FLOAT(type1Kind);
	int type2IsFloat = _JIT_TYPEKIND_IS_FLOAT(type2Kind);
	int type1IsLong = _JIT_TYPEKIND_IS_LONG(type1Kind);
	int type2IsLong = _JIT_TYPEKIND_IS_LONG(type2Kind);

	if(type1IsFloat || type2IsFloat)
	{
		return;
	}

	/* If the arguments mix I8 and I4, then cast the I4 value to I8 */
	if(type1IsLong && !type2IsLong)
	{
		if(isUnsigned)
		{
			newType2 = _IL_JIT_TYPE_UINT64;
		}
		else
		{
			newType2 = _IL_JIT_TYPE_INT64;
		}
		type2Kind = jit_type_get_kind(newType2);
		type2IsLong = 1;
	}
	else if(!type1IsLong && type2IsLong)
	{
		if(isUnsigned)
		{
			newType1 = _IL_JIT_TYPE_UINT64;
		}
		else
		{
			newType1 = _IL_JIT_TYPE_INT64;
		}
		type1Kind = jit_type_get_kind(newType1);
		type1IsLong = 1;
	}

	if(isUnsigned)
	{
		if(_JIT_TYPEKIND_IS_SIGNED(type1Kind))
		{
			if(type1IsLong)
			{
				newType1 = _IL_JIT_TYPE_UINT64;
			}
			else
			{
				newType1 = _IL_JIT_TYPE_UINT32;
			}
			type1Kind = jit_type_get_kind(newType1);
		}
		if(_JIT_TYPEKIND_IS_SIGNED(type2Kind))
		{
			if(type2IsLong)
			{
				newType2 = _IL_JIT_TYPE_UINT64;
			}
			else
			{
				newType2 = _IL_JIT_TYPE_UINT32;
			}
			type2Kind = jit_type_get_kind(newType2);
		}
	}
	else
	{
		if(_JIT_TYPEKIND_IS_UNSIGNED(type1Kind))
		{
			if(type1IsLong)
			{
				newType1 = _IL_JIT_TYPE_INT64;
			}
			else
			{
				newType1 = _IL_JIT_TYPE_INT32;
			}
			type1Kind = jit_type_get_kind(newType1);
		}
		if(_JIT_TYPEKIND_IS_UNSIGNED(type2Kind))
		{
			if(type2IsLong)
			{
				newType2 = _IL_JIT_TYPE_INT64;
			}
			else
			{
				newType2 = _IL_JIT_TYPE_INT32;
			}
			type2Kind = jit_type_get_kind(newType2);
		}
	}
	
	/* now do the conversion if necessairy. */
	if(type1 != newType1)
	{
		*value1 = _ILJitValueConvertImplicit(coder->jitFunction, *value1,
											 newType1);
	}
	if(type2 != newType2)
	{
		*value2 = _ILJitValueConvertImplicit(coder->jitFunction, *value2,
											 newType2);
	}
}

/*
 * Perform a class layout.
 */
static int _LayoutClass(ILExecThread *thread, ILClass *info)
{
	int result = 0;

	/* Check if the class is allready layouted. */
	if((info->userData) && !(((ILClassPrivate *)(info->userData))->inLayout))
	{
		return 1;
	}
	METADATA_WRLOCK(thread);
	result = _ILLayoutClass(_ILExecThreadProcess(thread), info);
	METADATA_UNLOCK(thread);
	return result; 
}

/*
 * Function to free the metadata attached to a jit_function_t.
 */
static void _ILJitMetaFreeFunc(void *data)
{
	if(data)
	{
		ILFree(data);
	}
}

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
/*
 * Get a complete methodname.
 * The string returned must be freed by the caller.
 */
static char *_ILJitGetMethodName(ILMethod *method)
{
	const char *methodName = ILMethod_Name(method);
	const char *className = ILClass_Name(ILMethod_Owner(method));
	const char *namespaceName = ILClass_Namespace(ILMethod_Owner(method));
	int namespaceLen = 0;
	int classLen = 0;
	int methodLen = 0;
	int len = 0;
	char *fullName = 0;

	if(namespaceName)
	{
		namespaceLen = strlen(namespaceName);
		len += namespaceLen + 1;
	}
	if(className)
	{
		classLen = strlen(className);
		len += classLen + 1;
	}
	if(methodName)
	{
		methodLen = strlen(methodName);
		len += methodLen;
	}
	if(len)
	{
		int pos = 0;

		if(!(fullName = ILMalloc(len + 1)))
		{
			return 0;
		}
		if(namespaceName)
		{
			strcpy(fullName, namespaceName);
			pos = namespaceLen;
			fullName[pos] = '.';
			pos++;
		}
		if(className)
		{
			strcpy(fullName + pos, className);
			pos += classLen;
			fullName[pos] = '.';
			pos++;
		}
		if(methodName)
		{
			strcpy(fullName + pos, methodName);
			pos += methodLen;
		}
	}
	return fullName;
}

/*
 * Set the full methodname in the functions metadata.
 */
static void _ILJitFunctionSetMethodName(ILJitFunction func, ILMethod *method)
{
	char *methodName = _ILJitGetMethodName(method);

	if(methodName)
	{
		jit_function_set_meta(func, IL_JIT_META_METHODNAME, methodName,
							  _ILJitMetaFreeFunc, 0);
	}
}

/*
 * Get the full methodname from the jit functions metadata.
 */
static char *_ILJitFunctionGetMethodName(ILJitFunction func)
{
	return (char *)jit_function_get_meta(func, IL_JIT_META_METHODNAME);
}
#endif

/*
 * Destroy every ILJitType in a ILJitTypes  structure 
 */
void ILJitTypesDestroy(ILJitTypes *jitTypes)
{
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
	type = ILTypeGetEnumType(type);
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
 * Rethrow the given exception.
 */
void ILRuntimeExceptionRethrow(ILObject *object)
{
	ILExecThread *thread = ILExecThreadCurrent();

	if(thread)
	{
		thread->thrownException = object;
	}
	if(object)
	{
		jit_exception_throw(object);
	}
}

/*
 * Throw the given exception.
 */
void ILRuntimeExceptionThrow(ILObject *object)
{
	ILExecThread *thread = ILExecThreadCurrent();
	System_Exception *exception = (System_Exception *)object;

	if(thread)
	{
		thread->thrownException = object;
		if(exception)
		{
			exception->stackTrace = _ILJitGetExceptionStackTrace(thread);
		}
	}
	if(exception)
	{
		jit_exception_throw(exception);
	}
}

/*
 * Throw an exception of the given type.
 * The caller has to make sure that the exception class is allready layouted.
 */
void ILRuntimeExceptionThrowClass(ILClass *classInfo)
{
	ILExecThread *thread = ILExecThreadCurrent();
	System_Exception *exception =
		(System_Exception *)_ILJitAlloc(classInfo,
							((ILClassPrivate *)(classInfo->userData))->size);

	/* We don't have to check exception for 0 because _ILJitAlloc would have */
	/* thrown an OutOfMenory exception then. */
	if(thread)
	{
		thread->thrownException = (ILObject *)exception;
		exception->stackTrace = _ILJitGetExceptionStackTrace(thread);
	}
	jit_exception_throw(exception);
}

/*
 * Throw an OutOfMemoryException.
 */
void ILRuntimeExceptionThrowOutOfMemory()
{
	ILExecThread *thread = ILExecThreadCurrent();
	void *exception = 0;

	if(thread)
	{
		thread->thrownException = thread->process->outOfMemoryObject;
		jit_exception_throw(exception);
		return;
	}
	jit_exception_builtin(JIT_RESULT_OUT_OF_MEMORY);
}

/*
 * Handle the managed safepoint flags after a native call was made.
 */
void ILRuntimeHandleManagedSafePointFlags(ILExecThread *thread)
{
	if((thread->managedSafePointFlags & _IL_MANAGED_SAFEPOINT_THREAD_ABORT) && ILThreadIsAbortRequested())
	{
		if(_ILExecThreadSelfAborting(thread) == 0)
		{
			jit_exception_throw(thread->thrownException);
		}
	}
	else if(thread->managedSafePointFlags & _IL_MANAGED_SAFEPOINT_THREAD_SUSPEND)
	{
		ILThreadAtomicStart();
		thread->managedSafePointFlags &= ~_IL_MANAGED_SAFEPOINT_THREAD_SUSPEND;
		ILThreadAtomicEnd();
		if(ILThreadGetState(thread->supportThread) & IL_TS_SUSPEND_REQUESTED)
		{
			_ILExecThreadSuspendThread(thread, thread->supportThread);
		}
	}
}

/*
 * Handle an exception thrown in an internal call.
 */
static void _ILJitHandleThrownException(ILJitFunction func,
										ILJitValue thread)
{ 
	ILJitValue thrownException = jit_insn_load_relative(func, thread,
									offsetof(ILExecThread, thrownException),
									_IL_JIT_TYPE_VPTR);
	jit_label_t label = jit_label_undefined;

	jit_insn_branch_if_not(func, thrownException, &label);
	jit_insn_call_native(func, "jit_exception_clear_last",
								jit_exception_clear_last,
								_ILJitSignature_JitExceptionClearLast,
								0, 0, JIT_CALL_NOTHROW);
	jit_insn_throw(func, thrownException);
	jit_insn_label(func, &label);
}

/*
 * Perform a class cast, taking arrays into account.
 */
ILInt32 ILRuntimeCanCastClass(ILMethod *method, ILObject *object, ILClass *toClass)
{
	ILImage *image = ILProgramItem_Image(method);
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

#ifdef IL_JIT_FNPTR_ILMETHOD
/*
 * Get the vtable pointer for an ILMethod.
 */
void *ILRuntimeMethodToVtablePointer(ILMethod *method)
{
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(method);

	if(!jitFunction)
	{
		/* The method's owner class needs to be layouted first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(method)))
		{
			/* TODO: Throw an exception here. */
			return 0;
		}
		if(!(jitFunction = ILJitFunctionFromILMethod(method)))
		{
			/* TODO: Throw an exception here. */
			return 0;
		}
	}
	return jit_function_to_vtable_pointer(jitFunction);
}
#endif

static void *_ILRuntimeLookupInterfaceMethod(ILClassPrivate *objectClassPrivate,
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
					return objectClassPrivate->jitVtable[index];
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

#ifdef IL_JIT_FNPTR_ILMETHOD
/*
 * This is the same function as above but returns the ILMethod instead of the
 * vtable pointer of the interface method.
 */
static void *_ILRuntimeLookupInterfaceILMethod(ILClassPrivate *objectClassPrivate,
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
#endif

/*
 * Get the pointer to base type from the JitTypes.
 * The pointer type is created on demand if not allready present.
 * Returns 0 when out of memory.
 */
static ILJitType _ILJitGetPointerTypeFromJitTypes(ILJitTypes *types)
{
	return _IL_JIT_TYPE_VPTR;
}

/*
 * Get the jit type representing the this pointer for the given ILType.
 * Returns 0 whne the type could not be found or out of memory.
 */
static ILJitType _ILJitGetThisType(ILType *type, ILExecProcess *process)
{
	return _IL_JIT_TYPE_VPTR;
}

/*
 * Get the jit type representing the argument type for the given ILType.
 * Returns 0 whne the type could not be found or out of memory.
 * TODO: Handle ref and out args.
 */
static ILJitType _ILJitGetArgType(ILType *type, ILExecProcess *process)
{
	if(ILType_IsClass(type))
	{
		return _IL_JIT_TYPE_VPTR;
	}
	else
	{
		ILJitTypes *types = _ILJitGetTypes(type, process);

		if(!types)
		{
			return 0;
		}
		return types->jitTypeBase;
	}
}

/*
 * Get the jit type representing the local type for the given ILType.
 * Returns 0 when the type could not be found or out of memory.
 */
static ILJitType _ILJitGetLocalsType(ILType *type, ILExecProcess *process)
{
	if(ILType_IsClass(type))
	{
		return _IL_JIT_TYPE_VPTR;
	}
	else
	{
		ILJitTypes *types = _ILJitGetTypes(type, process);

		if(!types)
		{
			return 0;
		}
		return types->jitTypeBase;
	}
}

/*
 * Get the jit type representing the return value for the given ILType.
 */
static ILJitType _ILJitGetReturnType(ILType *type, ILExecProcess *process)
{
	if(ILType_IsClass(type))
	{
		return _IL_JIT_TYPE_VPTR;
	}
	else
	{
		ILJitTypes *types = _ILJitGetTypes(type, process);

		if(!types)
		{
			return 0;
		}
		return types->jitTypeBase;
	}
}

/*
 * Get the jit type from an ILClass.
 */
static ILJitType _ILJitGetTypeFromClass(ILClass *info)
{
	ILClassPrivate *classPrivate = (ILClassPrivate *)info->userData;

	if(!classPrivate)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), info))
		{
			return 0;
		}
		classPrivate = (ILClassPrivate *)info->userData;
	}
	return(classPrivate->jitTypes.jitTypeBase);
}

/*
 * Get the size of an object.
 */
static ILUInt32 _ILJitGetSizeOfClass(ILClass *info)
{
	ILClassPrivate *classPrivate = (ILClassPrivate *)info->userData;

	if(!classPrivate)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), info))
		{
			return 0;
		}
		classPrivate = (ILClassPrivate *)info->userData;
	}
	return((ILUInt32)jit_type_get_size(classPrivate->jitTypes.jitTypeBase));
}

/*
 * Call the static constructor for a class if necessary.
 */
static void _ILJitCallStaticConstructor(ILJITCoder *coder, ILClass *classInfo,
								  int isCtor)
{
	if((classInfo->attributes & IL_META_TYPEDEF_CCTOR_ONCE) != 0)
	{
		/* We already know that the static constructor has been called,
		   so there is no point outputting a call to it again */
		return;
	}
	if(isCtor ||
	   (classInfo->attributes & IL_META_TYPEDEF_BEFORE_FIELD_INIT) == 0)
	{
		/* We must call the static constructor before instance
		   constructors, or before static methods when the
		   "beforefieldinit" attribute is not present */
		ILMethod *cctor = 0;
		while((cctor = (ILMethod *)ILClassNextMemberByKind
					(classInfo, (ILMember *)cctor,
					 IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if(ILMethod_IsStaticConstructor(cctor))
			{
				break;
			}
		}
		if(cctor != 0)
		{
			/* Don't call it if we are within the constructor already */
			if(cctor != coder->currentMethod)
			{
				/* Output a call to the static constructor */
			#ifdef IL_JIT_THREAD_IN_SIGNATURE
				jit_value_t thread = _ILJitCoderGetThread(coder);

				jit_insn_call(coder->jitFunction, "cctor",
								ILJitFunctionFromILMethod(cctor), 0,
								&thread, 1, 0);
			#else
				jit_insn_call(coder->jitFunction, "cctor",
								ILJitFunctionFromILMethod(cctor), 0,
								0, 0, 0);
			#endif
			}
		}
		else
		{
			/* This class does not have a static constructor,
			   so mark it so that we never do this test again */
			classInfo->attributes |= IL_META_TYPEDEF_CCTOR_ONCE;
		}
	}
}

/*
 * The exception handler which converts libjit inbuilt exceptions
 * into clr exceptions.
 */
void *_ILJitExceptionHandler(int exception_type)
{
    ILExecThread *thread = ILExecThreadCurrent();
    void *object = 0;

    switch(exception_type)
    {
		case(JIT_RESULT_OVERFLOW):
		{
			object = _ILSystemException(thread, "System.OverflowException");
		}
		break;

		case(JIT_RESULT_ARITHMETIC):
		{
			object = _ILSystemException(thread, "System.ArithmeticException");
		}
		break;

		case(JIT_RESULT_DIVISION_BY_ZERO):
		{
			object = _ILSystemException(thread, "System.DivideByZeroException");
		}
		break;

		case(JIT_RESULT_COMPILE_ERROR):
		{
			object = _ILSystemException(thread, "System.ExecutionEngineException");
		}
		break;

		case(JIT_RESULT_OUT_OF_MEMORY):
		{
			object = _ILSystemException(thread, "System.OutOfMemoryException");
		}    
		break;

		case(JIT_RESULT_NULL_REFERENCE):
		{
			object = _ILSystemException(thread, "System.NullReferenceException");
		}
		break;

		case(JIT_RESULT_NULL_FUNCTION):
		{
			object = _ILSystemException(thread, "System.MissingMethodException");
		}
		break;

		case(JIT_RESULT_CALLED_NESTED):
		{
			object = _ILSystemException(thread, "System.MissingMethodException");
		}
		break;

		case(JIT_RESULT_OUT_OF_BOUNDS):
		{
			object = _ILSystemException(thread, "System.IndexOutOfRangeException");
		}
		break;

		default:
		{
			object = _ILSystemException(thread, "System.Exception");
		}
		break;
	}
	thread->thrownException = object;
	return object;
}

/*
 * Generate the code to throw the current exception in the thread in libjit.
 */
static void _ILJitThrowCurrentException(ILJITCoder *coder)
{
	ILJitValue thread = _ILJitCoderGetThread(coder);
	ILJitValue thrownException = jit_insn_load_relative(coder->jitFunction,
									thread,
									offsetof(ILExecThread, thrownException), 
									jit_type_void_ptr);
	jit_insn_throw(coder->jitFunction, thrownException);
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


	// Initialize the TypedRef type in its jit representation.	
	if(!(_ILJitTypedRef = jit_type_create_struct(0, 0, 0)))
	{
		return 0;
	}
	jit_type_set_size_and_alignment(_ILJitTypedRef,
									sizeof(ILTypedRef),
									_IL_ALIGN_FOR_TYPE(void_p));
	_ILJitTypesInitBase(&_ILJitType_TYPEDREF, _ILJitTypedRef);

	/* Initialize the native method signatures. */
#ifndef IL_JIT_THREAD_IN_SIGNATURE
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILExecThreadCurrent = 
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, 0, 0, 1)))
	{
		return 0;
	}
#endif

	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_UINT32;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILJitAlloc = 
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 2, 1)))
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
	returnType = _IL_JIT_TYPE_VOID;
	if(!(_ILJitSignature_ILRuntimeExceptionRethrow =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 1, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VOID;
	if(!(_ILJitSignature_ILRuntimeExceptionThrow =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 1, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VOID;
	if(!(_ILJitSignature_ILRuntimeExceptionThrowClass =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 1, 1)))
	{
		return 0;
	}

	returnType = _IL_JIT_TYPE_VOID;
	if(!(_ILJitSignature_ILRuntimeExceptionThrowOutOfMemory =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, 0, 0, 1)))
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

	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	args[2] = _IL_JIT_TYPE_UINT32;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILRuntimeLookupInterfaceMethod = 
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 3, 1)))
	{
		return 0;
	}

	returnType = _IL_JIT_TYPE_VOID;
	if(!(_ILJitSignature_JitExceptionClearLast =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, 0, 0, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILJitGetExceptionStackTrace =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 1, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_NINT;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_malloc =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 1, 1)))
	{
		return 0;
	}

#ifdef IL_JIT_FNPTR_ILMETHOD
	args[0] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILRuntimeMethodToVtablePointer =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 1, 1)))
	{
		return 0;
	}
#endif

	/* Create the signatures for the inlined native function calls. */
	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VOID;
	if(!(_ILJitSignature_ILMonitorEnter =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 2, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VOID;
	if(!(_ILJitSignature_ILMonitorExit =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 2, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILGetClrType =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 2, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VOID;
	if(!(_ILJitSignature_ILRuntimeHandleManagedSafePointFlags =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 1, 1)))
	{
		return 0;
	}

	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILStringToUTF8 =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 2, 1)))
	{
		return 0;
	}
	
	args[0] = _IL_JIT_TYPE_VPTR;
	args[1] = _IL_JIT_TYPE_VPTR;
	returnType = _IL_JIT_TYPE_VPTR;
	if(!(_ILJitSignature_ILStringCreate =
		jit_type_create_signature(IL_JIT_CALLCONV_CDECL, returnType, args, 2, 1)))
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

	/* Intialize the pool for the method infos. */
	ILMemPoolInit(&(coder->methodPool), sizeof(ILJitMethodInfo), 100);

	/* Initialize the stack management. */
	coder->jitStack = 0;
	coder->stackTop = -1;
	coder->stackSize = 0;

	/* Initialize the parameter management. */
	_ILJitLocalSlotsInit(coder->jitParams)

	/* Initialize the locals management. */
	_ILJitLocalSlotsInit(coder->jitLocals)

	/* Init the current jitted function. */
	coder->jitFunction = 0;

	/* Init the label stuff. */
	ILMemPoolInit(&(coder->labelPool), sizeof(ILJITLabel), 8);
	coder->labelList = 0;
	coder->labelOutOfMemory = 0;

	/* Init the switch stuff. */
	coder->switchValue = 0;
	coder->numSwitch = 0;
	coder->maxSwitch = 0;

#ifndef IL_JIT_THREAD_IN_SIGNATURE
	coder->thread = 0;
#endif

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

	_ILJitLocalSlotsDestroy(coder->jitLocals)

	_ILJitLocalSlotsDestroy(coder->jitParams)

	if(coder->context)
	{
		jit_context_destroy(coder->context);
		coder->context = 0;
	}
	ILMemPoolDestroy(&(coder->labelPool));

	ILMemPoolDestroy(&(coder->methodPool));

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
 * Get the ILClass pointer from an object reference.
 */
static ILJitValue _ILJitGetObjectClass(ILJitFunction func, ILJitValue object)
{
	ILJitValue classPrivate = _ILJitGetObjectClassPrivate(func, object);

	return jit_insn_load_relative(func, classPrivate, 
								  offsetof(ILClassPrivate, classInfo),
								  _IL_JIT_TYPE_VPTR);
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
	ILJitValue temp = value;
	ILJitType type = jit_value_get_type(temp);
	ILJitType stackType = _ILJitTypeToStackType(type);

	if(type != stackType)
	{
		temp = jit_insn_convert(func, temp, stackType, 0);
	}

	/* We have only signed values on the stack. */
	AdjustSign(func, &temp, 0, 0);

	return temp;
}

/*
 * Emit the code that has to run before a native function is executed.
 */
static void _ILJitBeginNativeCall(ILJitFunction func, ILJitValue thread)
{
	ILJitValue zero = jit_value_create_nint_constant(func,
													 jit_type_sys_int,
													 (jit_nint)0);

	jit_insn_store_relative(func, thread,
							offsetof(ILExecThread, runningManagedCode),
							zero);
}

/*
 * Emit the code that has to run after a native function is executed.
 */
static void _ILJitEndNativeCall(ILJitFunction func, ILJitValue thread)
{
	ILJitValue one = jit_value_create_nint_constant(func,
													 jit_type_sys_int,
													 (jit_nint)1);
	ILJitValue temp = jit_insn_load_relative(func,
											thread,
											offsetof(ILExecThread, managedSafePointFlags),
											jit_type_sys_int);
	jit_label_t label = jit_label_undefined;

	jit_insn_store_relative(func, thread,
							offsetof(ILExecThread, runningManagedCode),
							one);
	jit_insn_branch_if_not(func, temp, &label);
	jit_insn_call_native(func,
						 "ILRuntimeHandleManagedSafePointFlags",
						 ILRuntimeHandleManagedSafePointFlags,
						 _ILJitSignature_ILRuntimeHandleManagedSafePointFlags,
						 &thread, 1, 0);

	jit_insn_label(func, &label);
	_ILJitHandleThrownException(func, thread);
}

/*
 * Check if a function is implemented by an internalcall.
 * Returns 0 if the function is not implemented by an internal call,
 * 1 if the function is implemented by an internal call and the function
 * is not an allocating constructor (it doesn't allocate the new object)
 * and 2 if the function is an allocating constructor.
 */
static int _ILJitFunctionIsInternal(ILJITCoder *coder, ILMethod *method,
									ILInternalInfo *fnInfo, int isConstructor)
{
	ILJitMethodInfo *jitMethodInfo = (ILJitMethodInfo *)(method->userData);

	if(jitMethodInfo)
	{
		if((jitMethodInfo->implementationType) & _IL_JIT_IMPL_INTERNALMASK)
		{
			fnInfo->func = jitMethodInfo->fnInfo.func;
			return (jitMethodInfo->implementationType) & _IL_JIT_IMPL_INTERNALMASK;
		}
	}
	return 0;
}

/*
 * Generate the code to call an internal function.
 */
static ILJitValue _ILJitCallInternal(ILJitFunction func,
									 ILJitValue thread,
									 ILMethod *method,
									 void *nativeFunction,
									 const char *methodName,
									 ILJitValue *args,
									 ILUInt32 numArgs)
{
	ILType *ilSignature = ILMethod_Signature(method);
	ILType *type = ILTypeGetEnumType(ILTypeGetParam(ilSignature, 0));
	/* Get the function to call. */
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(method);
	ILJitType signature = 0;
	ILJitType callSignature = 0;
	ILJitType returnType = 0;
	ILJitValue returnValue = 0;
	unsigned int numParams = 0;
	unsigned int totalParams = 0;
	int hasStructReturn = 0;
	ILUInt32 current = 0;

	if(!jitFunction)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(method)))
		{
			return 0;
		}
		if(!(jitFunction = ILJitFunctionFromILMethod(method)))
		{
			return 0;
		}
	}

	if(!(signature = jit_function_get_signature(jitFunction)))
	{
		return 0;
	}
	numParams = jit_type_num_params(signature);
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	totalParams = numParams;
#else
	/* otherwise we need the thread as an additional argument. */
	totalParams = numParams + 1;
#endif
	returnType = jit_type_get_return(signature);;

#ifdef IL_JIT_THREAD_IN_SIGNATURE
	if(numParams != (numArgs + 1))
#else
	if(numParams != numArgs)
#endif
	{
		printf("Number of args doesn't match: signature: %i - numArgs: %i\n", numParams, numArgs);
	}

	{
		/* We need to set the method member in the ILExecThread == arg[0]. */
		ILJitType paramType;
		ILJitType jitParamTypes[totalParams + 1];
		ILJitValue jitParams[totalParams + 1];
		ILUInt32 param = 1;

		/* We need to create a new Signature for the native Call with */
		/* an additional argument when the return value is a value type */
		/* and pointers for structs. */
		/* Set the current thread as arg[0]. */
		jitParamTypes[0] = _IL_JIT_TYPE_VPTR;
		jitParams[0] = thread;

		/* Check if the return type is a value type. */
		if(ILType_IsValueType(type))
		{
			++totalParams;
			jitParamTypes[1] = _IL_JIT_TYPE_VPTR;
			returnValue = jit_value_create(func, returnType);
			jitParams[1] = jit_insn_address_of(func, returnValue);
			returnType = _IL_JIT_TYPE_VOID;
			hasStructReturn = 1;
			++param;
		}

	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		for(current = 1; current < numParams; ++current)
	#else
		for(current = 0; current < numParams; ++current)
	#endif
		{
			paramType = jit_type_get_param(signature, current);

			if(jit_type_is_struct(paramType))
			{
				jitParamTypes[param] = _IL_JIT_TYPE_VPTR;
			#ifdef IL_JIT_THREAD_IN_SIGNATURE
				jitParams[param] = jit_insn_address_of(func, args[current - 1]);
			#else
				jitParams[param] = jit_insn_address_of(func, args[current]);
			#endif
			}
			else
			{
				jitParamTypes[param] = paramType;
			#ifdef IL_JIT_THREAD_IN_SIGNATURE
				jitParams[param] = args[current - 1];
			#else
				jitParams[param] = args[current];
			#endif
			}
			++param;
		}
		callSignature = jit_type_create_signature(IL_JIT_CALLCONV_CDECL,
												  returnType,
												  jitParamTypes,
												  totalParams, 1);

		_ILJitSetMethodInThread(func, thread, method);
		_ILJitBeginNativeCall(func, thread);
		if(!hasStructReturn)
		{
			returnValue = jit_insn_call_native(func, methodName, nativeFunction,
											   callSignature,
											   jitParams, totalParams, 0);
		}
		else
		{
			jit_insn_call_native(func, methodName, nativeFunction,
							 	 callSignature,
							 	 jitParams, totalParams, 0);
		}
		jit_type_free(callSignature);
	}
	_ILJitEndNativeCall(func, thread);

	return returnValue;
}

/*
 * Generate the stub for calling an internal function.
 */
static int _ILJitCompileInternal(ILJitFunction func)
{
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	ILExecThread *_thread = ILExecThreadCurrent();
	ILJITCoder *jitCoder = (ILJITCoder *)(_ILExecThreadProcess(_thread)->coder);
#endif
	ILMethod *method = (ILMethod *)jit_function_get_meta(func, IL_JIT_META_METHOD);
	ILJitMethodInfo *jitMethodInfo = (ILJitMethodInfo *)(method->userData);
	ILJitType signature = jit_function_get_signature(func);
	unsigned int numParams = jit_type_num_params(signature);
	ILJitValue returnValue = 0;
	ILJitValue thread = _ILJitFunctionGetThread(func);
	char *methodName = 0;
	ILJitValue paramValue;
	ILUInt32 current;
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	ILJitValue jitParams[numParams - 1];
#else
	ILJitValue jitParams[numParams];
#endif

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	methodName = _ILJitFunctionGetMethodName(func);
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout, "CompileInternalMethod: %s\n", methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

#ifdef IL_JIT_THREAD_IN_SIGNATURE
	for(current = 1; current < numParams; ++current)
	{
		if(!(paramValue = jit_value_get_param(func, current)))
		{
			return JIT_RESULT_OUT_OF_MEMORY;
		}
		jitParams[current - 1] = paramValue;
	}
	returnValue = _ILJitCallInternal(func, thread, method,
					 				jitMethodInfo->fnInfo.func, methodName,
									 jitParams, numParams - 1);
#else
	for(current = 0; current < numParams; ++current)
	{
		if(!(paramValue = jit_value_get_param(func, current)))
		{
			return JIT_RESULT_OUT_OF_MEMORY;
		}
		jitParams[current] = paramValue;
	}
	returnValue = _ILJitCallInternal(func, thread, method,
					 				jitMethodInfo->fnInfo.func, methodName,
									 jitParams, numParams);
#endif
	jit_insn_return(func, returnValue);	

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
#ifdef _IL_JIT_DUMP_FUNCTION
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		jit_dump_function(stdout, func, methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#ifdef _IL_JIT_DISASSEMBLE_FUNCTION
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		if(!jit_function_compile(func))
		{
			return IL_CODER_END_TOO_BIG;
		}
		ILMutexLock(globalTraceMutex);
		jit_dump_function(stdout, func, methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#endif
	return JIT_RESULT_OK;
}

/*
 * On demand code generator.for functions implemented in IL code.
 */
static int _ILJitCompile(jit_function_t func)
{
	ILExecThread *thread = ILExecThreadCurrent();
	ILMethod *method = (ILMethod *)jit_function_get_meta(func, IL_JIT_META_METHOD);

	if(!method)
	{
		return JIT_RESULT_COMPILE_ERROR;
	}

	if(!_ILConvertMethod(thread, method))
	{
		return JIT_RESULT_COMPILE_ERROR;
	}
	return JIT_RESULT_OK;
}

/*
 * Check if the given method is abstract (should have no implementation).
 */
static int _ILJitMethodIsAbstract(ILMethod *method)
{
	if(!method)
	{
		/* This is obviously a bug and should not happen. */
		return 0;
	}

	if(ILMethod_IsAbstract(method) ||
	   ILClass_IsInterface(ILMethod_Owner(method)))
	{
		return 1;
	}
	return 0;
}

#include "jitc_alloc.c"
#include "jitc_delegate.c"
#include "jitc_pinvoke.c"

/*
 * Create the signature type for an ILMethod.
 */
static ILJitType _ILJitCreateMethodSignature(ILJITCoder *coder,
											 ILMethod *method,
											 ILUInt32 implementationType)
{
	ILType *signature = ILMethod_Signature(method);
	ILType *type;

	/* number of args in the bytecode */
	/* Argument 0 is the type of the return value. */
	ILUInt32 num = ILTypeNumParams(signature);
	/* total number of args */
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	/* because we pass the ILExecThread as first arg we have to add one */
	ILUInt32 total = num + 1;
	/* We set jitArgc to 1 because we allways pass the current ILExecThread */
	/* in jitArgs[0]. */
	ILUInt32 jitArgc = 1;
#else
	ILUInt32 total = num;
	ILUInt32 jitArgc = 0;
#endif
	ILUInt32 current;
	jit_abi_t jitAbi = IL_JIT_CALLCONV_DEFAULT;
	/* JitType to hold the return type */
	ILJitType jitReturnType;
	/* calling convention for this function. */
	/* The type of the jit signature for this function. */
	ILJitType jitSignature;
	/* Flag if this is an array or string constructor. */
	int isArrayOrString = 0;
	/* Flag if the method is a ctor. */
	int isCtor = ILMethodIsConstructor(method);
	/* Some infos that we'll need later. */
	ILClass *info = ILMethod_Owner(method);

	if(ILType_HasThis(signature))
	{
		if(!isCtor)
		{
			/* we need an other arg for this */
			total++;
		}
		else
		{
			if(implementationType != _IL_JIT_IMPL_INTERNALALLOC)
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

	/* Array to hold the parameter types. */
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

#ifdef IL_JIT_THREAD_IN_SIGNATURE
	/* arg 0 is allways the ILExecThread */
	jitArgs[0] = _IL_JIT_TYPE_VPTR;
#endif

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
		#ifdef IL_JIT_THREAD_IN_SIGNATURE
			if(!(jitArgs[1] = _ILJitGetThisType(type, coder->process)))
		#else
			if(!(jitArgs[0] = _ILJitGetThisType(type, coder->process)))
		#endif
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
		jitAbi = IL_JIT_CALLCONV_VARARG;
	}
#endif

	if(!(jitSignature = jit_type_create_signature(jitAbi, jitReturnType,
													jitArgs, jitArgc, 1)))
	{
		return 0;
	}
	return jitSignature;
}

/*
 * Fill the MethodInfo and set the corresponding on demand compiler.
 */
static int _ILJitSetMethodInfo(ILJITCoder *jitCoder, ILMethod *method,
													 ILJitType jitSignature)
{
	ILClass *info;
	ILClassPrivate *classPrivate;
	ILUInt32 implementationType = 0;
	jit_on_demand_func onDemandCompiler = 0;
	ILInt32 setRecompilable = 0;
	ILInternalInfo fnInfo;

	if(!method)
	{
		return 0;
	}

	/* Get the method's owner class. */
	info = ILMethod_Owner(method);
	classPrivate = (ILClassPrivate *)(info->userData);
	fnInfo.func = 0;

	if(classPrivate->jitTypes.jitTypeKind != 0)
	{
		ILType *type = ILClassToType(info);

		/* Handle the special cases. */
		if(method == ILTypeGetDelegateMethod(type))
		{
			/* Flag method implemented in IL.. */
			implementationType = _IL_JIT_IMPL_DEFAULT;

			/* now set the on demand compiler function */
			onDemandCompiler = _ILJitCompileMultiCastDelegateInvoke;
		}
		else if (method == (ILMethod *)ILTypeGetDelegateBeginInvokeMethod(type))
		{
			/* Flag method implemented in IL.. */
			implementationType = _IL_JIT_IMPL_DEFAULT;

			/* now set the on demand compiler function */
			onDemandCompiler = _ILJitCompileDelegateBeginInvoke;
		}
		else if (method == (ILMethod *)ILTypeGetDelegateEndInvokeMethod(type))
		{
			/* Flag method implemented in IL.. */
			implementationType = _IL_JIT_IMPL_DEFAULT;

			/* now set the on demand compiler function */
			onDemandCompiler = _ILJitCompileDelegateEndInvoke;
		}
	}

	if(!onDemandCompiler)
	{
		ILMethodCode code;

		/* Get the method code */
		if(!ILMethodGetCode(method, &code))
		{
			code.code = 0;
		}
	
		/* Check if the method is implemented in IL. */
		if(code.code)
		{
			/* Flag method implemented in IL.. */
			implementationType = _IL_JIT_IMPL_DEFAULT;

			/* set the function recompilable. */
			setRecompilable = 1;

			/* now set the on demand compiler function */
			onDemandCompiler = _ILJitCompile;
		}
		else
		{
			/* This is a "PInvoke", "internalcall", or "runtime" method */
			ILExecThread *thread = ILExecThreadCurrent();
			ILPInvoke *pinv = ILPInvokeFind(method);
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
						return 0;
					}

				#ifdef IL_CONFIG_PINVOKE
					/* Find the module for the PInvoke record */
					module = ILPInvoke_Module(pinv);
					if(!module)
					{
						return 0;
					}
					name = ILModule_Name(module);
					if(!name || *name == '\0')
					{
						return 0;
					}
					moduleHandle = LocateExternalModule
									(ILExecThreadGetProcess(thread), name, pinv);
					if(!moduleHandle)
					{
						return 0;
					}

					/* Get the name of the function within the module */
					name = ILPInvoke_Alias(pinv);
					if(!name || *name == '\0')
					{
						name = ILMethod_Name(method);
					}

					/* Look up the method within the module */
					fnInfo.func = ILDynLibraryGetSymbol(moduleHandle, name);

					/* Bail out if we did not find the underlying native method */
					if(!(fnInfo.func))
					{
						return 0;
					}

					/* Flag the method pinvoke. */
					implementationType = _IL_JIT_IMPL_PINVOKE;

					/* now set the on demand compiler function */
					onDemandCompiler = _ILJitCompilePinvoke;

				#else /* !IL_CONFIG_PINVOKE */
					return 0;
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
						return 0;
					}

					/* Look up the internalcall function details */
					if(isConstructor)
					{
						if(ILClassIsValueType(info))
						{
							_ILFindInternalCall(_ILExecThreadProcess(thread),
												method, 0, &fnInfo);
							if(fnInfo.func)
							{
								/* Flag the method internal. */
								implementationType = _IL_JIT_IMPL_INTERNAL;
							}
						}
						else
						{
							if(!_ILFindInternalCall(_ILExecThreadProcess(thread),
													method, 0, &fnInfo))
							{
								if(!_ILFindInternalCall(_ILExecThreadProcess(thread),
														method, 1, &fnInfo))
								{
									implementationType = _IL_JIT_IMPL_DEFAULT;
								}
								if(fnInfo.func)
								{
									/* Flag the method an allocating constructor. */
									implementationType = _IL_JIT_IMPL_INTERNALALLOC;
								}
							}
							else
							{
								if(fnInfo.func)
								{
									/* Flag the method internal. */
									implementationType = _IL_JIT_IMPL_INTERNAL;
								}
								else
								{
									if(!_ILFindInternalCall(_ILExecThreadProcess(thread),
															method, 1, &fnInfo))
									{
										implementationType = _IL_JIT_IMPL_DEFAULT;
									}
									if(fnInfo.func)
									{
										/* Flag the method an allocating constructor. */
										implementationType = _IL_JIT_IMPL_INTERNALALLOC;
									}
								}
							}
						}
					}
					else
					{
						if(!_ILFindInternalCall(_ILExecThreadProcess(thread),
											method, 0, &fnInfo))
						{
							implementationType = _IL_JIT_IMPL_DEFAULT;
						}
						else
						{
							if(fnInfo.func)
							{
								/* Flag the method internal. */
								implementationType = _IL_JIT_IMPL_INTERNAL;
							}
						}
					}

					/* Bail out if the native method could not be found. */
					if(!(fnInfo.func))
					{
						return 0;
					}

					/* now set the on demand compiler. */
					onDemandCompiler = _ILJitCompileInternal;
				}
				break;

				default:
				{
					/* No idea how to invoke this method */
					return 0;
				}
				/* Not reached */
			}
		}
	}

	if(onDemandCompiler)
	{
		ILJitMethodInfo *jitMethodInfo = 0;
		ILInt32 signatureCreated = 0;
		ILJitFunction jitFunction = 0;

		if(!jitSignature)
		{
			if(!(jitSignature = _ILJitCreateMethodSignature(jitCoder, method,
															implementationType)))
			{
				return 0;
			}
			signatureCreated = 1;
		}
	
		/* Now we can create the jit function itself. */
		/* We must be able to create jit function prototypes while an other */
		/* function is on demand compiled. */
		if(!(jitFunction = jit_function_create(jitCoder->context, jitSignature)))
		{
			if(signatureCreated)
			{
				ILJitTypeDestroy(jitSignature);
			}
			return 0;
		}

		/* Set the ILMethod in the new functions metadata. */
		/* Because there has nothing to be freed we can use 0 for the free_func. */
		if(!jit_function_set_meta(jitFunction, IL_JIT_META_METHOD, method, 0, 0))
		{
			if(signatureCreated)
			{
				ILJitTypeDestroy(jitSignature);
			}
			return 0;
		}
	
		if(!(jitMethodInfo = ILMemPoolAlloc(&(jitCoder->methodPool),
											ILJitMethodInfo)))
		{
			if(signatureCreated)
			{
				ILJitTypeDestroy(jitSignature);
			}
			return 0;
		}

		/* Copy the infos to the MethodInfo structure. */
		jitMethodInfo->jitFunction = jitFunction;
		jitMethodInfo->implementationType = implementationType;
		jitMethodInfo->fnInfo.func = fnInfo.func;

		/* and link the new jitFunction to the method. */
		method->userData = (void *)jitMethodInfo;

		if(setRecompilable)
		{
			/* set the function recompilable. */
			jit_function_set_recompilable(jitFunction);
		}
		/* now set the on demand compiler function */
		jit_function_set_on_demand_compiler(jitFunction, onDemandCompiler);
											
	#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
		_ILJitFunctionSetMethodName(jitFunction, method);
	#endif

		return 1;
	}
	return 0;
}

/*
 * Create the jit function header for an ILMethod.
 * We allways pass the ILExecThread as arg 0.
 */
int ILJitFunctionCreate(ILCoder *_coder, ILMethod *method)
{
	ILJITCoder *coder = ((ILJITCoder *)_coder);

	/* Don't create the jit function twice. */
	if(method->userData)
	{
		return 1;
	}

	if(_ILJitMethodIsAbstract(method))
	{
		/* This Method is abstract so we do nothing. */
		return 1;
	}

	_ILJitSetMethodInfo(coder, method, 0);

	/* are we ready now ? */

	return 1;
}

/*
 * Create the jit function header for an ILMethod with the information from
 * a virtual ancestor.
 * We can reuse the signature in this case.
 */
int ILJitFunctionCreateFromAncestor(ILCoder *_coder, ILMethod *method,
													 ILMethod *virtualAncestor)
{
	ILJITCoder *jitCoder = ((ILJITCoder *)_coder);
	ILJitMethodInfo *ancestorInfo = (ILJitMethodInfo *)(virtualAncestor->userData);
	ILJitType jitSignature;

	/* Don't create the jit function twice. */
	if(method->userData)
	{
		return 1;
	}

	if(_ILJitMethodIsAbstract(method))
	{
		/* This Method is abstract so we do nothing. */
		return 1;
	}

	if(!ancestorInfo)
	{
		/* Tha ancestor has no jit function (might be abstract). */
		/* So we need to do it the hard way. */
		return ILJitFunctionCreate(_coder, method);
	}

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CreateMethodFromAncestor: Ancestor: %s.%s at Slot %i New: %s.%s\n", 
			ILClass_Name(ILMethod_Owner(virtualAncestor)),
			ILMethod_Name(virtualAncestor),
			virtualAncestor->index,
			ILClass_Name(ILMethod_Owner(method)),
			ILMethod_Name(method));
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	jitSignature = jit_function_get_signature(ancestorInfo->jitFunction);

	_ILJitSetMethodInfo(jitCoder, method, jitSignature);

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
				return  0;
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
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(method);

	if(!jitFunction)
	{
		/* We have to layout the class. */
		if(!_LayoutClass(thread, ILMethod_Owner(method)))
		{
			return 0;
		}
		jitFunction = ILJitFunctionFromILMethod(method);
	}

	if(!jit_function_apply(jitFunction, jitArgs, result))
	{
		return 0;
	}
	return 1;
}

/*
 * Get the ILJitFunction for an ILMethod.
 * Returns 0 if the jit function stub isn't created yet.
 */
ILJitFunction ILJitFunctionFromILMethod(ILMethod *method)
{
	if(method)
	{
		ILJitMethodInfo *jitMethodInfo = (ILJitMethodInfo *)(method->userData);

		if(jitMethodInfo)
		{
			return jitMethodInfo->jitFunction;
		}
	}
	return 0;
}

/*
 * Handle special cases of the Type creation like handles, ...
 * Returns the jit type for the special case or 0 if is no special case.
 */
static ILJitType _ILJitTypeSpecials(ILClassName *className)
{
	if(className->namespace && !strcmp(className->namespace, "System"))
	{
		if(!strcmp(className->name, "IntPtr"))
		{
			return _IL_JIT_TYPE_NINT;
		}
		if(!strcmp(className->name, "UIntPtr"))
		{
			return _IL_JIT_TYPE_NUINT;
		}
		if(!strcmp(className->name, "RuntimeTypeHandle"))
		{
			return _IL_JIT_TYPE_NINT;
		}
		if(!strcmp(className->name, "RuntimeMethodHandle"))
		{
			return _IL_JIT_TYPE_NINT;
		}
		if(!strcmp(className->name, "RuntimeFieldHandle"))
		{
			return _IL_JIT_TYPE_NINT;
		}
		if(!strcmp(className->name, "RuntimeArgumentHandle"))
		{
			return _IL_JIT_TYPE_NINT;
		}
	}
	return 0;
}

/*
 * Create the class/struct representation of a clr type for libjit.
 * and store the type in classPrivate.
 * Returns the 1 on success else 0
 */
int ILJitTypeCreate(ILClassPrivate *classPrivate, ILExecProcess *process)
{
	ILJitType jitType = 0;

	if(classPrivate->size >= 0)
	{
		ILType *type = ILClassToType(classPrivate->classInfo);

		/* When it's a runtime object check if it has to be handled special. */
		if(process->context->systemImage == classPrivate->classInfo->programItem.image)
		{
			jitType = _ILJitTypeSpecials(classPrivate->classInfo->className);
		}
		if(!jitType)
		{
			if(!(jitType = jit_type_create_struct(0, 0, 0)))
			{
				return 0;
			}
			jit_type_set_size_and_alignment(jitType,
											classPrivate->size,
											classPrivate->alignment);
		}
		classPrivate->jitTypes.jitTypeBase = jitType;
		/* Check if it's one of the classes thet need special handling. */
		if(ILTypeIsDelegateSubClass(type))
		{
			if(ILTypeIsDelegate(type))
			{
				/* This is a subclass of System.MulticastDelegate. */
				classPrivate->jitTypes.jitTypeKind = IL_JIT_TYPEKIND_MULTICASTDELEGATE;
			}
			else
			{
				classPrivate->jitTypes.jitTypeKind = IL_JIT_TYPEKIND_DELEGATE;
			}
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
			return &_ILJitType_TYPEDREF;
		}
		case IL_META_ELEMTYPE_PTR:
		case IL_META_ELEMTYPE_STRING:
		case IL_META_ELEMTYPE_BYREF:
		case IL_META_ELEMTYPE_ARRAY:
		case IL_META_ELEMTYPE_FNPTR:
		case IL_META_ELEMTYPE_OBJECT:
		case IL_META_ELEMTYPE_SZARRAY:
		{
			return &_ILJitType_VPTR;
		}
	}
	return 0;
}

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)

/*
 * Dump method profile information.
 */
int _ILDumpMethodProfile(FILE *stream, ILExecProcess *process)
{
	/* TODO */
	return 0;
}

#endif /* !IL_CONFIG_REDUCE_CODE */

#include "jitc_diag.c"
#include "jitc_locals.c"
#include "jitc_labels.c"

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
