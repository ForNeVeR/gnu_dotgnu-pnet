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
#include "cvm.h"
#include "lib_defs.h"
#include "method_cache.h"

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
	ILUInt32	   *localOffsets;
	ILUInt32		maxLocals;
	ILMemPool		labelPool;
	ILCVMLabel     *labelList;
	int				labelOutOfMemory;
	unsigned long	switchStart;
	ILMethod	   *currentMethod;

};

/*
 * Get the current method position.
 */
#define	CVM_POSN()		(ILCacheGetPosn(&(((ILCVMCoder *)coder)->codePosn)))

/*
 * Determine if "n" bytes at "addr" are valid.
 */
#define	CVM_VALID(addr,n)	\
			((((unsigned char *)(addr)) + (n)) <= \
				((ILCVMCoder *)coder)->codePosn.limit)

/*
 * Output a single byte to the CVM coder buffer.
 */
#define	CVM_BYTE(byte)		\
			do { \
				ILCacheByte(&(((ILCVMCoder *)coder)->codePosn), (byte)); \
			} while (0)

/*
 * Output a 32-bit value to the CVM coder buffer.
 */
#define	CVM_WORD(value)	\
			do { \
				ILCacheWord32(&(((ILCVMCoder *)coder)->codePosn), (value)); \
			} while (0)

/*
 * Output a pointer value to the CVM coder buffer.
 */
#ifdef IL_NATIVE_INT32
#define	CVM_PTR(value)	\
			do { \
				ILCacheWord32(&(((ILCVMCoder *)coder)->codePosn), (value)); \
			} while (0)
#else
#define	CVM_PTR(value)	\
			do { \
				ILCacheWord64(&(((ILCVMCoder *)coder)->codePosn), (value)); \
			} while (0)
#endif

/*
 * Output a wide instruction to the CVM coder buffer.
 */
#define	CVM_WIDE(opcode,value)	\
			do { \
				if((value) < 256) \
				{ \
					CVM_BYTE((opcode)); \
					CVM_BYTE((value)); \
				} \
				else \
				{ \
					CVM_BYTE(COP_WIDE); \
					CVM_BYTE((opcode)); \
					CVM_WORD((value)); \
				} \
			} while (0)

/*
 * Output a double wide instruction to the CVM coder buffer.
 */
#define	CVM_DWIDE(opcode,value1,value2)	\
			do { \
				if((value1) < 256 && (value2) < 256) \
				{ \
					CVM_BYTE((opcode)); \
					CVM_BYTE((value1)); \
					CVM_BYTE((value2)); \
				} \
				else \
				{ \
					CVM_BYTE(COP_WIDE); \
					CVM_BYTE((opcode)); \
					CVM_WORD((value1)); \
					CVM_WORD((value2)); \
				} \
			} while (0)

/*
 * Output a return instruction.
 */
#define	CVM_RETURN(size)	\
			do { \
				if((size) == 1) \
				{ \
					CVM_BYTE(COP_RETURN_1); \
				} \
				else if((size) == 2) \
				{ \
					CVM_BYTE(COP_RETURN_2); \
				} \
				else \
				{ \
					CVM_BYTE(COP_RETURN_N); \
					CVM_WORD((size)); \
				} \
			} while (0)

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
static ILUInt32 GetTypeSize(ILType *type)
{
	ILUInt32 size = ILSizeOfType(type);
	return (size + sizeof(CVMWord) - 1) / sizeof(CVMWord);
}

/*
 * Get the size of a type in stack words, taking float expansion into account.
 */
static ILUInt32 GetStackTypeSize(ILType *type)
{
	ILUInt32 size;
	if(type == ILType_Float32 || type == ILType_Float64)
	{
		return CVM_WORDS_PER_NATIVE_FLOAT;
	}
	else
	{
		size = ILSizeOfType(type);
	}
	return (size + sizeof(CVMWord) - 1) / sizeof(CVMWord);
}

/*
 * Create a new CVM coder instance.
 */
static ILCoder *CVMCoder_Create(ILUInt32 size)
{
	ILCVMCoder *coder;
	if((coder = (ILCVMCoder *)ILMalloc(sizeof(ILCVMCoder))) == 0)
	{
		return 0;
	}
	coder->coder.classInfo = &_ILCVMCoderClass;
	if((coder->cache = ILCacheCreate(0)) == 0)
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
	coder->localOffsets = 0;
	coder->maxLocals = 0;
	ILMemPoolInit(&(coder->labelPool), sizeof(ILCVMLabel), 8);
	coder->labelList = 0;
	coder->labelOutOfMemory = 0;
	coder->currentMethod = 0;
	return &(coder->coder);
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
static void CVMCoder_MarkBytecode(ILCoder *_coder, ILUInt32 offset)
{
	ILCacheMarkBytecode(&(((ILCVMCoder *)_coder)->codePosn), offset);
}

/*
 * Dump method profile information.
 */
int _ILDumpMethodProfile(FILE *stream, ILExecProcess *process)
{
	ILCache *cache = ((ILCVMCoder *)(process->coder))->cache;
	ILMethod **list;
	ILMethod **temp;
	ILMethod *method;
	ILClass *owner;
	const char *name;
	int haveCounts;
	int len;

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
	while((method = *temp++) != 0)
	{
		if(!(method->count))
		{
			continue;
		}
		owner = ILMethod_Owner(method);
		name = ILClass_Namespace(owner);
		len = 0;
		if(name)
		{
			fputs(name, stream);
			putc('.', stream);
			len += strlen(name) + 1;
		}
		name = ILClass_Name(owner);
		fputs(name, stream);
		fputs("::", stream);
		len += strlen(name) + 2;
		name = ILMethod_Name(method);
		fputs(name, stream);
		putc(' ', stream);
		len += strlen(name) + 1;
		while(len < 40)
		{
			putc(' ', stream);
			++len;
		}
		printf("%lu\n", (unsigned long)(method->count));
		haveCounts = 1;
	}

	/* Clean up and exit */
	ILFree(list);
	return haveCounts;
}

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
	CVMCoder_Alloc,
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
	CVMCoder_ValueCtorArgs,
	CVMCoder_CallMethod,
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
	CVMCoder_Rethrow,
	CVMCoder_Jsr,
	CVMCoder_RetFromJsr,
	CVMCoder_TryHandlerStart,
	CVMCoder_TryHandlerEnd,
	CVMCoder_Catch,
	CVMCoder_PCToHandler,
	CVMCoder_PCToMethod,
	CVMCoder_GetILOffset,
	CVMCoder_GetNativeOffset,
	CVMCoder_MarkBytecode,
	"sentinel"
};

#ifdef	__cplusplus
};
#endif
