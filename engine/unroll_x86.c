/*
 * unroll_x86.c - CVM unrolling module for x86.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
#include "cvm.h"
#include "method_cache.h"
#include "cvm_config.h"
#include "cvm_format.h"
#include "x86_codegen.h"
#include "il_dumpasm.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#if defined(IL_CVM_DIRECT) && defined(CVM_X86) && \
	defined(__GNUC__) && !defined(IL_NO_ASM)

/*
 * Registers that have a fixed meaning assigned by the CVM interpreter.
 */
#define	REG_PC			X86_ESI
#define	REG_STACK		X86_EDI
#define	REG_FRAME		X86_EBX

/*
 * Special register value that indicates "on FPU stack".
 */
#define	REG_FPU			8

/*
 * Unrolled code generation state.
 */
typedef struct
{
	unsigned char *out;			/* Code output buffer */
	int		regsUsed;			/* Registers currently in use */
	int		regsSaved;			/* Fixed registers that were saved */
	int		pseudoStack[16];	/* Registers that make up the pseudo stack */
	int		pseudoStackSize;	/* Size of the pseudo stack */
	int		fpStackSize;		/* Size of the floating-point stack */
	int		stackHeight;		/* Current virtual height of CVM stack */
	long	cachedLocal;		/* Local variable that was just stored */
	int		cachedReg;			/* Register for variable that was stored */
	int		thisValidated;		/* "this" has been checked for NULL */

} X86Unroll;

/*
 * Flag bits for determining if certain registers are in use or not.
 */
#define	REG_EAX_MASK		(1 << X86_EAX)
#define	REG_ECX_MASK		(1 << X86_ECX)
#define	REG_EDX_MASK		(1 << X86_EDX)
#define	REG_EBX_MASK		(1 << X86_EBX)
#define	REG_ESP_MASK		(1 << X86_ESP)
#define	REG_EBP_MASK		(1 << X86_EBP)
#define	REG_ESI_MASK		(1 << X86_ESI)
#define	REG_EDI_MASK		(1 << X86_EDI)
#define	REG_FPU_MASK		(1 << REG_FPU)

/*
 * Register allocation order for the word registers.  We can
 * reclaim ESI because we have to reload the PC at the end of
 * the unrolled sequence anyway.  We can reclaim EBP by saving
 * it on the system stack.
 */
static char const regAllocOrder[] = {X86_EAX, X86_ECX, X86_EDX,
									 X86_ESI, X86_EBP};
#define	regAllocOrderSize	(sizeof(regAllocOrder) / sizeof(char))

/*
 * Registers that must be saved on the system stack if we use them.
 */
#define	REG_NEED_SAVE_MASK	REG_EBP_MASK

/*
 * Flush all registers to the CVM operand stack.
 */
static void FlushRegisterStack(X86Unroll *unroll)
{
	int index, reg, height;

	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* Flush the word registers from bottom-most to top-most */
	for(index = 0; index < unroll->pseudoStackSize; ++index)
	{
		reg = unroll->pseudoStack[index];
		if(reg != REG_FPU)
		{
			/* Flush a word register */
			unroll->regsUsed &= ~(1 << reg);
			x86_mov_membase_reg(unroll->out, REG_STACK,
								unroll->stackHeight, reg, 4);
			unroll->stackHeight += 4;
		}
		else
		{
			/* Skip an FPU register */
			unroll->stackHeight += 12;
		}
	}

	/* Flush the FPU registers from top-most to bottom-most */
	height = unroll->stackHeight;
	for(index = unroll->pseudoStackSize - 1; index >= 0; --index)
	{
		reg = unroll->pseudoStack[index];
		if(reg != REG_FPU)
		{
			/* Skip a word register */
			height -= 4;
		}
		else
		{
			/* Flush an FPU register */
			--(unroll->fpStackSize);
			height -= 12;
			x86_fst80_membase(unroll->out, REG_STACK, height);
		}
	}

	/* Reset the pseudo stack size */
	unroll->pseudoStackSize = 0;
}

/*
 * Fix the height of the CVM stack to match the cached location.
 */
static void FixStackHeight(X86Unroll *unroll)
{
	if(unroll->stackHeight > 0)
	{
		x86_alu_reg_imm(unroll->out, X86_ADD, REG_STACK,
						unroll->stackHeight);
	}
	else if(unroll->stackHeight < 0)
	{
		x86_alu_reg_imm(unroll->out, X86_SUB, REG_STACK,
						-(unroll->stackHeight));
	}
}

