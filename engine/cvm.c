/*
 * cvm.c - Implementation of the "Converted Virtual Machine".
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
#include "cvm.h"
#include "ffi.h"
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef IL_NATIVE_WIN32
#include <float.h>
#define isnan(value)	_isnan((value))
#define HAVE_ISNAN 1
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Determine what CPU we are compiling for, and any
 * additional optimizations we can use for that CPU.
 */
#if defined(__i386) || defined(__i386__)
	#define	CVM_X86
	#define	CVM_LONGS_ALIGNED_WORD
	#define	CVM_REALS_ALIGNED_WORD
	#define	CVM_DOUBLES_ALIGNED_WORD
	#define CVM_WORDS_AND_PTRS_SAME_SIZE
#endif

/*
 * Macros that can be used to bind important interpreter loop
 * variables to specific CPU registers for greater speed.
 * If we don't do this, then gcc generates VERY bad code for
 * the inner interpreter loop.  It just isn't smart enough to
 * figure out that "pc", "stacktop", and "frame" are the
 * best values to put into registers.
 */
#if defined(CVM_X86) && defined(__GNUC__) && !defined(IL_NO_ASM)
    #define REGISTER_ASM_PC(x)              register x asm ("esi")
    #define REGISTER_ASM_STACK(x)           register x asm ("edi")
    #define REGISTER_ASM_FRAME(x)           register x asm ("ebx")

	/*
	 * Memory copies on x86 use esi and edi, but we want them
	 * for something else.  So we force the copy to go through
	 * a function to prevent register spills in gcc.  Similarly
	 * for memory zero'ing operations.
	 */
	#define	IL_MEMCPY(dst,src,len)			LocalMemCpy((dst), (src), (len))
	#define	IL_MEMMOVE(dst,src,len)			LocalMemMove((dst), (src), (len))
	#define	IL_MEMZERO(dst,len)				LocalMemZero((dst), (len))
	static void LocalMemCpy(void *dst, const void *src, unsigned len)
	{
		ILMemCpy(dst, src, len);
	}
	static void LocalMemMove(void *dst, const void *src, unsigned len)
	{
		ILMemMove(dst, src, len);
	}
	static void LocalMemZero(void *dst, unsigned len)
	{
		ILMemZero(dst, len);
	}
#else
    #define REGISTER_ASM_PC(x)              x
    #define REGISTER_ASM_STACK(x)           x
    #define REGISTER_ASM_FRAME(x)           x
	#define	IL_MEMCPY(dst,src,len)			(ILMemCpy((dst), (src), (len)))
	#define	IL_MEMMOVE(dst,src,len)			(ILMemMove((dst), (src), (len)))
	#define	IL_MEMZERO(dst,len)				(ILMemZero((dst), (len)))
#endif

/*
 * Modify the program counter and stack pointer.
 */
#define	MODIFY_PC_AND_STACK(pcmod,stkmod)	\
			do { \
				pc += (pcmod); \
				stacktop += (stkmod); \
			} while (0)

/*
 * Throw an overflow exception.
 */
