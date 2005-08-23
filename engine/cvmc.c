/*
 * cvmc.c - Coder implementation for CVM output.
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
#include "il_opcodes.h"
#include "il_utils.h"
#ifndef IL_WITHOUT_TOOLS
#include "il_dumpasm.h"
#endif
#include "cvm.h"
#include "lib_defs.h"
#include "method_cache.h"
#include "cvm_config.h"
#if defined(HAVE_LIBFFI)
#include "ffi.h"
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Define the structure of a CVM label.
 */
typedef struct _tagILCVMLabel ILCVMLabel;
struct _tagILCVMLabel
{
	ILUInt32	address;		/* Address in the IL code */
	ILUInt32	offset;			/* Offset in the CVM code */
	ILCVMLabel *next;			/* Next label block */
	ILCVMLabel *nextRef;		/* Next label reference block */

};
#define	ILCVM_LABEL_UNDEF		IL_MAX_UINT32

/*
 * Define the structure of a CVM coder's instance block.
 */
typedef struct _tagILCVMCoder ILCVMCoder;
struct _tagILCVMCoder
{
	ILCoder			coder;
	ILCache        *cache;
	ILCachePosn		codePosn;
	unsigned char  *start;
	unsigned char  *stackCheck;
	int				needTry;
	unsigned char  *tryHandler;
	long			height;
	long			minHeight;
	long			maxHeight;
	ILUInt32	   *argOffsets;
	ILUInt32		maxArgs;
	ILUInt32		varargIndex;
	ILUInt32	   *localOffsets;
	ILUInt32		maxLocals;
	ILMemPool		labelPool;
	ILCVMLabel     *labelList;
	int				labelOutOfMemory;
	unsigned char  *switchStart;
	ILMethod	   *currentMethod;
	int				debugEnabled;
	int				flags;
	long			nativeArgPosn;
	long			nativeArgHeight;
	ILExecProcess  *process;		/* Backpointer to the owning process */
};

/*
 * Convert a pointer to an ILCoder to a pointer to the ILCVMVoder instance
 */
#define _ILCoderToILCVMCoder(coder) ((ILCVMCoder *)coder)

/*
 * Include the CVM code generation macros.
 */
#include "cvmc_gen.h"

/*
 * Adjust the height of the CVM operand stack.
 */
#define	CVM_ADJUST(num)	\
			do { \
				((ILCVMCoder *)coder)->height += (long)(num); \
				if(((ILCVMCoder *)coder)->height > \
						((ILCVMCoder *)coder)->maxHeight) \
				{ \
					((ILCVMCoder *)coder)->maxHeight = \
						((ILCVMCoder *)coder)->height; \
				} \
			} while (0)

/*
 * Get the size of a type in stack words.
 */
static ILUInt32 GetTypeSize(ILExecProcess *process, ILType *type)
{
	ILUInt32 size = _ILSizeOfTypeLocked(process, type);
	return (size + sizeof(CVMWord) - 1) / sizeof(CVMWord);
}

/*
 * Get the size of a type in stack words, taking float expansion into account.
 */
static ILUInt32 GetStackTypeSize(ILExecProcess *process, ILType *type)
{
	ILUInt32 size;
	if(type == ILType_Float32 || type == ILType_Float64)
	{
		return CVM_WORDS_PER_NATIVE_FLOAT;
	}
	else
	{
		size = _ILSizeOfTypeLocked(process, type);
	}
	return (size + sizeof(CVMWord) - 1) / sizeof(CVMWord);
}

/*
 * Create a new CVM coder instance.
 */
static ILCoder *CVMCoder_Create(ILExecProcess *process, ILUInt32 size,
								unsigned long cachePageSize)
{
	ILCVMCoder *coder;
	if((coder = (ILCVMCoder *)ILMalloc(sizeof(ILCVMCoder))) == 0)
	{
		return 0;
	}
	coder->coder.classInfo = &_ILCVMCoderClass;
	if((coder->cache = ILCacheCreate(0, cachePageSize)) == 0)
	{
		ILFree(coder);
		return 0;
	}
	coder->start = 0;
	coder->stackCheck = 0;
	coder->needTry = 0;
	coder->tryHandler = 0;
	coder->height = 0;
	coder->minHeight = 0;
	coder->maxHeight = 0;
	coder->argOffsets = 0;
	coder->maxArgs = 0;
	coder->varargIndex = 0;
	coder->localOffsets = 0;
	coder->maxLocals = 0;
	ILMemPoolInit(&(coder->labelPool), sizeof(ILCVMLabel), 8);
	coder->labelList = 0;
	coder->labelOutOfMemory = 0;
	coder->switchStart = 0;
	coder->currentMethod = 0;
	coder->debugEnabled = 0;
	coder->flags = 0;
	coder->nativeArgPosn = 0;
	coder->nativeArgHeight = 0;
	coder->process = process;

	/* Call the interpreter to export the label tables for
	   use in code generation for direct threading */
	_ILCVMInterpreter(0);

	/* Initialize the native code unroller */
	if(!_ILCVMUnrollInit())
	{
		ILCoderDestroy(&(coder->coder));
		return 0;
	}

	/* Ready to go */
	return &(coder->coder);
}

