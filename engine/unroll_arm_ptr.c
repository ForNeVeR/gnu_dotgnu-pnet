/*
 * unroll_arm_ptr.c - Pointer handling for ARM CVM unrolling.
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

#ifdef IL_UNROLL_GLOBAL

/*
 * Check the contents of a register for NULL and re-execute
 * the current instruction in the interpreter if it is.
 */
static void CheckForNull(ARMUnroll *unroll, int reg, unsigned char *pc,
						 unsigned char *label, int popReg)
{
	arm_inst_ptr patch;

	/* Check the register's contents against NULL */
	arm_test_reg_imm8(unroll->out, ARM_CMP, reg, 0);
	patch = unroll->out;
	arm_branch_imm(unroll->out, ARM_CC_NE, 0);

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
	arm_patch(patch, unroll->out);
}

/*
 * Check an array access operation for exception conditions.
 * Upon successful execution, "reg" will point at the start
 * of the array contents.
 */
static void CheckArrayAccess(ARMUnroll *unroll, int reg, int reg2,
							 unsigned char *pc, unsigned char *label)
{
	arm_inst_ptr patch1;
	arm_inst_ptr patch2;

	/* Check the array reference against NULL */
	arm_test_reg_imm8(unroll->out, ARM_CMP, reg, 0);
	patch1 = unroll->out;
	arm_branch_imm(unroll->out, ARM_CC_EQ, 0);

	/* Check the array bounds */
	arm_load_advance(unroll->out, ARM_WORK, reg);
	arm_test_reg_reg(unroll->out, ARM_CMP, reg2, ARM_WORK);
	patch2 = unroll->out;
	arm_branch_imm(unroll->out, ARM_CC_LT_UN, 0);

	/* Re-execute the current instruction in the interpreter */
	arm_patch(patch1, unroll->out);
	ReExecute(unroll, pc, label);

	/* Continue with real execution here */
	arm_patch(patch2, unroll->out);
}

/*
 * Check a 2D array access operation for exception conditions.
 */
static void Check2DArrayAccess(ARMUnroll *unroll, int reg, int reg2, int reg3,
							   unsigned char *pc, unsigned char *label)
{
	arm_inst_ptr patch1;
	arm_inst_ptr patch2;
	arm_inst_ptr patch3;

	/* Check the array reference against NULL */
	arm_test_reg_imm8(unroll->out, ARM_CMP, reg, 0);
	patch1 = unroll->out;
	arm_branch_imm(unroll->out, ARM_CC_EQ, 0);

	/* Check the array bounds.  We assume that we can use the link
	   register as a work register because "lr" would have been
	   saved on entry to "_ILCVMInterpreter" */
	arm_load_membase(unroll->out, ARM_WORK, reg, 12);
	arm_alu_reg_reg(unroll->out, ARM_SUB, reg2, reg2, ARM_WORK);
	arm_load_membase(unroll->out, ARM_LINK, reg, 16);
	arm_test_reg_reg(unroll->out, ARM_CMP, reg2, ARM_LINK);
	patch2 = unroll->out;
	arm_branch_imm(unroll->out, ARM_CC_LT_UN, 0);
	arm_alu_reg_reg(unroll->out, ARM_ADD, reg2, reg2, ARM_WORK);
	patch3 = unroll->out;
	arm_jump_imm(unroll->out, 0);
	arm_patch(patch2, unroll->out);
	arm_load_membase(unroll->out, ARM_WORK, reg, 24);
	arm_alu_reg_reg(unroll->out, ARM_SUB, reg3, reg3, ARM_WORK);
	arm_load_membase(unroll->out, ARM_LINK, reg, 28);
	arm_test_reg_reg(unroll->out, ARM_CMP, reg3, ARM_LINK);
	patch2 = unroll->out;
	arm_branch_imm(unroll->out, ARM_CC_LT_UN, 0);
	arm_alu_reg_reg(unroll->out, ARM_ADD, reg3, reg3, ARM_WORK);
	arm_load_membase(unroll->out, ARM_WORK, reg, 12);
	arm_alu_reg_reg(unroll->out, ARM_ADD, reg2, reg2, ARM_WORK);

	/* Re-execute the current instruction in the interpreter */
	arm_patch(patch1, unroll->out);
	arm_patch(patch3, unroll->out);
	ReExecute(unroll, pc, label);

	/* Compute the address of the array element */
	arm_patch(patch2, unroll->out);
	arm_load_membase(unroll->out, ARM_WORK, reg, 20);
	arm_mul_reg_reg(unroll->out, reg2, reg2, ARM_WORK);
	arm_load_membase(unroll->out, ARM_WORK, reg, 32);
	arm_mul_reg_reg(unroll->out, reg3, reg3, ARM_WORK);
	arm_alu_reg_reg(unroll->out, ARM_ADD, reg2, reg2, reg3);
	arm_load_membase(unroll->out, ARM_WORK, reg, 4);
	arm_mul_reg_reg(unroll->out, reg2, reg2, ARM_WORK);
	arm_load_membase(unroll->out, reg, reg, 8);
	arm_alu_reg_reg(unroll->out, ARM_ADD, reg, reg, reg2);
}