/*
 * Perform an unconditional branch to a new location.  This is
 * also used when control falls out through the end of the block.
 * In that case, "pc" is the start of the following block.
 */
static void BranchToPC(X86Unroll *unroll, unsigned char *pc)
{
	/* Flush the register stack to the CVM stack */
	FlushRegisterStack(unroll);

	/* Update the REG_STACK register if necessary */
	FixStackHeight(unroll);
	unroll->stackHeight = 0;

	/* Restore the special registers that we used */
	if((unroll->regsSaved & REG_EBP_MASK) != 0)
	{
		x86_pop_reg(unroll->out, X86_EBP);
	}
	unroll->regsSaved = 0;

	/* Reload the CVM interpreter's program counter */
	x86_mov_reg_imm(unroll->out, REG_PC, (int)pc);

	/* Jump to the instruction at the PC */
	x86_jump_membase(unroll->out, REG_PC, 0);
}

/*
 * Re-execute the current instruction in the CVM interpreter,
 * to process exception conditions.
 */
static void ReExecute(X86Unroll *unroll, unsigned char *pc,
					  unsigned char *label)
{
	int index, reg, height, finalHeight;

	/* Flush the register stack, but don't change it as we will
	   still need it further down the code */
	height = unroll->stackHeight;
	for(index = 0; index < unroll->pseudoStackSize; ++index)
	{
		reg = unroll->pseudoStack[index];
		if(reg != REG_FPU)
		{
			/* Flush a word register */
			x86_mov_membase_reg(unroll->out, REG_STACK, height, reg, 4);
			height += 4;
		}
		else
		{
			/* Skip an FPU register */
			height += 12;
		}
	}
	finalHeight = height;
	for(index = unroll->pseudoStackSize - 1; index >= 0; --index)
	{
		reg = unroll->pseudoStack[index];
		if(reg != REG_FPU)
		{
			/* Skip a word register */
			height -= 4;
		}
		else
		{
			/* Flush an FPU register */
			height -= 12;
			x86_fst80_membase(unroll->out, REG_STACK, height);
		}
	}

	/* Fix up the stack height */
	if(finalHeight > 0)
	{
		x86_alu_reg_imm(unroll->out, X86_ADD, REG_STACK, finalHeight);
	}
	else if(finalHeight < 0)
	{
		x86_alu_reg_imm(unroll->out, X86_SUB, REG_STACK, -finalHeight);
	}

	/* Restore the saved special registers */
	if((unroll->regsSaved & REG_EBP_MASK) != 0)
	{
		x86_pop_reg(unroll->out, X86_EBP);
	}

	/* Reload PC for the current instruction */
	x86_mov_reg_imm(unroll->out, REG_PC, (int)pc);

	/* Jump into the CVM interpreter to re-execute the instruction */
	x86_jump_code(unroll->out, label);
}

/*
 * Get a register that can be used to store 32-bit word values.
 */
static int GetWordRegister(X86Unroll *unroll)
{
	int index, reg, regmask;

	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* Search for a free register */
	for(index = 0; index < regAllocOrderSize; ++index)
	{
		reg = regAllocOrder[index];
		regmask = (1 << reg);
		if((unroll->regsUsed & regmask) == 0)
		{
			unroll->pseudoStack[(unroll->pseudoStackSize)++] = reg;
			unroll->regsUsed |= regmask;
			if((regmask & REG_NEED_SAVE_MASK) != 0 &&
			   (unroll->regsSaved & regmask) == 0)
			{
				/* Save a special register on the system stack */
				x86_push_reg(unroll->out, reg);
				unroll->regsSaved |= regmask;
			}
			return reg;
		}
	}

	/* Spill the bottom-most register to the CVM stack and reuse it */
	reg = unroll->pseudoStack[0];
	if(reg != REG_FPU)
	{
		for(index = 1; index < unroll->pseudoStackSize; ++index)
		{
			unroll->pseudoStack[index - 1] = unroll->pseudoStack[index];
		}
		unroll->pseudoStack[unroll->pseudoStackSize - 1] = reg;
		x86_mov_membase_reg(unroll->out, REG_STACK,
							unroll->stackHeight, reg, 4);
		unroll->stackHeight += 4;
		return reg;
	}
	else
	{
		/* We have an FPU register at the bottom, so flush everything */
		FlushRegisterStack(unroll);

		/* Restart with the EAX register */
		unroll->regsUsed |= REG_EAX_MASK;
		unroll->pseudoStack[(unroll->pseudoStackSize)++] = X86_EAX;
		return X86_EAX;
	}
}

