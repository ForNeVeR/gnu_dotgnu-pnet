/*
 * verify_branch.c - Verify instructions related to branching.
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

#if defined(IL_VERIFY_GLOBALS)

/*
 * Type inference matrix for binary comparison operations.
 */
static char const binaryCompareMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary equality operations.
 */
static char const binaryEqualityMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_NO, T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_O,  T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_NO, T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for binary comparison operations,
 * when unsafe pointer comparisons are allowed.
 */
static char const unsafeCompareMatrix
			[ILEngineType_ValidTypes][ILEngineType_ValidTypes] =
{
		    /* I4    I8    I     F     &     O     *     MV */
	/* I4: */ {T_I4, T_NO, T_I,  T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I8: */ {T_NO, T_I8, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
	/* I:  */ {T_I,  T_NO, T_I,  T_NO, T_M,  T_NO, T_T,  T_NO},
	/* F:  */ {T_NO, T_NO, T_NO, T_F,  T_NO, T_NO, T_NO, T_NO},
	/* &:  */ {T_NO, T_NO, T_M,  T_NO, T_M,  T_NO, T_T,  T_NO},
	/* O:  */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_O,  T_NO, T_NO},
	/* *:  */ {T_NO, T_NO, T_T,  T_NO, T_T,  T_NO, T_T,  T_NO},
	/* MV: */ {T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO, T_NO},
};

/*
 * Type inference matrix for unary branch operators.
 */
static char const unaryBranchMatrix[ILEngineType_ValidTypes] =
{
    /* I4    I8    I     F     &     O     *    MV */
     T_I4, T_I8, T_I,  T_F,  T_M,  T_O,  T_T, T_NO,
};

#elif defined(IL_VERIFY_LOCALS)

ILUInt32 dest;

#else /* IL_VERIFY_CODE */

#define	GET_SHORT_DEST()	(offset + 2 + ((ILUInt32)(ILInt32)(pc[1])))
#define	GET_LONG_DEST()		(offset + 5 + ((ILUInt32)IL_READ_INT32(pc + 1)))

#define	VALIDATE_BRANCH_STACK(dest)	\
			do { \
			} while (0)

case IL_OP_BR_S:
{
	/* Unconditional short branch */
	dest = GET_SHORT_DEST();
	ILCoderBranch(coder, opcode, dest, ILEngineType_I4, ILEngineType_I4);
	VALIDATE_BRANCH_STACK(dest);
	lastWasJump = 1;
}
break;

case IL_OP_BRFALSE_S:
case IL_OP_BRTRUE_S:
{
	/* Unary conditional short branch */
  	dest = GET_SHORT_DEST();
unaryBranch:
	commonType = unaryBranchMatrix[STK_UNARY];
	if(commonType != ILEngineType_Invalid)
	{
		ILCoderBranch(coder, opcode, dest, commonType, commonType);
		--stackSize;
		VALIDATE_BRANCH_STACK(dest);
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_BEQ_S:
case IL_OP_BNE_UN_S:
{
	/* Binary equality short branch */
	dest = GET_SHORT_DEST();
binaryEquality:
	if(unsafeAllowed)
	{
		commonType = unsafeCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	else
	{
		commonType = binaryEqualityMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	if(commonType == ILEngineType_M || commonType == ILEngineType_T)
	{
		ILCoderBranchPtr(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
	}
	else if(commonType != ILEngineType_Invalid)
	{
		ILCoderBranch(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_BGT_UN_S:
{
	if(STK_BINARY_1 == ILEngineType_O && STK_BINARY_2 == ILEngineType_O)
	{
		/* "bgt.un" can be used on object references */
		dest = GET_SHORT_DEST();
		ILCoderBranch(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
		break;
	}
}
/* Fall through */

case IL_OP_BGE_S:
case IL_OP_BGT_S:
case IL_OP_BLE_S:
case IL_OP_BLT_S:
case IL_OP_BGE_UN_S:
case IL_OP_BLE_UN_S:
case IL_OP_BLT_UN_S:
{
	/* Binary conditional short branch */
	dest = GET_SHORT_DEST();
binaryBranch:
	if(unsafeAllowed)
	{
		commonType = unsafeCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	else
	{
		commonType = binaryCompareMatrix[STK_BINARY_1][STK_BINARY_2];
	}
	if(commonType == ILEngineType_M || commonType == ILEngineType_T)
	{
		ILCoderBranchPtr(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
	}
	else if(commonType != ILEngineType_Invalid)
	{
		ILCoderBranch(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_BR:
{
	/* Unconditional long branch */
	dest = GET_LONG_DEST();
	ILCoderBranch(coder, opcode, dest, ILEngineType_I4, ILEngineType_I4);
	VALIDATE_BRANCH_STACK(dest);
	lastWasJump = 1;
}
break;

case IL_OP_BRFALSE:
case IL_OP_BRTRUE:
{
	/* Unary conditional long branch */
	dest = GET_LONG_DEST();
	goto unaryBranch;
}
/* Not reached */

case IL_OP_BEQ:
case IL_OP_BNE_UN:
{
	/* Binary equality long branch */
	dest = GET_LONG_DEST();
	goto binaryEquality;
}
/* Not reached */

case IL_OP_BGT_UN:
{
	if(STK_BINARY_1 == ILEngineType_O && STK_BINARY_2 == ILEngineType_O)
	{
		/* "bgt.un" can be used on object references */
		dest = GET_LONG_DEST();
		ILCoderBranch(coder, opcode, dest, STK_BINARY_1, STK_BINARY_2);
		stackSize -= 2;
		VALIDATE_BRANCH_STACK(dest);
		break;
	}
}
/* Fall through */

case IL_OP_BGE:
case IL_OP_BGT:
case IL_OP_BLE:
case IL_OP_BLT:
case IL_OP_BGE_UN:
case IL_OP_BLE_UN:
case IL_OP_BLT_UN:
{
	/* Binary conditional long branch */
	dest = GET_LONG_DEST();
	goto binaryBranch;
}
/* Not reached */

case IL_OP_SWITCH:
{
	/* Switch statement */
}
break;

#endif /* IL_VERIFY_CODE */
