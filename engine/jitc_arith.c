/*
 * jitc_arith.c - Coder implementation for JIT arithmetic operations.
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
 * Handle a binary opcode.
 */
static void JITCoder_Binary(ILCoder *coder, int opcode,
							ILEngineType type1, ILEngineType type2)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue value1 = jitCoder->jitStack[jitCoder->stackTop - 2];
	ILJitValue value2 = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue result = 0;

	switch(opcode)
	{
		case IL_OP_ADD:
		{
			AdjustMixedBinary(jitCoder, 0, &value1, &value2);
			result = jit_insn_add(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_ADD_OVF:
		{
			AdjustMixedBinary(jitCoder, 0, &value1, &value2);
			result = jit_insn_add_ovf(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_ADD_OVF_UN:
		{
			AdjustMixedBinary(jitCoder, 1, &value1, &value2);
			result = jit_insn_add_ovf(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_SUB:
		{
			AdjustMixedBinary(jitCoder, 0, &value1, &value2);
			result = jit_insn_sub(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_SUB_OVF:
		{
			AdjustMixedBinary(jitCoder, 0, &value1, &value2);
			result = jit_insn_sub_ovf(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_SUB_OVF_UN:
		{
			AdjustMixedBinary(jitCoder, 1, &value1, &value2);
			result = jit_insn_sub_ovf(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_MUL:
		{
			AdjustMixedBinary(jitCoder, 0, &value1, &value2);
			result = jit_insn_mul(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_MUL_OVF:
		{
			AdjustMixedBinary(jitCoder, 0, &value1, &value2);
			result = jit_insn_mul_ovf(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_MUL_OVF_UN:
		{
			AdjustMixedBinary(jitCoder, 1, &value1, &value2);
			result = jit_insn_mul_ovf(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_DIV:
		{
			AdjustMixedBinary(jitCoder, 0, &value1, &value2);
			result = jit_insn_div(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_DIV_UN:
		{
			AdjustMixedBinary(jitCoder, 1, &value1, &value2);
			result = jit_insn_div(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_REM:
		{
			AdjustMixedBinary(jitCoder, 0, &value1, &value2);
			result = jit_insn_rem_ieee(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_REM_UN:
		{
			AdjustMixedBinary(jitCoder, 1, &value1, &value2);
			result = jit_insn_rem_ieee(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_AND:
		{
			result = jit_insn_and(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_OR:
		{
			result = jit_insn_or(jitCoder->jitFunction, value1, value2);
		}
		break;

		case IL_OP_XOR:
		{
			result = jit_insn_xor(jitCoder->jitFunction, value1, value2);
		}
		break;
		
		default:
		{
			return;
		}
	}

	jitCoder->jitStack[jitCoder->stackTop - 2] = result;
	JITC_ADJUST(jitCoder, -1);
}

/*
 * Handle a binary opcode when pointer arithmetic is involved.
 */
static void JITCoder_BinaryPtr(ILCoder *coder, int opcode,
							   ILEngineType type1, ILEngineType type2)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	switch(opcode)
	{
		case IL_OP_ADD:
		{
			jitCoder->jitStack[jitCoder->stackTop - 2] = jit_insn_add(jitCoder->jitFunction, 
									jitCoder->jitStack[jitCoder->stackTop - 2],
									jitCoder->jitStack[jitCoder->stackTop - 1]);
			JITC_ADJUST(jitCoder, -1);
		}
		case IL_OP_ADD_OVF_UN:
		{
			jitCoder->jitStack[jitCoder->stackTop - 2] = jit_insn_add_ovf(jitCoder->jitFunction, 
									jitCoder->jitStack[jitCoder->stackTop - 2],
									jitCoder->jitStack[jitCoder->stackTop - 1]);
			JITC_ADJUST(jitCoder, -1);
		}
		break;

		case IL_OP_SUB:
		{
			jitCoder->jitStack[jitCoder->stackTop - 2] = jit_insn_sub(jitCoder->jitFunction, 
									jitCoder->jitStack[jitCoder->stackTop - 2],
									jitCoder->jitStack[jitCoder->stackTop - 1]);
			JITC_ADJUST(jitCoder, -1);
		}
		case IL_OP_SUB_OVF_UN:
		{
			jitCoder->jitStack[jitCoder->stackTop - 2] = jit_insn_sub_ovf(jitCoder->jitFunction, 
									jitCoder->jitStack[jitCoder->stackTop - 2],
									jitCoder->jitStack[jitCoder->stackTop - 1]);
			JITC_ADJUST(jitCoder, -1);
		}
		break;
	}
}

/*
 * Handle a shift opcode.
 */
static void JITCoder_Shift(ILCoder *coder, int opcode,
						   ILEngineType type1, ILEngineType type2)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	/* Determine how to perform the operation */
	switch(opcode)
	{
		case IL_OP_SHL:
		{
			jitCoder->jitStack[jitCoder->stackTop - 2] = jit_insn_shl(jitCoder->jitFunction, 
									jitCoder->jitStack[jitCoder->stackTop - 2],
									jitCoder->jitStack[jitCoder->stackTop - 1]);
			JITC_ADJUST(jitCoder, -1);
		}
		break;

		case IL_OP_SHR:
		{
			jitCoder->jitStack[jitCoder->stackTop - 2] = jit_insn_shr(jitCoder->jitFunction, 
									jitCoder->jitStack[jitCoder->stackTop - 2],
									jitCoder->jitStack[jitCoder->stackTop - 1]);
			JITC_ADJUST(jitCoder, -1);
		}
		break;

		case IL_OP_SHR_UN:
		{
			jitCoder->jitStack[jitCoder->stackTop - 2] = jit_insn_shr(jitCoder->jitFunction,
									jitCoder->jitStack[jitCoder->stackTop - 2],
									jitCoder->jitStack[jitCoder->stackTop - 1]);		
			JITC_ADJUST(jitCoder, -1);
		}
		break;
	}
}

/*
 * Handle a unary opcode.
 */
static void JITCoder_Unary(ILCoder *coder, int opcode, ILEngineType type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	switch(opcode)
	{
		case IL_OP_NEG:
		{
			jitCoder->jitStack[jitCoder->stackTop - 1] = jit_insn_neg(jitCoder->jitFunction, 
									jitCoder->jitStack[jitCoder->stackTop - 1]);
		}
		break;

		case IL_OP_NOT:
		{
			jitCoder->jitStack[jitCoder->stackTop - 1] = jit_insn_not(jitCoder->jitFunction, 
									jitCoder->jitStack[jitCoder->stackTop - 1]);
		}
		break;

		case IL_OP_CKFINITE:
		{
			/* Check the stack Top-most F value to see if it is finite */
			jitCoder->jitStack[jitCoder->stackTop - 1] = jit_insn_is_finite(jitCoder->jitFunction,
									jitCoder->jitStack[jitCoder->stackTop - 1]);
		}
		break;
	}
}

#endif	/* IL_JITC_CODE */
