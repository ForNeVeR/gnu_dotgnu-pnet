/*
 * unroll_arm_var.c - Variable handling for ARM CVM unrolling.
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

case COP_ILOAD_0:
case COP_PLOAD_0:
{
	/* Unroll an access to frame variable 0 */
	UNROLL_START();
	reg = GetCachedWordRegister(&unroll, 0);
	if(reg != -1)
	{
		arm_load_membase(unroll.out, reg, REG_FRAME, 0);
	}
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ILOAD_1:
case COP_PLOAD_1:
{
	/* Unroll an access to frame variable 1 */
	UNROLL_START();
	reg = GetCachedWordRegister(&unroll, 1);
	if(reg != -1)
	{
		arm_load_membase(unroll.out, reg, REG_FRAME, 4);
	}
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ILOAD_2:
case COP_PLOAD_2:
{
	/* Unroll an access to frame variable 2 */
	UNROLL_START();
	reg = GetCachedWordRegister(&unroll, 2);
	if(reg != -1)
	{
		arm_load_membase(unroll.out, reg, REG_FRAME, 8);
	}
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ILOAD_3:
case COP_PLOAD_3:
{
	/* Unroll an access to frame variable 3 */
	UNROLL_START();
	reg = GetCachedWordRegister(&unroll, 3);
	if(reg != -1)
	{
		arm_load_membase(unroll.out, reg, REG_FRAME, 12);
	}
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ILOAD:
case COP_PLOAD:
{
	/* Unroll an access to an arbitrary frame variable */
	unsigned temp = CVM_ARG_WIDE_SMALL;
	UNROLL_START();
	reg = GetCachedWordRegister(&unroll, temp);
	if(reg != -1)
	{
		arm_load_membase(unroll.out, reg, REG_FRAME, temp * 4);
	}
	MODIFY_UNROLL_PC(CVM_LEN_WIDE_SMALL);
}
break;

case COP_BLOAD:
{
	/* Unroll an access to a byte frame variable */
	unsigned temp = CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetCachedWordRegister(&unroll, temp);
	if(reg != -1)
	{
		arm_load_membase_byte(unroll.out, reg, REG_FRAME, temp * 4);
	}
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_ISTORE_0:
case COP_PSTORE_0:
{
	/* Unroll a store to frame variable 0 */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_store_membase(unroll.out, reg, REG_FRAME, 0);
	FreeTopRegister(&unroll, 0);
	if(unroll.thisValidated > 0)
	{
		/* The "this" variable must be re-validated */
		unroll.thisValidated = 0;
	}
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ISTORE_1:
case COP_PSTORE_1:
{
	/* Unroll a store to frame variable 1 */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_store_membase(unroll.out, reg, REG_FRAME, 4);
	FreeTopRegister(&unroll, 1);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ISTORE_2:
case COP_PSTORE_2:
{
	/* Unroll a store to frame variable 2 */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_store_membase(unroll.out, reg, REG_FRAME, 8);
	FreeTopRegister(&unroll, 2);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ISTORE_3:
case COP_PSTORE_3:
{
	/* Unroll a store to frame variable 3 */
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_store_membase(unroll.out, reg, REG_FRAME, 12);
	FreeTopRegister(&unroll, 3);
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_ISTORE:
case COP_PSTORE:
{
	/* Unroll a store to an arbitrary frame variable */
	unsigned temp = CVM_ARG_WIDE_SMALL;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_store_membase(unroll.out, reg, REG_FRAME, temp * 4);
	FreeTopRegister(&unroll, temp);
	if(temp == 0 && unroll.thisValidated > 0)
	{
		/* The "this" variable must be re-validated */
		unroll.thisValidated = 0;
	}
	MODIFY_UNROLL_PC(CVM_LEN_WIDE_SMALL);
}
break;

case COP_BSTORE:
{
	/* Unroll a store to a byte frame variable */
	unsigned temp = CVM_ARG_BYTE;
	UNROLL_START();
	reg = GetTopWordRegister(&unroll);
	arm_store_membase_byte(unroll.out, reg, REG_FRAME, temp * 4);
	FreeTopRegister(&unroll, temp);
	MODIFY_UNROLL_PC(CVM_LEN_BYTE);
}
break;

case COP_WADDR:
{
	/* Get the address of a frame word */
	unsigned temp = CVM_ARG_WIDE_SMALL;
	UNROLL_START();
	reg = GetWordRegister(&unroll);
	arm_mov_reg_reg(unroll.out, reg, REG_FRAME);
	if(temp != 0)
	{
		arm_alu_reg_imm(unroll.out, ARM_ADD, reg, reg, temp * 4);
	}
	else
	{
		/* We don't know if someone might write to this address,
		   so we have to assume that "this" will need to be
		   validated always from now on */
		unroll.thisValidated = -1;
	}
	MODIFY_UNROLL_PC(CVM_LEN_WIDE_SMALL);
}
break;

case COP_MK_LOCAL_1:
{
	/* Make a single local variable word */
	UNROLL_START();
	FlushRegisterStack(&unroll);
	arm_mov_reg_imm8(unroll.out, ARM_WORK, 0);
	arm_store_membase(unroll.out, ARM_WORK, REG_STACK, unroll.stackHeight);
	unroll.stackHeight += 4;
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_MK_LOCAL_2:
{
	/* Make two local variable words */
	UNROLL_START();
	FlushRegisterStack(&unroll);
	arm_mov_reg_imm8(unroll.out, ARM_WORK, 0);
	arm_store_membase(unroll.out, ARM_WORK, REG_STACK, unroll.stackHeight);
	arm_store_membase(unroll.out, ARM_WORK, REG_STACK, unroll.stackHeight + 4);
	unroll.stackHeight += 8;
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_MK_LOCAL_3:
{
	/* Make three local variable words */
	UNROLL_START();
	FlushRegisterStack(&unroll);
	arm_mov_reg_imm8(unroll.out, ARM_WORK, 0);
	arm_store_membase(unroll.out, ARM_WORK, REG_STACK, unroll.stackHeight);
	arm_store_membase(unroll.out, ARM_WORK, REG_STACK, unroll.stackHeight + 4);
	arm_store_membase(unroll.out, ARM_WORK, REG_STACK, unroll.stackHeight + 8);
	unroll.stackHeight += 12;
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_DUP:
{
	/* Duplicate the top word on the stack */
	UNROLL_START();
	reg = PeekTopWordRegister(&unroll);
	if(reg != -1)
	{
		/* The top is already in a register, so move it to a new register */
		reg2 = GetWordRegister(&unroll);
		if(unroll.pseudoStackSize > 1)
		{
			arm_mov_reg_reg(unroll.out, reg2, reg);
		}
		else
		{
			/* "GetWordRegister" flushed all registers, so the value
			   we want to duplicate is now on the CVM stack */
			arm_load_membase(unroll.out, reg2, REG_STACK,
							 unroll.stackHeight - 4);
		}
	}
	else
	{
		/* The top is on the CVM stack */
		reg = GetWordRegister(&unroll);
		arm_load_membase(unroll.out, reg, REG_STACK, unroll.stackHeight - 4);
	}
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

case COP_POP:
{
	/* Pop the top word on the stack */
	UNROLL_START();
	reg = PeekTopWordRegister(&unroll);
	if(reg != -1)
	{
		/* Abandon the register's contents */
		FreeTopRegister(&unroll, -1);
	}
	else
	{
		/* Flush the register stack and then decrease its height */
		FlushRegisterStack(&unroll);
		unroll.stackHeight -= 4;
	}
	MODIFY_UNROLL_PC(CVM_LEN_NONE);
}
break;

#endif /* IL_UNROLL_CASES */
