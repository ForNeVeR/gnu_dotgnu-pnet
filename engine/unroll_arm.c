/*
 * unroll_arm.c - CVM unrolling module for ARM.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef IL_CVM_DIRECT_UNROLLED_ARM

#include "arm_codegen.h"
#include "il_dumpasm.h"

/*
 * Registers that have a special meaning while executing code.
 */
#define	REG_PC			ARM_R4
#define	REG_STACK		ARM_R5
#define	REG_FRAME		ARM_R6

/*
 * Unrolled code generation state.
 */
typedef struct
{
	arm_inst_ptr out;			/* Code output buffer */
	int		regsUsed;			/* Registers currently in use */
	int		regsSaved;			/* Fixed registers that were saved */
	int		pseudoStack[16];	/* Registers that make up the pseudo stack */
	int		pseudoStackSize;	/* Size of the pseudo stack */
	int		stackHeight;		/* Current virtual height of CVM stack */
	long	cachedLocal;		/* Local variable that was just stored */
	int		cachedReg;			/* Register for variable that was stored */
	int		thisValidated;		/* "this" has been checked for NULL */

} ARMUnroll;

/*
 * Flag bits for determining if certain registers are in use or not.
 */
#define	REG_R0_MASK			(1 << ARM_R0)
#define	REG_R1_MASK			(1 << ARM_R1)
#define	REG_R2_MASK			(1 << ARM_R2)
#define	REG_R3_MASK			(1 << ARM_R3)
#define	REG_R4_MASK			(1 << ARM_R4)
#define	REG_R5_MASK			(1 << ARM_R5)
#define	REG_R6_MASK			(1 << ARM_R6)
#define	REG_R7_MASK			(1 << ARM_R7)
#define	REG_R8_MASK			(1 << ARM_R8)
#define	REG_R9_MASK			(1 << ARM_R9)
#define	REG_R10_MASK		(1 << ARM_R10)
#define	REG_R11_MASK		(1 << ARM_R11)
#define	REG_R12_MASK		(1 << ARM_R12)
#define	REG_R13_MASK		(1 << ARM_R13)
#define	REG_R14_MASK		(1 << ARM_R14)
#define	REG_R15_MASK		(1 << ARM_R15)

/*
 * Register allocation order for the word registers.
 */
static char const regAllocOrder[] = {ARM_R0, ARM_R1, ARM_R2, ARM_R3,
									 ARM_R4, ARM_R7, ARM_R8, ARM_R9};
#define	regAllocOrderSize	(sizeof(regAllocOrder) / sizeof(char))

/*
 * Registers that must be saved on the system stack if we use them.
 */
#define	REG_NEED_SAVE_MASK	(REG_R7_MASK | REG_R8_MASK | REG_R9_MASK)

/*
 * Helper routine for the complex cases of "arm_mov_reg_imm".
 */
