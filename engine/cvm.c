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
#include "lib_defs.h"
#include "cvm.h"
#include "cvm_config.h"
#include "cvm_format.h"
#if defined(HAVE_LIBFFI)
#include "ffi.h"
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Note: the configuration macros that used to be here
 * are now in "cvm_config.h".
 */

/*
 * Macros that can be used to bind important interpreter loop
 * variables to specific CPU registers for greater speed.
 * If we don't do this, then gcc generates VERY bad code for
 * the inner interpreter loop.  It just isn't smart enough to
 * figure out that "pc", "stacktop", and "frame" are the
 * best values to put into registers.
 */
#if defined(CVM_X86) && defined(__GNUC__) && !defined(IL_NO_ASM)

	#define REGISTER_ASM_X86 1

    #define REGISTER_ASM_PC(x)              register x asm ("esi")
    #define REGISTER_ASM_STACK(x)           register x asm ("edi")
    #define REGISTER_ASM_FRAME(x)           register x asm ("ebx")

	/*
	 * Memory copies on x86 use esi and edi, but we want them
	 * for something else.  So we force the copy to go through
	 * a function to prevent register spills in gcc.  Similarly
	 * for memory set operations.
	 */
	#define	IL_MEMCPY(dst,src,len)			LocalMemCpy((dst), (src), (len))
	#define	IL_MEMMOVE(dst,src,len)			LocalMemMove((dst), (src), (len))
	#define	IL_MEMZERO(dst,len)				LocalMemZero((dst), (len))
	#define	IL_MEMSET(dst,ch,len)			LocalMemSet((dst), (ch), (len))
	#define	IL_MEMCMP(dst,src,len)			LocalMemCmp((dst), (src), (len))
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
	static void LocalMemSet(void *dst, int ch, unsigned len)
	{
		ILMemSet(dst, ch, len);
	}
	static int LocalMemCmp(void *dst, const void *src, unsigned len)
	{
		return ILMemCmp(dst, src, len);
	}
#elif defined(CVM_X86_64) && defined(__GNUC__) && !defined(IL_NO_ASM)
	/* 16 registers - so we can avoid using esi, edi and ebx. */
	#define REGISTER_ASM_PC(x)              register x asm("r12")
	#define REGISTER_ASM_STACK(x)           register x asm("r14") 
	#define REGISTER_ASM_FRAME(x)           register x asm("r15") 
	#define	IL_MEMCPY(dst,src,len)			(ILMemCpy((dst), (src), (len)))
	#define	IL_MEMMOVE(dst,src,len)			(ILMemMove((dst), (src), (len)))
	#define	IL_MEMZERO(dst,len)				(ILMemZero((dst), (len)))
	#define	IL_MEMSET(dst,ch,len)			(ILMemSet((dst), (ch), (len)))
	#define	IL_MEMCMP(dst,src,len)			(ILMemCmp((dst), (src), (len)))

	#define X86_64_CGOTO(pc) __asm__ __volatile__ (\
								"jmpq *(%0)" \
								:: "r" (pc) )
	/* VM_CGOTO_SWITCH segfaults with just a jmpq */
	#define VM_CGOTO_PREFIXSWITCH(val) X86_64_CGOTO(pc)
	#define VM_CGOTO_BREAK(val) X86_64_CGOTO(pc)
	#define VM_CGOTO_BREAKNOEND(val) X86_64_CGOTO(pc)
	
#elif defined(CVM_ARM) && defined(__GNUC__) && !defined(IL_NO_ASM)
    #define REGISTER_ASM_PC(x)              register x asm ("r4")
    #define REGISTER_ASM_STACK(x)           register x asm ("r5")
    #define REGISTER_ASM_FRAME(x)           register x asm ("r6")
	#define	IL_MEMCPY(dst,src,len)			(ILMemCpy((dst), (src), (len)))
	#define	IL_MEMMOVE(dst,src,len)			(ILMemMove((dst), (src), (len)))
	#define	IL_MEMZERO(dst,len)				(ILMemZero((dst), (len)))
	#define	IL_MEMSET(dst,ch,len)			(ILMemSet((dst), (ch), (len)))
	#define	IL_MEMCMP(dst,src,len)			(ILMemCmp((dst), (src), (len)))
