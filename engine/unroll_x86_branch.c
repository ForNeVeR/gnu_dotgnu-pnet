/*
 * unroll_x86_branch.c - Branch handling for x86 CVM unrolling.
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
 * Perform a conditional branch to one of two program locations.
 * It is assumed that "cond" refers to the inverse of the condition
 * that we are really testing.
 */
static void BranchOnCondition(X86Unroll *unroll,
							  int cond, int isSigned,
						      unsigned char *truePC,
							  unsigned char *falsePC)
{
	unsigned char *patch;

	/* Flush the registers and restore special values.  Because this only
	   uses "mov" and "pop" operations, it will not affect the flags */
	FlushRegisterStack(unroll);
	if((unroll->regsSaved & REG_EBP_MASK) != 0)
	{
		x86_pop_reg(unroll->out, X86_EBP);
	}
	unroll->regsSaved = 0;

	/* Test the condition in such a way that we branch if false */
	patch = unroll->out;
	x86_branch8(unroll->out, cond, 0, isSigned);

	/* Output the jump to the true PC */
	FixStackHeight(unroll);
	x86_mov_reg_imm(unroll->out, REG_PC, (int)truePC);
	x86_jump_membase(unroll->out, REG_PC, 0);

	/* Back-patch the branch instruction to point here */
	x86_patch(patch, unroll->out);

	/* Output the jump to the false PC */
	FixStackHeight(unroll);
	unroll->stackHeight = 0;
	x86_mov_reg_imm(unroll->out, REG_PC, (int)falsePC);
	x86_jump_membase(unroll->out, REG_PC, 0);
}

#elif defined(IL_UNROLL_CASES)

case COP_BR:
{
	/* Branch unconditionally to a destination */
	UNROLL_BRANCH_START();
	BranchToPC(&unroll, CVM_ARG_BRANCH_SHORT);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BEQ:
case COP_BR_PEQ:
{
	/* Branch if equal */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_NE, 0,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BNE:
case COP_BR_PNE:
{
	/* Branch if not equal */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_EQ, 0,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BLT:
{
	/* Branch if less than */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_GE, 1,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BLT_UN:
{
	/* Branch if unsigned less than */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_GE, 0,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BLE:
{
	/* Branch if less than or equal */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_GT, 1,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BLE_UN:
{
	/* Branch if unsigned less than or equal */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_GT, 0,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BGT:
{
	/* Branch if greater than */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_LE, 1,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BGT_UN:
{
	/* Branch if unsigned greater than */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_LE, 0,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BGE:
{
	/* Branch if greater than or equal */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_LT, 1,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BGE_UN:
{
	/* Branch if unsigned greater than or equal */
	UNROLL_BRANCH_START();
	GetTopTwoWordRegisters(&unroll, &reg, &reg2);
	x86_alu_reg_reg(unroll.out, X86_CMP, reg, reg2);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_LT, 0,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BRTRUE:
case COP_BRNONNULL:
{
	/* Branch if non-zero */
	UNROLL_BRANCH_START();
	reg = GetTopWordRegister(&unroll);
	x86_alu_reg_reg(unroll.out, X86_OR, reg, reg);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_EQ, 0,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_BRFALSE:
case COP_BRNULL:
{
	/* Branch if zero */
	UNROLL_BRANCH_START();
	reg = GetTopWordRegister(&unroll);
	x86_alu_reg_reg(unroll.out, X86_OR, reg, reg);
	FreeTopRegister(&unroll, -1);
	BranchOnCondition(&unroll, X86_CC_NE, 0,
					  CVM_ARG_BRANCH_SHORT, pc + CVM_LEN_BRANCH);
	MODIFY_UNROLL_PC(CVM_LEN_BRANCH);
	UNROLL_FLUSH();
}
break;

case COP_SWITCH:
{
	/* Switch statement */
	unsigned char *patch;
	UNROLL_BRANCH_START();
	reg = GetTopWordRegister(&unroll);
	x86_alu_reg_imm(unroll.out, X86_CMP, reg, CVM_ARG_SWITCH_LIMIT);
	FreeTopRegister(&unroll, -1);
	FlushRegisterStack(&unroll);
	patch = unroll.out;
	x86_branch8(unroll.out, X86_CC_GE, 0, 0);
	x86_mov_reg_memindex(unroll.out, REG_PC, X86_NOBASEREG,
						 (int)(pc + 12), reg, 2, 4);
	x86_jump_membase(unroll.out, REG_PC, 0);
	x86_patch(patch, unroll.out);
	x86_mov_reg_imm(unroll.out, REG_PC, (int)CVM_ARG_SWITCH_DEFAULT);
	x86_jump_membase(unroll.out, REG_PC, 0);
	pc = CVM_ARG_SWITCH_DEFAULT;
	UNROLL_FLUSH();
}
break;

#endif /* IL_UNROLL_CASES */