arm_inst_ptr _arm_mov_reg_imm(arm_inst_ptr inst, int reg, int value)
{
	/* Handle bytes in various positions */
	if((value & 0x000000FF) == value)
	{
		arm_mov_reg_imm8(inst, reg, value);
		return inst;
	}
	else if((value & 0x0000FF00) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, (value >> 8), 12);
		return inst;
	}
	else if((value & 0x00FF0000) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, (value >> 16), 8);
		return inst;
	}
	else if((value & 0xFF000000) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 24) & 0xFF), 4);
		return inst;
	}

	/* Handle inverted bytes in various positions */
	value = ~value;
	if((value & 0x000000FF) == value)
	{
		arm_mov_reg_imm8(inst, reg, value);
		arm_alu_reg(inst, ARM_MVN, reg, reg);
		return inst;
	}
	else if((value & 0x0000FF00) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, (value >> 8), 12);
		arm_alu_reg(inst, ARM_MVN, reg, reg);
		return inst;
	}
	else if((value & 0x00FF0000) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, (value >> 16), 8);
		arm_alu_reg(inst, ARM_MVN, reg, reg);
		return inst;
	}
	else if((value & 0xFF000000) == value)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 24) & 0xFF), 4);
		arm_alu_reg(inst, ARM_MVN, reg, reg);
		return inst;
	}

	/* Build the value the hard way, byte by byte */
	value = ~value;
	if((value & 0xFF000000) != 0)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 24) & 0xFF), 4);
		if((value & 0x00FF0000) != 0)
		{
			arm_alu_reg_imm8_rotate
				(inst, ARM_ADD, reg, reg, ((value >> 16) & 0xFF), 8);
		}
		if((value & 0x0000FF00) != 0)
		{
			arm_alu_reg_imm8_rotate
				(inst, ARM_ADD, reg, reg, ((value >> 8) & 0xFF), 12);
		}
		if((value & 0x000000FF) != 0)
		{
			arm_alu_reg_imm8(inst, ARM_ADD, reg, reg, (value & 0xFF));
		}
	}
	else if((value & 0x00FF0000) != 0)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 16) & 0xFF), 8);
		if((value & 0x0000FF00) != 0)
		{
			arm_alu_reg_imm8_rotate
				(inst, ARM_ADD, reg, reg, ((value >> 8) & 0xFF), 12);
		}
		if((value & 0x000000FF) != 0)
		{
			arm_alu_reg_imm8(inst, ARM_ADD, reg, reg, (value & 0xFF));
		}
	}
	else if((value & 0x0000FF00) != 0)
	{
		arm_mov_reg_imm8_rotate(inst, reg, ((value >> 8) & 0xFF), 12);
		if((value & 0x000000FF) != 0)
		{
			arm_alu_reg_imm8(inst, ARM_ADD, reg, reg, (value & 0xFF));
		}
	}
	else
	{
		arm_mov_reg_imm8(inst, reg, (value & 0xFF));
	}
	return inst;
}

/*
 * Helper routine for the complex cases of "arm_alu_reg_imm".
 */
arm_inst_ptr _arm_alu_reg_imm(arm_inst_ptr inst, int opc,
							  int dreg, int sreg, int imm,
							  int saveWork)
{
	int tempreg;
	if(saveWork)
	{
		if(dreg != ARM_R2 && sreg != ARM_R2)
		{
			tempreg = ARM_R2;
		}
		else if(dreg != ARM_R3 && sreg != ARM_R3)
		{
			tempreg = ARM_R3;
		}
		else
		{
			tempreg = ARM_R4;
		}
		arm_push_reg(inst, tempreg);
	}
	else
	{
		tempreg = ARM_WORK;
	}
	_arm_mov_reg_imm(inst, tempreg, imm);
	arm_alu_reg_reg(inst, opc, dreg, sreg, tempreg);
	if(saveWork)
	{
		arm_pop_reg(inst, tempreg);
	}
	return inst;
}

/*
 * Unload the machine state and jump to a particular PC.  We assume that
 * "stacktop" has already been unloaded by a call to "FixStackHeight".
 */
static void UnloadMachineState(ARMUnroll *unroll, unsigned char *pc,
							   unsigned char *label)
{
	int offset;
	if(!label)
	{
		/* Jump to the contents of the specified PC */
		arm_load_membase(unroll->out, REG_PC, ARM_PC, 0);
		arm_load_membase(unroll->out, ARM_PC, REG_PC, 0);
		*((unroll->out)++) = (unsigned int)pc;
	}
	else
	{
		/* Load "pc" back into the CVM interpreter's frame and
		   then jump directly to instruction handler at "label".
		   This avoids the need for an indirect jump instruction */
		arm_load_membase(unroll->out, REG_PC, ARM_PC, 0);
		offset = (int)(((unsigned char *)(label)) -
				       (((unsigned char *)(unroll->out)) + 8));
		if(offset >= -0x04000000 && offset < 0x04000000)
		{
			arm_jump_imm(unroll->out, offset);
			*((unroll->out)++) = (unsigned int)pc;
		}
		else
		{
			arm_load_membase(unroll->out, ARM_PC, ARM_PC, 0);
			*((unroll->out)++) = (unsigned int)pc;
			*((unroll->out)++) = (unsigned int)label;
		}
	}
}

/*
 * Flush all registers to the CVM operand stack.
 */