/*
 * Enable debug mode in an CVM coder instance.
 */
static void CVMCoder_EnableDebug(ILCoder *coder)
{
	((ILCVMCoder *)coder)->debugEnabled = 1;
}

/*
 * Allocate memory within a CVM coder instance.
 */
static void *CVMCoder_Alloc(ILCoder *_coder, ILUInt32 size)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	return ILCacheAllocNoMethod(coder->cache, size);
}

/*
 * Get the size of the method cache.
 */
static unsigned long CVMCoder_GetCacheSize(ILCoder *_coder)
{
	return ILCacheGetSize(((ILCVMCoder *)_coder)->cache);
}

/*
 * Destroy a CVM coder instance.
 */
static void CVMCoder_Destroy(ILCoder *_coder)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	ILCacheDestroy(coder->cache);
	if(coder->argOffsets)
	{
		ILFree(coder->argOffsets);
	}
	if(coder->localOffsets)
	{
		ILFree(coder->localOffsets);
	}
	ILMemPoolDestroy(&(coder->labelPool));
	ILFree(coder);
}

/*
 * Get an IL offset from a native offset within a method.
 */
static ILUInt32 CVMCoder_GetILOffset(ILCoder *_coder, void *start,
									 ILUInt32 offset, int exact)
{
	return ILCacheGetBytecode(((ILCVMCoder *)_coder)->cache, start,
							  offset, exact);
}

/*
 * Get a native offset from an IL offset within a method.
 */
static ILUInt32 CVMCoder_GetNativeOffset(ILCoder *_coder, void *start,
									     ILUInt32 offset, int exact)
{
	return ILCacheGetNative(((ILCVMCoder *)_coder)->cache, start,
							offset, exact);
}

/*
 * Mark the current position with a bytecode offset.
 */
static void CVMCoder_MarkBytecode(ILCoder *coder, ILUInt32 offset)
{
	ILCacheMarkBytecode(&(((ILCVMCoder *)coder)->codePosn), offset);
	if(((ILCVMCoder *)coder)->debugEnabled)
	{
		CVM_OUT_BREAK(IL_BREAK_DEBUG_LINE);
	}
}

/*
 * Get a block of method cache memory for use in code unrolling.
 */
int _ILCVMStartUnrollBlock(ILCoder *_coder, int align, ILCachePosn *posn)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	return (ILCacheStartMethod(coder->cache, posn, align, 0) != 0);
}

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)

/*
 * Dump method profile information.
 */
int _ILDumpMethodProfile(FILE *stream, ILExecProcess *process)
{
	ILCache *cache = ((ILCVMCoder *)(process->coder))->cache;
	ILMethod **list;
	ILMethod **temp;
	ILMethod *method;
	int haveCounts;

	/* Get the list of all translated methods from the cache */
	list = (ILMethod **)ILCacheGetMethodList(cache);
	if(!list)
	{
		return 0;
	}

	/* Sort the method list into decreasing order of count */
	if(list[0] != 0 && list[1] != 0)
	{
		ILMethod **outer;
		ILMethod **inner;
		for(outer = list; outer[1] != 0; ++outer)
		{
			for(inner = outer + 1; inner[0] != 0; ++inner)
			{
				if(outer[0]->count < inner[0]->count)
				{
					method = outer[0];
					outer[0] = inner[0];
					inner[0] = method;
				}
			}
		}
	}

	/* Print the method information */
	haveCounts = 0;
	temp = list;
#ifdef ENHANCED_PROFILER
	printf ("   Count    Total  Average\n             time     time\n");
#endif
	while((method = *temp++) != 0)
	{
		if(!(method->count))
		{
			continue;
		}
#ifdef ENHANCED_PROFILER
		printf("%8lu %8lu %8lu   ", (unsigned long)(method->count),
			(unsigned long)(method->time), (unsigned long)(method->time) / (unsigned long)(method->count));
#else
 		printf("%8lu    ", (unsigned long)(method->count));
#endif
		ILDumpMethodType(stdout, ILProgramItem_Image(method),
						 ILMethod_Signature(method), 0,
						 ILMethod_Owner(method), ILMethod_Name(method), 0);
		putc('\n', stdout);
		haveCounts = 1;
	}

	/* Clean up and exit */
	ILFree(list);
	return haveCounts;
}

#endif /* !IL_CONFIG_REDUCE_CODE */

/*
 * Include the rest of the CVM conversion routines from other files.
 * We split the implementation to make it easier to maintain the code.
 */