/*
 * Get a 32-bit word register, but try to reuse the previously
 * popped register if the local variable offsets are identical.
 * This is used to optimise the common case of storing to a local
 * and then immediately reloading it.  Returns -1 if the register
 * was reused.
 */
static int GetCachedWordRegister(X86Unroll *unroll, long local)
{
	if(unroll->cachedLocal == local)
	{
		/* Push the previous register back onto the stack */
		int reg = unroll->cachedReg;
		unroll->pseudoStack[(unroll->pseudoStackSize)++] = reg;
		unroll->regsUsed |= (1 << reg);

		/* We can only do this once: use a new register if the
		   variable is loaded again */
		unroll->cachedLocal = -1;
		unroll->cachedReg = -1;

		/* Tell the caller that the top-most register was reused */
		return -1;
	}
	else
	{
		return GetWordRegister(unroll);
	}
}

/*
 * Get a register that can be used to store floating-point values.
 * The register will always be ST(0).
 */
static void GetFPRegister(X86Unroll *unroll)
{
	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* If the FPU stack is full, then flush and restart */
	if(unroll->fpStackSize >= 8)
	{
		FlushRegisterStack(unroll);
	}

	/* Allocate ST(0) for the caller */
	++(unroll->fpStackSize);
	unroll->pseudoStack[(unroll->pseudoStackSize)++] = REG_FPU;
}

/*
 * Get the top-most word value on the stack into a register.
 */
static int GetTopWordRegister(X86Unroll *unroll)
{
	int reg;

	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* Check for an existing word register on the stack */
	if(unroll->pseudoStackSize > 0)
	{
		reg = unroll->pseudoStack[unroll->pseudoStackSize - 1];
		if(reg != REG_FPU)
		{
			return reg;
		}
		FlushRegisterStack(unroll);
	}

	/* Load the top of the CVM stack into EAX */
	unroll->regsUsed |= REG_EAX_MASK;
	unroll->pseudoStack[0] = X86_EAX;
	unroll->pseudoStackSize = 1;
	unroll->stackHeight -= 4;
	x86_mov_reg_membase(unroll->out, X86_EAX, REG_STACK,
						unroll->stackHeight, 4);
	return X86_EAX;
}

/*
 * Get the two top-most word values on the stack into registers.
 * "reg1" will be the lower of the two.
 */
static void GetTopTwoWordRegisters(X86Unroll *unroll, int *reg1, int *reg2)
{
	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* See if we already have two word registers in play */
	if(unroll->pseudoStackSize > 1)
	{
		*reg1 = unroll->pseudoStack[unroll->pseudoStackSize - 2];
		if(*reg1 != REG_FPU)
		{
			*reg2 = unroll->pseudoStack[unroll->pseudoStackSize - 1];
			if(*reg2 != REG_FPU)
			{
				return;
			}
		}
	}

	/* See if we have one word register in play */
	if(unroll->pseudoStackSize == 1)
	{
		*reg2 = unroll->pseudoStack[0];
		if(*reg2 != REG_FPU)
		{
			/* Allocate another word register */
			*reg1 = GetWordRegister(unroll);

			/* Swap the register stack positions */
			unroll->pseudoStack[0] = *reg1;
			unroll->pseudoStack[1] = *reg2;

			/* Load the top of the CVM stack into reg1 */
			unroll->stackHeight -= 4;
			x86_mov_reg_membase(unroll->out, *reg1, REG_STACK,
								unroll->stackHeight, 4);
			return;
		}
	}

	/* We may have an FPU register in play, so flush it */
	FlushRegisterStack(unroll);

	/* Load the top of the CVM stack into EAX and ECX */
	unroll->regsUsed |= REG_EAX_MASK | REG_ECX_MASK;
	unroll->pseudoStack[0] = X86_EAX;
	unroll->pseudoStack[1] = X86_ECX;
	unroll->pseudoStackSize = 2;
	unroll->stackHeight -= 8;
	x86_mov_reg_membase(unroll->out, X86_EAX, REG_STACK,
						unroll->stackHeight, 4);
	x86_mov_reg_membase(unroll->out, X86_ECX, REG_STACK,
						unroll->stackHeight + 4, 4);
	*reg1 = X86_EAX;
	*reg2 = X86_ECX;
}