static void FlushRegisterStack(ARMUnroll *unroll)
{
	int index, reg;

	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* Flush the word registers from bottom-most to top-most */
	for(index = 0; index < unroll->pseudoStackSize; ++index)
	{
		reg = unroll->pseudoStack[index];
		unroll->regsUsed &= ~(1 << reg);
		arm_store_membase(unroll->out, reg, REG_STACK, unroll->stackHeight);
		unroll->stackHeight += 4;
	}

	/* Reset the pseudo stack size */
	unroll->pseudoStackSize = 0;
}

/*
 * Fix the height of the CVM stack to match the cached location.
 */
static void FixStackHeight(ARMUnroll *unroll)
{
	if(unroll->stackHeight > 0)
	{
		arm_alu_reg_imm(unroll->out, ARM_ADD, REG_STACK,
						REG_STACK, unroll->stackHeight);
	}
	else if(unroll->stackHeight < 0)
	{
		arm_alu_reg_imm(unroll->out, ARM_SUB, REG_STACK,
						REG_STACK, -(unroll->stackHeight));
	}
}

/*
 * Perform an unconditional branch to a new location.  This is
 * also used when control falls out through the end of the block.
 * In that case, "pc" is the start of the following block.
 */
static void BranchToPC(ARMUnroll *unroll, unsigned char *pc)
{
	int reg;

	/* Flush the register stack to the CVM stack */
	FlushRegisterStack(unroll);

	/* Update the REG_STACK register if necessary */
	FixStackHeight(unroll);
	unroll->stackHeight = 0;

	/* Restore the special registers that we used */
	for(reg = 15; reg >= 0; --reg)
	{
		if((unroll->regsSaved & (1 <<reg)) != 0)
		{
			arm_pop_reg(unroll->out, reg);
		}
	}
	unroll->regsSaved = 0;

	/* Unload the machine state */
	UnloadMachineState(unroll, pc, 0);
}

/*
 * Re-execute the current instruction in the CVM interpreter,
 * to process exception conditions.
 */
static void ReExecute(ARMUnroll *unroll, unsigned char *pc,
					  unsigned char *label)
{
	int index, reg, height;

	/* Flush the register stack, but don't change it as we will
	   still need it further down the code */
	height = unroll->stackHeight;
	for(index = 0; index < unroll->pseudoStackSize; ++index)
	{
		reg = unroll->pseudoStack[index];
		arm_store_membase(unroll->out, reg, REG_STACK, height);
		height += 4;
	}

	/* Fix up the stack height */
	if(height > 0)
	{
		arm_alu_reg_imm(unroll->out, ARM_ADD, REG_STACK,
					    REG_STACK, height);
	}
	else if(height < 0)
	{
		arm_alu_reg_imm(unroll->out, ARM_SUB, REG_STACK,
						REG_STACK, -height);
	}

	/* Restore the saved special registers */
	for(reg = 15; reg >= 0; --reg)
	{
		if((unroll->regsSaved & (1 <<reg)) != 0)
		{
			arm_pop_reg(unroll->out, reg);
		}
	}

	/* Unload the machine state and jump to the specified label */
	UnloadMachineState(unroll, pc, label);
}

/*
 * Get a register that can be used to store 32-bit word values.
 */
static int GetWordRegister(ARMUnroll *unroll)
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
				arm_push_reg(unroll->out, reg);
				unroll->regsSaved |= regmask;
			}
			return reg;
		}
	}

	/* Spill the bottom-most register to the CVM stack and reuse it */
	reg = unroll->pseudoStack[0];
	for(index = 1; index < unroll->pseudoStackSize; ++index)
	{
		unroll->pseudoStack[index - 1] = unroll->pseudoStack[index];
	}
	unroll->pseudoStack[unroll->pseudoStackSize - 1] = reg;
	arm_store_membase(unroll->out, reg, REG_STACK, unroll->stackHeight);
	unroll->stackHeight += 4;
	return reg;
}

/*
 * Get a 32-bit word register, but try to reuse the previously
 * popped register if the local variable offsets are identical.
 * This is used to optimise the common case of storing to a local
 * and then immediately reloading it.  Returns -1 if the register
 * was reused.
 */
static int GetCachedWordRegister(ARMUnroll *unroll, long local)
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
 * Get the top-most word value on the stack into a register.
 */
