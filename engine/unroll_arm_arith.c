/*
 * unroll_arm_arith.c - Arithmetic handling for ARM CVM unrolling.
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

#ifdef IL_UNROLL_CASES

/*
 * We don't unroll integer division because ARM doesn't have a special
 * purpose instruction for it and it is too hard to do long-hand.
 *
 * We also don't unroll floating-point arithmetic because some ARM
 * boards don't have an FPU or FPU emulation, and so the system must
 * use soft-float libraries instead.  It is too involved to figure out
 * what to output, so we let the interpreter worry about it instead.
 */

case COP_IADD:
{
	/* Add integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_alu_reg_reg(unroll.out, ARM_ADD, reg, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ISUB:
{
	/* Subtract integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_alu_reg_reg(unroll.out, ARM_SUB, reg, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IMUL:
{
	/* Multiply integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_mul_reg_reg(unroll.out, reg, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_INEG:
{
	/* Negate integer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_alu_reg_imm8(unroll.out, ARM_RSB, reg, reg, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IAND:
{
	/* Bitwise and integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_alu_reg_reg(unroll.out, ARM_AND, reg, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IOR:
{
	/* Bitwise or integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_alu_reg_reg(unroll.out, ARM_ORR, reg, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IXOR:
{
	/* Bitwise xor integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_alu_reg_reg(unroll.out, ARM_EOR, reg, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_INOT:
{
	/* Bitwise not integer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_alu_reg(unroll.out, ARM_MVN, reg, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ISHL:
case COP_ISHR:
case COP_ISHR_UN:
{
	/* Bitwise shift integers */
	int opc;
	UNROLL_START();
	if(opcode == COP_ISHL)
	{
		opc = ARM_SHL;
	}
	else if(opcode == COP_ISHR)
	{
		opc = ARM_SAR;
	}
	else
	{
		opc = ARM_SHR;
	}
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_alu_reg_imm(unroll.out, ARM_AND, reg2, reg2, 0x1F);
	arm_shift_reg_reg(unroll.out, opc, reg, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_ICMP:
{
	/* Compare integer values with -1, 0, or 1 result */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_test_reg_reg(unroll.out, ARM_CMP, reg, reg2);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 1, ARM_CC_GT);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 0, ARM_CC_LE);
	arm_alu_reg_cond(unroll.out, ARM_MVN, reg, reg, ARM_CC_LT);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_PCMP:
case 0x100 + COP_PREFIX_ICMP_UN:
{
	/* Compare unsigned word values with -1, 0, or 1 result */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	arm_test_reg_reg(unroll.out, ARM_CMP, reg, reg2);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 1, ARM_CC_GT_UN);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 0, ARM_CC_LE_UN);
	arm_alu_reg_cond(unroll.out, ARM_MVN, reg, reg, ARM_CC_LT_UN);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETEQ:
{
	/* Set if top of stack is equal to zero */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_test_reg_imm8(unroll.out, ARM_CMP, reg, 0);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 1, ARM_CC_EQ);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 0, ARM_CC_NE);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETNE:
{
	/* Set if top of stack is not equal to zero */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_test_reg_imm8(unroll.out, ARM_CMP, reg, 0);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 1, ARM_CC_NE);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 0, ARM_CC_EQ);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETLT:
{
	/* Set if top of stack is less than zero */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_test_reg_imm8(unroll.out, ARM_CMP, reg, 0);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 1, ARM_CC_LT);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 0, ARM_CC_GE);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETLE:
{
	/* Set if top of stack is less than or equal to zero */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_test_reg_imm8(unroll.out, ARM_CMP, reg, 0);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 1, ARM_CC_LE);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 0, ARM_CC_GT);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETGT:
{
	/* Set if top of stack is greater than zero */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_test_reg_imm8(unroll.out, ARM_CMP, reg, 0);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 1, ARM_CC_GT);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 0, ARM_CC_LE);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETGE:
{
	/* Set if top of stack is greater than or equal to zero */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_test_reg_imm8(unroll.out, ARM_CMP, reg, 0);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 1, ARM_CC_GE);
	arm_alu_reg_imm8_cond(unroll.out, ARM_MOV, reg, 0, 0, ARM_CC_LT);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

#endif /* IL_UNROLL_CASES */
