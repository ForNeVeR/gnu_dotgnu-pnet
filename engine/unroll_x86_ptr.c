/*
 * unroll_x86_ptr.c - Pointer handling for x86 CVM unrolling.
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

#ifdef IL_UNROLL_GLOBAL

/*
 * Check the contents of a register for NULL and re-execute
 * the current instruction in the interpreter if it is.
 */
static void CheckForNull(X86Unroll *unroll, int reg, unsigned char *pc,
						 unsigned char *label, int popReg)
{
	unsigned char *patch;

	/* Check the register's contents against NULL */
	x86_alu_reg_reg(unroll->out, X86_OR, reg, reg);
	patch = unroll->out;
	x86_branch32(unroll->out, X86_CC_NE, 0, 0);

	/* Re-execute the current instruction in the interpreter */
	if(popReg)
	{
		--(unroll->pseudoStackSize);
		ReExecute(unroll, pc, label);
		++(unroll->pseudoStackSize);
	}
	else
	{
		ReExecute(unroll, pc, label);
	}

	/* Continue with real execution here */
	x86_patch(patch, unroll->out);
}

#elif defined(IL_UNROLL_CASES)

case COP_BREAD:
{
	/* Read a signed byte value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	x86_widen_membase(unroll.out, reg, reg, 0, 1, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_UBREAD:
{
	/* Read an unsigned byte value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	x86_widen_membase(unroll.out, reg, reg, 0, 0, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_SREAD:
{
	/* Read a signed short value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	x86_widen_membase(unroll.out, reg, reg, 0, 1, 1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_USREAD:
{
	/* Read an unsigned short value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	x86_widen_membase(unroll.out, reg, reg, 0, 0, 1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IREAD:
case COP_PREAD:
{
	/* Read a word value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	x86_mov_reg_membase(unroll.out, reg, reg, 0, 4);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FREAD:
{
	/* Read a float32 value from a pointer */
	UNROLL_START();
	if(unroll.fpStackSize >= 8)
	{
		/* The FP stack is full, so flush and restart */
		FlushRegisterStack(&unroll);
	}
	reg = GetTopWordRegister(&unroll);
	x86_fld_membase(unroll.out, reg, 0, 0);
	FreeTopRegister(&unroll, -1);
	PushRegister(&unroll, REG_FPU);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_DREAD:
{
	/* Read a float64 value from a pointer */
	UNROLL_START();
	if(unroll.fpStackSize >= 8)
	{
		/* The FP stack is full, so flush and restart */
		FlushRegisterStack(&unroll);
	}
	reg = GetTopWordRegister(&unroll);
	x86_fld_membase(unroll.out, reg, 0, 1);
	FreeTopRegister(&unroll, -1);
	PushRegister(&unroll, REG_FPU);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_BWRITE:
{
	/* Write a byte value to a pointer */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_mov_membase_reg(unroll.out, reg, 0, reg2, 1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_SWRITE:
{
	/* Write a short value to a pointer */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_mov_membase_reg(unroll.out, reg, 0, reg2, 2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IWRITE:
case COP_PWRITE:
{
	/* Write a word value to a pointer */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_mov_membase_reg(unroll.out, reg, 0, reg2, 4);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FWRITE:
{
	/* Write a float32 value to a pointer */
	UNROLL_START();
	reg = GetWordAndFPRegisters(&unroll);
	x86_fst_membase(unroll.out, reg, 0, 0, 1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_DWRITE:
{
	/* Write a float64 value to a pointer */
	UNROLL_START();
	reg = GetWordAndFPRegisters(&unroll);
	x86_fst_membase(unroll.out, reg, 0, 1, 1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_BWRITE_R:
{
	/* Write a byte value to a pointer with reversed arguments */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_mov_membase_reg(unroll.out, reg2, 0, reg, 1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_SWRITE_R:
{
	/* Write a short value to a pointer with reversed arguments */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_mov_membase_reg(unroll.out, reg2, 0, reg, 2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IWRITE_R:
case COP_PWRITE_R:
{
	/* Write a word value to a pointer with reversed arguments */
	UNROLL_START();
	x86_nop(unroll.out);
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_mov_membase_reg(unroll.out, reg2, 0, reg, 4);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FWRITE_R:
{
	/* Write a float32 value to a pointer with reversed arguments */
	UNROLL_START();
	reg = GetFPAndWordRegisters(&unroll);
	x86_fst_membase(unroll.out, reg, 0, 0, 1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_DWRITE_R:
{
	/* Write a float64 value to a pointer with reversed arguments */
	UNROLL_START();
	reg = GetFPAndWordRegisters(&unroll);
	x86_fst_membase(unroll.out, reg, 0, 1, 1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_PADD_OFFSET:
{
	/* Add an offset value to a pointer */
	unsigned value = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	x86_alu_reg_imm(unroll.out, X86_ADD, reg, value);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_PADD_I4:
case COP_PADD_I4_R:
{
	/* Add an integer value to a pointer */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_ADD, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_PSUB:
case COP_PSUB_I4:
{
	/* Subtract pointers, or subtract an integer from a pointer */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_SUB, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_BREAD_FIELD:
{
	/* Read a byte field from an object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_widen_membase(unroll.out, reg, reg, (int)temp, 1, 0);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_UBREAD_FIELD:
{
	/* Read an unsigned byte field from an object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_widen_membase(unroll.out, reg, reg, (int)temp, 0, 0);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_SREAD_FIELD:
{
	/* Read a short field from an object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_widen_membase(unroll.out, reg, reg, (int)temp, 1, 1);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_USREAD_FIELD:
{
	/* Read an unsigned byte field from an object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_widen_membase(unroll.out, reg, reg, (int)temp, 0, 1);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_IREAD_FIELD:
case COP_PREAD_FIELD:
{
	/* Read a word field from an object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_mov_reg_membase(unroll.out, reg, reg, (int)temp, 4);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_BWRITE_FIELD:
{
	/* Write a byte field to an object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_mov_membase_reg(unroll.out, reg, (int)temp, reg2, 1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_SWRITE_FIELD:
{
	/* Write a short field to an object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_mov_membase_reg(unroll.out, reg, (int)temp, reg2, 2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_IWRITE_FIELD:
case COP_PWRITE_FIELD:
{
	/* Write a word field to an object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_mov_membase_reg(unroll.out, reg, (int)temp, reg2, 4);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_IREAD_THIS:
case COP_PREAD_THIS:
{
	/* Read a word field from the "this" object */
	unsigned temp = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();

	/* Load "this" into a new register */
	reg = GetWordRegister(&unroll);
	x86_mov_reg_membase(unroll.out, reg, REG_FRAME, 0, 4);

	/* Check "this" for NULL if we haven't done so already.
	   If "thisValidated" is -1, then it indicates that the
	   "this" variable has been addressed using "waddr" and
	   so we must always validate it just in case someone
	   writes to that address further down the code */
	if(unroll.thisValidated <= 0)
	{
		CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 1);
		if(!(unroll.thisValidated))
		{
			unroll.thisValidated = 1;
		}
	}

	/* Read the contents of the field */
	x86_mov_reg_membase(unroll.out, reg, reg, (int)temp, 4);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

#endif /* IL_UNROLL_CASES */
