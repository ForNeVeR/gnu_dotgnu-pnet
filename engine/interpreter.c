/*
 * interpreter.c - Bytecode interpreter for the IL instruction set.
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

#include "engine.h"
#include "il_opcodes.h"
#ifdef HAVE_MATH_H
#include <math.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Check to see if a floating point number is finite.
 */
#ifdef HAVE_FINITE
	#define	FLOAT_IS_FINITE(value)	(finite((value)))
#else
	#if defined(HAVE_ISNAN) && defined(HAVE_ISINF)
		#define	FLOAT_IS_FINITE(value)	(!isnan((value)) && \
										  isinf((value)) == 0)
	#else
		#error "Don't know how to determine if floating point numbers are finite"
	#endif
#endif

/*
 * Modify the program counter and the position of the operand stack top.
 */
#define	MODIFY_PC_AND_STACK(pcinc,stkinc)	\
			((pc) += (pcinc), (stacktop) += (stkinc))

/*
 * Adjust the contents of an argument or variable after storing.
 */
#define	ADJUST_STORE(var,type)	\
		do { \
			if((type) == IL_TYPE_INT8) \
			{ \
				(var).un.i4Value = (ILInt32)(ILInt8)((var).un.i4Value); \
			} \
			else if((type) == IL_TYPE_UINT8) \
			{ \
				(var).un.i4Value = (ILInt32)(ILUInt8)((var).un.i4Value); \
			} \
			else if((type) == IL_TYPE_INT16) \
			{ \
				(var).un.i4Value = (ILInt32)(ILInt16)((var).un.i4Value); \
			} \
			else if((type) == IL_TYPE_UINT16) \
			{ \
				(var).un.i4Value = (ILInt32)(ILUInt16)((var).un.i4Value); \
			} \
			else if((type) == IL_TYPE_FLOAT32) \
			{ \
				(var).un.fValue = \
						(ILNativeFloat)(ILFloat)((var).un.fValue); \
			} \
			else if((type) == IL_TYPE_FLOAT64) \
			{ \
				(var).un.fValue = \
						(ILNativeFloat)(ILDouble)((var).un.fValue); \
			} \
			(var).actualType = (short)(type); \
		} while (0)

/*
 * Throw an exception that resulted from incorrect values on the stack.
 */
#define	ILVALUE_EXCEPTION()	\
		do { \
		} while (0)

/*
 * Throw an arithmetic exception.
 */
#define	ARITHMETIC_EXCEPTION()	\
		do { \
		} while (0)

/*
 * Throw a division by zero exception.
 */
#define	ZERO_DIV_EXCEPTION()	\
		do { \
		} while (0)