/*
 * Get the top-most stack value into ST(0).
 */
static void GetTopFPRegister(X86Unroll *unroll)
{
	int reg;

	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* Check for an existing FP register on the stack */
	if(unroll->pseudoStackSize > 0)
	{
		reg = unroll->pseudoStack[unroll->pseudoStackSize - 1];
		if(reg == REG_FPU)
		{
			return;
		}
	}

	/* Flush the register stack and then reload the top into ST(0) */
	FlushRegisterStack(unroll);
	unroll->pseudoStack[0] = REG_FPU;
	unroll->pseudoStackSize = 1;
	unroll->stackHeight -= 12;
	x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight);
	++(unroll->fpStackSize);
}

/*
 * Get the two top-most FP values on the stack into ST(1) and ST(0).
 * If "reversed" is non-zero, then reverse ST(1) and ST(0).
 */
static void GetTopTwoFPRegisters(X86Unroll *unroll, int reversed)
{
	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* See if we already have two FP registers in play */
	if(unroll->pseudoStackSize > 1)
	{
		if(unroll->pseudoStack[unroll->pseudoStackSize - 2] == REG_FPU &&
		   unroll->pseudoStack[unroll->pseudoStackSize - 1] == REG_FPU)
		{
			if(reversed)
			{
				x86_fxch(unroll->out, 1);
			}
			return;
		}
	}

	/* See if we have one FP register in play */
	if(unroll->pseudoStackSize == 1)
	{
		if(unroll->pseudoStack[0] == REG_FPU)
		{
			/* Load the top of the CVM stack into ST(0) */
			unroll->stackHeight -= 12;
			x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight);

			/* Put the values into the correct order */
			if(!reversed)
			{
				x86_fxch(unroll->out, 1);
			}

			/* We now have an extra FP value in the register stack */
			unroll->pseudoStack[1] = REG_FPU;
			unroll->pseudoStackSize = 2;
			++(unroll->fpStackSize);
			return;
		}
	}

	/* We may have word registers in play, so flush them */
	FlushRegisterStack(unroll);

	/* Load the top of the CVM stack into ST(1) and ST(0) */
	unroll->stackHeight -= 24;
	if(reversed)
	{
		x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight + 12);
		x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight);
	}
	else
	{
		x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight);
		x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight + 12);
	}
	unroll->pseudoStack[0] = REG_FPU;
	unroll->pseudoStack[1] = REG_FPU;
	unroll->pseudoStackSize = 2;
	unroll->fpStackSize += 2;
}

/*
 * Get the two top-most stack values in a word and an FP register.
 * Returns the identifier for the word register.  The FP register is ST(0).
 */
static int GetWordAndFPRegisters(X86Unroll *unroll)
{
	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* Check for the expected information on the stack */
	if(unroll->pseudoStackSize > 1 &&
	   unroll->pseudoStack[unroll->pseudoStackSize - 2] != REG_FPU &&
	   unroll->pseudoStack[unroll->pseudoStackSize - 1] == REG_FPU)
	{
		return unroll->pseudoStack[unroll->pseudoStackSize - 2];
	}

	/* If we have 1 FP value on the stack, then load the word into EAX */
	if(unroll->pseudoStackSize == 1 &&
	   unroll->pseudoStack[unroll->pseudoStackSize - 1] == REG_FPU)
	{
		unroll->pseudoStack[0] = X86_EAX;
		unroll->pseudoStack[1] = REG_FPU;
		unroll->pseudoStackSize = 2;
		unroll->stackHeight -= 4;
		x86_mov_reg_membase(unroll->out, X86_EAX, REG_STACK,
						    unroll->stackHeight, 4);
		unroll->regsUsed |= REG_EAX_MASK;
		return X86_EAX;
	}

	/* Flush the register stack and then reload into EAX and ST(0) */
	FlushRegisterStack(unroll);
	unroll->pseudoStack[0] = X86_EAX;
	unroll->pseudoStack[1] = REG_FPU;
	unroll->pseudoStackSize = 2;
	unroll->stackHeight -= 16;
	x86_mov_reg_membase(unroll->out, X86_EAX, REG_STACK,
					    unroll->stackHeight, 4);
	x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight + 4);
	unroll->regsUsed |= REG_EAX_MASK;
	++(unroll->fpStackSize);
	return X86_EAX;
}

