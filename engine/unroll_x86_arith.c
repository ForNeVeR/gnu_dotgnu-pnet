/*
 * unroll_x86_arith.c - Arithmetic handling for x86 CVM unrolling.
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
 * Perform an integer division or remainder.
 */
static void Divide(X86Unroll *unroll, int isSigned, int wantRemainder,
				   unsigned char *pc, unsigned char *label)
{
	unsigned char *patch1, *patch2, *patch3;

	/* Get the arguments into EAX and ECX so we know where they are */
	if(unroll->pseudoStackSize != 2 ||
	   unroll->pseudoStack[0] != X86_EAX ||
	   unroll->pseudoStack[1] != X86_ECX)
	{
		FlushRegisterStack(unroll);
		unroll->stackHeight -= 8;
		x86_mov_reg_membase(unroll->out, X86_EAX, REG_STACK,
							unroll->stackHeight, 4);
		x86_mov_reg_membase(unroll->out, X86_ECX, REG_STACK,
							unroll->stackHeight + 4, 4);
		unroll->pseudoStack[0] = X86_EAX;
		unroll->pseudoStack[1] = X86_ECX;
		unroll->pseudoStackSize = 2;
		unroll->regsUsed |= (REG_EAX_MASK | REG_ECX_MASK);
	}

	/* Check for conditions that may cause an exception */
	x86_alu_reg_imm(unroll->out, X86_CMP, X86_ECX, 0);
	patch1 = unroll->out;
	x86_branch8(unroll->out, X86_CC_EQ, 0, 0);
	x86_alu_reg_imm(unroll->out, X86_CMP, X86_ECX, -1);
	patch2 = unroll->out;
	x86_branch32(unroll->out, X86_CC_NE, 0, 0);
	x86_alu_reg_imm(unroll->out, X86_CMP, X86_EAX, (int)0x80000000);
	patch3 = unroll->out;
	x86_branch32(unroll->out, X86_CC_NE, 0, 0);
	x86_patch(patch1, unroll->out);

	/* Re-execute the division instruction to throw the exception */
	ReExecute(unroll, pc, label);

	/* Perform the division */
	x86_patch(patch2, unroll->out);
	x86_patch(patch3, unroll->out);
	if(isSigned)
	{
		x86_cdq(unroll->out);
	}
	else
	{
		x86_clear_reg(unroll->out, X86_EDX);
	}
	x86_div_reg(unroll->out, X86_ECX, isSigned);

	/* Pop ECX from the pseudo stack */
	FreeTopRegister(unroll, -1);

	/* If we want the remainder, then replace EAX with EDX on the stack */
	if(wantRemainder)
	{
		unroll->pseudoStack[0] = X86_EDX;
		unroll->regsUsed = REG_EDX_MASK;
	}
}

/*
 * Set a word register based on a condition code.
 */
static void SetCondRegister(X86Unroll *unroll, int cond)
{
	int reg = GetTopWordRegister(unroll);
	if(cond == X86_CC_EQ || cond == X86_CC_NE)
	{
		x86_alu_reg_reg(unroll->out, X86_OR, reg, reg);
	}
	else
	{
		x86_alu_reg_imm(unroll->out, X86_CMP, reg, 0);
	}
	if(reg == X86_EAX || reg == X86_ECX || reg == X86_EDX)
	{
		/* Use a SETcc instruction if we have a basic register */
		x86_set_reg(unroll->out, cond, reg, 1);
		x86_widen_reg(unroll->out, reg, reg, 0, 0);
	}
	else
	{
		/* The register is not useable as an 8-bit destination */
		unsigned char *patch1, *patch2;
		patch1 = unroll->out;
		x86_branch8(unroll->out, cond, 0, 1);
		x86_clear_reg(unroll->out, reg);
		patch2 = unroll->out;
		x86_jump8(unroll->out, 0);
		x86_patch(patch1, unroll->out);
		x86_mov_reg_imm(unroll->out, reg, 1);
		x86_patch(patch2, unroll->out);
	}
}

#elif defined(IL_UNROLL_CASES)

