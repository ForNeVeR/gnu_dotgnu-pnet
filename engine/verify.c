/*
 * verify.c - Bytecode verifier for the engine.
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
#include "il_coder.h"
#include "il_opcodes.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Jump target information.
 */
typedef struct _tagJumpTarget JumpTarget;
struct _tagJumpTarget
{
	JumpTarget *next;
	ILUInt32	offset;
	ILUInt32	stackSize;

};

/*
 * Stack item information.
 */
typedef struct
{
	ILEngineType	engineType;
	ILType	   	   *typeInfo;

} StackItem;

/*
 * Temporary memory allocator.
 */
typedef struct
{
	char	buffer[256];
	int		posn;
	void   *overflow;

} TempAllocator;

/*
 * Allocate a block of zero'ed memory from a temporary memory allocator.
 */
static void *TempAllocate(TempAllocator *allocator, unsigned long size)
{
	void *ptr;

	/* Round the size to a multiple of the machine word size */
	if(sizeof(unsigned long) > sizeof(void *))
	{
		size = (size + sizeof(unsigned long) - 1) &
				~(sizeof(unsigned long) - 1);
	}
	else
	{
		size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
	}

	/* Can we allocate from the primary buffer? */
	if(size < 256 && (allocator->posn + (int)size) < 256)
	{
		ptr = (void *)(allocator->buffer + allocator->posn);
		allocator->posn += (int)size;
		return ptr;
	}

	/* Allocate from the system heap */
	ptr = ILMalloc(sizeof(void *) + size);
	if(!ptr)
	{
		return 0;
	}
	*((void **)ptr) = allocator->overflow;
	allocator->overflow = ptr;
	return (void *)(((void **)ptr) + 1);
}

/*
 * Destroy a temporary memory allocator.
 */
static void TempAllocatorDestroy(TempAllocator *allocator)
{
	void *ptr, *nextPtr;
	ptr = allocator->overflow;
	while(ptr != 0)
	{
		nextPtr = *((void **)ptr);
		ILFree(ptr);
		ptr = nextPtr;
	}
}

/*
 * Number of bytes needed for a bit mask of a specific size.
 */
#define	BYTES_FOR_MASK(size)	\
			(((size) + (sizeof(unsigned long) * 8) - 1) / 8)

/*
 * Mark an instruction starting point in a jump mask.
 */
static IL_INLINE void MarkInsnStart(unsigned long *jumpMask, ILUInt32 offset)
{
	jumpMask[offset / (sizeof(unsigned long) * 4)] |=
		(((unsigned long)1) << ((offset % (sizeof(unsigned long) * 4)) * 2));
}

/*
 * Mark a jump target point in a jump mask.
 */
static IL_INLINE void MarkJumpTarget(unsigned long *jumpMask, ILUInt32 offset)
{
	jumpMask[offset / (sizeof(unsigned long) * 4)] |=
		(((unsigned long)2) << ((offset % (sizeof(unsigned long) * 4)) * 2));
}

/*
 * Determine if a code offset is an instruction start.
 */
static IL_INLINE int IsInsnStart(unsigned long *jumpMask, ILUInt32 offset)
{
	return ((jumpMask[offset / (sizeof(unsigned long) * 4)] &
		(((unsigned long)1) << ((offset % (sizeof(unsigned long) * 4)) * 2)))
				!= 0);
}

/*
 * Determine if a code offset is a jump target.
 */
static IL_INLINE int IsJumpTarget(unsigned long *jumpMask, ILUInt32 offset)
{
	return ((jumpMask[offset / (sizeof(unsigned long) * 4)] &
		(((unsigned long)2) << ((offset % (sizeof(unsigned long) * 4)) * 2)))
				!= 0);
}

/*
 * Process a jump target.
 */
static int ProcessJumpTarget(JumpTarget **targetList, ILUInt32 offset,
							 StackItem *stack, ILUInt32 stackSize,
							 TempAllocator *allocator)
{
	JumpTarget *target;

	/* Search for an existing target block for this offset */
	target = *targetList;
	while(target != 0)
	{
		if(target->offset == offset)
		{
			/* Verify the current stack information against
			   the information recorded for the target */
			if(stackSize != target->stackSize ||
			   (stackSize != 0 &&
			    ILMemCmp(target + 1, stack,
						 sizeof(StackItem) * stackSize) != 0))
			{
				return 0;
			}
			return 1;
		}
		target = target->next;
	}

	/* Allocate a new jump target block */
	target = (JumpTarget *)TempAllocate
			(allocator, sizeof(JumpTarget) + (sizeof(StackItem) * stackSize));
	if(!target)
	{
		return 0;
	}
	target->next = *targetList;
	*targetList = target;
	target->offset = offset;
	target->stackSize = stackSize;
	if(stackSize > 0)
	{
		ILMemCpy(target + 1, stack, sizeof(StackItem) * stackSize);
	}
	return 1;
}