/*
 * Get the two top-most stack values in an FP and a word register.
 * Returns the identifier for the word register.  The FP register is ST(0).
 */
static int GetFPAndWordRegisters(X86Unroll *unroll)
{
	int reg;

	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* Check for the expected information on the stack */
	if(unroll->pseudoStackSize > 1 &&
	   unroll->pseudoStack[unroll->pseudoStackSize - 2] == REG_FPU &&
	   unroll->pseudoStack[unroll->pseudoStackSize - 1] != REG_FPU)
	{
		return unroll->pseudoStack[unroll->pseudoStackSize - 1];
	}

	/* If we have 1 word value on the stack, then load the FP into ST(0) */
	if(unroll->pseudoStackSize == 1 &&
	   unroll->pseudoStack[unroll->pseudoStackSize - 1] != REG_FPU)
	{
		reg = unroll->pseudoStack[0];
		unroll->pseudoStack[0] = REG_FPU;
		unroll->pseudoStack[1] = reg;
		unroll->pseudoStackSize = 2;
		unroll->stackHeight -= 12;
		x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight);
		++(unroll->fpStackSize);
		return reg;
	}

	/* Flush the register stack and then reload into ST(0) and EAX */
	FlushRegisterStack(unroll);
	unroll->pseudoStack[0] = REG_FPU;
	unroll->pseudoStack[1] = X86_EAX;
	unroll->pseudoStackSize = 2;
	unroll->stackHeight -= 16;
	x86_fld80_membase(unroll->out, REG_STACK, unroll->stackHeight);
	x86_mov_reg_membase(unroll->out, X86_EAX, REG_STACK,
					    unroll->stackHeight + 12, 4);
	unroll->regsUsed |= REG_EAX_MASK;
	++(unroll->fpStackSize);
	return X86_EAX;
}

/*
 * Free the top-most register on the pseudo stack, and record the
 * local variable that it was just stored in (-1 if no local).
 */
static void FreeTopRegister(X86Unroll *unroll, long local)
{
	int reg = unroll->pseudoStack[--(unroll->pseudoStackSize)];
	unroll->cachedLocal = local;
	unroll->cachedReg = reg;
	if(reg != REG_FPU)
	{
		unroll->regsUsed &= ~(1 << reg);
	}
	else
	{
		--(unroll->fpStackSize);
	}
}

/*
 * Peek at the top-most word register on the stack.  Returns
 * -1 if the top-most stack value is not in a word register.
 */
static int PeekTopWordRegister(X86Unroll *unroll)
{
	if(unroll->pseudoStackSize > 0)
	{
		int reg = unroll->pseudoStack[unroll->pseudoStackSize - 1];
		if(reg != REG_FPU)
		{
			unroll->cachedLocal = -1;
			unroll->cachedReg = -1;
			return reg;
		}
	}
	return -1;
}

/*
 * Push a register onto the stack directly.
 */
static void PushRegister(X86Unroll *unroll, int reg)
{
	unroll->pseudoStack[(unroll->pseudoStackSize)++] = reg;
	if(reg == REG_FPU)
	{
		++(unroll->fpStackSize);
	}
	else
	{
		unroll->regsUsed |= (1 << reg);
	}
}

/*
 * Start an unrolled code section if necessary.
 */
#define	UNROLL_START()	\
			do { \
				if(!inUnrollBlock) \
				{ \
					overwritePC = pc; \
					unrollStart = unroll.out; \
					inUnrollBlock = 1; \
				} \
			} while (0)

/*
 * If an unrolled code section is in progress, then continue,
 * otherwise don't unroll the current instruction.  This is
 * typically used for branch instructions which should not be
 * unrolled at the start of a basic block.
 */
#define	UNROLL_BRANCH_START()	\
			do { \
				if(!inUnrollBlock) \
				{ \
					goto defaultCase; \
				} \
			} while (0)

/*
 * Flush the current unrolled code section.  Note: we assume
 * that the write to "overwritePC" will be more or less atomic
 * so that any other threads in the system that are executing
 * the method will instantly thread through to the new code
 * when execution returns to "overwritePC".
 */
#define	UNROLL_FLUSH()	\
			do { \
				*((void **)overwritePC) = unrollStart; \
				inUnrollBlock = 0; \
				unroll.cachedLocal = -1; \
				unroll.thisValidated = 0; \
			} while (0)

/*
 * Modify the "pc" variable to account for an instruction length.
 */