case COP_IADD:
{
	/* Add integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_ADD, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ISUB:
{
	/* Subtract integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_SUB, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IMUL:
{
	/* Multiply integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_imul_reg_reg(unroll.out, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IDIV:
{
	/* Divide integers */
	UNROLL_START();
	Divide(&unroll, 1, 0, pc, (unsigned char *)inst);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IDIV_UN:
{
	/* Divide unsigned integers */
	UNROLL_START();
	Divide(&unroll, 0, 0, pc, (unsigned char *)inst);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IREM:
{
	/* Remainder integers */
	UNROLL_START();
	Divide(&unroll, 1, 1, pc, (unsigned char *)inst);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IREM_UN:
{
	/* Remainder unsigned integers */
	UNROLL_START();
	Divide(&unroll, 0, 1, pc, (unsigned char *)inst);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_INEG:
{
	/* Negate integer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	x86_neg_reg(unroll.out, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FADD:
{
	/* Add floating point */
	UNROLL_START();
	GetTopTwoFPRegisters(&unroll, 0);
	x86_fp_op_reg(unroll.out, X86_FADD, 1, 1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FSUB:
{
	/* Subtract floating point */
	UNROLL_START();
	GetTopTwoFPRegisters(&unroll, 0);
	x86_fp_op_reg(unroll.out, X86_FSUB, 1, 1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FMUL:
{
	/* Multiply floating point */
	UNROLL_START();
	GetTopTwoFPRegisters(&unroll, 0);
	x86_fp_op_reg(unroll.out, X86_FMUL, 1, 1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FDIV:
{
	/* Divide floating point */
	UNROLL_START();
	GetTopTwoFPRegisters(&unroll, 0);
	x86_fp_op_reg(unroll.out, X86_FDIV, 1, 1);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FREM:
{
	/* Remainder floating point */
	unsigned char *label;
	UNROLL_START();
	GetTopTwoFPRegisters(&unroll, 1);
	if((unroll.regsUsed & REG_EAX_MASK) != 0)
	{
		x86_push_reg(unroll.out, X86_EAX);
	}
	label = unroll.out;
	x86_fprem1(unroll.out);
	x86_fnstsw(unroll.out);
	x86_alu_reg_imm(unroll.out, X86_AND, X86_EAX, 0x0400);
	x86_branch(unroll.out, X86_CC_NZ, label, 0);
	x86_fstp(unroll.out, 1);
	if((unroll.regsUsed & REG_EAX_MASK) != 0)
	{
		x86_pop_reg(unroll.out, X86_EAX);
	}
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_FNEG:
{
	/* Negate floating point */
	UNROLL_START();
	GetTopFPRegister(&unroll);
	x86_fchs(unroll.out);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IAND:
{
	/* Bitwise and integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_AND, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IOR:
{
	/* Bitwise or integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_OR, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_IXOR:
{
	/* Bitwise xor integers */
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_XOR, reg, reg2);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_INOT:
{
	/* Bitwise not integer */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	x86_not_reg(unroll.out, reg);
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
		opc = X86_SHL;
	}
	else if(opcode == COP_ISHR)
	{
		opc = X86_SAR;
	}
	else
	{
		opc = X86_SHR;
	}
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_imm(unroll.out, X86_AND, reg2, 0x1F);
	if(reg2 == X86_ECX)
	{
		/* The shift value is already in ECX */
		x86_shift_reg(unroll.out, opc, reg);
		FreeTopRegister(&unroll, -1);
	}
	else if(reg == X86_ECX)
	{
		/* The value to be shifted is in ECX, so swap the order */
		x86_xchg_reg_reg(unroll.out, reg, reg2, 4);
		x86_shift_reg(unroll.out, opc, reg2);
		FreeTopRegister(&unroll, -1);
		FreeTopRegister(&unroll, -1);
		PushRegister(&unroll, reg2);
	}
	else if((unroll.regsUsed & REG_ECX_MASK) == 0)
	{
		/* ECX is unused, so move the shift value into it */
		x86_mov_reg_reg(unroll.out, X86_ECX, reg2, 4);
		x86_shift_reg(unroll.out, opc, reg);
		FreeTopRegister(&unroll, -1);
	}
	else
	{
		/* Save ECX, perform the shift, and then restore ECX */
		x86_push_reg(unroll.out, X86_ECX);
		x86_mov_reg_reg(unroll.out, X86_ECX, reg2, 4);
		x86_shift_reg(unroll.out, opc, reg);
		x86_pop_reg(unroll.out, X86_ECX);
		FreeTopRegister(&unroll, -1);
	}
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_FCMPL:
case 0x100 + COP_PREFIX_FCMPG:
{
	/* Compare floating point values */
	unsigned char *patch1, *patch2, *patch3;
	UNROLL_START();
	GetTopTwoFPRegisters(&unroll, 0);

	/* We need the EAX register to store the FPU status word */
	if((unroll.regsUsed & REG_EAX_MASK) != 0)
	{
		x86_push_reg(unroll.out, X86_EAX);
	}

	/* Compare the values and get the FPU status word */
	x86_fcompp(unroll.out);
	x86_fnstsw(unroll.out);
	x86_alu_reg_imm(unroll.out, X86_AND, X86_EAX, 0x4500);

	/* Decode the FPU status word to determine the result */
	x86_alu_reg_imm(unroll.out, X86_CMP, X86_EAX, 0x4000);		/* eq */
	patch1 = unroll.out;
	x86_branch8(unroll.out, X86_CC_NE, 0, 0);
	x86_clear_reg(unroll.out, X86_EAX);
	patch2 = unroll.out;
	x86_jump8(unroll.out, 0);
	x86_patch(patch1, unroll.out);
	if(opcode == (0x100 + COP_PREFIX_FCMPL))
	{
		x86_alu_reg_imm(unroll.out, X86_CMP, X86_EAX, 0x0100);	/* gt */
		patch1 = unroll.out;
		x86_branch8(unroll.out, X86_CC_NE, 0, 0);
		x86_mov_reg_imm(unroll.out, X86_EAX, 1);
		patch3 = unroll.out;
		x86_jump8(unroll.out, 0);
		x86_patch(patch1, unroll.out);
		x86_mov_reg_imm(unroll.out, X86_EAX, -1);
	}
	else
	{
		x86_alu_reg_imm(unroll.out, X86_CMP, X86_EAX, 0x0000);	/* lt */
		patch1 = unroll.out;
		x86_branch8(unroll.out, X86_CC_NE, 0, 0);
		x86_mov_reg_imm(unroll.out, X86_EAX, -1);
		patch3 = unroll.out;
		x86_jump8(unroll.out, 0);
		x86_patch(patch1, unroll.out);
		x86_mov_reg_imm(unroll.out, X86_EAX, 1);
	}
	x86_patch(patch2, unroll.out);
	x86_patch(patch3, unroll.out);

	/* Rearrange the register stack to reflect the position of the result */
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	if((unroll.regsUsed & REG_EAX_MASK) != 0)
	{
		x86_xchg_membase_reg(unroll.out, X86_ESP, 0, X86_EAX, 4);
		if(unroll.regsSaved)
		{
			reg = GetWordRegister(&unroll);
			x86_pop_reg(unroll.out, reg);
		}
		else
		{
			/* We have to allow for EBP to be saved during "GetWordRegister" */
			reg = GetWordRegister(&unroll);
			if(unroll.regsSaved)
			{
				x86_pop_reg(unroll.out, reg);
				x86_xchg_membase_reg(unroll.out, X86_ESP, 0, reg, 4);
			}
			else
			{
				x86_pop_reg(unroll.out, reg);
			}
		}
	}
	else
	{
		PushRegister(&unroll, X86_EAX);
	}
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_ICMP:
{
	/* Compare integer values with -1, 0, or 1 result */
	unsigned char *patch1, *patch2, *patch3;
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	patch1 = unroll.out;
	x86_branch8(unroll.out, X86_CC_GE, 0, 1);
	x86_mov_reg_imm(unroll.out, reg, -1);
	patch2 = unroll.out;
	x86_jump8(unroll.out, 0);
	x86_patch(patch1, unroll.out);
	patch1 = unroll.out;
	x86_branch8(unroll.out, X86_CC_EQ, 0, 0);
	x86_mov_reg_imm(unroll.out, reg, 1);
	patch3 = unroll.out;
	x86_jump8(unroll.out, 0);
	x86_patch(patch1, unroll.out);
	x86_clear_reg(unroll.out, reg);
	x86_patch(patch2, unroll.out);
	x86_patch(patch3, unroll.out);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_PCMP:
case 0x100 + COP_PREFIX_ICMP_UN:
{
	/* Compare unsigned word values with -1, 0, or 1 result */
	unsigned char *patch1, *patch2, *patch3;
	UNROLL_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	patch1 = unroll.out;
	x86_branch8(unroll.out, X86_CC_GE, 0, 0);
	x86_mov_reg_imm(unroll.out, reg, -1);
	patch2 = unroll.out;
	x86_jump8(unroll.out, 0);
	x86_patch(patch1, unroll.out);
	patch1 = unroll.out;
	x86_branch8(unroll.out, X86_CC_EQ, 0, 0);
	x86_mov_reg_imm(unroll.out, reg, 1);
	patch3 = unroll.out;
	x86_jump8(unroll.out, 0);
	x86_patch(patch1, unroll.out);
	x86_clear_reg(unroll.out, reg);
	x86_patch(patch2, unroll.out);
	x86_patch(patch3, unroll.out);
	FreeTopRegister(&unroll, -1);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETEQ:
{
	/* Set if top of stack is equal to zero */
	UNROLL_START();
	SetCondRegister(&unroll, X86_CC_EQ);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETNE:
{
	/* Set if top of stack is not equal to zero */
	UNROLL_START();
	SetCondRegister(&unroll, X86_CC_NE);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETLT:
{
	/* Set if top of stack is less than zero */
	UNROLL_START();
	SetCondRegister(&unroll, X86_CC_LT);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETLE:
{
	/* Set if top of stack is less than or equal to zero */
	UNROLL_START();
	SetCondRegister(&unroll, X86_CC_LE);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETGT:
{
	/* Set if top of stack is greater than zero */
	UNROLL_START();
	SetCondRegister(&unroll, X86_CC_GT);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

case 0x100 + COP_PREFIX_SETGE:
{
	/* Set if top of stack is greater than or equal to zero */
	UNROLL_START();
	SetCondRegister(&unroll, X86_CC_GE);
	MODIFY_UNROLL_PC(CVMP_LEN_NONE);
}
break;

#endif /* IL_UNROLL_CASES */
