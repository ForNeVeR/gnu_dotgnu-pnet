/*
 * jitc_branch.c - Coder implementation for JIT branches.
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

#ifdef IL_JITC_CODE

/*
 * Look for a label for a specific IL address.  Create
 * a new label if necessary.
 */
static ILJITLabel *GetLabel(ILJITCoder *coder, ILUInt32 address)
{
	ILJITLabel *label;
	label = coder->labelList;
	while(label != 0)
	{
		if(label->address == address)
		{
			return label;
		}
		label = label->next;
	}
	label = ILMemPoolAlloc(&(coder->labelPool), ILJITLabel);
	if(label)
	{
		label->address = address;
		label->label = jit_label_undefined;
		label->next = coder->labelList;
		coder->labelList = label;
	}
	else
	{
		coder->labelOutOfMemory = 1;
	}
	return label;
}

/*
 * Handle a label.
 */
static void JITCoder_Label(ILCoder *coder, ILUInt32 offset)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJITLabel *label = GetLabel(jitCoder, offset);

	if(label)
	{
		jit_insn_label(jitCoder->jitFunction, &(label->label));
	}
}

/*
 * Check if the typeKind is a long.
 */
#ifdef IL_NATIVE_INT64
#define _JIT_TYPEKIND_IS_LONG(typeKind) \
((typeKind == JIT_TYPE_LONG)	|| \
 (typeKind == JIT_TYPE_ULONG)	|| \
 (typeKind == JIT_TYPE_NINT)	|| \
 (typeKind == JIT_TYPE_NUINT)	|| \
 (type1Kind == JIT_TYPE_PTR))
#else
#define _JIT_TYPEKIND_IS_LONG(typeKind) \
((typeKind == JIT_TYPE_LONG)	|| \
 (typeKind == JIT_TYPE_ULONG))
#endif

/*
 * Check if the typeKind is unsigned.
 */
#define _JIT_TYPEKIND_IS_UNSIGNED(typeKind) \
((typeKind == JIT_TYPE_ULONG)	|| \
 (typeKind == JIT_TYPE_NUINT)	|| \
 (typeKind == JIT_TYPE_UINT)	|| \
 (typeKind == JIT_TYPE_USHORT)	|| \
 (type1Kind == JIT_TYPE_UBYTE))

/*
 * Check if the typeKind is signed.
 */
#define _JIT_TYPEKIND_IS_SIGNED(typeKind) \
((typeKind == JIT_TYPE_LONG)	|| \
 (typeKind == JIT_TYPE_NINT)	|| \
 (typeKind == JIT_TYPE_INT)	|| \
 (typeKind == JIT_TYPE_SHORT)	|| \
 (type1Kind == JIT_TYPE_SBYTE))

/*
 * Readjust the stack to normalize binary operands when
 * I and I4 are mixed together.  Also determine which of
 * I4 or I8 to use if the operation involves I.
 */
static void AdjustMixedBinary(ILJITCoder *coder, int isUnsigned,
							  ILJitValue *value1, ILJitValue *value2)
							  
