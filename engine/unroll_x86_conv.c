/*
 * unroll_x86_conv.c - Conversion handling for x86 CVM unrolling.
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
 * Get a word register that is guaranteed to be one of the
 * basic byte registers (EAX, ECX, or EDX - EBX is reserved).
 */
static int GetBasicRegister(X86Unroll *unroll)
{
	int reg = GetWordRegister(unroll);
	if(reg == X86_EAX || reg == X86_ECX || reg == X86_EDX)
	{
		/* This register can be used as a basic register */
		return reg;
	}
	else
	{
		/* Flush the register stack and reload the value into EAX */
		FlushRegisterStack(unroll);
		return GetWordRegister(unroll);
	}
}

#elif defined(IL_UNROLL_CASES)

case COP_I2B:
{
	/* Convert an integer into a byte */
	UNROLL_START();
	reg = GetBasicRegister(&unroll);
	x86_widen_reg(unroll.out, reg, reg, 1, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2UB:
{
	/* Convert an integer into an unsigned byte */
	UNROLL_START();
	reg = GetBasicRegister(&unroll);
	x86_widen_reg(unroll.out, reg, reg, 0, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2S:
{
	/* Convert an integer into a short */
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_widen_reg(unroll.out, reg, reg, 1, 1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2US:
{
	/* Convert an integer into an unsigned short */
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	x86_widen_reg(unroll.out, reg, reg, 0, 1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_F2F:
{
	/* Truncate a floating point value to float32 */
	UNROLL_START();
	GetTopFPRegister(&unroll);
	x86_alu_reg_imm(unroll.out, X86_SUB, X86_ESP, 4);
	x86_fst_membase(unroll.out, X86_ESP, 0, 0, 1);
	x86_fld_membase(unroll.out, X86_ESP, 0, 0);
	x86_alu_reg_imm(unroll.out, X86_ADD, X86_ESP, 4);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_F2D:
{
	/* Truncate a floating point value to float64 */
	UNROLL_START();
	GetTopFPRegister(&unroll);
	x86_alu_reg_imm(unroll.out, X86_SUB, X86_ESP, 8);
	x86_fst_membase(unroll.out, X86_ESP, 0, 1, 1);
	x86_fld_membase(unroll.out, X86_ESP, 0, 1);
	x86_alu_reg_imm(unroll.out, X86_ADD, X86_ESP, 8);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#endif /* IL_UNROLL_CASES */