/*
 * Helper macros for defining type inference matrices.
 */
#define	T_I4		(char)ILEngineType_I4
#define	T_I8		(char)ILEngineType_I8
#define	T_I			(char)ILEngineType_I
#define	T_F			(char)ILEngineType_F
#define	T_U			(char)ILEngineType_U
#define	T_M			(char)ILEngineType_M
#define	T_O			(char)ILEngineType_O
#define	T_T			(char)ILEngineType_T
#define	T_MV		(char)ILEngineType_MV
#define	T_NO		(char)ILEngineType_Invalid

/*
 * Type inference matrix for binary numeric operations.
 */
static char const binaryNumericMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary integer operations.
 */
static char const binaryIntegerMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary shift operations.
 */
static char const binaryShiftMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_I8, T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for addition when unsafe types are permitted.
 */
static char const addUnsafeMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_M,  T_NO, T_T,  T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_M,  T_NO, T_T,  T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_M,  T_NO, T_M,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* *:  */ {T_T,  T_NO, T_T,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for subtraction when unsafe types are permitted.
 */
static char const subUnsafeMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_M,  T_NO, T_M,  T_NO, T_I,  T_NO, T_I,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* *:  */ {T_T,  T_NO, T_T,  T_NO, T_I,  T_NO, T_I,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary comparison operations.
 */
static char const binaryCompareMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary equality operations.
 */
static char const binaryEqualityMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_O,  T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary comparison operations,
 * when unsafe pointer comparisons are allowed.
 */
static char const unsafeCompareMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_M,  T_NO, T_T,  T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_M,  T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_O,  T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_T,  T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Convert a type into an engine type.
 */
static ILEngineType TypeToEngineType(ILType *type)
{
	type = ILTypeGetEnumType(type);
	if(ILType_IsPrimitive(type))
	{
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:		return ILEngineType_I4;

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:		return ILEngineType_I8;

			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:		return ILEngineType_I;

			case IL_META_ELEMTYPE_R4:
			case IL_META_ELEMTYPE_R8:
			case IL_META_ELEMTYPE_R:		return ILEngineType_F;
		}
		return ILEngineType_I4;
	}
	else if(ILType_IsValueType(type))
	{
		return ILEngineType_MV;
	}
	else if(ILType_IsComplex(type) && type != 0)
	{
		switch(type->kind)
		{
			case IL_TYPE_COMPLEX_PTR:
			{
				/* Unsafe pointers are represented as native integers */
				return ILEngineType_I;
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_BYREF:
			{
				/* Reference values are managed pointers */
				return ILEngineType_M;
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_PINNED:
			{
				/* Pinned types are the same as their underlying type */
				return TypeToEngineType(type->un.refType);
			}
			/* Not reached */

			case IL_TYPE_COMPLEX_CMOD_REQD:
			case IL_TYPE_COMPLEX_CMOD_OPT:
			{
				/* Strip the modifier and inspect the underlying type */
				return TypeToEngineType(type->un.modifier.type);
			}
			/* Not reached */
		}
	}
	return ILEngineType_O;
}

/*
 * Determine if a stack item is assignment-compatible with
 * a particular memory slot (argument, local, field, etc).
 */
static int AssignCompatible(StackItem *item, ILType *type)
{
	ILClass *classInfo;

	if(item->engineType == ILEngineType_I4 ||
	   item->engineType == ILEngineType_I)
	{
		type = ILTypeGetEnumType(type);
		switch((unsigned long)type)
		{
			case (unsigned long)ILType_Boolean:
			case (unsigned long)ILType_Int8:
			case (unsigned long)ILType_UInt8:
			case (unsigned long)ILType_Int16:
			case (unsigned long)ILType_UInt16:
			case (unsigned long)ILType_Char:
			case (unsigned long)ILType_Int32:
			case (unsigned long)ILType_UInt32:
			case (unsigned long)ILType_Int:
			case (unsigned long)ILType_UInt:	return 1;
			default: break;
		}
		return 0;
	}
	else if(item->engineType == ILEngineType_I8)
	{
		type = ILTypeGetEnumType(type);
		return (type == ILType_Int64 || type == ILType_UInt64);
	}
	else if(item->engineType == ILEngineType_F)
	{
		return (type == ILType_Float32 ||
		        type == ILType_Float64 ||
		        type == ILType_Float);
	}
	else if(item->engineType == ILEngineType_O)
	{
		if(!(item->typeInfo))
		{
			/* A "null" constant was pushed, which is
			   compatible with any class type */
			return ILType_IsClass(type);
		}
		else if(ILType_IsClass(item->typeInfo) && ILType_IsClass(type))
		{
			/* Is the type a regular class or an interface? */
			classInfo = ILType_ToClass(type);
			if(!ILClass_IsInterface(classInfo))
			{
				/* Regular class: the value must inherit from the type */
				return ILClassInheritsFrom(ILType_ToClass(item->typeInfo),
										   classInfo);
			}
			else
			{
				/* Interface which the value must implement or inherit from */
				return ILClassImplements(ILType_ToClass(item->typeInfo),
										 classInfo) ||
				       ILClassInheritsFrom(ILType_ToClass(item->typeInfo),
										   classInfo);
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		/* TODO */
		return 0;
	}
}

/*
 * Helper macros for getting the types of the top two
 * stack elements for binary and unary operators.
 */
#define	STK_BINARY_1	(stack[stackSize - 2].engineType)
#define	STK_BINARY_2	(stack[stackSize - 1].engineType)
#define	STK_UNARY		(stack[stackSize - 1].engineType)

/*
 * Bailout routines for various kinds of verification failure.
 */
#define	VERIFY_TRUNCATED()		goto cleanup
#define	VERIFY_BRANCH_ERROR()	goto cleanup
#define	VERIFY_INSN_ERROR()		goto cleanup
#define	VERIFY_STACK_ERROR()	goto cleanup
#define	VERIFY_TYPE_ERROR()		goto cleanup

/*
 * Check for certain opcode types.
 */
#define	IsEqualityBranch(opcode)	\
			((opcode) == IL_OP_BEQ_S || (opcode) == IL_OP_BNE_UN_S || \
			 (opcode) == IL_OP_BEQ || (opcode) == IL_OP_BNE_UN)

int _ILVerify(ILCoder *coder, ILMethod *method,
			  ILMethodCode *code, int unsafeAllowed)
{
	TempAllocator allocator;
	unsigned long *jumpMask;
	unsigned char *pc;
	ILUInt32 len;
	int result;
	unsigned opcode;
	ILUInt32 insnSize;
	int insnType;
	ILUInt32 offset;
	JumpTarget *targetList;
	StackItem *stack;
	ILUInt32 stackSize;
	const ILOpcodeInfo *insn;
	ILType *signature;
	ILType *type;
	ILUInt32 numArgs;
	ILUInt32 argNum;
	ILUInt32 numLocals;
	ILType *localVars;
	ILValueType commonType;
	ILUInt32 dest;
	int lastWasJump;

restart:
	result = 0;
	targetList = 0;

	/* Set up the coder to process the method */
	if(!ILCoderSetup(coder, method, code))
	{
		goto cleanup;
	}

	/* Initialize the memory allocator that is used for temporary
	   allocation during bytecode verification */
	ILMemZero(allocator.buffer, sizeof(allocator.buffer));
	allocator.posn = 0;
	allocator.overflow = 0;

	/* Allocate the jump target mask */
	jumpMask = (unsigned long *)TempAllocate
					(&allocator, BYTES_FOR_MASK(code->codeLen * 2));
	if(!jumpMask)
	{
		goto cleanup;
	}

	/* Scan the code looking for all jump targets, and validating
	   that all instructions are more or less valid */
	pc = code->code;
	len = code->codeLen;
	while(len > 0)
	{
		/* Mark this position in the jump mask as an instruction start */
		MarkInsnStart(jumpMask, (ILUInt32)(pc - (unsigned char *)(code->code)));

		/* Fetch the instruction size and type */
		opcode = (unsigned)(pc[0]);
		if(opcode != IL_OP_PREFIX)
		{
			/* Regular opcode */
			insnSize = (ILUInt32)(ILMainOpcodeTable[opcode].size);
			if(len < insnSize)
			{
				VERIFY_TRUNCATED();
			}
			insnType = ILMainOpcodeTable[opcode].args;
		}
		else
		{
			/* Prefixed opcode */
			if(len < 2)
			{
				VERIFY_TRUNCATED();
			}
			opcode = (unsigned)(pc[1]);
			insnSize = (ILUInt32)(ILPrefixOpcodeTable[opcode].size);
			if(len < insnSize)
			{
				VERIFY_TRUNCATED();
			}
			insnType = ILPrefixOpcodeTable[opcode].args;
			opcode += IL_OP_PREFIX;
		}

		/* Determine how to handle this type of instruction */
		switch(insnType)
		{
			case IL_OPCODE_ARGS_SHORT_JUMP:
			{
				/* 8-bit jump offset */
				offset = (ILUInt32)((pc + insnSize) -
										(unsigned char *)(code->code)) +
						 (ILUInt32)(ILInt32)(ILInt8)(pc[1]);
				if(offset >= code->codeLen)
				{
					VERIFY_BRANCH_ERROR();
				}
				MarkJumpTarget(jumpMask, offset);
			}
			break;

			case IL_OPCODE_ARGS_LONG_JUMP:
			{
				/* 32-bit jump offset */
				offset = (ILUInt32)((pc + insnSize) -
										(unsigned char *)(code->code)) +
						 (ILUInt32)(IL_READ_INT32(pc + 1));
				if(offset >= code->codeLen)
				{
					VERIFY_BRANCH_ERROR();
				}
				MarkJumpTarget(jumpMask, offset);
			}
			break;

			case IL_OPCODE_ARGS_SWITCH:
			{
				/* Switch statement */
				if(len < 5)
				{
					VERIFY_TRUNCATED();
				}
				numArgs = IL_READ_UINT32(pc + 1);
				insnSize = 5 + numArgs * 4;
				if(numArgs >= 0x20000000 || len < insnSize)
				{
					VERIFY_TRUNCATED();
				}
				while(numArgs > 0)
				{
					--numArgs;
					offset = (ILUInt32)((pc + insnSize) -
											(unsigned char *)(code->code)) +
							 (ILUInt32)(IL_READ_INT32(pc + 5 + numArgs * 4));
					if(offset >= code->codeLen)
					{
						VERIFY_BRANCH_ERROR();
					}
					MarkJumpTarget(jumpMask, offset);
				}
			}
			break;

			case IL_OPCODE_ARGS_ANN_DATA:
			{
				/* Variable-length annotation data */
				if(opcode == IL_OP_ANN_DATA_S)
				{
					if(len < 2)
					{
						VERIFY_TRUNCATED();
					}
					insnSize = (((ILUInt32)(pc[1])) & 0xFF) + 2;
					if(len < insnSize)
					{
						VERIFY_TRUNCATED();
					}
				}
				else
				{
					if(len < 6)
					{
						VERIFY_TRUNCATED();
					}
					insnSize = (IL_READ_UINT32(pc + 2) + 6);
					if(len < insnSize)
					{
						VERIFY_TRUNCATED();
					}
				}
			}
			break;

			case IL_OPCODE_ARGS_ANN_PHI:
			{
				/* Variable-length annotation data */
				if(len < 3)
				{
					VERIFY_TRUNCATED();
				}
				insnSize = ((ILUInt32)IL_READ_UINT16(pc + 1)) * 2 + 3;
				if(len < insnSize)
				{
					VERIFY_TRUNCATED();
				}
			}
			break;

			case IL_OPCODE_ARGS_INVALID:
			{
				VERIFY_INSN_ERROR();
			}
			break;

			default: break;
		}

		/* Advance to the next instruction */
		pc += insnSize;
		len -= insnSize;
	}

	/* Make sure that all jump targets are instruction starts */
	len = code->codeLen;
	while(len > 0)
	{
		--len;
		if(IsJumpTarget(jumpMask, len) && !IsInsnStart(jumpMask, len))
		{
			VERIFY_BRANCH_ERROR();
		}
	}

	/* Create the stack */
	stack = (StackItem *)TempAllocate
				(&allocator, sizeof(StackItem) * code->maxStack);
	if(!stack)
	{
		goto cleanup;
	}
	stackSize = 0;

	/* Get the method signature, plus the number of arguments and locals */
	signature = ILMethod_Signature(method);
	numArgs = signature->num;
	if(code->localVarSig)
	{
		localVars = ILStandAloneSigGetType(code->localVarSig);
		numLocals = ILTypeNumLocals(localVars);
	}
	else
	{
		localVars = 0;
		numLocals = 0;
	}

	/* Verify the code */
	pc = code->code;
	len = code->codeLen;
	lastWasJump = 0;
	while(len > 0)
	{
		/* Is this a jump target? */
		offset = (ILUInt32)(pc - (unsigned char *)(code->code));
		if(IsJumpTarget(jumpMask, offset))
		{
			/* Verify the stack information */
			if(!ProcessJumpTarget(&targetList, offset, stack,
								  stackSize, &allocator))
			{
				goto cleanup;
			}

			/* Notify the coder of a label at this position */
			ILCoderLabel(coder, offset);
			for(dest = 0; dest < stackSize; ++dest)
			{
				ILCoderStackItem(coder, stack[dest].engineType,
								 stack[dest].typeInfo);
			}
		}

		/* Fetch the instruction information block */
		opcode = pc[0];
		if(opcode != IL_OP_PREFIX)
		{
			insn = &(ILMainOpcodeTable[opcode]);
		}
		else
		{
			opcode = pc[1];
			insn = &(ILPrefixOpcodeTable[opcode]);
			opcode += IL_OP_PREFIX;
		}
		insnSize = (ILUInt32)(insn->size);

		/* Validate the stack height changes */
		if(stackSize < ((ILUInt32)(insn->popped)) ||
		   (stackSize - ((ILUInt32)(insn->popped)) + ((ILUInt32)(insn->pushed)))
		   		> code->maxStack)
		{
			VERIFY_STACK_ERROR();
		}

		/* Verify the instruction */
		lastWasJump = 0;
		switch(opcode)
		{
			case IL_OP_NOP: break;

			case IL_OP_LDARG_0:
			case IL_OP_LDARG_1:
			case IL_OP_LDARG_2:
			case IL_OP_LDARG_3:
			{
				/* Load one of the first four arguments onto the stack */
				if((opcode - IL_OP_LDARG_0) >= numArgs)
				{
					VERIFY_INSN_ERROR();
				}
				stack[stackSize].typeInfo =
						ILTypeGetParam(signature, opcode - IL_OP_LDARG_0 + 1);
				stack[stackSize].engineType =
						TypeToEngineType(stack[stackSize].typeInfo);
				ILCoderLoadArg(coder, opcode - IL_OP_LDARG_0,
							   stack[stackSize].typeInfo);
				++stackSize;
			}
			break;

			case IL_OP_LDLOC_0:
			case IL_OP_LDLOC_1:
			case IL_OP_LDLOC_2:
			case IL_OP_LDLOC_3:
			{
				/* Load one of the first four locals onto the stack */
				if((opcode - IL_OP_LDLOC_0) >= numLocals)
				{
					VERIFY_INSN_ERROR();
				}
				stack[stackSize].typeInfo =
						ILTypeGetLocal(localVars, opcode - IL_OP_LDLOC_0);
				stack[stackSize].engineType =
						TypeToEngineType(stack[stackSize].typeInfo);
				ILCoderLoadLocal(coder, opcode - IL_OP_LDLOC_0,
							     stack[stackSize].typeInfo);
				++stackSize;
			}
			break;

			case IL_OP_STLOC_0:
			case IL_OP_STLOC_1:
			case IL_OP_STLOC_2:
			case IL_OP_STLOC_3:
			{
				/* Store the stack top to one of the first four locals */
				if((opcode - IL_OP_STLOC_0) >= numLocals)
				{
					VERIFY_INSN_ERROR();
				}
				if(!AssignCompatible(&(stack[stackSize - 1]),
									 ILTypeGetLocal
									 	(localVars, opcode - IL_OP_STLOC_0)))
				{
					VERIFY_TYPE_ERROR();
				}
				ILCoderStoreLocal(coder, opcode - IL_OP_STLOC_0,
								  stack[stackSize - 1].engineType,
								  ILTypeGetLocal
								  	(localVars, opcode - IL_OP_STLOC_0));
				--stackSize;
			}
			break;

			case IL_OP_LDARG_S:
			{
				/* Load an argument onto the stack */
				argNum = (ILUInt32)(pc[1]);
				if(argNum >= numArgs)
				{
					VERIFY_INSN_ERROR();
				}
				stack[stackSize].typeInfo =
						ILTypeGetParam(signature, argNum + 1);
				stack[stackSize].engineType =
						TypeToEngineType(stack[stackSize].typeInfo);
				ILCoderLoadArg(coder, argNum, stack[stackSize].typeInfo);
				++stackSize;
			}
			break;

			case IL_OP_LDARGA_S:
			{
				/* Load the address of an argument onto the stack */
				argNum = (ILUInt32)(pc[1]);
				if(argNum >= numArgs)
				{
					VERIFY_INSN_ERROR();
				}
				stack[stackSize].typeInfo =
						ILTypeGetParam(signature, argNum + 1);
				stack[stackSize].engineType = ILEngineType_T;
				ILCoderAddrOfArg(coder, argNum);
				++stackSize;
			}
			break;

			case IL_OP_STARG_S:
			{
				/* Store the top of the stack into an argument */
				argNum = (ILUInt32)(pc[1]);
				if(argNum >= numArgs)
				{
					VERIFY_INSN_ERROR();
				}
				type = ILTypeGetParam(signature, argNum + 1);
				if(!AssignCompatible(&(stack[stackSize - 1]), type))
				{
					VERIFY_TYPE_ERROR();
				}
				ILCoderStoreArg(coder, argNum,
							    stack[stackSize - 1].engineType, type);
				--stackSize;
			}
			break;

			case IL_OP_LDLOC_S:
			{
				/* Load a local variable onto the stack */
				argNum = (ILUInt32)(pc[1]);
				if(argNum >= numLocals)
				{
					VERIFY_INSN_ERROR();
				}
				stack[stackSize].typeInfo =
						ILTypeGetLocal(localVars, argNum);
				stack[stackSize].engineType =
						TypeToEngineType(stack[stackSize].typeInfo);
				ILCoderLoadLocal(coder, argNum, stack[stackSize].typeInfo);
				++stackSize;
			}
			break;

			case IL_OP_LDLOCA_S:
			{
				/* Load the address of a local variable onto the stack */
				argNum = (ILUInt32)(pc[1]);
				if(argNum >= numLocals)
				{
					VERIFY_INSN_ERROR();
				}
				stack[stackSize].typeInfo = ILTypeGetLocal(localVars, argNum);
				stack[stackSize].engineType = ILEngineType_T;
				ILCoderAddrOfLocal(coder, argNum);
				++stackSize;
			}
			break;

			case IL_OP_STLOC_S:
			{
				/* Store the top of the stack into a local variable */
				argNum = (ILUInt32)(pc[1]);
				if(argNum >= numArgs)
				{
					VERIFY_INSN_ERROR();
				}
				type = ILTypeGetLocal(localVars, argNum);
				if(!AssignCompatible(&(stack[stackSize - 1]), type))
				{
					VERIFY_TYPE_ERROR();
				}
				ILCoderStoreLocal(coder, argNum,
							      stack[stackSize - 1].engineType, type);
				--stackSize;
			}
			break;

			case IL_OP_LDNULL:
			{
				/* Load the "null" constant onto the stack */
				ILCoderConstant(coder, opcode, pc + 1);
				stack[stackSize].engineType = ILEngineType_O;
				stack[stackSize].typeInfo = 0;
				++stackSize;
			}
			break;

			case IL_OP_LDC_I4_M1:
			case IL_OP_LDC_I4_0:
			case IL_OP_LDC_I4_1:
			case IL_OP_LDC_I4_2:
			case IL_OP_LDC_I4_3:
			case IL_OP_LDC_I4_4:
			case IL_OP_LDC_I4_5:
			case IL_OP_LDC_I4_6:
			case IL_OP_LDC_I4_7:
			case IL_OP_LDC_I4_8:
			case IL_OP_LDC_I4_S:
			case IL_OP_LDC_I4:
			{
				/* 32-bit integer constants */
				ILCoderConstant(coder, opcode, pc + 1);
				stack[stackSize].engineType = ILEngineType_I4;
				stack[stackSize].typeInfo = 0;
				++stackSize;
			}
			break;

			case IL_OP_LDC_I8:
			{
				/* 64-bit integer constants */
				ILCoderConstant(coder, opcode, pc + 1);
				stack[stackSize].engineType = ILEngineType_I8;
				stack[stackSize].typeInfo = 0;
				++stackSize;
			}
			break;

			case IL_OP_LDC_R4:
			{
				/* 32-bit floating point constants */
				ILCoderConstant(coder, opcode, pc + 1);
				stack[stackSize].engineType = ILEngineType_F;
				stack[stackSize].typeInfo = 0;
				++stackSize;
			}
			break;

			case IL_OP_LDC_R8:
			{
				/* 64-bit floating point constants */
				ILCoderConstant(coder, opcode, pc + 1);
				stack[stackSize].engineType = ILEngineType_F;
				stack[stackSize].typeInfo = 0;
				++stackSize;
			}
			break;

			case IL_OP_DUP:
			{
				/* Duplicate the current top of stack */
				ILCoderDup(coder, stack[stackSize - 1].engineType,
						   stack[stackSize - 1].typeInfo);
				stack[stackSize] = stack[stackSize - 1];
				++stackSize;
			}
			break;

			case IL_OP_POP:
			{
				/* Pop the current top of stack */
				ILCoderPop(coder, stack[stackSize - 1].engineType,
						   stack[stackSize - 1].typeInfo);
				--stackSize;
			}
			break;

			case IL_OP_JMP:
			{
				/* TODO */
			}
			break;

			case IL_OP_CALL:
			{
				/* TODO */
			}
			break;

			case IL_OP_CALLI:
			{
				/* TODO */
			}
			break;

			case IL_OP_RET:
			{
				/* TODO */
				lastWasJump = 1;
			}
			break;

			case IL_OP_BR_S:
			{
				/* Unconditional short branch */
				ILCoderBranch(coder, opcode,
							  offset + ((ILUInt32)(ILInt32)(pc[1])),
							  ILEngineType_I4, ILEngineType_I4);
				lastWasJump = 1;
			}
			break;

			case IL_OP_BRFALSE_S:
			case IL_OP_BRTRUE_S:
			{
				/* Unary conditional short branch */
			  	dest = offset + ((ILUInt32)(ILInt32)(pc[1]));
			unaryBranch:
				if(STK_UNARY == ILEngineType_I4 ||
				   STK_UNARY == ILEngineType_I ||
				   STK_UNARY == ILEngineType_M ||
				   STK_UNARY == ILEngineType_O ||
				   STK_UNARY == ILEngineType_T)
				{
					ILCoderBranch(coder, opcode, dest, STK_UNARY, STK_UNARY);
					--stackSize;
				}
				else
				{
					VERIFY_TYPE_ERROR();
				}
			}
			break;

#if 0
			case IL_OP_BEQ_S:
			case IL_OP_BGE_S:
			case IL_OP_BGT_S:
			case IL_OP_BLE_S:
			case IL_OP_BLT_S:
			case IL_OP_BNE_UN_S:
			case IL_OP_BGE_UN_S:
			case IL_OP_BGT_UN_S:
			case IL_OP_BLE_UN_S:
			case IL_OP_BLT_UN_S:
			{
				/* Binary conditional short branch */
			  	dest = offset + ((ILUInt32)(ILInt32)(pc[1]));
			binaryBranch:
				commonType = CommonBinaryArith(STK_BINARY_1, STK_BINARY_2);
				if(commonType != IL_TYPE_VOID)
				{
					/* Compare numeric values */
					ILCoderBranch(coder, opcode, dest, STK_BINARY_1,
								  STK_BINARY_2);
					stackSize -= 2;
				}
				else if(STK_BINARY_1 == IL_TYPE_OBJECT_REF &&
						STK_BINARY_2 == IL_TYPE_OBJECT_REF &&
						IsEqualityBranch(opcode))
				{
					/* Compare object references */
					ILCoderBranch(coder, opcode, dest, STK_BINARY_1,
								  STK_BINARY_2, IL_TYPE_OBJECT_REF);
					stackSize -= 2;
				}
				else if(!unsafeAllowed)
				{
					VERIFY_TYPE_ERROR();
				}
				else if((STK_BINARY_1 == IL_TYPE_MANAGED_PTR ||
				         STK_BINARY_1 == IL_TYPE_TRANSIENT_PTR) &&
						(IL_TYPE_IS_I4(STK_BINARY_2) ||
						 IL_TYPE_IS_I(STK_BINARY_2)) &&
						IsEqualityBranch(opcode))
				{
					ILCoderBranch(coder, opcode, dest, STK_BINARY_1,
								  STK_BINARY_2, STK_BINARY_1);
					stackSize -= 2;
				}
				else if((STK_BINARY_2 == IL_TYPE_MANAGED_PTR ||
				         STK_BINARY_2 == IL_TYPE_TRANSIENT_PTR) &&
						(IL_TYPE_IS_I4(STK_BINARY_1) ||
						 IL_TYPE_IS_I(STK_BINARY_1)) &&
						IsEqualityBranch(opcode))
				{
					ILCoderBranch(coder, opcode, dest, STK_BINARY_1,
								  STK_BINARY_2, STK_BINARY_2);
					stackSize -= 2;
				}
				else if((STK_BINARY_1 == IL_TYPE_MANAGED_PTR ||
				         STK_BINARY_1 == IL_TYPE_TRANSIENT_PTR) &&
				        (STK_BINARY_2 == IL_TYPE_MANAGED_PTR ||
				         STK_BINARY_2 == IL_TYPE_TRANSIENT_PTR))
				{
					ILCoderBranch(coder, opcode, dest, STK_BINARY_1,
								  STK_BINARY_2, STK_BINARY_1);
					stackSize -= 2;
				}
				else
				{
					VERIFY_TYPE_ERROR();
				}
			}
			break;
#endif

			case IL_OP_BR:
			{
				/* Unconditional long branch */
				ILCoderBranch(coder, IL_OP_BR,
							  offset + ((ILUInt32)IL_READ_INT32(pc + 1)),
							  ILEngineType_I4, ILEngineType_I4);
				lastWasJump = 1;
			}
			break;

			case IL_OP_BRFALSE:
			case IL_OP_BRTRUE:
			{
				/* Unary conditional long branch */
				dest = offset + ((ILUInt32)IL_READ_INT32(pc + 1));
				goto unaryBranch;
			}
			/* Not reached */

#if 0
			case IL_OP_BEQ:
			case IL_OP_BGE:
			case IL_OP_BGT:
			case IL_OP_BLE:
			case IL_OP_BLT:
			case IL_OP_BNE_UN:
			case IL_OP_BGE_UN:
			case IL_OP_BGT_UN:
			case IL_OP_BLE_UN:
			case IL_OP_BLT_UN:
			{
				/* Binary conditional long branch */
				dest = offset + ((ILUInt32)IL_READ_INT32(pc + 1));
				goto binaryBranch;
			}
			/* Not reached */
#endif

			case IL_OP_SWITCH:
			{
				/* Switch statement */
			}
			break;

			case IL_OP_ADD:
			case IL_OP_ADD_OVF_UN:
			{
				/* Addition operators that may involve pointers */
				if(unsafeAllowed)
				{
					commonType = addUnsafeMatrix[STK_BINARY_1][STK_BINARY_2];
				}
				else
				{
					commonType = binaryNumericMatrix
									[STK_BINARY_1][STK_BINARY_2];
				}
				if(commonType == ILEngineType_F && opcode == IL_OP_ADD_OVF_UN)
				{
					/* Cannot use float values with overflow instructions */
					VERIFY_TYPE_ERROR();
				}
				else if(commonType == ILEngineType_M ||
				        commonType == ILEngineType_T)
				{
					ILCoderBinaryPtr(coder, opcode, STK_BINARY_1, STK_BINARY_2);
					STK_BINARY_1 = commonType;
					--stackSize;
				}
				else if(commonType != ILEngineType_Invalid)
				{
					ILCoderBinary(coder, opcode, STK_BINARY_1, STK_BINARY_2);
					STK_BINARY_1 = commonType;
					--stackSize;
				}
				else
				{
					VERIFY_TYPE_ERROR();
				}
			}
			break;

			case IL_OP_SUB:
			case IL_OP_SUB_OVF_UN:
			{
				/* Subtraction operators that may involve pointers */
				if(unsafeAllowed)
				{
					commonType = addUnsafeMatrix[STK_BINARY_1][STK_BINARY_2];
				}
				else
				{
					commonType = binaryNumericMatrix
									[STK_BINARY_1][STK_BINARY_2];
				}
				if(commonType == ILEngineType_F && opcode == IL_OP_SUB_OVF_UN)
				{
					/* Cannot use float values with overflow instructions */
					VERIFY_TYPE_ERROR();
				}
				else if(commonType == ILEngineType_M ||
				        commonType == ILEngineType_T)
				{
					ILCoderBinaryPtr(coder, opcode, STK_BINARY_1, STK_BINARY_2);
					STK_BINARY_1 = commonType;
					--stackSize;
				}
				else if(commonType != ILEngineType_Invalid)
				{
					ILCoderBinary(coder, opcode, STK_BINARY_1, STK_BINARY_2);
					STK_BINARY_1 = commonType;
					--stackSize;
				}
				else
				{
					VERIFY_TYPE_ERROR();
				}
			}
			break;

			case IL_OP_MUL:
			case IL_OP_DIV:
			case IL_OP_REM:
			{
				/* Arithmetic operators that do not involve pointers */
				commonType = binaryNumericMatrix[STK_BINARY_1][STK_BINARY_2];
				if(commonType != ILEngineType_Invalid)
				{
					ILCoderBinary(coder, opcode, STK_BINARY_1, STK_BINARY_2);
					STK_BINARY_1 = commonType;
					--stackSize;
				}
				else
				{
					VERIFY_TYPE_ERROR();
				}
			}
			break;

			case IL_OP_DIV_UN:
			case IL_OP_REM_UN:
			case IL_OP_ADD_OVF:
			case IL_OP_SUB_OVF:
			case IL_OP_MUL_OVF:
			case IL_OP_MUL_OVF_UN:
			case IL_OP_AND:
			case IL_OP_OR:
			case IL_OP_XOR:
			{
				/* Arithmetic operators that only apply to integers */
				commonType = binaryIntegerMatrix[STK_BINARY_1][STK_BINARY_2];
				if(commonType != ILEngineType_Invalid)
				{
					ILCoderBinary(coder, opcode, STK_BINARY_1, STK_BINARY_2);
					STK_BINARY_1 = commonType;
					--stackSize;
				}
				else
				{
					VERIFY_TYPE_ERROR();
				}
			}
			break;

			case IL_OP_SHL:
			case IL_OP_SHR:
			case IL_OP_SHR_UN:
			{
				/* Shift operators */
				commonType = binaryShiftMatrix[STK_BINARY_1][STK_BINARY_2];
				if(commonType != ILEngineType_Invalid)
				{
					ILCoderShift(coder, opcode, STK_BINARY_1, STK_BINARY_2);
					STK_BINARY_1 = commonType;
					--stackSize;
				}
				else
				{
					VERIFY_TYPE_ERROR();
				}
			}
			break;
		}

		/* Advance to the next instruction */
		pc += insnSize;
		len -= insnSize;
	}

	/* If the last instruction was not a jump, then the code
	   may fall off the end of the method */
	if(!lastWasJump)
	{
		VERIFY_INSN_ERROR();
	}

	/* Finish processing using the coder */
	result = ILCoderFinish(coder);

	/* Do we need to restart due to cache exhaustion in the coder? */
	if(ILCoderRestart(coder))
	{
		TempAllocatorDestroy(&allocator);
		goto restart;
	}

	/* Clean up and exit */
cleanup:
	TempAllocatorDestroy(&allocator);
	return result;
}

#ifdef	__cplusplus
};
#endif