#elif defined(CVM_PPC) && defined(__GNUC__) && !defined(IL_NO_ASM)
    #define REGISTER_ASM_PC(x)              register x asm ("r18")
    #define REGISTER_ASM_STACK(x)           register x asm ("r19")
    #define REGISTER_ASM_FRAME(x)           register x asm ("r20")
    #define IL_MEMCPY(dst,src,len)          (ILMemCpy((dst), (src), (len)))
    #define IL_MEMMOVE(dst,src,len)         (ILMemMove((dst), (src), (len)))
    #define IL_MEMZERO(dst,len)             (ILMemZero((dst), (len)))
    #define IL_MEMSET(dst,ch,len)           (ILMemSet((dst), (ch), (len)))
    #define IL_MEMCMP(dst,src,len)          (ILMemCmp((dst), (src), (len)))
#elif defined(CVM_IA64) && defined(__GNUC__) && !defined(IL_NO_ASM)
    #define REGISTER_ASM_PC(x)              register x asm ("r4")
    #define REGISTER_ASM_STACK(x)           register x asm ("r5")
    #define REGISTER_ASM_FRAME(x)           register x asm ("r6")
	#define	IL_MEMCPY(dst,src,len)			(ILMemCpy((dst), (src), (len)))
	#define	IL_MEMMOVE(dst,src,len)			(ILMemMove((dst), (src), (len)))
	#define	IL_MEMZERO(dst,len)				(ILMemZero((dst), (len)))
	#define	IL_MEMSET(dst,ch,len)			(ILMemSet((dst), (ch), (len)))
	#define	IL_MEMCMP(dst,src,len)			(ILMemCmp((dst), (src), (len)))
#else
	#define IL_NO_REGISTERS_USED 1
    #define REGISTER_ASM_PC(x)              x
    #define REGISTER_ASM_STACK(x)           x
    #define REGISTER_ASM_FRAME(x)           x
	#define	IL_MEMCPY(dst,src,len)			(ILMemCpy((dst), (src), (len)))
	#define	IL_MEMMOVE(dst,src,len)			(ILMemMove((dst), (src), (len)))
	#define	IL_MEMZERO(dst,len)				(ILMemZero((dst), (len)))
	#define	IL_MEMSET(dst,ch,len)			(ILMemSet((dst), (ch), (len)))
	#define	IL_MEMCMP(dst,src,len)			(ILMemCmp((dst), (src), (len)))
#endif

#if defined(IL_USE_INTERRUPT_BASED_X)
	#if defined(IL_INTERRUPT_HAVE_X86_CONTEXT) && defined(REGISTER_ASM_X86)
		/* If the interrupt subsystem can provide us the x86 registers at the
		   time of interrupt then we don't need to save anything */
		#define INTERRUPT_BACKUP_FRAME()
		#define INTERRUPT_BACKUP_PC_STACKTOP_FRAME()

		/* We can restore locals directly from the register state
		   at the time of interrupt */
		#define INTERRUPT_RESTORE_FROM_THREAD() \
			do \
			{ \
				volatile int tempreg; \
				tempreg = thread->interruptContext.Ebx; \
				__asm__ __volatile__ \
				( "mov %0, %%ebx;" : : "m"(tempreg) ); \
				tempreg = thread->interruptContext.Edi; \
				__asm__ __volatile__ \
				( "mov %0, %%edi;" : : "m"(tempreg) ); \
				tempreg = thread->interruptContext.Esi; \
				__asm__ __volatile__ \
				( "mov %0, %%esi;" : : "m"(tempreg) ); \
			} \
			while (0);
	#else
		#define INTERRUPT_RESTORE_FROM_THREAD()
	#endif
#endif

#if defined(IL_USE_INTERRUPT_BASED_NULL_POINTER_CHECKS)
	#define BEGIN_NULL_CHECK(x)
	#define BEGIN_NULL_CHECK_STMT(x) x;
	#define END_NULL_CHECK()
#else
	#define BEGIN_NULL_CHECK(x) \
		if ((x) != 0) \
		{

	#define BEGIN_NULL_CHECK_STMT(x) BEGIN_NULL_CHECK(x)

	#define END_NULL_CHECK() \
		} \
		else \
		{ \
			NULL_POINTER_EXCEPTION(); \
		}
#endif

#if defined(IL_USE_INTERRUPT_BASED_INT_DIVIDE_BY_ZERO_CHECKS)
	#define BEGIN_INT_ZERO_DIV_CHECK(x)
	#define END_INT_ZERO_DIV_CHECK()
#else
	#define BEGIN_INT_ZERO_DIV_CHECK(x) \
		if ((x) != 0) \
		{

	#define END_INT_ZERO_DIV_CHECK() \
		} \
		else \
		{ \
			ARITHMETIC_EXCEPTION(); \
		}
#endif