static int GetTopWordRegister(ARMUnroll *unroll)
{
	int reg;

	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* Check for an existing word register on the stack */
	if(unroll->pseudoStackSize > 0)
	{
		return unroll->pseudoStack[unroll->pseudoStackSize - 1];
	}

	/* Load the top of the CVM stack into a new register */
	reg = GetWordRegister(unroll);
	unroll->stackHeight -= 4;
	arm_load_membase(unroll->out, reg, REG_STACK, unroll->stackHeight);
	return reg;
}

/*
 * Get the two top-most word values on the stack into registers.
 * "reg1" will be the lower of the two.
 */
static void GetTopTwoWordRegisters(ARMUnroll *unroll, int *reg1, int *reg2)
{
	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* See if we already have two word registers in play */
	if(unroll->pseudoStackSize > 1)
	{
		*reg1 = unroll->pseudoStack[unroll->pseudoStackSize - 2];
		*reg2 = unroll->pseudoStack[unroll->pseudoStackSize - 1];
		return;
	}

	/* See if we have one word register in play */
	if(unroll->pseudoStackSize == 1)
	{
		/* Get the current top of stack into "reg2" */
		*reg2 = unroll->pseudoStack[0];

		/* Allocate another word register */
		*reg1 = GetWordRegister(unroll);

		/* Swap the register stack positions */
		unroll->pseudoStack[0] = *reg1;
		unroll->pseudoStack[1] = *reg2;

		/* Load the top of the CVM stack into reg1 */
		unroll->stackHeight -= 4;
		arm_load_membase(unroll->out, *reg1, REG_STACK, unroll->stackHeight);
		return;
	}

	/* Load the top of the CVM stack into two new registers */
	*reg1 = GetWordRegister(unroll);
	*reg2 = GetWordRegister(unroll);
	unroll->stackHeight -= 8;
	arm_load_membase(unroll->out, *reg1, REG_STACK, unroll->stackHeight);
	arm_load_membase(unroll->out, *reg2, REG_STACK, unroll->stackHeight + 4);
}

/*
 * Get the three top-most word values on the stack into registers.
 * "reg1" will be the lowest of the three.
 */
static void GetTopThreeWordRegisters(ARMUnroll *unroll,
									 int *reg1, int *reg2, int *reg3)
{
	/* Clear the cached local information */
	unroll->cachedLocal = -1;
	unroll->cachedReg = -1;

	/* See if we already have three word registers in play */
	if(unroll->pseudoStackSize > 2)
	{
		*reg1 = unroll->pseudoStack[unroll->pseudoStackSize - 3];
		*reg2 = unroll->pseudoStack[unroll->pseudoStackSize - 2];
		*reg3 = unroll->pseudoStack[unroll->pseudoStackSize - 1];
		return;
	}

	/* See if we have two word registers in play */
	if(unroll->pseudoStackSize == 2)
	{
		/* Get the two values that we already have loaded */
		*reg2 = unroll->pseudoStack[0];
		*reg3 = unroll->pseudoStack[1];

		/* Allocate another word register */
		*reg1 = GetWordRegister(unroll);

		/* Swap the register stack positions */
		unroll->pseudoStack[0] = *reg1;
		unroll->pseudoStack[1] = *reg2;
		unroll->pseudoStack[2] = *reg3;

		/* Load the top of the CVM stack into reg1 */
		unroll->stackHeight -= 4;
		arm_load_membase(unroll->out, *reg1, REG_STACK, unroll->stackHeight);
		return;
	}

	/* See if we have one word register in play */
	if(unroll->pseudoStackSize == 1)
	{
		/* Get the value that we already have loaded */
		*reg3 = unroll->pseudoStack[0];

		/* Allocate two other word registers */
		*reg1 = GetWordRegister(unroll);
		*reg2 = GetWordRegister(unroll);

		/* Swap the register stack positions */
		unroll->pseudoStack[0] = *reg1;
		unroll->pseudoStack[1] = *reg2;
		unroll->pseudoStack[2] = *reg3;

		/* Load the top of the CVM stack into reg1 and reg2 */
		unroll->stackHeight -= 8;
		arm_load_membase(unroll->out, *reg1, REG_STACK, unroll->stackHeight);
		arm_load_membase(unroll->out, *reg2, REG_STACK,
						 unroll->stackHeight + 4);
		return;
	}

	/* Load the top of the CVM stack into three new registers */
	*reg1 = GetWordRegister(unroll);
	*reg2 = GetWordRegister(unroll);
	*reg3 = GetWordRegister(unroll);
	unroll->stackHeight -= 12;
	arm_load_membase(unroll->out, *reg1, REG_STACK, unroll->stackHeight);
	arm_load_membase(unroll->out, *reg2, REG_STACK, unroll->stackHeight + 4);
	arm_load_membase(unroll->out, *reg3, REG_STACK, unroll->stackHeight + 8);
}