{
	ILJitType type1 = jit_value_get_type(*value1);
	ILJitType newType1 = type1;
	ILJitType type2 = jit_value_get_type(*value2);
	ILJitType newType2 = type2;
	int type1Kind = jit_type_get_kind(type1);
	int type2Kind = jit_type_get_kind(type2);
	int type1IsLong = _JIT_TYPEKIND_IS_LONG(type1Kind);
	int type2IsLong = _JIT_TYPEKIND_IS_LONG(type2Kind);

	/* If the arguments mix I8 and I4, then cast the I4 value to I8 */
	if(type1IsLong && !type2IsLong)
	{
		if(isUnsigned)
		{
			newType2 = _IL_JIT_TYPE_UINT64;
		}
		else
		{
			newType2 = _IL_JIT_TYPE_INT64;
		}
		type2Kind = jit_type_get_kind(newType2);
		type2IsLong = 1;
	}
	else if(!type1IsLong && type2IsLong)
	{
		if(isUnsigned)
		{
			newType1 = _IL_JIT_TYPE_UINT64;
		}
		else
		{
			newType1 = _IL_JIT_TYPE_INT64;
		}
		type1Kind = jit_type_get_kind(newType1);
		type1IsLong = 1;
	}

	if(isUnsigned)
	{
		if(_JIT_TYPEKIND_IS_SIGNED(type1Kind))
		{
			if(type1IsLong)
			{
				newType1 = _IL_JIT_TYPE_UINT64;
			}
			else
			{
				newType1 = _IL_JIT_TYPE_UINT32;
			}
			type1Kind = jit_type_get_kind(newType1);
		}
		if(_JIT_TYPEKIND_IS_SIGNED(type2Kind))
		{
			if(type2IsLong)
			{
				newType2 = _IL_JIT_TYPE_UINT64;
			}
			else
			{
				newType2 = _IL_JIT_TYPE_UINT32;
			}
			type2Kind = jit_type_get_kind(newType2);
		}
	}
	else
	{
		if(_JIT_TYPEKIND_IS_UNSIGNED(type1Kind))
		{
			if(type1IsLong)
			{
				newType1 = _IL_JIT_TYPE_INT64;
			}
			else
			{
				newType1 = _IL_JIT_TYPE_INT32;
			}
			type1Kind = jit_type_get_kind(newType1);
		}
		if(_JIT_TYPEKIND_IS_UNSIGNED(type2Kind))
		{
			if(type2IsLong)
			{
				newType2 = _IL_JIT_TYPE_INT64;
			}
			else
			{
				newType2 = _IL_JIT_TYPE_INT32;
			}
			type2Kind = jit_type_get_kind(newType2);
		}
	}
	
	/* now do the conversion if necessairy. */
	if(type1 != newType1)
	{
		*value1 = jit_insn_convert(coder->jitFunction, *value1, newType1, 0);
	}
	if(type2 != newType2)
	{
		*value2 = jit_insn_convert(coder->jitFunction, *value2, newType2, 0);
	}
}

/*
 * Output a comparision between the 2 top most values on the evaluation stack.
 * The result value is returned.
 */
