/*
 * unroll_x86_const.c - Constant handling for x86 CVM unrolling.
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

#ifdef IL_UNROLL_CASES

case COP_LDNULL:
case COP_LDC_I4_0:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_clear_reg(unroll.out, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_M1:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, -1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_1:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, 1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_2:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, 2);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_3:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, 3);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_4:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, 4);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_5:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, 5);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_6:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, 6);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_7:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, 7);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_8:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, 8);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_LDC_I4_S:
case COP_LDC_I4:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, (int)CVM_ARG_WORD);
	MODIFY_UNROLL_PC(CVM_LEN_WORD);
}
break;

case COP_LDC_R4:
{
	UNROLL_START();
	GetFPRegister(&unroll);
	x86_fld(unroll.out, (int)(pc + sizeof(void *)), 0);
	MODIFY_UNROLL_PC(CVM_LEN_FLOAT);
}
break;

case COP_LDC_R8:
{
	UNROLL_START();
	GetFPRegister(&unroll);
	x86_fld(unroll.out, (int)(pc + sizeof(void *)), 1);
	MODIFY_UNROLL_PC(CVM_LEN_DOUBLE);
}
break;

case COP_LDTOKEN:
{
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_mov_reg_imm(unroll.out, reg, (int)(CVM_ARG_PTR(void *)));
	MODIFY_UNROLL_PC(CVM_LEN_PTR);
}
break;

#endif /* IL_UNROLL_CASES */