#define	OVERFLOW_EXCEPTION()	\
			do { \
				stacktop[0].ptrValue = \
					SystemException(thread, "OverflowException", \
									(ILInt32)(pc - pcstart)); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw an arithmetic exception.
 */
#define	ARITHMETIC_EXCEPTION()	\
			do { \
				stacktop[0].ptrValue = \
					SystemException(thread, "ArithmeticException", \
									(ILInt32)(pc - pcstart)); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw a division by zero exception.
 */
#define	ZERO_DIV_EXCEPTION()	\
			do { \
				stacktop[0].ptrValue = \
					SystemException(thread, "DivideByZeroException", \
									(ILInt32)(pc - pcstart)); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw a null pointer exception.
 */
#define	NULL_POINTER_EXCEPTION()	\
			do { \
				stacktop[0].ptrValue = \
					SystemException(thread, "NullReferenceException", \
									(ILInt32)(pc - pcstart)); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw a stack overflow exception.
 */
#define	STACK_OVERFLOW_EXCEPTION()	\
			do { \
				stacktop[0].ptrValue = \
					SystemException(thread, "StackOverflowException", \
									(ILInt32)(pc - pcstart)); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw a missing method exception.
 */
#define	MISSING_METHOD_EXCEPTION()	\
			do { \
				stacktop[0].ptrValue = \
					SystemException(thread, "MissingMethodException", \
									(ILInt32)(pc - pcstart)); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Read a long value from a stack position.
 */
static IL_INLINE ILInt64 ReadLong(CVMWord *stack)
{
#ifdef CVM_LONGS_ALIGNED_WORD
	return *((ILInt64 *)stack);
#else
	ILInt64 temp;
	ILMemCpy(&temp, stack, sizeof(ILInt64));
	return temp;
#endif
}

/*
 * Write a long value to a stack position.
 */
static IL_INLINE void WriteLong(CVMWord *stack, ILInt64 value)
{
#ifdef CVM_LONGS_ALIGNED_WORD
	*((ILInt64 *)stack) = value;
#else
	ILMemCpy(stack, &value, sizeof(ILInt64));
#endif
}

/*
 * Read an unsigned long value from a stack position.
 */
static IL_INLINE ILUInt64 ReadULong(CVMWord *stack)
{
#ifdef CVM_LONGS_ALIGNED_WORD
	return *((ILUInt64 *)stack);
#else
	ILUInt64 temp;
	ILMemCpy(&temp, stack, sizeof(ILUInt64));
	return temp;
#endif
}

/*
 * Write an unsigned long value to a stack position.
 */
static IL_INLINE void WriteULong(CVMWord *stack, ILUInt64 value)
{
#ifdef CVM_LONGS_ALIGNED_WORD
	*((ILUInt64 *)stack) = value;
#else
	ILMemCpy(stack, &value, sizeof(ILUInt64));
#endif
}

/*
 * Read a native float value from a stack position.
 */
static IL_INLINE ILNativeFloat ReadFloat(CVMWord *stack)
{
#ifdef CVM_REALS_ALIGNED_WORD
	return *((ILNativeFloat *)stack);
#else
	ILNativeFloat temp;
	ILMemCpy(&temp, stack, sizeof(ILNativeFloat));
	return temp;
#endif
}

/*
 * Write a native float value to a stack position.
 */
static IL_INLINE void WriteFloat(CVMWord *stack, ILNativeFloat value)
{
#ifdef CVM_REALS_ALIGNED_WORD
	*((ILNativeFloat *)stack) = value;
#else
	ILMemCpy(stack, &value, sizeof(ILNativeFloat));
#endif
}

/*
 * Read a double value from a stack position.
 */
static IL_INLINE ILDouble ReadDouble(CVMWord *stack)
{
#ifdef CVM_DOUBLES_ALIGNED_WORD
	return *((ILDouble *)stack);
#else
	ILDouble temp;
	ILMemCpy(&temp, stack, sizeof(ILDouble));
	return temp;
#endif
}

/*
 * Write a double value to a stack position.
 */
static IL_INLINE void WriteDouble(CVMWord *stack, ILDouble value)
{
#ifdef CVM_DOUBLES_ALIGNED_WORD
	*((ILDouble *)stack) = value;
#else
	ILMemCpy(stack, &value, sizeof(ILDouble));
#endif
}

/*
 * Read a pointer value from a program position.
 */
static IL_INLINE void *ReadPointer(unsigned char *pc)
{
#ifdef CVM_X86
	/* The x86 can read values from non-aligned addresses */
	return *((void **)pc);
#else
	/* We need to be careful about alignment on other platforms */
	if(sizeof(void *) == 4)
	{
		return (void *)(IL_READ_UINT32(pc));
	}
	else
	{
		return (void *)(IL_READ_UINT64(pc));
	}
#endif
}

/*
 * Create a system exception object of a particular class.
 */
static void *SystemException(ILExecThread *thread, const char *className,
							 ILInt32 pcoffset)
{
	return 0;
}

/*
 * Copy the temporary state into the thread object.
 */
#define	COPY_STATE_TO_THREAD()	\
			do { \
				thread->pcstart = pcstart; \
				thread->pc = (ILUInt32)(pc - pcstart); \
				thread->frame = (ILUInt32)(frame - stackbottom); \
				thread->stackTop = stacktop; \
				thread->method = method; \
			} while (0)

/* Define global variables that are used by the instruction categories */
#define IL_CVM_GLOBALS
#include "cvm_var.c"
#include "cvm_ptr.c"
#include "cvm_stack.c"
#include "cvm_arith.c"
#include "cvm_conv.c"
#include "cvm_const.c"
#include "cvm_branch.c"
#include "cvm_call.c"
#include "cvm_except.c"
#include "cvm_compare.c"
#undef IL_CVM_GLOBALS

int _ILCVMInterpreter(ILExecThread *thread)
{
	REGISTER_ASM_PC(unsigned char *pc) = thread->pcstart + thread->pc;
	REGISTER_ASM_STACK(CVMWord *stacktop) = thread->stackTop;
	REGISTER_ASM_FRAME(CVMWord *frame) = thread->stackBase + thread->frame;
	int divResult;
	unsigned char *pcstart = thread->pcstart;
	CVMWord *stackmax = thread->stackLimit;
	CVMWord *stackbottom = thread->stackBase;
	ILMethod *method = thread->method;

	/* Define local variables that are used by the instruction categories */
	#define IL_CVM_LOCALS
	#include "cvm_var.c"
	#include "cvm_ptr.c"
	#include "cvm_stack.c"
	#include "cvm_arith.c"
	#include "cvm_conv.c"
	#include "cvm_const.c"
	#include "cvm_branch.c"
	#include "cvm_call.c"
	#include "cvm_except.c"
	#include "cvm_compare.c"
	#undef IL_CVM_LOCALS

	for(;;)
	{
		switch(pc[0])
		{
			case COP_NOP:
			{
				/* The world's simplest instruction */
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			/* Include the instruction categories for the main switch */
			#define IL_CVM_MAIN
			#include "cvm_var.c"
			#include "cvm_ptr.c"
			#include "cvm_stack.c"
			#include "cvm_arith.c"
			#include "cvm_conv.c"
			#include "cvm_const.c"
			#include "cvm_branch.c"
			#include "cvm_call.c"
			#include "cvm_except.c"
			#include "cvm_compare.c"
			#undef IL_CVM_MAIN

			case COP_WIDE:
			{
				switch(pc[1])
				{
					/* Include the instruction categories for the wide switch */
					#define IL_CVM_WIDE
					#include "cvm_var.c"
					#include "cvm_ptr.c"
					#include "cvm_stack.c"
					#include "cvm_arith.c"
					#include "cvm_conv.c"
					#include "cvm_const.c"
					#include "cvm_branch.c"
					#include "cvm_call.c"
					#include "cvm_except.c"
					#include "cvm_compare.c"
					#undef IL_CVM_WIDE

					default:
					{
						/* Treat all other wide opcodes as NOP */
						MODIFY_PC_AND_STACK(2, 0);
					}
					break;
				}
			}
			break;

			case COP_PREFIX:
			{
				/* Execute a prefixed opcode */
				switch(pc[1])
				{
					/* Include instruction categories for the prefix switch */
					#define IL_CVM_PREFIX
					#include "cvm_var.c"
					#include "cvm_ptr.c"
					#include "cvm_stack.c"
					#include "cvm_arith.c"
					#include "cvm_conv.c"
					#include "cvm_const.c"
					#include "cvm_branch.c"
					#include "cvm_call.c"
					#include "cvm_except.c"
					#include "cvm_compare.c"
					#undef IL_CVM_PREFIX

					default:
					{
						/* Treat all other prefixed opcodes as NOP */
						MODIFY_PC_AND_STACK(2, 0);
					}
					break;
				}
			}
			break;

			default:
			{
				/* Treat all other opcodes as NOP */
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;
		}
	}

	/* We should never get here, but keep the compiler happy */
	return 0;
}

#ifdef	__cplusplus
};
#endif