/*
 * Free the top-most register on the pseudo stack, and record the
 * local variable that it was just stored in (-1 if no local).
 */
static void FreeTopRegister(ARMUnroll *unroll, long local)
{
	int reg = unroll->pseudoStack[--(unroll->pseudoStackSize)];
	unroll->cachedLocal = local;
	unroll->cachedReg = reg;
	unroll->regsUsed &= ~(1 << reg);
}

/*
 * Peek at the top-most word register on the stack.  Returns
 * -1 if the top-most stack value is not in a word register.
 */
static int PeekTopWordRegister(ARMUnroll *unroll)
{
	if(unroll->pseudoStackSize > 0)
	{
		int reg = unroll->pseudoStack[unroll->pseudoStackSize - 1];
		unroll->cachedLocal = -1;
		unroll->cachedReg = -1;
		return reg;
	}
	return -1;
}

/*
 * Start an unrolled code section if necessary.
 */
#define	UNROLL_START()	\
			do { \
				if(!inUnrollBlock) \
				{ \
					overwritePC = pc; \
					unrollStart = (unsigned char *)(unroll.out); \
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
	/* The code generator is designed for little-endian ARM configurations,
	   so bail out early if the machine is running as big-endian instead */
	union 
	{
		unsigned char buf[2];
		short value;
	} test;
	test.value = 0x0102;
	return (test.buf[0] == 0x02);
}

/*
 * Minimum buffer size.  If the amount of memory in the memory
 * cache falls below this value, then we stop the unroll process.
 * This value should be big enough to handle the largest block
 * of ARM code that may be output by an unroll case followed
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
#include "unroll_arm_arith.c"
#include "unroll_arm_branch.c"
#include "unroll_arm_const.c"
#include "unroll_arm_conv.c"
#include "unroll_arm_ptr.c"
#include "unroll_arm_var.c"
#undef	IL_UNROLL_GLOBAL

/* Imported from "cvm_lengths.c" */
extern unsigned char const _ILCVMLengths[512];

/* Imported from "cvmc.c" */
int _ILCVMStartUnrollBlock(ILCoder *_coder, int align, ILCachePosn *posn);

int _ILCVMUnrollMethod(ILCoder *coder, unsigned char *pc, ILMethod *method)
{
	ARMUnroll unroll;
	int inUnrollBlock;
	unsigned char *inst;
	int opcode;
	unsigned char *overwritePC;
	unsigned char *unrollStart;
	int reg, reg2, reg3;
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
	unroll.out = (int *)posn.ptr;
	unroll.regsUsed = 0;
	unroll.regsSaved = 0;
	unroll.pseudoStackSize = 0;
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
		if((posn.limit - ((unsigned char *)(unroll.out))) < UNROLL_BUFMIN)
		{
			break;
		}

		/* Determine what to do based on the opcode */
		switch(opcode)
		{
			#define	IL_UNROLL_CASES
			#include "unroll_arm_arith.c"
			#include "unroll_arm_branch.c"
			#include "unroll_arm_const.c"
			#include "unroll_arm_conv.c"
			#include "unroll_arm_ptr.c"
			#include "unroll_arm_var.c"
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
	DumpCode(method, posn.ptr,
		     (int)(((unsigned char *)(unroll.out)) - posn.ptr));
#endif

	/* Update the method cache to reflect the final position */
	ILCacheFlush(posn.ptr, (long)(((unsigned char *)(unroll.out)) - posn.ptr));
	posn.ptr = (unsigned char *)(unroll.out);
	ILCacheEndMethod(&posn);
	return 1;
}

#endif /* !IL_CVM_DIRECT_UNROLLED_ARM */

#ifdef	__cplusplus
};
#endif
