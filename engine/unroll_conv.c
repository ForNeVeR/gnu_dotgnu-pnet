/*
 * unroll_conv.c - Conversion handling for generic CVM unrolling.
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
	reg = GetTopWordRegister(&unroll, MD_REG1_32BIT);
	md_reg_to_sbyte(unroll.out, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2UB:
{
	/* Convert an integer into an unsigned byte */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll, MD_REG1_32BIT);
	md_reg_to_byte(unroll.out, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2S:
{
	/* Convert an integer into a short */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll, MD_REG1_32BIT);
	md_reg_to_short(unroll.out, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_I2US:
{
	/* Convert an integer into an unsigned short */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll, MD_REG1_32BIT);
	md_reg_to_ushort(unroll.out, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#ifdef MD_HAS_FP

#ifdef md_conv_sword_32_float

case COP_I2F:
{
	/* Read a float32 value from a pointer */
	UNROLL_START();
	CheckFPFull(&unroll);
	reg = GetTopWordRegister(&unroll, MD_REG1_32BIT);
	reg2 = GetFPRegister(&unroll);
	md_conv_sword_32_float(unroll.out, reg2, reg);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	PushRegister(&unroll, reg2, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#endif /* md_conv_sword_32_float */

#ifdef md_conv_uword_32_float

case COP_IU2F:
{
	/* Read a float32 value from a pointer */
	UNROLL_START();
	CheckFPFull(&unroll);
	reg = GetTopWordRegister(&unroll, MD_REG1_32BIT);
	reg2 = GetFPRegister(&unroll);
	md_conv_uword_32_float(unroll.out, reg2, reg);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	PushRegister(&unroll, reg2, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#endif /* md_conv_uword_32_float */

#ifdef md_conv_sword_64_float

/*
 * TODO: Handle 32 bit archs.
 */
case COP_L2F:
{
	/* Read a float32 value from a pointer */
	UNROLL_START();
	CheckFPFull(&unroll);
	reg = GetTopWordRegister(&unroll, MD_REG1_NATIVE);
	reg2 = GetFPRegister(&unroll);
	md_conv_sword_64_float(unroll.out, reg2, reg);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	PushRegister(&unroll, reg2, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#endif /* md_conv_sword_64_float */

#ifdef md_conv_uword_64_float

/*
 * TODO: Handle 32 bit archs.
 */
case COP_LU2F:
{
	/* Read a float32 value from a pointer */
	UNROLL_START();
	CheckFPFull(&unroll);
	reg = GetTopWordRegister(&unroll, MD_REG1_NATIVE);
	reg2 = GetFPRegister(&unroll);
	md_conv_uword_64_float(unroll.out, reg2, reg);
	FreeTopRegister(&unroll, -1);
	FreeTopRegister(&unroll, -1);
	PushRegister(&unroll, reg2, 0);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#endif /* md_conv_uword_64_float */

case COP_F2F:
{
	/* Truncate a floating point value to float32 */
	UNROLL_START();
	reg = GetTopFPRegister(&unroll);
	md_reg_to_float_32(unroll.out, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_F2D:
{
	/* Truncate a floating point value to float64 */
	UNROLL_START();
	reg = GetTopFPRegister(&unroll);
	md_reg_to_float_64(unroll.out, reg);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#endif /* MD_HAS_FP */

#endif /* IL_UNROLL_CASES */