#define	MODIFY_UNROLL_PC(len)	\
			do { \
				pc += (len); \
			} while (0)

/*
 * Table that maps direct instruction pointers into opcodes.
 */
static unsigned short *ptrToOpcode;
static unsigned char *minPtrValue;
static unsigned char *maxPtrValue;

int _ILCVMUnrollInit(void)
{
	void *minPtr;
	void *maxPtr;
	void *tempPtr;
	int index;
	int size;

	/* Get the minimum and maximum label pointers from the CVM interpreter */
	minPtr = CVM_LABEL_FOR_OPCODE(0);
	maxPtr = minPtr;
	for(index = 1; index < 256; ++index)
	{
		tempPtr = CVM_LABEL_FOR_OPCODE(index);
		if(tempPtr < minPtr)
		{
			minPtr = tempPtr;
		}
		if(tempPtr > maxPtr)
		{
			maxPtr = tempPtr;
		}
	}
	for(index = 0; index < 256; ++index)
	{
		tempPtr = CVMP_LABEL_FOR_OPCODE(index);
		if(tempPtr < minPtr)
		{
			minPtr = tempPtr;
		}
		if(tempPtr > maxPtr)
		{
			maxPtr = tempPtr;
		}
	}

	/* Allocate space for the table and initialize to "nop" */
	size = (int)(((unsigned char *)maxPtr) - ((unsigned char *)minPtr) + 1);
	if((ptrToOpcode = (unsigned short *)ILMalloc(size * 2)) == 0)
	{
		return 0;
	}
	ILMemZero(ptrToOpcode, size * 2);

	/* Populate the mapping table */
	for(index = 0; index < 256; ++index)
	{
		tempPtr = CVM_LABEL_FOR_OPCODE(index);
		ptrToOpcode[((unsigned char *)tempPtr) - ((unsigned char *)minPtr)]
				= index;
	}
	for(index = 0; index < 256; ++index)
	{
		tempPtr = CVMP_LABEL_FOR_OPCODE(index);
		ptrToOpcode[((unsigned char *)tempPtr) - ((unsigned char *)minPtr)]
				= 0x100 + index;
	}
	minPtrValue = (unsigned char *)minPtr;
	maxPtrValue = (unsigned char *)maxPtr;

	/* Ready to go */
	return 1;
}

int _ILCVMUnrollPossible(void)
{
	return 1;
}

/*
 * Minimum buffer size.  If the amount of memory in the memory
 * cache falls below this value, then we stop the unroll process.
 * This value should be big enough to handle the largest block
 * of x86 code that may be output by an unroll case followed
 * by a full flush of the register stack.
 */
#define	UNROLL_BUFMIN		256

/*
 * Define this to enable debugging of translated code.
 */
/*#define UNROLL_DEBUG*/

#ifdef UNROLL_DEBUG

/*
 * Dump translated code to stdout.
 */
static void DumpCode(ILMethod *method, unsigned char *start, int len)
{
	char cmdline[BUFSIZ];
	FILE *file = fopen("/tmp/unroll.s", "w");
	if(!file)
	{
		return;
	}
	ILDumpMethodType(stdout, ILProgramItem_Image(method),
					 ILMethod_Signature(method), 0,
					 ILMethod_Owner(method),
					 ILMethod_Name(method), method);
	fputs(" ->\n", stdout);
	fflush(stdout);
	while(len > 0)
	{
		fprintf(file, ".byte %d\n", (int)(*start));
		++start;
		--len;
	}
	fclose(file);
	sprintf(cmdline, "as /tmp/unroll.s -o /tmp/unroll.o;objdump --adjust-vma=%ld -d /tmp/unroll.o", (long)start);
	system(cmdline);
	unlink("/tmp/unroll.s");
	unlink("/tmp/unroll.o");
	putc('\n', stdout);
	fflush(stdout);
}

#endif /* UNROLL_DEBUG */

/*
 * Include the global definitions needed by the cases.
 */
#define	IL_UNROLL_GLOBAL
#include "unroll_x86_arith.c"
#include "unroll_x86_branch.c"
#include "unroll_x86_const.c"
#include "unroll_x86_conv.c"
#include "unroll_x86_ptr.c"
#include "unroll_x86_var.c"
#undef	IL_UNROLL_GLOBAL

/* Imported from "cvm_lengths.c" */
extern unsigned char const _ILCVMLengths[512];