/*
 * int overflow checks
 */
#if defined(IL_USE_INTERRUPT_BASED_INT_OVERFLOW_CHECKS)
	#define BEGIN_INT_OVERFLOW_CHECK(x)
	#define END_INT_OVERFLOW_CHECK()
#else
	#define BEGIN_INT_OVERFLOW_CHECK(x) \
		if (x) \
		{

	#define END_INT_OVERFLOW_CHECK() \
		} \
		else \
		{ \
			ARITHMETIC_EXCEPTION(); \
		}
#endif

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
/* Global lock for trace outputs */
ILMutex *globalTraceMutex;
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
 * Modify the program counter and stack pointer in reverse order
 * because the "stkmod" value is extracted from the program.
 */
#define	MODIFY_PC_AND_STACK_REVERSE(pcmod,stkmod)	\
			do { \
				stacktop += (stkmod); \
				pc += (pcmod); \
			} while (0)

/*
 * Throw an overflow exception.
 */
#define	OVERFLOW_EXCEPTION()	\
			do { \
				COPY_STATE_TO_THREAD(); \
				stacktop[0].ptrValue = \
					_ILSystemException(thread, "System.OverflowException"); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw an arithmetic exception.
 */
#define	ARITHMETIC_EXCEPTION()	\
			do { \
				COPY_STATE_TO_THREAD(); \
				stacktop[0].ptrValue = \
					_ILSystemException(thread, "System.ArithmeticException"); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw a division by zero exception.
 */
#define	ZERO_DIV_EXCEPTION()	\
			do { \
				COPY_STATE_TO_THREAD(); \
				stacktop[0].ptrValue = \
					_ILSystemException(thread, \
									   "System.DivideByZeroException"); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw a null pointer exception.
 */
#define	NULL_POINTER_EXCEPTION()	\
			do { \
				COPY_STATE_TO_THREAD(); \
				stacktop[0].ptrValue = \
					_ILSystemException(thread, \
									   "System.NullReferenceException"); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw a stack overflow exception.
 */
#define	STACK_OVERFLOW_EXCEPTION()	\
			do { \
				COPY_STATE_TO_THREAD(); \
				stacktop[0].ptrValue = \
					_ILSystemException(thread, \
									   "System.StackOverflowException"); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw a missing method exception.
 */
#define	MISSING_METHOD_EXCEPTION()	\
			do { \
				COPY_STATE_TO_THREAD(); \
				stacktop[0].ptrValue = \
					_ILSystemException(thread, \
									   "System.MissingMethodException"); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Process an exception that was thrown by "_ILConvertMethod".
 * The state is already copied into the thread, with the
 * exception ready to be detected by "RESTORE_STATE_FROM_THREAD".
 */
#define	CONVERT_FAILED_EXCEPTION()	\
			do { \
				RESTORE_STATE_FROM_THREAD(); \
			} while (0)

/*
 * Throw an invalid cast exception.
 */
#define	INVALID_CAST_EXCEPTION()	\
			do { \
				COPY_STATE_TO_THREAD(); \
				stacktop[0].ptrValue = \
					_ILSystemException(thread, "System.InvalidCastException"); \
				stacktop += 1; \
				goto throwException; \
			} while (0)

/*
 * Throw an array index out of range exception.
 */
#define	ARRAY_INDEX_EXCEPTION()	\
			do { \
				COPY_STATE_TO_THREAD(); \
				stacktop[0].ptrValue = \
					_ILSystemException(thread, \
									   "System.IndexOutOfRangeException"); \
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
	IL_MEMCPY(&temp, stack, sizeof(ILInt64));
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
	IL_MEMCPY(stack, &value, sizeof(ILInt64));
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
	IL_MEMCPY(&temp, stack, sizeof(ILUInt64));
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
	IL_MEMCPY(stack, &value, sizeof(ILUInt64));
#endif
}

#ifdef IL_CONFIG_FP_SUPPORTED

/*
 * Read a native float value from a stack position.
 */
static IL_INLINE ILNativeFloat ReadFloat(CVMWord *stack)
{
#ifdef CVM_REALS_ALIGNED_WORD
	return *((ILNativeFloat *)stack);
#else
	ILNativeFloat temp;
	IL_MEMCPY(&temp, stack, sizeof(ILNativeFloat));
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
	IL_MEMCPY(stack, &value, sizeof(ILNativeFloat));
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
	IL_MEMCPY(&temp, stack, sizeof(ILDouble));
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
	IL_MEMCPY(stack, &value, sizeof(ILDouble));
#endif
}

#endif /* !IL_CONFIG_FP_SUPPORTED */

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
#if SIZEOF_VOID_P == 4
	return (void *)(IL_READ_UINT32(pc));
#else
	return (void *)(IL_READ_UINT64(pc));
#endif
#endif
}

/*
 * Copy the temporary state into the thread object.
 */
#define	COPY_STATE_TO_THREAD()	\
			do { \
				thread->pc = pc; \
				thread->frame = frame; \
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
#include "cvm_inline.c"
#undef IL_CVM_GLOBALS

/*
 * Define instruction label tables, if necessary.
 */
CVM_DEFINE_TABLES();

int _ILCVMInterpreter(ILExecThread *thread)
{
	REGISTER_ASM_PC(unsigned char *pc);
	REGISTER_ASM_STACK(CVMWord *stacktop);
	REGISTER_ASM_FRAME(CVMWord *frame);
	int divResult;
	CVMWord *stackmax;
	ILMethod *method;
	void *nativeArgs[CVM_MAX_NATIVE_ARGS + 1];

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
	#include "cvm_inline.c"
	#include "cvm_interrupt.c"
	#undef IL_CVM_LOCALS

	/* Include helper definitions and macros for the switch loop
	   that handle computed goto labels if necessary */
	#include "cvm_labels.h"

	/* Export the goto label tables from the interpreter if necessary */
	if(!thread)
	{
		CVM_EXPORT_TABLES();
		return 0;
	}

	/* Cache the engine state in local variables */
	pc = thread->pc;
	stacktop = thread->stackTop;
	frame = thread->frame;
	stackmax = thread->stackLimit;
	method = thread->method;
	thread->runningManagedCode = 1;

	#define IL_CVM_PRELUDE
	#include "cvm_interrupt.c"
	#undef IL_CVM_PRELUDE

	/* Enter the main instruction loop */
	for(;;)
	{
		CVM_DUMP();
		VMSWITCH(pc[0])
		{
			/**
			 * <opcode name="nop" group="Miscellaneous instructions">
			 *   <operation>Do nothing</operation>
			 *
			 *   <format>nop</format>
			 *   <dformat>{nop}</dformat>
			 *
			 *   <form name="nop" code="COP_NOP"/>
			 *
			 *   <description>Do nothing.</description>
			 * </opcode>
			 */
			VMCASE(COP_NOP):
			{
				/* The world's simplest instruction */
				MODIFY_PC_AND_STACK(CVM_LEN_NONE, 0);
			}
			VMBREAK(COP_NOP);

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
			#include "cvm_inline.c"
			#undef IL_CVM_MAIN

			/**
			 * <opcode name="break" group="Miscellaneous instructions">
			 *   <operation>Mark the position of a breakpoint</operation>
			 *
			 *   <format>break<fsep/>subcode</format>
			 *
			 *   <dformat>{break}<fsep/>subcode</dformat>
			 *
			 *   <form name="break" code="COP_BREAK"/>
			 *
			 *   <description>This instruction marks a position in the
			 *   CVM bytecode that may be used as a breakpoint in
			 *   debug versions of the runtime engine.<p/>
			 *
			 *   Every potentional position for a breakpoint is marked,
			 *   even if those positions will never have active breakpoints
			 *   set on them.  The runtime engine keeps a list of active
			 *   breakpoints, which is inspected at each potentional
			 *   breakpoint.</description>
			 * </opcode>
			 */
			VMCASE(COP_BREAK):
			{
			#ifdef IL_CONFIG_DEBUG_LINES
				/* Check the breakpoint against the watch list */
				if(_ILIsBreak(thread, method))
				{
					COPY_STATE_TO_THREAD();
					_ILBreak(thread, (int)CVM_ARG_BREAK_SUBCODE);
					RESTORE_STATE_FROM_THREAD();
				}
			#endif
				MODIFY_PC_AND_STACK(CVM_LEN_BREAK, 0);
			}
			VMBREAK(COP_BREAK);

			/**
			 * <opcode name="wide" group="Miscellaneous instructions">
			 *   <operation>Modify an instruction to its wide form</operation>
			 *
			 *   <format>wide<fsep/>opcode<fsep/>...</format>
			 *
			 *   <form name="wide" code="COP_WIDE"/>
			 *
			 *   <description>The <i>wide</i> instruction modifies another
			 *   instruction to take longer operands.  The format of the
			 *   operands depends upon the <i>opcode</i>.</description>
			 *
			 *   <notes>The documentation for other instructions includes
			 *   information on their wide forms where appropriate.<p/>
			 *
			 *   There is no direct format for this instruction,
			 *   because <i>wide</i> is not required for the direct
			 *   encoding.</notes>
			 * </opcode>
			 */
#ifndef IL_CVM_DIRECT
			VMCASE(COP_WIDE):
			{
				CVM_WIDE_DUMP();
				switch(CVM_ARG_SUB_OPCODE)
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
					#include "cvm_inline.c"
					#undef IL_CVM_WIDE

					default:
					{
						/* Treat all other wide opcodes as NOP */
						MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 0);
					}
					break;
				}
			}
			VMBREAK(COP_WIDE);
#else
			VMCASE(COP_WIDE):
			{
				/* We don't need "wide" in direct mode, so just stub it out */
				MODIFY_PC_AND_STACK(CVM_LEN_NONE, 0);
			}
			VMBREAK(COP_WIDE);
#endif

			/**
			 * <opcode name="prefix" group="Miscellaneous instructions">
			 *   <operation>Prefix an alternative instruction</operation>
			 *
			 *   <format>prefix<fsep/>opcode<fsep/>...</format>
			 *
			 *   <form name="prefix" code="COP_PREFIX"/>
			 *
			 *   <description>The <i>prefix</i> instruction is used to
			 *   switch the runtime engine into an alternative instruction
			 *   set.  The alternative instruction is <i>opcode</i>.
			 *   Prefixing is necessary because the VM has more than
			 *   256 distinct instructions.</description>
			 *
			 *   <notes>There is no direct format for this instruction,
			 *   because <i>prefix</i> is not required for the direct
			 *   encoding.</notes>
			 * </opcode>
			 */
			VMCASE(COP_PREFIX):
			{
				/* Execute a prefixed opcode */
#ifndef IL_CVM_DIRECT
				CVM_PREFIX_DUMP();
				VMPREFIXSWITCH(CVM_ARG_SUB_OPCODE)
				{
#else
				/* We don't need "prefix" in direct mode, so just stub it out */
				switch(1)
				{
				case 0:
					VMPREFIXDEFAULT:
					{
						MODIFY_PC_AND_STACK(CVM_LEN_NONE, 0);
					}
					break;
				}
			}
			VMBREAK(COP_PREFIX);
#endif
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
					#include "cvm_inline.c"
					#undef IL_CVM_PREFIX

					/**
					 * <opcode name="unroll_method"
					 *         group="Miscellaneous instructions">
					 *   <operation>Mark a method for unrolling</operation>
					 *
					 *   <dformat>{unroll_method}</dformat>
					 *
					 *   <form name="unroll_method"
					 *         code="COP_PREFIX_UNROLL_METHOD"/>
					 *
					 *   <description>The <i>unroll_method</i> instruction is
					 *   used in direct code to trigger native code unrolling.
					 *   <p/>
					 *
					 *   Unrolling converts fragments of the method into
					 *   native code for the underlying CPU, to speed up
					 *   execution.</description>
					 *
					 *   <notes>There is no bytecode format for this
					 *   instruction, because unrolling is not possible
					 *   with the bytecode encoding.<p/>
					 *
					 *   In a method that can support unrolling, the
					 *   <i>nop</i> instruction is used to mark
					 *   a label, so that the unroller can process labels
					 *   in a single translation pass.  The <i>prefix</i>
					 *   instruction is used to mark the end of the method's
					 *   code, so that the unroller knows where to stop.
					 *   </notes>
					 * </opcode>
					 */
					VMCASE(COP_PREFIX_UNROLL_METHOD):
					{
						/* Unroll the current method to native code */
					#ifdef IL_CVM_DIRECT_UNROLLED
						if(_ILUnrollMethod(thread, thread->process->coder,
										   pc, method))
						{
							VMSWITCH(0);
						}
						else
					#endif
						{
							MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 0);
						}
					}
					VMBREAK(COP_PREFIX_UNROLL_METHOD);

#ifndef IL_CVM_DIRECT
					VMPREFIXDEFAULT:
					{
						/* Treat all other prefixed opcodes as NOP */
						MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 0);
					}
					VMBREAK(COP_PREFIX);
				}
			}
			VMOUTERBREAK;
#endif

			VMDEFAULT:
			{
				/* Treat all other opcodes as NOP */
				MODIFY_PC_AND_STACK(CVM_LEN_NONE, 0);
			}
			VMBREAK(_DEFAULT_MAIN);
		}
	}

	/* We should never get here, but keep the compiler happy */
	return 0;
}

#ifdef	__cplusplus
};
#endif
