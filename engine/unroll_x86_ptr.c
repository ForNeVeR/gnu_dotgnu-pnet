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

/*
 * Check an array access operation for exception conditions.
 */
static void CheckArrayAccess(X86Unroll *unroll, int reg, int reg2,
							 unsigned char *pc, unsigned char *label)
{
	unsigned char *patch1;
	unsigned char *patch2;

	/* Check the array reference against NULL */
	x86_alu_reg_reg(unroll->out, X86_OR, reg, reg);
	patch1 = unroll->out;
	x86_branch8(unroll->out, X86_CC_EQ, 0, 0);

	/* Check the array bounds */
	x86_alu_reg_membase(unroll->out, X86_CMP, reg2, reg, 0);
	patch2 = unroll->out;
	x86_branch32(unroll->out, X86_CC_LT, 0, 0);

	/* Re-execute the current instruction in the interpreter */
	x86_patch(patch1, unroll->out);
	ReExecute(unroll, pc, label);

	/* Continue with real execution here */
	x86_patch(patch2, unroll->out);
}

/*
 * Check a 2D array access operation for exception conditions.
 */
static void Check2DArrayAccess(X86Unroll *unroll, int reg, int reg2, int reg3,
							   unsigned char *pc, unsigned char *label)
{
	unsigned char *patch1;
	unsigned char *patch2;
	unsigned char *patch3;

	/* Check the array reference against NULL */
	x86_alu_reg_reg(unroll->out, X86_OR, reg, reg);
	patch1 = unroll->out;
	x86_branch8(unroll->out, X86_CC_EQ, 0, 0);

	/* Check the array bounds */
	x86_alu_reg_membase(unroll->out, X86_SUB, reg2, reg, 12);
	x86_alu_reg_membase(unroll->out, X86_CMP, reg2, reg, 16);
	patch2 = unroll->out;
	x86_branch32(unroll->out, X86_CC_LT, 0, 0);
	x86_alu_reg_membase(unroll->out, X86_ADD, reg2, reg, 12);
	patch3 = unroll->out;
	x86_jump8(unroll->out, 0);
	x86_patch(patch2, unroll->out);
	x86_alu_reg_membase(unroll->out, X86_SUB, reg3, reg, 24);
	x86_alu_reg_membase(unroll->out, X86_CMP, reg3, reg, 28);
	patch2 = unroll->out;
	x86_branch32(unroll->out, X86_CC_LT, 0, 0);
	x86_alu_reg_membase(unroll->out, X86_ADD, reg2, reg, 12);
	x86_alu_reg_membase(unroll->out, X86_ADD, reg3, reg, 28);

	/* Re-execute the current instruction in the interpreter */
	x86_patch(patch1, unroll->out);
	x86_patch(patch3, unroll->out);
	ReExecute(unroll, pc, label);

	/* Compute the address of the array element */
	x86_patch(patch2, unroll->out);
	x86_imul_reg_membase(unroll->out, reg2, reg, 20);
	x86_imul_reg_membase(unroll->out, reg3, reg, 32);
	x86_alu_reg_reg(unroll->out, X86_ADD, reg2, reg3);
	x86_imul_reg_membase(unroll->out, reg2, reg, 4);
	x86_mov_reg_membase(unroll->out, reg, reg, 8, 4);
	x86_alu_reg_reg(unroll->out, X86_ADD, reg, reg2);
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
	MovMembaseRegByte(&unroll, reg, 0, reg2);
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
	MovMembaseRegByte(&unroll, reg2, 0, reg);
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

case COP_BREAD_ELEM:
{
	/* Read a byte from an array */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	CheckArrayAccess(&unroll, reg, reg2, pc, (unsigned char *)inst);
	x86_widen_memindex(unroll.out, reg, reg, 4, reg2, 0, 1, 0);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_UBREAD_ELEM:
{
	/* Read an unsigned byte from an array */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	CheckArrayAccess(&unroll, reg, reg2, pc, (unsigned char *)inst);
	x86_widen_memindex(unroll.out, reg, reg, 4, reg2, 0, 0, 0);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_SREAD_ELEM:
{
	/* Read a short from an array */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	CheckArrayAccess(&unroll, reg, reg2, pc, (unsigned char *)inst);
	x86_widen_memindex(unroll.out, reg, reg, 4, reg2, 1, 1, 1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_USREAD_ELEM:
{
	/* Read an unsigned short from an array */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	CheckArrayAccess(&unroll, reg, reg2, pc, (unsigned char *)inst);
	x86_widen_memindex(unroll.out, reg, reg, 4, reg2, 1, 0, 1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IREAD_ELEM:
case COP_PREAD_ELEM:
{
	/* Read a word from an array */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	CheckArrayAccess(&unroll, reg, reg2, pc, (unsigned char *)inst);
	x86_mov_reg_memindex(unroll.out, reg, reg, 4, reg2, 2, 4);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_BWRITE_ELEM:
{
	/* Write a byte to an array */
	UNROLL_START();
	GetTopThreeWordRegisters(&unroll, &reg, &reg2, &reg3);
	CheckArrayAccess(&unroll, reg, reg2, pc, (unsigned char *)inst);
	MovMemindexRegByte(&unroll, reg, 4, reg2, 0, reg3);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_SWRITE_ELEM:
{
	/* Write a short to an array */
	UNROLL_START();
	GetTopThreeWordRegisters(&unroll, &reg, &reg2, &reg3);
	CheckArrayAccess(&unroll, reg, reg2, pc, (unsigned char *)inst);
	x86_mov_memindex_reg(unroll.out, reg, 4, reg2, 1, reg3, 2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IWRITE_ELEM:
case COP_PWRITE_ELEM:
{
	/* Write a word to an array */
	UNROLL_START();
	GetTopThreeWordRegisters(&unroll, &reg, &reg2, &reg3);
	CheckArrayAccess(&unroll, reg, reg2, pc, (unsigned char *)inst);
	x86_mov_memindex_reg(unroll.out, reg, 4, reg2, 2, reg3, 4);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ARRAY_LEN:
{
	/* Get the length of an array */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	CheckForNull(&unroll, reg, pc, (unsigned char *)inst, 0);
	x86_mov_reg_membase(unroll.out, reg, reg, 0, 4);
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
	MovMembaseRegByte(&unroll, reg, (int)temp, reg2);
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

case 0x100 + COP_PREFIX_GET2D:
{
	/* Get the array object reference and indices into registers */
	UNROLL_START();
	GetTopThreeWordRegisters(&unroll, &reg, &reg2, &reg3);

	/* Check the access and compute the element address */
	Check2DArrayAccess(&unroll, reg, reg2, reg3, pc, (unsigned char *)inst);

	/* Pop unnecessary registers: the address is now in "reg" */
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);

	/* Skip to the next instruction */
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SET2D:
{
	unsigned temp = (unsigned)CVMP_ARG_WORD;
	int offset;

	/* Flush the register stack onto the real stack so that we know
	   where everything is */
	UNROLL_START();
	FlushRegisterStack(&unroll);
	offset = unroll.stackHeight - ((int)((temp + 3) * 4));

	/* Fetch the array object reference and indices */
	reg = X86_EAX;
	reg2 = X86_ECX;
	reg3 = X86_EDX;
	x86_mov_reg_membase(unroll.out, reg, REG_STACK, offset, 4);
	x86_mov_reg_membase(unroll.out, reg2, REG_STACK, offset + 4, 4);
	x86_mov_reg_membase(unroll.out, reg3, REG_STACK, offset + 8, 4);

	/* Check the access and compute the element address */
	Check2DArrayAccess(&unroll, reg, reg2, reg3, pc, (unsigned char *)inst);

	/* Shift everything down on the stack to squash out the indices */
	x86_mov_membase_reg(unroll.out, REG_STACK, offset, reg, 4);
	while(temp > 0)
	{
		offset += 4;
		x86_mov_reg_membase(unroll.out, reg2, REG_STACK, offset + 8, 4);
		x86_mov_membase_reg(unroll.out, REG_STACK, offset, reg2, 4);
		--temp;
	}

	/* Adjust the top of stack pointer */
	unroll.stackHeight -= 8;

	/* Skip to the next instruction */
	MODIFY_UNROLL_PC(CVMP_LEN_WORD);
}
break;

#endif /* IL_UNROLL_CASES */