/* Imported from "cvmc.c" */
int _ILCVMStartUnrollBlock(ILCoder *_coder, int align, ILCachePosn *posn);

int _ILCVMUnrollMethod(ILCoder *coder, unsigned char *pc, ILMethod *method)
{
	X86Unroll unroll;
	int inUnrollBlock;
	unsigned char *inst;
	int opcode;
	unsigned char *overwritePC;
	unsigned char *unrollStart;
	int reg, reg2;
	ILCachePosn posn;

	/* Find some room in the cache */
	if(!_ILCVMStartUnrollBlock(coder, 32, &posn))
	{
		return 0;
	}
	if((posn.limit - posn.ptr) < UNROLL_BUFMIN)
	{
		/* Insufficient space to unroll the method */
		ILCacheEndMethod(&posn);
		return 0;
	}

	/* Initialize the local unroll state */
	unroll.out = posn.ptr;
	unroll.regsUsed = 0;
	unroll.regsSaved = 0;
	unroll.pseudoStackSize = 0;
	unroll.fpStackSize = 0;
	unroll.stackHeight = 0;
	unroll.cachedLocal = -1;
	unroll.cachedReg = -1;
	unroll.thisValidated = 0;
	inUnrollBlock = 0;
	overwritePC = 0;
	unrollStart = 0;

	/* Unroll the code */
	for(;;)
	{
		/* Fetch the next instruction pointer, and convert into an opcode */
		inst = *((void **)pc);
		if(inst >= minPtrValue && inst <= maxPtrValue)
		{
			opcode = (int)(ptrToOpcode[inst - minPtrValue]);
		}
		else
		{
			opcode = COP_NOP;
		}

		/* Bail out if we've reached the end of the method */
		if(opcode == COP_PREFIX)
		{
			/* Special direct instruction that marks the end of the method */
			break;
		}

		/* Bail out if we are low on space */
		if((posn.limit - unroll.out) < UNROLL_BUFMIN)
		{
			break;
		}

		/* Determine what to do based on the opcode */
		switch(opcode)
		{
			#define	IL_UNROLL_CASES
			#include "unroll_x86_arith.c"
			#include "unroll_x86_branch.c"
			#include "unroll_x86_const.c"
			#include "unroll_x86_conv.c"
			#include "unroll_x86_ptr.c"
			#include "unroll_x86_var.c"
			#undef	IL_UNROLL_CASES

			case COP_NOP:
			{
				/* The "nop" instruction is used to mark labels */
				if(inUnrollBlock)
				{
					/* Branch to just after the "nop", because there
					   is no point trying to execute it */
					BranchToPC(&unroll, pc + CVM_LEN_NONE);
					UNROLL_FLUSH();
				}
				pc += CVM_LEN_NONE;
			}
			break;

			case 0x100 + COP_PREFIX_UNROLL_METHOD:
			{
				/* This is usually the first instruction that is
				   replaced by unrolled code, so optimise it away */
				UNROLL_START();
				MODIFY_UNROLL_PC(CVMP_LEN_NONE);
			}
			break;

			default:
			{
			defaultCase:
				/* Everything else terminates an unroll block */
				if(inUnrollBlock)
				{
					BranchToPC(&unroll, pc);
					UNROLL_FLUSH();
				}

				/* Skip the instruction that could not be unrolled */
				if(opcode != COP_SWITCH)
				{
					pc += (int)_ILCVMLengths[opcode];
				}
				else
				{
					pc = CVM_ARG_SWITCH_DEFAULT;
				}
			}
			break;
		}
	}

	/* Flush the last block that was converted */
	if(inUnrollBlock)
	{
		BranchToPC(&unroll, pc);
		UNROLL_FLUSH();
	}

#ifdef UNROLL_DEBUG
	/* Dump the translated code */
	DumpCode(method, posn.ptr, (int)(unroll.out - posn.ptr));
#endif

	/* Update the method cache to reflect the final position */
	posn.ptr = unroll.out;
	ILCacheEndMethod(&posn);
	return 1;
}

#else /* !i386 */

/*
 * Stub out the unroll API on other platforms.
 */

int _ILCVMUnrollInit(void)
{
	return 1;
}

int _ILCVMUnrollPossible(void)
{
	return 0;
}

int _ILCVMUnrollMethod(ILCoder *coder, unsigned char *pc, ILMethod *method)
{
	return 0;
}

#endif /* !i386 */

#ifdef	__cplusplus
};
#endif