void _ILInterpreter(ILExecThread *thread)
{
	unsigned char *pc;
	ILValue *stacktop;
	ILValue *args;
	ILValue *locals;
	ILValue *variable;
	int actualType;
	ILUInt32 num;
	int result, cmp;

	/* Unload the thread state into locals */
	pc = thread->pc;
	if(!pc)
	{
		/* Nothing is currently queued for execution */
		return;
	}
	stacktop = thread->stackTop;
	args = thread->args;
	locals = thread->locals;

	/* Enter the main dispatch loop */
	for(;;)
	{
		switch(pc[0])
		{
			case IL_OP_NOP:
			{
				/* The world's simplest instruction */
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_BREAK:
			{
				/* We don't support breakpoints at present, so
				   treat this as a nop */
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

#define	IL_OP_LDARG_(n)	\
			case IL_OP_LDARG_##n: \
			{ \
				*stacktop = args[(n)]; \
				MODIFY_PC_AND_STACK(1, 1); \
			} \
			break

			/* Load arguments 0..3 onto the stack */
			IL_OP_LDARG_(0);
			IL_OP_LDARG_(1);
			IL_OP_LDARG_(2);
			IL_OP_LDARG_(3);

#define	IL_OP_LDLOC_(n)	\
			case IL_OP_LDLOC_##n: \
			{ \
				*stacktop = locals[(n)]; \
				MODIFY_PC_AND_STACK(1, 1); \
			} \
			break

			/* Load locals 0..3 onto the stack */
			IL_OP_LDLOC_(0);
			IL_OP_LDLOC_(1);
			IL_OP_LDLOC_(2);
			IL_OP_LDLOC_(3);

#define	IL_OP_STLOC_(n)	\
			case IL_OP_STLOC_##n: \
			{ \
				actualType = locals[(n)].actualType; \
				locals[(n)] = stacktop[-1]; \
				ADJUST_STORE(locals[(n)], actualType); \
				MODIFY_PC_AND_STACK(1, -1); \
			} \
			break

			/* Store the top of stack into locals 0..3 */
			IL_OP_STLOC_(0);
			IL_OP_STLOC_(1);
			IL_OP_STLOC_(2);
			IL_OP_STLOC_(3);

			case IL_OP_LDARG_S:
			{
				/* Load an argument onto the stack */
				*stacktop = args[pc[1]];
				MODIFY_PC_AND_STACK(2, 1);
			}
			break;

			case IL_OP_LDARGA_S:
			{
				/* Load the address of an argument onto the stack */
				/* TO DO */
			}
			break;

			case IL_OP_STARG_S:
			{
				/* Store the top of stack into an argument */
				variable = &(args[pc[1]]);
				actualType = variable->actualType;
				*variable = stacktop[-1];
				ADJUST_STORE(*variable, actualType);
				MODIFY_PC_AND_STACK(2, -1);
			}
			break;

			case IL_OP_LDLOC_S:
			{
				/* Load a local onto the stack */
				*stacktop = locals[pc[1]];
				MODIFY_PC_AND_STACK(2, 1);
			}
			break;

			case IL_OP_LDLOCA_S:
			{
				/* Load the address of a local onto the stack */
				/* TO DO */
			}
			break;

			case IL_OP_STLOC_S:
			{
				/* Store the top of stack into a local */
				variable = &(locals[pc[1]]);
				actualType = variable->actualType;
				*variable = stacktop[-1];
				ADJUST_STORE(*variable, actualType);
				MODIFY_PC_AND_STACK(2, -1);
			}
			break;

			case IL_OP_LDNULL:
			{
				/* Load null onto the stack */
				stacktop->valueType = IL_TYPE_OBJECT_REF;
				stacktop->un.oValue = 0;
				MODIFY_PC_AND_STACK(1, 1);
			}
			break;

#define	IL_OP_LDC_I4_(name,value) \
			case IL_OP_LDC_I4_##name: \
			{ \
				stacktop->valueType = IL_TYPE_INT32; \
				stacktop->un.i4Value = (value); \
				MODIFY_PC_AND_STACK(1, 1); \
			} \
			break

			/* Load simple numeric constants */
			IL_OP_LDC_I4_(M1, -1);
			IL_OP_LDC_I4_(0, 0);
			IL_OP_LDC_I4_(1, 1);
			IL_OP_LDC_I4_(2, 2);
			IL_OP_LDC_I4_(3, 3);
			IL_OP_LDC_I4_(4, 4);
			IL_OP_LDC_I4_(5, 5);
			IL_OP_LDC_I4_(6, 6);
			IL_OP_LDC_I4_(7, 7);
			IL_OP_LDC_I4_(8, 8);

			case IL_OP_LDC_I4_S:
			{
				/* Load a single-byte numeric constant */
				stacktop->valueType = IL_TYPE_INT32;
				stacktop->un.i4Value = (ILInt32)(ILInt8)(pc[1]);
				MODIFY_PC_AND_STACK(2, 1);
			}
			break;

			case IL_OP_LDC_I4:
			{
				/* Load a 4-byte numeric constant */
				stacktop->valueType = IL_TYPE_INT32;
				stacktop->un.i4Value = IL_READ_INT32(pc + 1);
				MODIFY_PC_AND_STACK(5, 1);
			}
			break;

			case IL_OP_LDC_I8:
			{
				/* Load an 8-byte numeric constant */
				stacktop->valueType = IL_TYPE_INT64;
				stacktop->un.i8Value = IL_READ_INT64(pc + 1);
				MODIFY_PC_AND_STACK(9, 1);
			}
			break;

			case IL_OP_LDC_R4:
			{
				/* Load a 4-byte floating-point constant */
				stacktop->valueType = IL_TYPE_NATIVE_FLOAT;
				stacktop->un.fValue = IL_READ_FLOAT(pc + 1);
				MODIFY_PC_AND_STACK(5, 1);
			}
			break;

			case IL_OP_LDC_R8:
			{
				/* Load an 8-byte floating-point constant */
				stacktop->valueType = IL_TYPE_NATIVE_FLOAT;
				stacktop->un.fValue = IL_READ_DOUBLE(pc + 1);
				MODIFY_PC_AND_STACK(9, 1);
			}
			break;

			case IL_OP_LDPTR:
			{
				/* Load pointer */
				/* TO DO */
			}
			break;

			case IL_OP_DUP:
			{
				/* Duplicate the top of stack */
				stacktop[0] = stacktop[-1];
				MODIFY_PC_AND_STACK(1, 1);
			}
			break;

			case IL_OP_POP:
			{
				/* Pop the top of stack */
				MODIFY_PC_AND_STACK(1, -1);
			}
			break;

			case IL_OP_JMP:
			{
				/* Jump to a method: this is used in unsafe code */
			}
			break;

			case IL_OP_CALL:
			{
				/* Call a method */
				/* TO DO */
			}
			break;

			case IL_OP_CALLI:
			{
				/* Call a method using an indirect function pointer */
				/* TO DO */
			}
			break;

			case IL_OP_RET:
			{
				/* Return from method */
				/* TO DO */
			}
			break;

			case IL_OP_BR_S:
			{
				/* Branch unconditionally to short address */
				pc += 2 + (ILInt32)(ILInt8)(pc[1]);
			}
			break;

			case IL_OP_BRFALSE_S:
			{
				/* Branch to short address if false */
				--stacktop;
				if(IL_TYPE_IS_I4(stacktop->valueType))
				{
					if(stacktop->un.i4Value == 0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				else if(IL_TYPE_IS_I(stacktop->valueType))
				{
					if(stacktop->un.iValue == 0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				else if(IL_TYPE_IS_PTR(stacktop->valueType))
				{
					if(stacktop->un.oValue == (void *)0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				else if(IL_TYPE_IS_I8(stacktop->valueType))
				{
					if(stacktop->un.i8Value == 0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				else if(IL_TYPE_IS_F(stacktop->valueType))
				{
					if(stacktop->un.fValue == (ILNativeFloat)0.0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				pc += 2;
			}
			break;

			case IL_OP_BRTRUE_S:
			{
				/* Branch to short address if true */
				--stacktop;
				if(IL_TYPE_IS_I4(stacktop->valueType))
				{
					if(stacktop->un.i4Value != 0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				else if(IL_TYPE_IS_I(stacktop->valueType))
				{
					if(stacktop->un.iValue != 0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				else if(IL_TYPE_IS_PTR(stacktop->valueType))
				{
					if(stacktop->un.oValue != (void *)0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				else if(IL_TYPE_IS_I8(stacktop->valueType))
				{
					if(stacktop->un.i8Value != 0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				else if(IL_TYPE_IS_F(stacktop->valueType))
				{
					if(stacktop->un.fValue == (ILNativeFloat)0.0)
					{
						pc += 2 + (ILInt32)(ILInt8)(pc[1]);
						break;
					}
				}
				pc += 2;
			}
			break;

#define	IL_OP_BCMP_S(name, op, lessop)	\
			case IL_OP_##name##_S: \
			{ \
				stacktop -= 2; \
				if(IL_TYPE_IS_I4(stacktop[0].valueType) && \
				   IL_TYPE_IS_I4(stacktop[1].valueType)) \
				{ \
					if(stacktop[0].un.i4Value op \
							stacktop[1].un.i4Value) \
					{ \
						pc += 2 + (ILInt32)(ILInt8)(pc[1]); \
						break; \
					} \
				} \
				else if(IL_TYPE_IS_I(stacktop[0].valueType) && \
				        IL_TYPE_IS_I(stacktop[1].valueType)) \
				{ \
					if(stacktop[0].un.iValue op \
							stacktop[1].un.iValue) \
					{ \
						pc += 2 + (ILInt32)(ILInt8)(pc[1]); \
						break; \
					} \
				} \
				else if((result = ILValueCompare(&(stacktop[0]), \
												 &(stacktop[1]), \
												 lessop, &cmp)) \
								== ILVALUE_RESULT_OK) \
				{ \
					if(cmp op 0) \
					{ \
						pc += 2 + (ILInt32)(ILInt8)(pc[1]); \
						break; \
					} \
				} \
				else \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				pc += 2; \
			} \
			break

			/* Signed branch comparisons with short target addresses */
			IL_OP_BCMP_S(BEQ, ==, 0);
			IL_OP_BCMP_S(BGE, >=, 1);
			IL_OP_BCMP_S(BGT, >,  1);
			IL_OP_BCMP_S(BLE, <=, 0);
			IL_OP_BCMP_S(BLT, <,  0);

#define	IL_OP_BCMP_UN_S(name, op, lessop)	\
			case IL_OP_##name##_UN_S: \
			{ \
				stacktop -= 2; \
				if(IL_TYPE_IS_I4(stacktop[0].valueType) && \
				   IL_TYPE_IS_I4(stacktop[1].valueType)) \
				{ \
					if(((ILUInt32)(stacktop[0].un.i4Value)) op \
							((ILUInt32)(stacktop[1].un.i4Value))) \
					{ \
						pc += 2 + (ILInt32)(ILInt8)(pc[1]); \
						break; \
					} \
				} \
				else if(IL_TYPE_IS_I(stacktop[0].valueType) && \
				        IL_TYPE_IS_I(stacktop[1].valueType)) \
				{ \
					if(((ILNativeUInt)(stacktop[0].un.iValue)) op \
							((ILNativeUInt)(stacktop[1].un.iValue))) \
					{ \
						pc += 2 + (ILInt32)(ILInt8)(pc[1]); \
						break; \
					} \
				} \
				else if((result = ILValueCompareUn(&(stacktop[0]), \
												   &(stacktop[1]), \
												   lessop, &cmp)) \
								== ILVALUE_RESULT_OK) \
				{ \
					if(cmp op 0) \
					{ \
						pc += 2 + (ILInt32)(ILInt8)(pc[1]); \
						break; \
					} \
				} \
				else \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				pc += 2; \
			} \
			break

			/* Unsigned branch comparisons with short target addresses */
			IL_OP_BCMP_UN_S(BNE, !=, 0);
			IL_OP_BCMP_UN_S(BGE, >=, 1);
			IL_OP_BCMP_UN_S(BGT, >,  1);
			IL_OP_BCMP_UN_S(BLE, <=, 0);
			IL_OP_BCMP_UN_S(BLT, <,  0);

			case IL_OP_BR:
			{
				/* Branch unconditionally to long address */
				pc += 5 + IL_READ_INT32(pc + 1);
			}
			break;

			case IL_OP_BRFALSE:
			{
				/* Branch to long address if false */
				--stacktop;
				if(IL_TYPE_IS_I4(stacktop->valueType))
				{
					if(stacktop->un.i4Value == 0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				else if(IL_TYPE_IS_I(stacktop->valueType))
				{
					if(stacktop->un.iValue == 0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				else if(IL_TYPE_IS_PTR(stacktop->valueType))
				{
					if(stacktop->un.oValue == (void *)0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				else if(IL_TYPE_IS_I8(stacktop->valueType))
				{
					if(stacktop->un.i8Value == 0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				else if(IL_TYPE_IS_F(stacktop->valueType))
				{
					if(stacktop->un.fValue == (ILNativeFloat)0.0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				pc += 5;
			}
			break;

			case IL_OP_BRTRUE:
			{
				/* Branch to long address if true */
				--stacktop;
				if(IL_TYPE_IS_I4(stacktop->valueType))
				{
					if(stacktop->un.i4Value != 0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				else if(IL_TYPE_IS_I(stacktop->valueType))
				{
					if(stacktop->un.iValue != 0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				else if(IL_TYPE_IS_PTR(stacktop->valueType))
				{
					if(stacktop->un.oValue != (void *)0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				else if(IL_TYPE_IS_I8(stacktop->valueType))
				{
					if(stacktop->un.i8Value != 0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				else if(IL_TYPE_IS_F(stacktop->valueType))
				{
					if(stacktop->un.fValue != (ILNativeFloat)0.0)
					{
						pc += 5 + IL_READ_INT32(pc + 1);
						break;
					}
				}
				pc += 5;
			}
			break;

#define	IL_OP_BCMP(name, op, lessop)	\
			case IL_OP_##name: \
			{ \
				stacktop -= 2; \
				if(IL_TYPE_IS_I4(stacktop[0].valueType) && \
				   IL_TYPE_IS_I4(stacktop[1].valueType)) \
				{ \
					if(stacktop[0].un.i4Value op \
							stacktop[1].un.i4Value) \
					{ \
						pc += 5 + IL_READ_INT32(pc + 1); \
						break; \
					} \
				} \
				else if(IL_TYPE_IS_I(stacktop[0].valueType) && \
				        IL_TYPE_IS_I(stacktop[1].valueType)) \
				{ \
					if(stacktop[0].un.iValue op \
							stacktop[1].un.iValue) \
					{ \
						pc += 5 + IL_READ_INT32(pc + 1); \
						break; \
					} \
				} \
				else if((result = ILValueCompare(&(stacktop[0]), \
												 &(stacktop[1]), \
												 lessop, &cmp)) \
								== ILVALUE_RESULT_OK) \
				{ \
					if(cmp op 0) \
					{ \
						pc += 5 + IL_READ_INT32(pc + 1); \
						break; \
					} \
				} \
				else \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				pc += 5; \
			} \
			break

			/* Signed branch comparisons with long target addresses */
			IL_OP_BCMP(BEQ, ==, 0);
			IL_OP_BCMP(BGE, >=, 1);
			IL_OP_BCMP(BGT, >,  1);
			IL_OP_BCMP(BLE, <=, 0);
			IL_OP_BCMP(BLT, <,  0);

#define	IL_OP_BCMP_UN(name, op, lessop)	\
			case IL_OP_##name##_UN: \
			{ \
				stacktop -= 2; \
				if(IL_TYPE_IS_I4(stacktop[0].valueType) && \
				   IL_TYPE_IS_I4(stacktop[1].valueType)) \
				{ \
					if(((ILUInt32)(stacktop[0].un.i4Value)) op \
							((ILUInt32)(stacktop[1].un.i4Value))) \
					{ \
						pc += 5 + IL_READ_INT32(pc + 1); \
						break; \
					} \
				} \
				else if(IL_TYPE_IS_I(stacktop[0].valueType) && \
				        IL_TYPE_IS_I(stacktop[1].valueType)) \
				{ \
					if(((ILNativeUInt)(stacktop[0].un.iValue)) op \
							((ILNativeUInt)(stacktop[1].un.iValue))) \
					{ \
						pc += 5 + IL_READ_INT32(pc + 1); \
						break; \
					} \
				} \
				else if((result = ILValueCompareUn(&(stacktop[0]), \
												   &(stacktop[1]), \
												   lessop, &cmp)) \
								== ILVALUE_RESULT_OK) \
				{ \
					if(cmp op 0) \
					{ \
						pc += 5 + IL_READ_INT32(pc + 1); \
						break; \
					} \
				} \
				else \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				pc += 5; \
			} \
			break

			/* Unsigned branch comparisons with long target addresses */
			IL_OP_BCMP_UN(BNE, !=, 0);
			IL_OP_BCMP_UN(BGE, >=, 1);
			IL_OP_BCMP_UN(BGT, >,  1);
			IL_OP_BCMP_UN(BLE, <=, 0);
			IL_OP_BCMP_UN(BLT, <,  0);

			case IL_OP_SWITCH:
			{
				/* Switch on an integer value */
				--stacktop;
				num = IL_READ_UINT32(pc + 1);
				if(IL_TYPE_IS_I4(stacktop[0].valueType))
				{
					if(((ILUInt32)(stacktop[-1].un.i4Value)) < num)
					{
						pc += 5 + num * 4 + IL_READ_INT32(pc + 5 +
								((ILUInt32)(stacktop[0].un.i4Value)) * 4);
						break;
					}
				}
				else if(IL_TYPE_IS_I(stacktop[0].valueType))
				{
					if(((ILNativeUInt)(stacktop[-1].un.iValue))
								< ((ILNativeUInt)num))
					{
						pc += 5 + num * 4 + IL_READ_INT32(pc + 5 +
								((ILUInt32)(stacktop[0].un.iValue)) * 4);
						break;
					}
				}
				pc += 5 + num * 4;
			}
			break;

			case IL_OP_LDIND_I1:
			case IL_OP_LDIND_U1:
			case IL_OP_LDIND_I2:
			case IL_OP_LDIND_U2:
			case IL_OP_LDIND_I4:
			case IL_OP_LDIND_U4:
			case IL_OP_LDIND_I8:
			case IL_OP_LDIND_I:
			case IL_OP_LDIND_R4:
			case IL_OP_LDIND_R8:
			case IL_OP_LDIND_REF:
			case IL_OP_STIND_REF:
			case IL_OP_STIND_I1:
			case IL_OP_STIND_I2:
			case IL_OP_STIND_I4:
			case IL_OP_STIND_I8:
			case IL_OP_STIND_R4:
			case IL_OP_STIND_R8:
			{
				/* These instructions are used in unsafe code */
			}
			break;

#define	IL_OP_ARITH(name, op, def)	\
			case IL_OP_##name: \
			{ \
				if(IL_TYPE_IS_I4(stacktop[-2].valueType) && \
				   IL_TYPE_IS_I4(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.i4Value op \
						stacktop[-1].un.i4Value; \
					stacktop[-2].valueType = IL_TYPE_INT32; \
				} \
				else if(IL_TYPE_IS_I(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.iValue op \
						stacktop[-1].un.iValue; \
					stacktop[-2].valueType = IL_TYPE_NATIVE_INT; \
				} \
				else if(IL_TYPE_IS_I8(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I8(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.i8Value op \
						stacktop[-1].un.i8Value; \
					stacktop[-2].valueType = IL_TYPE_INT64; \
				} \
				else if(IL_TYPE_IS_F(stacktop[-2].valueType) && \
				        IL_TYPE_IS_F(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.fValue op \
						stacktop[-1].un.fValue; \
					stacktop[-2].valueType = IL_TYPE_NATIVE_FLOAT; \
				} \
				else if((result = def(&(stacktop[-2]), &(stacktop[-1]))) \
							!= ILVALUE_RESULT_OK) \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				MODIFY_PC_AND_STACK(1, -1); \
			} \
			break

#define	IL_OP_ARITH_DIV(name, op, def)	\
			case IL_OP_##name: \
			{ \
				if(IL_TYPE_IS_I4(stacktop[-2].valueType) && \
				   IL_TYPE_IS_I4(stacktop[-1].valueType)) \
				{ \
					if(stacktop[-1].un.i4Value != 0) \
					{ \
						if(stacktop[-1].un.i4Value != -1 || \
						   stacktop[-2].un.i4Value != IL_MIN_INT32) \
						{ \
							stacktop[-2].un.i4Value op \
								stacktop[-1].un.i4Value; \
							stacktop[-2].valueType = IL_TYPE_INT32; \
						} \
						else \
						{ \
							ARITHMETIC_EXCEPTION(); \
						} \
					} \
					else \
					{ \
						ZERO_DIV_EXCEPTION(); \
					} \
				} \
				else if(IL_TYPE_IS_I(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I(stacktop[-1].valueType)) \
				{ \
					if(stacktop[-1].un.iValue != 0) \
					{ \
						if(stacktop[-1].un.iValue != -1 || \
						   stacktop[-2].un.iValue != IL_MIN_NATIVE_INT) \
						{ \
							stacktop[-2].un.iValue op \
								stacktop[-1].un.iValue; \
							stacktop[-2].valueType = IL_TYPE_NATIVE_INT; \
						} \
						else \
						{ \
							ARITHMETIC_EXCEPTION(); \
						} \
					} \
					else \
					{ \
						ZERO_DIV_EXCEPTION(); \
					} \
				} \
				else if(IL_TYPE_IS_I8(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I8(stacktop[-1].valueType)) \
				{ \
					if(stacktop[-1].un.i8Value != 0) \
					{ \
						if(stacktop[-1].un.i8Value != -1 || \
						   stacktop[-2].un.i8Value != IL_MIN_INT64) \
						{ \
							stacktop[-2].un.i8Value op \
								stacktop[-1].un.i8Value; \
							stacktop[-2].valueType = IL_TYPE_INT64; \
						} \
						else \
						{ \
							ARITHMETIC_EXCEPTION(); \
						} \
					} \
					else \
					{ \
						ZERO_DIV_EXCEPTION(); \
					} \
				} \
				else if((result = def(&(stacktop[-2]), &(stacktop[-1]))) \
							!= ILVALUE_RESULT_OK) \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				MODIFY_PC_AND_STACK(1, -1); \
			} \
			break

#define	IL_OP_ARITH_DIV_UN(name, op, def)	\
			case IL_OP_##name: \
			{ \
				if(IL_TYPE_IS_I4(stacktop[-2].valueType) && \
				   IL_TYPE_IS_I4(stacktop[-1].valueType)) \
				{ \
					if(stacktop[-1].un.i4Value != 0) \
					{ \
						stacktop[-2].un.i4Value = \
							(ILInt32)(((ILUInt32)(stacktop[-2].un.i4Value)) op \
									  ((ILUInt32)(stacktop[-1].un.i4Value))); \
						stacktop[-2].valueType = IL_TYPE_UINT32; \
					} \
					else \
					{ \
						ZERO_DIV_EXCEPTION(); \
					} \
				} \
				else if(IL_TYPE_IS_I(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I(stacktop[-1].valueType)) \
				{ \
					if(stacktop[-1].un.iValue != 0) \
					{ \
						stacktop[-2].un.iValue = \
							(ILNativeInt) \
								(((ILNativeUInt)(stacktop[-2].un.iValue)) op \
							     ((ILNativeUInt)(stacktop[-1].un.iValue))); \
						stacktop[-2].valueType = IL_TYPE_NATIVE_UINT; \
					} \
					else \
					{ \
						ZERO_DIV_EXCEPTION(); \
					} \
				} \
				else if(IL_TYPE_IS_I8(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I8(stacktop[-1].valueType)) \
				{ \
					if(stacktop[-1].un.i8Value != 0) \
					{ \
						stacktop[-2].un.i8Value = \
							(ILInt64)(((ILUInt64)(stacktop[-2].un.i8Value)) op \
							          ((ILUInt64)(stacktop[-1].un.i8Value))); \
						stacktop[-2].valueType = IL_TYPE_UINT64; \
					} \
					else \
					{ \
						ZERO_DIV_EXCEPTION(); \
					} \
				} \
				else if((result = def(&(stacktop[-2]), &(stacktop[-1]))) \
							!= ILVALUE_RESULT_OK) \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				MODIFY_PC_AND_STACK(1, -1); \
			} \
			break

			/* Arithmetic instructions */
			IL_OP_ARITH(ADD, +=, ILValueAdd);
			IL_OP_ARITH(SUB, -=, ILValueSub);
			IL_OP_ARITH(MUL, *=, ILValueMul);
			IL_OP_ARITH_DIV(DIV, /=, ILValueDiv);
			IL_OP_ARITH_DIV_UN(DIV_UN, /, ILValueDivUn);
			IL_OP_ARITH_DIV(REM, %=, ILValueRem);
			IL_OP_ARITH_DIV_UN(REM_UN, %, ILValueRemUn);

			case IL_OP_NEG:
			{
				/* Arithmetic negation */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value = -(stacktop[-1].un.i4Value);
				}
				else if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.iValue = -(stacktop[-1].un.iValue);
				}
				else if(IL_TYPE_IS_I8(stacktop[-1].valueType))
				{
					stacktop[-1].un.i8Value = -(stacktop[-1].un.i8Value);
				}
				else if(IL_TYPE_IS_F(stacktop[-1].valueType))
				{
					stacktop[-1].un.fValue = -(stacktop[-1].un.fValue);
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

#define	IL_OP_BITWISE(name, assignop, def)	\
			case IL_OP_##name: \
			{ \
				if(IL_TYPE_IS_I4(stacktop[-2].valueType) && \
				   IL_TYPE_IS_I4(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.i4Value assignop \
						stacktop[-1].un.i4Value; \
					stacktop[-2].valueType = IL_TYPE_INT32; \
				} \
				else if(IL_TYPE_IS_I(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.iValue assignop \
						stacktop[-1].un.iValue; \
					stacktop[-2].valueType = IL_TYPE_NATIVE_INT; \
				} \
				else if(IL_TYPE_IS_I8(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I8(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.i8Value assignop \
						stacktop[-1].un.i8Value; \
					stacktop[-2].valueType = IL_TYPE_INT64; \
				} \
				else if((result = def(&(stacktop[-2]), &(stacktop[-1]))) \
							!= ILVALUE_RESULT_OK) \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				MODIFY_PC_AND_STACK(1, -1); \
			} \
			break
#define	IL_OP_SHIFT(name, assignop, def)	\
			case IL_OP_##name: \
			{ \
				if(IL_TYPE_IS_I4(stacktop[-2].valueType) && \
				   IL_TYPE_IS_I4(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.i4Value assignop \
						((ILUInt32)(stacktop[-1].un.i4Value)); \
				} \
				else if(IL_TYPE_IS_I4(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.i4Value assignop \
						((ILNativeUInt)(stacktop[-1].un.iValue)); \
				} \
				else if((result = def(&(stacktop[-2]), &(stacktop[-1]))) \
							!= ILVALUE_RESULT_OK) \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				MODIFY_PC_AND_STACK(1, -1); \
			} \
			break
#define	IL_OP_USHIFT(name, op, def)	\
			case IL_OP_##name: \
			{ \
				if(IL_TYPE_IS_I4(stacktop[-2].valueType) && \
				   IL_TYPE_IS_I4(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.i4Value = \
						(ILInt32)(((ILUInt32)(stacktop[-2].un.i4Value)) op \
								  ((ILUInt32)(stacktop[-1].un.i4Value))); \
				} \
				else if(IL_TYPE_IS_I4(stacktop[-2].valueType) && \
				        IL_TYPE_IS_I(stacktop[-1].valueType)) \
				{ \
					stacktop[-2].un.i4Value = \
						(ILInt32)(((ILUInt32)(stacktop[-2].un.i4Value)) op \
								  ((ILNativeUInt)(stacktop[-1].un.iValue))); \
				} \
				else if((result = def(&(stacktop[-2]), &(stacktop[-1]))) \
							!= ILVALUE_RESULT_OK) \
				{ \
					ILVALUE_EXCEPTION(); \
				} \
				MODIFY_PC_AND_STACK(1, -1); \
			} \
			break

			/* Bitwise and shift operations */
			IL_OP_BITWISE(AND,    &=,  ILValueAnd);
			IL_OP_BITWISE(OR,     |=,  ILValueOr);
			IL_OP_BITWISE(XOR,    ^=,  ILValueXor);
			IL_OP_SHIFT  (SHL,    <<=, ILValueShl);
			IL_OP_SHIFT  (SHR,    >>=, ILValueShr);
			IL_OP_USHIFT (SHR_UN, >>,  ILValueUShr);

			case IL_OP_NOT:
			{
				/* Bitwise complement of a value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						~(stacktop[-1].un.i4Value);
				}
				else if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.iValue =
						~(stacktop[-1].un.iValue);
				}
				else if(IL_TYPE_IS_I8(stacktop[-1].valueType))
				{
					stacktop[-1].un.i8Value =
						~(stacktop[-1].un.i8Value);
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_I1:
			{
				/* Convert the stack top into a signed 8-bit value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(ILInt8)(stacktop[-1].un.i4Value);
					stacktop[-1].valueType = IL_TYPE_INT32;
				}
				else if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(ILInt8)(stacktop[-1].un.iValue);
					stacktop[-1].valueType = IL_TYPE_INT32;
				}
				else if((result = ILValueConvI1(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_I2:
			{
				/* Convert the stack top into a signed 16-bit value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(ILInt16)(stacktop[-1].un.i4Value);
					stacktop[-1].valueType = IL_TYPE_INT32;
				}
				else if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(ILInt16)(stacktop[-1].un.iValue);
					stacktop[-1].valueType = IL_TYPE_INT32;
				}
				else if((result = ILValueConvI2(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_I4:
			{
				/* Convert the stack top into a signed 32-bit value */
				if(IL_TYPE_IS_I8(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(stacktop[-1].un.i8Value);
					stacktop[-1].valueType = IL_TYPE_INT32;
				}
				else if((result = ILValueConvI4(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_I:
			{
				/* Convert the stack top into a native integer value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.iValue =
						(ILNativeInt)(stacktop[-1].un.i4Value);
					stacktop[-1].valueType = IL_TYPE_NATIVE_INT;
				}
				else if(IL_TYPE_IS_I8(stacktop[-1].valueType))
				{
					stacktop[-1].un.iValue =
						(ILNativeInt)(stacktop[-1].un.i8Value);
					stacktop[-1].valueType = IL_TYPE_NATIVE_INT;
				}
				else if((result = ILValueConvI(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_I8:
			{
				/* Convert the stack top into a signed 64-bit value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.i8Value =
						(ILInt64)(stacktop[-1].un.i4Value);
					stacktop[-1].valueType = IL_TYPE_INT64;
				}
				else if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.i8Value =
						(ILInt64)(stacktop[-1].un.iValue);
					stacktop[-1].valueType = IL_TYPE_INT64;
				}
				else if((result = ILValueConvI8(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_R4:
			{
				/* Convert the stack top into a 32-bit floating point value */
				if((result = ILValueConvR4(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_R8:
			{
				/* Convert the stack top into a 64-bit floating point value */
				if((result = ILValueConvR8(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_U4:
			{
				/* Convert the stack top into an unsigned 32-bit value */
				if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(ILUInt32)(stacktop[-1].un.iValue);
					stacktop[-1].valueType = IL_TYPE_UINT32;
				}
				else if(IL_TYPE_IS_I8(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(ILUInt32)(ILUInt16)(stacktop[-1].un.i8Value);
					stacktop[-1].valueType = IL_TYPE_UINT32;
				}
				else if((result = ILValueConvU4(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_U:
			{
				/* Convert the stack top into an unsigned native int value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.iValue = (ILNativeInt)
						(ILNativeUInt)(ILUInt32)(stacktop[-1].un.i4Value);
					stacktop[-1].valueType = IL_TYPE_NATIVE_UINT;
				}
				else if(IL_TYPE_IS_I8(stacktop[-1].valueType))
				{
					stacktop[-1].un.iValue = (ILNativeInt)
						(ILNativeUInt)(ILUInt64)(stacktop[-1].un.i8Value);
					stacktop[-1].valueType = IL_TYPE_NATIVE_UINT;
				}
				else if((result = ILValueConvU(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_U8:
			{
				/* Convert the stack top into an unsigned 64-bit value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.i8Value =
						(ILUInt64)(ILUInt32)(stacktop[-1].un.i4Value);
					stacktop[-1].valueType = IL_TYPE_UINT64;
				}
				else if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.i8Value =
						(ILUInt64)(ILNativeUInt)(stacktop[-1].un.iValue);
					stacktop[-1].valueType = IL_TYPE_UINT64;
				}
				else if((result = ILValueConvU8(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CALLVIRT:
			{
				/* Call a virtual method */
				/* TO DO */
			}
			break;

			case IL_OP_CPOBJ:
			{
				/* Copy a value type */
				/* TO DO */
			}
			break;

			case IL_OP_LDOBJ:
			{
				/* Load a value type to the stack */
				/* TO DO */
			}
			break;

			case IL_OP_LDSTR:
			{
				/* Load a string value onto the stack */
				/* TO DO */
			}
			break;

			case IL_OP_NEWOBJ:
			{
				/* Create a new object */
				/* TO DO */
			}
			break;

			case IL_OP_CASTCLASS:
			{
				/* Cast an object to a class */
				/* TO DO */
			}
			break;

			case IL_OP_ISINST:
			{
				/* Test if an object is an instance of a class or interface */
			}
			break;

			case IL_OP_CONV_R_UN:
			{
				/* Convert the unsigned stack top into a floating point value */
				if((result = ILValueConvRUn(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_ANN_DATA_S:
			{
				/* This is a multi-byte nop annotation, used to insert
				   optimization data into the instruction stream.  It isn't
				   of any use to us */
				pc += 2 + (ILUInt32)(pc[1]);
			}
			break;

			case IL_OP_BOX:
			case IL_OP_BOX_OLD:
			{
				/* Convert a value type into an object reference */
				/* TO DO */
			}
			break;

			case IL_OP_UNBOX:
			{
				/* Convert an object reference into a value type */
				/* TO DO */
			}
			break;

			case IL_OP_THROW:
			{
				/* Throw an exception */
				/* TO DO */
			}
			break;

			case IL_OP_LDFLD:
			{
				/* Load a field from an object */
				/* TO DO */
			}
			break;

			case IL_OP_LDFLDA:
			{
				/* Load the address of an object's field */
				/* TO DO */
			}
			break;

			case IL_OP_STFLD:
			{
				/* Store into a field of an object */
				/* TO DO */
			}
			break;

			case IL_OP_LDSFLD:
			{
				/* Load a static field of a class */
				/* TO DO */
			}
			break;

			case IL_OP_LDSFLDA:
			{
				/* Load the address of a static field of a class */
				/* TO DO */
			}
			break;

			case IL_OP_STSFLD:
			{
				/* Store into a static field of a class */
				/* TO DO */
			}
			break;

			case IL_OP_STOBJ:
			{
				/* Store a value type from the stack into memory */
				/* TO DO */
			}
			break;

			case IL_OP_CONV_OVF_I1_UN:
			{
				/* Convert the stack top into an 8-bit value with overflow */
				if((result = ILValueConvOvfI1Un(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_I2_UN:
			{
				/* Convert the stack top into an 16-bit value with overflow */
				if((result = ILValueConvOvfI2Un(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_I4_UN:
			{
				/* Convert the stack top into an 32-bit value with overflow */
				if((result = ILValueConvOvfI4Un(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_I_UN:
			{
				/* Convert the stack top into a native int with overflow */
				if((result = ILValueConvOvfIUn(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_I8_UN:
			{
				/* Convert the stack top into an 64-bit value with overflow */
				if((result = ILValueConvOvfI8Un(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U1_UN:
			{
				/* Convert the stack top into an 8-bit value with overflow */
				if((result = ILValueConvOvfU1Un(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U2_UN:
			{
				/* Convert the stack top into an 16-bit value with overflow */
				if((result = ILValueConvOvfU2Un(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U4_UN:
			{
				/* Convert the stack top into an 32-bit value with overflow */
				if((result = ILValueConvOvfU4Un(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U_UN:
			{
				/* Convert the stack top into a native uint with overflow */
				if((result = ILValueConvOvfUUn(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U8_UN:
			{
				/* Convert the stack top into an 64-bit value with overflow */
				if((result = ILValueConvOvfU8Un(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_NEWARR:
			{
				/* Create a new array */
				/* TO DO */
			}
			break;

			case IL_OP_LDLEN:
			{
				/* Load the length of an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEMA:
			{
				/* Load the address of an array element */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_I1:
			{
				/* Load a signed 8-bit element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_U1:
			{
				/* Load an unsigned 8-bit element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_I2:
			{
				/* Load a signed 16-bit element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_U2:
			{
				/* Load an unsigned 16-bit element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_I4:
			{
				/* Load a 32-bit element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_U4:
			{
				/* Load a 32-bit unsigned element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_I8:
			{
				/* Load a 64-bit element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_I:
			{
				/* Load a native integer element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_R4:
			{
				/* Load a 32-bit floating point element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_R8:
			{
				/* Load a 64-bit floating point element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_LDELEM_REF:
			{
				/* Load an object reference element from an array */
				/* TO DO */
			}
			break;

			case IL_OP_STELEM_I:
			{
				/* Store a native integer element into an array */
				/* TO DO */
			}
			break;

			case IL_OP_STELEM_I1:
			{
				/* Store an 8-bit element into an array */
				/* TO DO */
			}
			break;

			case IL_OP_STELEM_I2:
			{
				/* Store a 16-bit element into an array */
				/* TO DO */
			}
			break;

			case IL_OP_STELEM_I4:
			{
				/* Store a 32-bit element into an array */
				/* TO DO */
			}
			break;

			case IL_OP_STELEM_I8:
			{
				/* Store a 64-bit element into an array */
				/* TO DO */
			}
			break;

			case IL_OP_STELEM_R4:
			{
				/* Store a 32-bit floating point element into an array */
				/* TO DO */
			}
			break;

			case IL_OP_STELEM_R8:
			{
				/* Store a 64-bit floating point element into an array */
				/* TO DO */
			}
			break;

			case IL_OP_STELEM_REF:
			{
				/* Store an object reference element into an array */
				/* TO DO */
			}
			break;

			case IL_OP_CONV_OVF_I1:
			{
				/* Convert the stack top into a 8-bit value with overflow */
				if((result = ILValueConvOvfI1(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U1:
			{
				/* Convert the stack top into a 8-bit value with overflow */
				if((result = ILValueConvOvfU1(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_I2:
			{
				/* Convert the stack top into a 16-bit value with overflow */
				if((result = ILValueConvOvfI2(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U2:
			{
				/* Convert the stack top into a 16-bit value with overflow */
				if((result = ILValueConvOvfU2(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_I4:
			{
				/* Convert the stack top into a 32-bit value with overflow */
				if((result = ILValueConvOvfI4(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_I:
			{
				/* Convert the stack top into a native int with overflow */
				if((result = ILValueConvOvfI(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U4:
			{
				/* Convert the stack top into a 32-bit value with overflow */
				if((result = ILValueConvOvfU4(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U:
			{
				/* Convert the stack top into a native uint with overflow */
				if((result = ILValueConvOvfU4(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
			}
			break;

			case IL_OP_CONV_OVF_I8:
			{
				/* Convert the stack top into a 64-bit value with overflow */
				if((result = ILValueConvOvfI8(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_OVF_U8:
			{
				/* Convert the stack top into a 64-bit value with overflow */
				if((result = ILValueConvOvfU8(&(stacktop[-1]))) !=
						ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_REFANYVAL:
			{
				/* Load the address out of a typed reference */
				/* TO DO */
			}
			break;

			case IL_OP_CKFINITE:
			{
				/* Check for a finite number */
				if(FLOAT_IS_FINITE(stacktop[-1].un.fValue))
				{
					MODIFY_PC_AND_STACK(1, 0);
				}
				else
				{
					ARITHMETIC_EXCEPTION();
				}
			}
			break;

			case IL_OP_MKREFANY:
			{
				/* Push a typed reference on the stack */
				/* TO DO */
			}
			break;

			case IL_OP_ANN_CALL:
			{
				/* Annotations aren't of any use to us, so skip them */
				MODIFY_PC_AND_STACK(5, 0);
			}
			break;

			case IL_OP_ANN_CATCH:
			{
				/* Annotations aren't of any use to us, so skip them */
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_ANN_DEAD:
			{
				/* Annotations aren't of any use to us, so skip them */
				MODIFY_PC_AND_STACK(3, 0);
			}
			break;

			case IL_OP_ANN_HOISTED:
			{
				/* Annotations aren't of any use to us, so skip them */
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_ANN_HOISTED_CALL:
			{
				/* Annotations aren't of any use to us, so skip them */
				MODIFY_PC_AND_STACK(5, 0);
			}
			break;

			case IL_OP_ANN_LAB:
			case IL_OP_ANN_DEF:
			{
				/* Annotations that aren't defined by Microsoft's spec!! */
			}
			break;

			case IL_OP_ANN_REF_S:
			{
				/* Annotations aren't of any use to us, so skip them */
				MODIFY_PC_AND_STACK(2, 0);
			}
			break;

			case IL_OP_ANN_PHI:
			{
				/* Skip a static single assignment annotation, which
				   is variable in length */
				pc += 2 + 2 * ((ILUInt32)(pc[1]));
			}
			break;

			case IL_OP_LDTOKEN:
			{
				/* Load the runtime representation of a metadata token */
				/* TO DO */
			}
			break;

			case IL_OP_CONV_U2:
			{
				/* Convert the stack top into an unsigned 16-bit value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(ILUInt32)(ILUInt16)(stacktop[-1].un.i4Value);
					stacktop[-1].valueType = IL_TYPE_UINT32;
				}
				else if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value = (ILInt32)(ILUInt32)
						(ILUInt16)(ILNativeUInt)(stacktop[-1].un.iValue);
					stacktop[-1].valueType = IL_TYPE_UINT32;
				}
				else if((result = ILValueConvU2(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_CONV_U1:
			{
				/* Convert the stack top into an unsigned 8-bit value */
				if(IL_TYPE_IS_I4(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value =
						(ILInt32)(ILUInt32)(ILUInt8)(stacktop[-1].un.i4Value);
					stacktop[-1].valueType = IL_TYPE_UINT32;
				}
				else if(IL_TYPE_IS_I(stacktop[-1].valueType))
				{
					stacktop[-1].un.i4Value = (ILInt32)(ILUInt32)
						(ILUInt8)(ILNativeUInt)(stacktop[-1].un.iValue);
					stacktop[-1].valueType = IL_TYPE_UINT32;
				}
				else if((result = ILValueConvU1(&(stacktop[-1]))) !=
								ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, 0);
			}
			break;

			case IL_OP_ADD_OVF:
			{
				/* Add two signed values and check for overflow */
				if((result = ILValueAddOvf(&(stacktop[-2]),
										   &(stacktop[-1])))
							!= ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, -1);
			}
			break;

			case IL_OP_ADD_OVF_UN:
			{
				/* Add two unsigned values and check for overflow */
				if((result = ILValueAddOvfUn(&(stacktop[-2]),
										     &(stacktop[-1])))
							!= ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, -1);
			}
			break;

			case IL_OP_MUL_OVF:
			{
				/* Multiply two signed values and check for overflow */
				if((result = ILValueMulOvf(&(stacktop[-2]),
										   &(stacktop[-1])))
							!= ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, -1);
			}
			break;

			case IL_OP_MUL_OVF_UN:
			{
				/* Multiply two unsigned values and check for overflow */
				if((result = ILValueMulOvfUn(&(stacktop[-2]),
										     &(stacktop[-1])))
							!= ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, -1);
			}
			break;

			case IL_OP_SUB_OVF:
			{
				/* Subtract two signed values and check for overflow */
				if((result = ILValueSubOvf(&(stacktop[-2]),
										   &(stacktop[-1])))
							!= ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, -1);
			}
			break;

			case IL_OP_SUB_OVF_UN:
			{
				/* Subtract two unsigned values and check for overflow */
				if((result = ILValueSubOvfUn(&(stacktop[-2]),
										     &(stacktop[-1])))
							!= ILVALUE_RESULT_OK)
				{
					ILVALUE_EXCEPTION();
				}
				MODIFY_PC_AND_STACK(1, -1);
			}
			break;

			case IL_OP_ENDFINALLY:
			{
				/* Return from a finally clause */
				/* TO DO */
			}
			break;

			case IL_OP_LEAVE:
			{
				/* Leave a protected region with a long target address */
				/* TO DO */
			}
			break;

			case IL_OP_LEAVE_S:
			{
				/* Leave a protected region with a short target address */
				/* TO DO */
			}
			break;

			case IL_OP_STIND_I:
			{
				/* This instruction is used in unsafe code */
			}
			break;

			case IL_OP_PREFIX:
			{
				/* Handle a prefixed instruction */
				switch(pc[1])
				{
					case IL_PREFIX_OP_ARGLIST:
					{
						/* Get argument list */
						/* TO DO */
					}
					break;

#define	IL_PREFIX_OP_CMP(name, op, lessop)	\
					case IL_PREFIX_OP_##name: \
					{ \
						if((result = ILValueCompare(&(stacktop[-2]), \
													&(stacktop[-1]), \
													lessop, &cmp)) \
									== ILVALUE_RESULT_OK) \
						{ \
							stacktop[-2].un.i4Value = (cmp op 0); \
							stacktop[-2].valueType = IL_TYPE_INT32; \
						} \
						else \
						{ \
							ILVALUE_EXCEPTION(); \
						} \
						MODIFY_PC_AND_STACK(2, -1); \
					} \
					break

#define	IL_PREFIX_OP_CMP_UN(name, op, lessop)	\
					case IL_PREFIX_OP_##name: \
					{ \
						if((result = ILValueCompareUn(&(stacktop[-2]), \
													  &(stacktop[-1]), \
													  lessop, &cmp)) \
									== ILVALUE_RESULT_OK) \
						{ \
							stacktop[-2].un.i4Value = (cmp op 0); \
							stacktop[-2].valueType = IL_TYPE_INT32; \
						} \
						else \
						{ \
							ILVALUE_EXCEPTION(); \
						} \
						MODIFY_PC_AND_STACK(2, -1); \
					} \
					break

					/* Compare values and push a result code */
					IL_PREFIX_OP_CMP   (CEQ,    ==, 0);
					IL_PREFIX_OP_CMP   (CGT,    >,  1);
					IL_PREFIX_OP_CMP_UN(CGT_UN, >,  1);
					IL_PREFIX_OP_CMP   (CLT,    <,  0);
					IL_PREFIX_OP_CMP_UN(CLT_UN, <,  0);

					case IL_PREFIX_OP_LDFTN:
					{
						/* Load method pointer */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_LDVIRTFTN:
					{
						/* Load a virtual method pointer */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_JMPI:
					{
						/* Jump via method pointer.  This is unsafe */
					}
					break;

					case IL_PREFIX_OP_LDARG:
					{
						/* Load an argument onto the stack */
						*stacktop = args[IL_READ_UINT16(pc + 2)];
						MODIFY_PC_AND_STACK(4, 1);
					}
					break;

					case IL_PREFIX_OP_LDARGA:
					{
						/* Load the address of an argument onto the stack */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_STARG:
					{
						/* Store the top of stack into an argument */
						variable = &(args[IL_READ_UINT16(pc + 2)]);
						actualType = variable->actualType;
						*variable = stacktop[-1];
						ADJUST_STORE(*variable, actualType);
						MODIFY_PC_AND_STACK(4, -1);
					}
					break;

					case IL_PREFIX_OP_LDLOC:
					{
						/* Load a local onto the stack */
						*stacktop = locals[IL_READ_UINT16(pc + 2)];
						MODIFY_PC_AND_STACK(4, 1);
					}
					break;

					case IL_PREFIX_OP_LDLOCA:
					{
						/* Load the address of a local onto the stack */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_STLOC:
					{
						/* Store the top of stack into a local */
						variable = &(locals[IL_READ_UINT16(pc + 2)]);
						actualType = variable->actualType;
						*variable = stacktop[-1];
						ADJUST_STORE(*variable, actualType);
						MODIFY_PC_AND_STACK(4, -1);
					}
					break;

					case IL_PREFIX_OP_LOCALLOC:
					{
						/* Allocate memory from the local dynamic memory pool */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_ENDFILTER:
					{
						/* Return from an exception filter clause */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_UNALIGNED:
					{
						/* Handle unaligned data access */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_VOLATILE:
					{
						/* Volatile doesn't mean anything to the interpreter,
						   so simply skip the prefix */
						MODIFY_PC_AND_STACK(2, 0);
					}
					break;

					case IL_PREFIX_OP_TAIL:
					{
						/* Tail calls are not yet handled.  Skip the prefix
						   and fall through to the following "call" */
						MODIFY_PC_AND_STACK(2, 0);
					}
					break;

					case IL_PREFIX_OP_INITOBJ:
					{
						/* Intialize a value type */
					}
					break;

					case IL_PREFIX_OP_ANN_LIVE:
					{
						/* Annotations aren't of any use to us, so skip them */
						MODIFY_PC_AND_STACK(4, 0);
					}
					break;

					case IL_PREFIX_OP_CPBLK:
					{
						/* Copy data from memory to memory: this is unsafe */
					}
					break;

					case IL_PREFIX_OP_INITBLK:
					{
						/* Initialize a block of memory: this is unsafe */
					}
					break;

					case IL_PREFIX_OP_ANN_REF:
					{
						/* Annotations aren't of any use to us, so skip them */
						MODIFY_PC_AND_STACK(4, 0);
					}
					break;

					case IL_PREFIX_OP_RETHROW:
					{
						/* Rethrow the current exception */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_SIZEOF:
					{
						/* Load the size in bytes of a value type */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_REFANYTYPE:
					{
						/* Load the type out of a typed reference */
						/* TO DO */
					}
					break;

					case IL_PREFIX_OP_ANN_DATA:
					{
						/* This is a multi-byte nop annotation, used to insert
						   optimization data into the instruction stream.
						   It isn't of any use to us */
						pc += 6 + IL_READ_UINT32(pc + 2);
					}
					break;

					case IL_PREFIX_OP_ANN_ARG:
					{
						/* Annotation that isn't defined by
						   Microsoft's specification!! */
					}
					break;

					default:
					{
						/* Unknown prefixed instruction */
					}
					break;
				}
			}
			break;

			default:
			{
				/* This should never happen because the verifier
				   should have already bailed out for this case */
			}
			break;
		}
	}
}

#ifdef	__cplusplus
};
#endif
