/*
 * unroll_arm_conv.c - Conversion handling for ARM CVM unrolling.
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

case COP_I2B:
{
	/* Convert an integer into a byte */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_shift_reg_imm8(unroll.out, ARM_SHL, reg, reg, 24);
	arm_shift_reg_imm8(unroll.out, ARM_SAR, reg, reg, 24);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2UB:
{
	/* Convert an integer into an unsigned byte */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_alu_reg_imm8(unroll.out, ARM_AND, reg, reg, 0xFF);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2S:
{
	/* Convert an integer into a short */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_shift_reg_imm8(unroll.out, ARM_SHL, reg, reg, 16);
	arm_shift_reg_imm8(unroll.out, ARM_SAR, reg, reg, 16);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2US:
{
	/* Convert an integer into an unsigned short */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_shift_reg_imm8(unroll.out, ARM_SHL, reg, reg, 16);
	arm_shift_reg_imm8(unroll.out, ARM_SHR, reg, reg, 16);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#endif /* IL_UNROLL_CASES */