static ILJitValue OutputCompare(ILJITCoder *coder, int opcode,
				   		     ILJitValue *value1, ILJitValue *value2)
{
	switch(opcode)
	{
		case IL_OP_PREFIX + IL_PREFIX_OP_CEQ:
		case IL_OP_BEQ:
		{
			/* Test two values for equality */
			AdjustMixedBinary(coder, 0, value1, value2);
			return jit_insn_eq(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_BNE_UN:
		{
			/* Test two unsigned values for inequality */
			AdjustMixedBinary(coder, 1, value1, value2);
			return jit_insn_ne(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_PREFIX + IL_PREFIX_OP_CGT:
		case IL_OP_BGT:
		{
			/* Test two signed values for greater than */
			AdjustMixedBinary(coder, 0, value1, value2);
			return jit_insn_gt(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_PREFIX + IL_PREFIX_OP_CGT_UN:
		case IL_OP_BGT_UN:
		{
			/* Test two unsigned values for greater than */
			AdjustMixedBinary(coder, 1, value1, value2);
			return jit_insn_gt(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_BGE:
		{
			/* Test two signed values for greater than  or equal */
			AdjustMixedBinary(coder, 0, value1, value2);
			return jit_insn_ge(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_BGE_UN:
		{
			/* Test two unsigned values for greater than  or equal */
			AdjustMixedBinary(coder, 1, value1, value2);
			return jit_insn_ge(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_PREFIX + IL_PREFIX_OP_CLT:
		case IL_OP_BLT:
		{
			/* Test two signed values for less than */
			AdjustMixedBinary(coder, 0, value1, value2);
			return jit_insn_lt(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_PREFIX + IL_PREFIX_OP_CLT_UN:
		case IL_OP_BLT_UN:
		{
			/* Test two unsigned values for less than */
			AdjustMixedBinary(coder, 1, value1, value2);
			return jit_insn_lt(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_BLE:
		{
			/* Test two signed values for less than or equal */
			AdjustMixedBinary(coder, 0, value1, value2);
			return jit_insn_le(coder->jitFunction, *value1, *value2);
		}
		break;

		case IL_OP_BLE_UN:
		{
			/* Test two unsigned values for less than  or equal */
			AdjustMixedBinary(coder, 1, value1, value2);
			return jit_insn_lt(coder->jitFunction, *value1, *value2);
		}
		break;
	}
	return 0;
}

/*
 * Output a branch instruction using a JIT coder.
 */
static void JITCoder_Branch(ILCoder *coder, int opcode, ILUInt32 dest,
				   		    ILEngineType type1, ILEngineType type2)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJITLabel *label = GetLabel(jitCoder, dest);
	ILJitValue temp;

	/* Determine what form of branch to use */
	switch(opcode)
	{
		case IL_OP_BR:
		case IL_OP_BR_S:
		case IL_OP_LEAVE:
		case IL_OP_LEAVE_S:
		{
			/* Unconditional branch */
			jit_insn_branch(jitCoder->jitFunction, &(label->label));
		}
		break;

		case IL_OP_BRTRUE_S:
		case IL_OP_BRTRUE:
		{
			/* Branch if the top-most stack item is true */
			jit_insn_branch_if(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop - 1],
								&(label->label));
			JITC_ADJUST(jitCoder, -1);
		}
		break;

		case IL_OP_BRFALSE_S:
		case IL_OP_BRFALSE:
		{
			/* Branch if the top-most stack item is false */
			jit_insn_branch_if_not(jitCoder->jitFunction,
									jitCoder->jitStack[jitCoder->stackTop - 1],
									&(label->label));
			JITC_ADJUST(jitCoder, -1);
		}
		break;

		case IL_OP_BEQ:
		case IL_OP_BEQ_S:
		{
			/* Equality testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BEQ,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BNE_UN:
		case IL_OP_BNE_UN_S:
		{
			/* Unsigned inequality testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BNE_UN,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BGT:
		case IL_OP_BGT_S:
		{
			/* Signed greater than testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BGT,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BGT_UN:
		case IL_OP_BGT_UN_S:
		{
			/* Unsigned greater than testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BGT_UN,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BGE:
		case IL_OP_BGE_S:
		{
			/* Signed greater than or equal testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BGE,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BGE_UN:
		case IL_OP_BGE_UN_S:
		{
			/* Unsigned greater than or equal testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BGE_UN,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BLT:
		case IL_OP_BLT_S:
		{
			/* Signed less than testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BLT,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BLT_UN:
		case IL_OP_BLT_UN_S:
		{
			/* Unsigned less than testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BLT_UN,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BLE:
		case IL_OP_BLE_S:
		{
			/* Signed less than or equal testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BLE,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;

		case IL_OP_BLE_UN:
		case IL_OP_BLE_UN_S:
		{
			/* Unsigned less than or equal testing branch */
			temp = OutputCompare(jitCoder, IL_OP_BLE_UN,
								 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
								 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
			jit_insn_branch_if(jitCoder->jitFunction, temp, &(label->label));
			JITC_ADJUST(jitCoder, -2);
		}
		break;
	}

}

/*
 * Output the start of a table-based switch statement.
 */
static void JITCoder_SwitchStart(ILCoder *coder, ILUInt32 numEntries)
{
}

/*
 * Output an entry for a table-based switch statement.
 */
static void JITCoder_SwitchEntry(ILCoder *_coder, ILUInt32 dest)
{
}

/*
 * Output a comparison instruction.
 */
static void JITCoder_Compare(ILCoder *coder, int opcode,
				   		     ILEngineType type1, ILEngineType type2,
							 int invertTest)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue temp;

	temp = OutputCompare(jitCoder, opcode,
						 &(jitCoder->jitStack[jitCoder->stackTop - 2]),
						 &(jitCoder->jitStack[jitCoder->stackTop - 1]));
	if(invertTest)
	{
		temp = jit_insn_to_not_bool(jitCoder->jitFunction, temp);
	}
	else
	{
		temp = jit_insn_to_bool(jitCoder->jitFunction, temp);
	}
	jitCoder->jitStack[jitCoder->stackTop - 2] = temp;
	JITC_ADJUST(jitCoder, -1);
}

#endif	/* IL_JITC_CODE */
