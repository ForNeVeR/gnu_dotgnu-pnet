/*
 * cvm_const.c - Opcodes for loading constants onto the stack.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

#if defined(IL_CVM_GLOBALS)

/* No globals required */

#elif defined(IL_CVM_LOCALS)

/* No locals required */

#elif defined(IL_CVM_MAIN)

case COP_LDNULL:
{
	/* Load the "null" pointer value to the stack top */
	stacktop[0].ptrValue = 0;
	MODIFY_PC_AND_STACK(1, 1);
}
break;

#define	COP_LDC_I4_VALUE(name,value)	\
case COP_LDC_I4_##name: \
{ \
	stacktop[0].intValue = (ILInt32)(value); \
	MODIFY_PC_AND_STACK(1, 1); \
} \
break \

/* Load simple integer constants onto the stack */
COP_LDC_I4_VALUE(M1, -1);
COP_LDC_I4_VALUE(0, 0);
COP_LDC_I4_VALUE(1, 1);
COP_LDC_I4_VALUE(2, 2);
COP_LDC_I4_VALUE(3, 3);
COP_LDC_I4_VALUE(4, 4);
COP_LDC_I4_VALUE(5, 5);
COP_LDC_I4_VALUE(6, 6);
COP_LDC_I4_VALUE(7, 7);
COP_LDC_I4_VALUE(8, 8);

case COP_LDC_I4_S:
{
	/* Load an 8-bit integer constant onto the stack */
	stacktop[0].intValue = (ILInt32)(ILInt8)(pc[1]);
	MODIFY_PC_AND_STACK(2, 1);
}
break;

case COP_LDC_I4:
{
	/* Load a 32-bit integer constant onto the stack */
	stacktop[0].intValue = IL_READ_INT32(pc + 1);
	MODIFY_PC_AND_STACK(5, 1);
}
break;

case COP_LDC_I8:
{
	/* Load a 64-bit integer constant onto the stack */
	WriteLong(&(stacktop[0]), IL_READ_INT64(pc + 1));
	MODIFY_PC_AND_STACK(9, CVM_WORDS_PER_LONG);
}
break;

case COP_LDC_R4:
{
	/* Load a 32-bit floating point value onto the stack */
	WriteFloat(&(stacktop[0]),
		(ILNativeFloat)IL_READ_FLOAT(pc + 1));
	MODIFY_PC_AND_STACK(5, CVM_WORDS_PER_NATIVE_FLOAT);
}
break;

case COP_LDC_R8:
{
	/* Load a 64-bit floating point value onto the stack */
	WriteFloat(&(stacktop[0]),
		(ILNativeFloat)IL_READ_DOUBLE(pc + 1));
	MODIFY_PC_AND_STACK(9, CVM_WORDS_PER_NATIVE_FLOAT);
}
break;

#endif /* IL_CVM_MAIN */
