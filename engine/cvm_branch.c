/*
 * cvm_branch.c - Opcodes for branching within a method.
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

/*
 * Branch instructions are always 6 bytes long, but may
 * contain either short or long forms of a branch.  The
 * short form is "opcode offset pad" where "offset" is an
 * 8-bit offset and "pad" is 4 bytes of padding.  The
 * long form is "br.long opcode loffset" where "loffset"
 * is a 32-bit offset.  This allows the code generator to
 * output branch instructions with a uniform length, while
 * the interpreter runs faster on short branches.
 */

case COP_BR:
{
	/* Unconditional branch */
	pc += (ILInt32)(ILInt8)(pc[1]);
}
break;

case COP_BEQ:
{
	/* Branch if the top two integers are equal */
	if(stacktop[-2].intValue == stacktop[-1].intValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BNE:
{
	/* Branch if the top two integers are not equal */
	if(stacktop[-2].intValue != stacktop[-1].intValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BLT:
{
	/* Branch if the top two integers are less than */
	if(stacktop[-2].intValue < stacktop[-1].intValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BLT_UN:
{
	/* Branch if the top two unsigned integers are less than */
	if(stacktop[-2].uintValue < stacktop[-1].uintValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BLE:
{
	/* Branch if the top two integers are less than or equal */
	if(stacktop[-2].intValue <= stacktop[-1].intValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BLE_UN:
{
	/* Branch if the top two unsigned integers
	   are less than or equal */
	if(stacktop[-2].uintValue <= stacktop[-1].uintValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BGT:
{
	/* Branch if the top two integers are greater than */
	if(stacktop[-2].intValue > stacktop[-1].intValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BGT_UN:
{
	/* Branch if the top two unsigned integers are greater than */
	if(stacktop[-2].uintValue > stacktop[-1].uintValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BGE:
{
	/* Branch if the top two integers are greater than or equal */
	if(stacktop[-2].intValue >= stacktop[-1].intValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BGE_UN:
{
	/* Branch if the top two unsigned integers
	   are greater than or equal */
	if(stacktop[-2].uintValue >= stacktop[-1].uintValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BRTRUE:
{
	/* Branch if the top value is non-zero */
	if(stacktop[-1].intValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 1;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -1);
	}
}
break;

case COP_BRFALSE:
{
	/* Branch if the top value is zero */
	if(!(stacktop[-1].intValue))
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 1;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -1);
	}
}
break;

case COP_BRNULL:
{
	/* Branch if the top value is null */
	if(!(stacktop[-1].ptrValue))
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 1;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -1);
	}
}
break;

case COP_BRNONNULL:
{
	/* Branch if the top value is non-null */
	if(stacktop[-1].ptrValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 1;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -1);
	}
}
break;

case COP_BR_PEQ:
{
	/* Branch if the top two pointers are equal */
	if(stacktop[-2].ptrValue == stacktop[-1].ptrValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BR_PNE:
{
	/* Branch if the top two pointers are not equal */
	if(stacktop[-2].ptrValue != stacktop[-1].ptrValue)
	{
		pc += (ILInt32)(ILInt8)(pc[1]);
		stacktop -= 2;
	}
	else
	{
		MODIFY_PC_AND_STACK(6, -2);
	}
}
break;

case COP_BR_LONG:
{
	/* Determine which type of long branch to use */
	switch(pc[1])
	{
		case COP_BR:
		default:
		{
			/* Unconditional branch */
			pc += IL_READ_INT32(pc + 2);
		}
		break;
		
		case COP_BEQ:
		{
			/* Branch if the top two integers are equal */
			if(stacktop[-2].intValue == stacktop[-1].intValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BNE:
		{
			/* Branch if the top two integers are not equal */
			if(stacktop[-2].intValue != stacktop[-1].intValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BLT:
		{
			/* Branch if the top two integers are less than */
			if(stacktop[-2].intValue < stacktop[-1].intValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BLT_UN:
		{
			/* Branch if the top two unsigned integers
			   are less than */
			if(stacktop[-2].uintValue < stacktop[-1].uintValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BLE:
		{
			/* Branch if the top two integers are
			   less than or equal */
			if(stacktop[-2].intValue <= stacktop[-1].intValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BLE_UN:
		{
			/* Branch if the top two unsigned integers
			   are less than or equal */
			if(stacktop[-2].uintValue <= stacktop[-1].uintValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BGT:
		{
			/* Branch if the top two integers are greater than */
			if(stacktop[-2].intValue > stacktop[-1].intValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BGT_UN:
		{
			/* Branch if the top two unsigned integers
			   are greater than */
			if(stacktop[-2].uintValue > stacktop[-1].uintValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BGE:
		{
			/* Branch if the top two integers are
			   greater than or equal */
			if(stacktop[-2].intValue >= stacktop[-1].intValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BGE_UN:
		{
			/* Branch if the top two unsigned integers
			   are greater than or equal */
			if(stacktop[-2].uintValue >= stacktop[-1].uintValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BRTRUE:
		{
			/* Branch if the top value is non-zero */
			if(stacktop[-1].intValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 1;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -1);
			}
		}
		break;
		
		case COP_BRFALSE:
		{
			/* Branch if the top value is zero */
			if(!(stacktop[-1].intValue))
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 1;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -1);
			}
		}
		break;
		
		case COP_BRNULL:
		{
			/* Branch if the top value is null */
			if(!(stacktop[-1].ptrValue))
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 1;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -1);
			}
		}
		break;
		
		case COP_BRNONNULL:
		{
			/* Branch if the top value is non-null */
			if(stacktop[-1].ptrValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 1;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -1);
			}
		}
		break;

		case COP_BR_PEQ:
		{
			/* Branch if the top two pointers are equal */
			if(stacktop[-2].ptrValue == stacktop[-1].ptrValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;
		
		case COP_BR_PNE:
		{
			/* Branch if the top two pointers are not equal */
			if(stacktop[-2].ptrValue != stacktop[-1].ptrValue)
			{
				pc += IL_READ_INT32(pc + 2);
				stacktop -= 2;
			}
			else
			{
				MODIFY_PC_AND_STACK(6, -2);
			}
		}
		break;

		case COP_JSR:
		{
			/* Long form of the "jsr" opcode */
			stacktop[0].intValue = (ILInt32)((pc + 6) - pcstart);
			pc += IL_READ_INT32(pc + 2);
			stacktop += 1;
		}
		break;
	}
}
break;

case COP_SWITCH:
{
	/* Process a switch statement */
	if(stacktop[-1].uintValue < IL_READ_UINT32(pc + 1))
	{
		/* Jump to a specific case */
		tempNum = 9 + stacktop[-1].uintValue * 4;
		pc += IL_READ_INT32(pc + tempNum);
	}
	else
	{
		/* Jump to the default case */
		pc += IL_READ_INT32(pc + 5);
	}
}
break;

#endif /* IL_CVM_MAIN */