#define	IL_CVMC_CODE
#include "cvmc_setup.c"
#include "cvmc_const.c"
#include "cvmc_arith.c"
#include "cvmc_var.c"
#include "cvmc_stack.c"
#include "cvmc_ptr.c"
#include "cvmc_branch.c"
#include "cvmc_except.c"
#include "cvmc_conv.c"
#include "cvmc_obj.c"
#include "cvmc_call.c"
#undef	IL_CVMC_CODE

/*
 * Define the CVM coder class.
 */
ILCoderClass const _ILCVMCoderClass =
{
	CVMCoder_Create,
	CVMCoder_EnableDebug,
	CVMCoder_Alloc,
	CVMCoder_GetCacheSize,
	CVMCoder_Setup,
	CVMCoder_SetupExtern,
	CVMCoder_SetupExternCtor,
	CVMCoder_CtorOffset,
	CVMCoder_Destroy,
	CVMCoder_Finish,
	CVMCoder_Label,
	CVMCoder_StackRefresh,
	CVMCoder_Constant,
	CVMCoder_StringConstant,
	CVMCoder_Binary,
	CVMCoder_BinaryPtr,
	CVMCoder_Shift,
	CVMCoder_Unary,
	CVMCoder_LoadArg,
	CVMCoder_StoreArg,
	CVMCoder_AddrOfArg,
	CVMCoder_LoadLocal,
	CVMCoder_StoreLocal,
	CVMCoder_AddrOfLocal,
	CVMCoder_Dup,
	CVMCoder_Pop,
	CVMCoder_ArrayAccess,
	CVMCoder_PtrAccess,
	CVMCoder_PtrAccessManaged,
	CVMCoder_Branch,
	CVMCoder_SwitchStart,
	CVMCoder_SwitchEntry,
	CVMCoder_Compare,
	CVMCoder_Conv,
	CVMCoder_ToPointer,
	CVMCoder_PtrPrefix,
	CVMCoder_ArrayLength,
	CVMCoder_NewArray,
	CVMCoder_LocalAlloc,
	CVMCoder_CastClass,
	CVMCoder_LoadField,
	CVMCoder_LoadStaticField,
	CVMCoder_LoadThisField,
	CVMCoder_LoadFieldAddr,
	CVMCoder_LoadStaticFieldAddr,
	CVMCoder_StoreField,
	CVMCoder_StoreStaticField,
	CVMCoder_CopyObject,
	CVMCoder_CopyBlock,
	CVMCoder_InitObject,
	CVMCoder_InitBlock,
	CVMCoder_Box,
	CVMCoder_BoxSmaller,
	CVMCoder_Unbox,
	CVMCoder_MakeTypedRef,
	CVMCoder_RefAnyVal,
	CVMCoder_RefAnyType,
	CVMCoder_PushToken,
	CVMCoder_SizeOf,
	CVMCoder_ArgList,
	CVMCoder_UpConvertArg,
	CVMCoder_DownConvertArg,
	CVMCoder_PackVarArgs,
	CVMCoder_ValueCtorArgs,
	CVMCoder_CheckCallNull,
	CVMCoder_CallMethod,
	CVMCoder_CallIndirect,
	CVMCoder_CallCtor,
	CVMCoder_CallVirtual,
	CVMCoder_CallInterface,
	CVMCoder_CallInlineable,
	CVMCoder_JumpMethod,
	CVMCoder_ReturnInsn,
	CVMCoder_LoadFuncAddr,
	CVMCoder_LoadVirtualAddr,
	CVMCoder_LoadInterfaceAddr,
	CVMCoder_SetupExceptions,
	CVMCoder_Throw,
	CVMCoder_SetStackTrace,
	CVMCoder_Rethrow,
	CVMCoder_Jsr,
	CVMCoder_RetFromJsr,
	CVMCoder_TryHandlerStart,
	CVMCoder_TryHandlerEnd,
	CVMCoder_Catch,
	CVMCoder_EndCatchFinally,
	CVMCoder_Finally,
	CVMCoder_PCToHandler,
	CVMCoder_PCToMethod,
	CVMCoder_GetILOffset,
	CVMCoder_GetNativeOffset,
	CVMCoder_MarkBytecode,
	CVMCoder_MarkEnd,
	CVMCoder_SetFlags,
	CVMCoder_GetFlags,
	CVMCoder_AllocExtraLocal,
	CVMCoder_PushThread,
	CVMCoder_LoadNativeArgAddr,
	CVMCoder_LoadNativeLocalAddr,
	CVMCoder_StartFfiArgs,
	CVMCoder_PushRawArgPointer,
	CVMCoder_CallFfi,
	CVMCoder_CheckNull,
	CVMCoder_Convert,
	CVMCoder_ConvertCustom,
	"sentinel"
};

#ifdef	__cplusplus
};
#endif
