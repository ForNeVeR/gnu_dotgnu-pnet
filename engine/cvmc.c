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
	unsigned char  *buffer;
	unsigned long	posn;
	unsigned long	start;
	unsigned long	len;
	unsigned long	stackCheck;
	ILUInt32		generation;
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
 * Output a single byte to the CVM coder buffer.
 */
#define	CVM_BYTE(byte)	\
			do { \
				if(((ILCVMCoder *)coder)->posn < ((ILCVMCoder *)coder)->len) \
				{ \
					((ILCVMCoder *)coder)->buffer \
						[(((ILCVMCoder *)coder)->posn)++] = \
							(unsigned char)(byte); \
				} \
			} while (0)

/*
 * Output a 32-bit value to the CVM coder buffer.
 */
#define	CVM_WORD(value)	\
			do { \
				CVM_BYTE((value)); \
				CVM_BYTE((value) >> 8); \
				CVM_BYTE((value) >> 16); \
				CVM_BYTE((value) >> 24); \
			} while (0)

/*
 * Output a pointer value to the CVM coder buffer.
 */
#ifdef IL_NATIVE_INT32
#define	CVM_PTR(value)	\
			do { \
				CVM_BYTE(((ILUInt32)(value))); \
				CVM_BYTE(((ILUInt32)(value)) >> 8); \
				CVM_BYTE(((ILUInt32)(value)) >> 16); \
				CVM_BYTE(((ILUInt32)(value)) >> 24); \
			} while (0)
#else
#define	CVM_PTR(value)	\
			do { \
				CVM_BYTE(((ILUInt64)(value))); \
				CVM_BYTE(((ILUInt64)(value)) >> 8); \
				CVM_BYTE(((ILUInt64)(value)) >> 16); \
				CVM_BYTE(((ILUInt64)(value)) >> 24); \
				CVM_BYTE(((ILUInt64)(value)) >> 32); \
				CVM_BYTE(((ILUInt64)(value)) >> 40); \
				CVM_BYTE(((ILUInt64)(value)) >> 48); \
				CVM_BYTE(((ILUInt64)(value)) >> 56); \
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
	coder->buffer = (unsigned char *)ILMalloc(size);
	if(!(coder->buffer))
	{
		ILFree(coder);
		return 0;
	}
	coder->posn = 0;
	coder->start = 0;
	coder->len = size;
	coder->stackCheck = 0;
	coder->generation = 1;
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
 * Get the generation count for a CVM coder instance.
 */
static ILUInt32 CVMCoder_Generation(ILCoder *coder)
{
	return ((ILCVMCoder *)coder)->generation;
}

/*
 * Allocate memory within a CVM coder instance.
 */
static void *CVMCoder_Alloc(ILCoder *_coder, ILUInt32 size)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	void *ptr;
	if((coder->posn % sizeof(unsigned long)) != 0)
	{
		/* Align the buffer before we allocate from it */
		coder->posn += sizeof(unsigned long) -
					   (coder->posn % sizeof(unsigned long));
		if(coder->posn > coder->len)
		{
			coder->posn = coder->len;
			return 0;
		}
	}
	if((coder->posn + size) <= coder->len)
	{
		ptr = coder->buffer + coder->posn;
		coder->posn += size;
		return ptr;
	}
	else
	{
		return 0;
	}
}

/*
 * Destroy a CVM coder instance.
 */
static void CVMCoder_Destroy(ILCoder *_coder)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	ILFree(coder->buffer);
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
 * Flush a CVM coder instance.
 */
static void CVMCoder_Flush(ILCoder *_coder)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	coder->posn = 0;
	coder->start = 0;
	ILMemPoolClear(&(coder->labelPool));
	coder->labelList = 0;
	coder->labelOutOfMemory = 0;
	++(coder->generation);
}

/*
 * Determine if a CVM coder restart is required.
 */
static int CVMCoder_Restart(ILCoder *_coder)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	if(coder->posn >= coder->len && coder->start != 0 &&
	   !(coder->labelOutOfMemory))
	{
		/* The buffer overflowed and there is more than one
		   method in the buffer, so restart this method */
		return 1;
	}
	return 0;
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
	CVMCoder_Generation,
	CVMCoder_Alloc,
	CVMCoder_Setup,
	CVMCoder_SetupExtern,
	CVMCoder_SetupExternCtor,
	CVMCoder_CtorOffset,
	CVMCoder_Destroy,
	CVMCoder_Flush,
	CVMCoder_Finish,
	CVMCoder_Restart,
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
	CVMCoder_JumpMethod,
	CVMCoder_ReturnInsn,
};

#ifdef	__cplusplus
};
#endif