#elif defined(IL_UNROLL_CASES)

case COP_BREAD:
{
	/* Read a signed byte value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_load_membase_sbyte(unroll.out, reg, reg, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_UBREAD:
{
	/* Read an unsigned byte value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_load_membase_byte(unroll.out, reg, reg, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_SREAD:
{
	/* Read a signed short value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_load_membase_short(unroll.out, reg, reg, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_USREAD:
{
	/* Read an unsigned short value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_load_membase_ushort(unroll.out, reg, reg, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IREAD:
case COP_PREAD:
{
	/* Read a word value from a pointer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_load_membase(unroll.out, reg, reg, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_BWRITE:
{
	/* Write a byte value to a pointer */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_store_membase_byte(unroll.out, reg2, reg, 0);
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
	arm_store_membase_short(unroll.out, reg2, reg, 0);
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
	arm_store_membase(unroll.out, reg2, reg, 0);
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
	arm_store_membase_byte(unroll.out, reg, reg2, 0);
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
	arm_store_membase_short(unroll.out, reg, reg2, 0);
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
	arm_store_membase(unroll.out, reg, reg2, 0);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_PADD_OFFSET:
{
	/* Add an offset value to a pointer */
	unsigned val = (unsigned)CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_alu_reg_imm(unroll.out, ARM_ADD, reg, reg, val);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_PADD_I4:
case COP_PADD_I4_R:
{
	/* Add an integer value to a pointer */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_alu_reg_reg(unroll.out, ARM_ADD, reg, reg, reg2);
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
	arm_alu_reg_reg(unroll.out, ARM_SUB, reg, reg, reg2);
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
	arm_load_memindex_sbyte(unroll.out, reg, reg, reg2);
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
	arm_load_memindex_byte(unroll.out, reg, reg, reg2);
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
	arm_load_memindex_short(unroll.out, reg, reg, reg2);
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
	arm_load_memindex_ushort(unroll.out, reg, reg, reg2);
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
	arm_load_memindex(unroll.out, reg, reg, reg2);
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
	arm_store_memindex_byte(unroll.out, reg3, reg, reg2);
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
	arm_store_memindex_short(unroll.out, reg3, reg, reg2);
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
	arm_store_memindex(unroll.out, reg3, reg, reg2);
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
	arm_load_membase(unroll.out, reg, reg, 0);
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
	arm_load_membase_sbyte(unroll.out, reg, reg, (int)temp);
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
	arm_load_membase_byte(unroll.out, reg, reg, (int)temp);
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
	arm_load_membase_short(unroll.out, reg, reg, (int)temp);
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
	arm_load_membase_ushort(unroll.out, reg, reg, (int)temp);
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
	arm_load_membase(unroll.out, reg, reg, (int)temp);
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
	arm_store_membase_byte(unroll.out, reg2, reg, (int)temp);
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
	arm_store_membase_short(unroll.out, reg2, reg, (int)temp);
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
	arm_store_membase(unroll.out, reg2, reg, (int)temp);
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
	arm_load_membase(unroll.out, reg, REG_FRAME, 0);

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
	arm_load_membase(unroll.out, reg, reg, (int)temp);
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
	reg = regAllocOrder[0];
	reg2 = regAllocOrder[1];
	reg3 = regAllocOrder[2];
	arm_load_membase(unroll.out, reg, REG_STACK, offset);
	arm_load_membase(unroll.out, reg2, REG_STACK, offset + 4);
	arm_load_membase(unroll.out, reg3, REG_STACK, offset + 8);

	/* Check the access and compute the element address */
	Check2DArrayAccess(&unroll, reg, reg2, reg3, pc, (unsigned char *)inst);

	/* Shift everything down on the stack to squash out the indices */
	arm_store_membase(unroll.out, reg, REG_STACK, offset);
	while(temp > 0)
	{
		offset += 4;
		arm_load_membase(unroll.out, reg2, REG_STACK, offset + 8);
		arm_store_membase(unroll.out, reg2, REG_STACK, offset);
		--temp;
	}

	/* Adjust the top of stack pointer */
	unroll.stackHeight -= 8;

	/* Skip to the next instruction */
	MODIFY_UNROLL_PC(CVMP_LEN_WORD);
}
break;

#endif /* IL_UNROLL_CASES */
