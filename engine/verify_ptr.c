/*
 * verify_ptr.c - Verify instructions related to pointers and arrays.
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
 * Helper macros for getting the types of the top three
 * stack elements for ternary operators.
 */
#define	STK_TERNARY_1	(stack[stackSize - 3].engineType)
#define	STK_TERNARY_2	(stack[stackSize - 2].engineType)
#define	STK_TERNARY_3	(stack[stackSize - 1].engineType)

/*
 * Determine if "stackType" is compatible with "insnType".
 */
static int PtrCompatible(ILType *stackType, ILType *insnType)
{
	/* Resolve enumerations */
	stackType = ILTypeGetEnumType(stackType);

	/* Bail out early if the types are compatible */
	if(stackType == insnType)
	{
		return 1;
	}

	/* Test for signed/unsigned variants */
	if(stackType == ILType_Boolean || stackType == ILType_Int8 ||
	   stackType == ILType_UInt8)
	{
		return (insnType == ILType_Int8 || insnType == ILType_UInt8);
	}
	if(stackType == ILType_Int16 || stackType == ILType_UInt16 ||
	   stackType == ILType_Char)
	{
		return (insnType == ILType_Int16 || insnType == ILType_UInt16);
	}
	if(stackType == ILType_Int32 || stackType == ILType_UInt32)
	{
		return (insnType == ILType_Int32 || insnType == ILType_UInt32);
	}
	if(stackType == ILType_Int64 || stackType == ILType_UInt64)
	{
		return (insnType == ILType_Int64 || insnType == ILType_UInt64);
	}
	if(stackType == ILType_Int || stackType == ILType_UInt)
	{
		return (insnType == ILType_Int || insnType == ILType_UInt);
	}

	/* The types are not compatible */
	return 0;
}

/*
 * Determine if a type is a zero-based, single-dimensional array, and
 * get its element type.  Returns NULL if not a suitable type, or
 * ILType_Void if the type is "null".  Note: the signature parser in
 * "image/sig_parse.c" ensures that it is impossible to construct an
 * array whose element type is "void".
 */
static ILType *ArrayElementType(ILType *type)
{
	if(type == 0)
	{
		/* The type of the array is "null" */
		return ILType_Void;
	}
	else if(ILType_IsSimpleArray(type))
	{
		/* Single-dimensional array with a lower bound of zero */
		return ILType_ElemType(type);
	}
	else
	{
		/* Not a suitable array type */
		return 0;
	}
}

/*
 * Get a class token from the instruction stream.  Returns NULL
 * if not an accessible class token for the current method.
 */
static ILClass *GetClassToken(ILMethod *method, unsigned char *pc)
{
	ILUInt32 token;
	ILClass *classInfo;

	/* Fetch the token from the instruction's arguments */
	if(pc[0] != IL_OP_PREFIX)
	{
		token = IL_READ_UINT32(pc + 1);
	}
	else
	{
		token = IL_READ_UINT32(pc + 2);
	}

	/* The token must be a TypeRef, TypeDef, or TypeSpec */
	if((token & IL_META_TOKEN_MASK) != IL_META_TOKEN_TYPE_REF &&
	   (token & IL_META_TOKEN_MASK) != IL_META_TOKEN_TYPE_DEF &&
	   (token & IL_META_TOKEN_MASK) != IL_META_TOKEN_TYPE_SPEC)
	{
		return 0;
	}

	/* Get the token and resolve it */
	classInfo = ILProgramItemToClass
		((ILProgramItem *)ILImageTokenInfo(ILProgramItem_Image(method), token));
	if(classInfo)
	{
		classInfo = ILClassResolve(classInfo);
	}
	if(!classInfo || ILClassIsRef(classInfo))
	{
		return 0;
	}

	/* Check the accessibility of the class */
	if(!ILClassAccessible(classInfo, ILMethod_Owner(method)))
	{
		return 0;
	}

	/* We have the requested class */
	return classInfo;
}

/*
 * Get a value type token, based on a TypeDef or TypeRef.
 * Returns NULL if the token is not a value type.
 */
static ILClass *GetValueTypeToken(ILMethod *method, unsigned char *pc)
{
	ILClass *classInfo = GetClassToken(method, pc);
	if(!classInfo)
	{
		return 0;
	}
	if(!ILClassIsValueType(classInfo))
	{
		return 0;
	}
	return classInfo;
}

/*
 * Get the type form of a TypeDef or TypeRef token.
 */
static ILType *GetTypeToken(ILMethod *method, unsigned char *pc)
{
	ILClass *classInfo = GetClassToken(method, pc);
	if(classInfo)
	{
		return ILClassToType(classInfo);
	}
	else
	{
		return 0;
	}
}

#elif defined(IL_VERIFY_LOCALS)

ILType *elemType;
ILClass *classInfo;
ILType *classType;

#else /* IL_VERIFY_CODE */

#define	VERIFY_LDIND(name,type,engineType)	\
case IL_OP_LDIND_##name: \
{ \
	if(STK_UNARY == ILEngineType_M || STK_UNARY == ILEngineType_T) \
	{ \
		if(unsafeAllowed || \
		   PtrCompatible(stack[stackSize - 1].typeInfo, (type))) \
		{ \
			ILCoderPtrAccess(coder, opcode); \
			STK_UNARY = (engineType); \
			STK_UNARY_TYPEINFO = 0; \
		} \
		else \
		{ \
			VERIFY_TYPE_ERROR(); \
		} \
	} \
	else if(unsafeAllowed && \
	        (STK_UNARY == ILEngineType_I4 || STK_UNARY == ILEngineType_I)) \
	{ \
		ILCoderToPointer(coder, STK_UNARY, (ILEngineStackItem *)0); \
		ILCoderPtrAccess(coder, opcode); \
		STK_UNARY = (engineType); \
		STK_UNARY_TYPEINFO = 0; \
	} \
	else \
	{ \
		VERIFY_TYPE_ERROR(); \
	} \
} \
break

#define	VERIFY_STIND(name,type,engineType)	\
case IL_OP_STIND_##name: \
{ \
	if(STK_BINARY_1 == ILEngineType_M || STK_BINARY_1 == ILEngineType_T) \
	{ \
		if(unsafeAllowed || \
		   (STK_BINARY_2 == (engineType) && \
		    PtrCompatible(stack[stackSize - 2].typeInfo, (type)))) \
		{ \
			ILCoderPtrAccess(coder, opcode); \
			stackSize -= 2; \
		} \
		else \
		{ \
			VERIFY_TYPE_ERROR(); \
		} \
	} \
	else if(unsafeAllowed && \
	        (STK_BINARY_1 == ILEngineType_I4 || \
			 STK_BINARY_1 == ILEngineType_I) && \
			(STK_BINARY_2 == (engineType) || \
			 ((engineType) == ILEngineType_I && \
			  (STK_BINARY_2 == ILEngineType_M || \
			   STK_BINARY_2 == ILEngineType_T)))) \
	{ \
		ILCoderToPointer(coder, STK_BINARY_1, &(stack[stackSize - 1])); \
		ILCoderPtrAccess(coder, opcode); \
		stackSize -= 2; \
	} \
	else \
	{ \
		VERIFY_TYPE_ERROR(); \
	} \
} \
break

#define	VERIFY_LDELEM(name,type,engineType)	\
case IL_OP_LDELEM_##name: \
{ \
	if(STK_BINARY_1 == ILEngineType_O && \
	   (STK_BINARY_2 == ILEngineType_I4 || STK_BINARY_2 == ILEngineType_I) && \
	   (elemType = ArrayElementType(stack[stackSize - 2].typeInfo)) != 0) \
	{ \
		if(elemType == ILType_Void || PtrCompatible(elemType, (type))) \
		{ \
			ILCoderArrayAccess(coder, opcode, STK_BINARY_2, elemType); \
			STK_BINARY_1 = (engineType); \
			STK_TYPEINFO_1 = 0; \
			--stackSize; \
		} \
		else \
		{ \
			VERIFY_TYPE_ERROR(); \
		} \
	} \
	else \
	{ \
		VERIFY_TYPE_ERROR(); \
	} \
} \
break

#define	VERIFY_STELEM(name,type,engineType)	\
case IL_OP_STELEM_##name: \
{ \
	if(STK_TERNARY_1 == ILEngineType_O && \
	   (STK_TERNARY_2 == ILEngineType_I4 || \
	    STK_TERNARY_2 == ILEngineType_I) && \
	   (elemType = ArrayElementType(stack[stackSize - 3].typeInfo)) != 0 && \
	   STK_TERNARY_3 == (engineType)) \
	{ \
		if(elemType == ILType_Void || PtrCompatible(elemType, (type))) \
		{ \
			ILCoderArrayAccess(coder, opcode, STK_TERNARY_2, elemType); \
			stackSize -= 3; \
		} \
		else \
		{ \
			VERIFY_TYPE_ERROR(); \
		} \
	} \
	else \
	{ \
		VERIFY_TYPE_ERROR(); \
	} \
} \
break

VERIFY_LDIND(I1, ILType_Int8,    ILEngineType_I4);
VERIFY_LDIND(U1, ILType_UInt8,   ILEngineType_I4);
VERIFY_LDIND(I2, ILType_Int16,   ILEngineType_I4);
VERIFY_LDIND(U2, ILType_UInt16,  ILEngineType_I4);
VERIFY_LDIND(I4, ILType_Int32,   ILEngineType_I4);
VERIFY_LDIND(U4, ILType_UInt32,  ILEngineType_I4);
VERIFY_LDIND(I8, ILType_Int64,   ILEngineType_I8);
VERIFY_LDIND(I,  ILType_Int,     ILEngineType_I);
VERIFY_LDIND(R4, ILType_Float32, ILEngineType_F);
VERIFY_LDIND(R8, ILType_Float64, ILEngineType_F);

VERIFY_STIND(I1, ILType_Int8,    ILEngineType_I4);
VERIFY_STIND(I2, ILType_Int16,   ILEngineType_I4);
VERIFY_STIND(I4, ILType_Int32,   ILEngineType_I4);
VERIFY_STIND(I8, ILType_Int64,   ILEngineType_I8);
VERIFY_STIND(I,  ILType_Int,     ILEngineType_I);
VERIFY_STIND(R4, ILType_Float32, ILEngineType_F);
VERIFY_STIND(R8, ILType_Float64, ILEngineType_F);

case IL_OP_LDIND_REF:
{
	/* Load an object reference from a pointer */
	if(STK_UNARY == ILEngineType_M || STK_UNARY == ILEngineType_T)
	{
		if(IsObjectRef(stack[stackSize - 1].typeInfo))
		{
			ILCoderPtrAccess(coder, opcode);
			stack[stackSize - 1].engineType = ILEngineType_O;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else if(unsafeAllowed &&
	        (STK_UNARY == ILEngineType_I4 || STK_UNARY == ILEngineType_I))
	{
		/* Note: we set the type of the object reference to "null",
		   which makes it compatible with any object destination.
		   There really is nothing else that we can do because there
		   is no way to determine the actual type */
		ILCoderToPointer(coder, STK_UNARY, (ILEngineStackItem *)0);
		ILCoderPtrAccess(coder, opcode);
		stack[stackSize - 1].engineType = ILEngineType_O;
		stack[stackSize - 1].typeInfo = 0;
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_STIND_REF:
{
	/* Store an object reference to a pointer */
	if(STK_BINARY_1 == ILEngineType_M || STK_BINARY_1 == ILEngineType_T)
	{
		if(STK_BINARY_2 == ILEngineType_O &&
		   IsObjectRef(stack[stackSize - 2].typeInfo))
		{
	   		if(AssignCompatible(method, &(stack[stackSize - 1]),
							    stack[stackSize - 2].typeInfo,
								unsafeAllowed))
			{
				ILCoderPtrAccess(coder, opcode);
				stackSize -= 2;
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else if(unsafeAllowed &&
	        (STK_BINARY_1 == ILEngineType_I4 ||
			 STK_BINARY_1 == ILEngineType_I) &&
			STK_BINARY_2 == ILEngineType_O)
	{
		ILCoderToPointer(coder, STK_BINARY_1, &(stack[stackSize - 1]));
		ILCoderPtrAccess(coder, opcode);
		stackSize -= 2;
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_LDOBJ:
{
	/* Load a value type from a pointer */
	classInfo = GetValueTypeToken(method, pc);
	classType = (classInfo ? ILClassToType(classInfo) : 0);
	if(STK_UNARY == ILEngineType_M || STK_UNARY == ILEngineType_T)
	{
		if(classInfo &&
		   ILTypeIdentical(stack[stackSize - 1].typeInfo, classType))
		{
			ILCoderPtrAccessManaged(coder, opcode, classInfo);
			stack[stackSize - 1].engineType = TypeToEngineType(classType);
			stack[stackSize - 1].typeInfo = classType;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else if(unsafeAllowed &&
	        (STK_UNARY == ILEngineType_I4 || STK_UNARY == ILEngineType_I))
	{
		if(classInfo)
		{
			ILCoderToPointer(coder, STK_UNARY, (ILEngineStackItem *)0);
			ILCoderPtrAccessManaged(coder, opcode, classInfo);
			stack[stackSize - 1].engineType = TypeToEngineType(classType);
			stack[stackSize - 1].typeInfo = classType;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_STOBJ:
{
	/* Store a value type to a pointer */
	classInfo = GetValueTypeToken(method, pc);
	if(STK_BINARY_1 == ILEngineType_M || STK_BINARY_1 == ILEngineType_T)
	{
		/* NOTE: ILTypeIdentical(stack[stackSize - 1].typeInfo,
		   				    ILType_FromValueType(classInfo))
		   was removed as ECMA spec leaves that check as unspecified. */
		if((STK_BINARY_2 == ILEngineType_MV || STK_BINARY_2 == ILEngineType_I)
			&& classInfo)
		{
			ILCoderPtrAccessManaged(coder, opcode, classInfo);
			stackSize -= 2;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else if(unsafeAllowed &&
	        (STK_BINARY_1 == ILEngineType_I4 ||
			 STK_BINARY_1 == ILEngineType_I))
	{
		if(STK_BINARY_2 == ILEngineType_MV && classInfo &&
		   ILTypeIdentical(stack[stackSize - 1].typeInfo,
		   				   ILType_FromValueType(classInfo)))
		{
			ILCoderToPointer(coder, STK_BINARY_1, &(stack[stackSize - 1]));
			ILCoderPtrAccessManaged(coder, opcode, classInfo);
			stackSize -= 2;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_UNALIGNED:
{
	/* Process an "unaligned" prefix for pointer access */
	if(pc[3] == IL_OP_LDIND_I1  || pc[3] == IL_OP_LDIND_U1  ||
	   pc[3] == IL_OP_LDIND_I2  || pc[3] == IL_OP_LDIND_U2  ||
	   pc[3] == IL_OP_LDIND_I4  || pc[3] == IL_OP_LDIND_U4  ||
	   pc[3] == IL_OP_LDIND_I8  || pc[3] == IL_OP_LDIND_I   ||
	   pc[3] == IL_OP_LDIND_R4  || pc[3] == IL_OP_LDIND_R8  ||
	   pc[3] == IL_OP_LDIND_REF || pc[3] == IL_OP_LDIND_REF ||
	   pc[3] == IL_OP_STIND_I1  || pc[3] == IL_OP_STIND_I2  ||
	   pc[3] == IL_OP_STIND_I4  || pc[3] == IL_OP_STIND_I8  ||
	   pc[3] == IL_OP_STIND_I   || pc[3] == IL_OP_STIND_R4  ||
	   pc[3] == IL_OP_STIND_R8  || pc[3] == IL_OP_STIND_REF ||
	   pc[3] == IL_OP_LDFLD     || pc[3] == IL_OP_STFLD     ||
	   pc[3] == IL_OP_LDOBJ     || pc[3] == IL_OP_STOBJ     ||
	   (pc[3] == IL_OP_PREFIX && pc[4] == IL_PREFIX_OP_INITBLK) ||
	   (pc[3] == IL_OP_PREFIX && pc[4] == IL_PREFIX_OP_CPBLK) ||
	   (pc[3] == IL_OP_PREFIX && pc[4] == IL_PREFIX_OP_UNALIGNED) ||
	   (pc[3] == IL_OP_PREFIX && pc[4] == IL_PREFIX_OP_VOLATILE))
	{
		if(pc[2] == 1 || pc[2] == 2 || pc[2] == 4)
		{
			ILCoderPtrPrefix(coder, (int)(pc[2]));
		}
		else
		{
			VERIFY_INSN_ERROR();
		}
	}
	else
	{
		VERIFY_INSN_ERROR();
	}
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_VOLATILE:
{
	/* Process a "volatile" prefix for pointer access */
	if(pc[2] == IL_OP_LDIND_I1  || pc[2] == IL_OP_LDIND_U1  ||
	   pc[2] == IL_OP_LDIND_I2  || pc[2] == IL_OP_LDIND_U2  ||
	   pc[2] == IL_OP_LDIND_I4  || pc[2] == IL_OP_LDIND_U4  ||
	   pc[2] == IL_OP_LDIND_I8  || pc[2] == IL_OP_LDIND_I   ||
	   pc[2] == IL_OP_LDIND_R4  || pc[2] == IL_OP_LDIND_R8  ||
	   pc[2] == IL_OP_LDIND_REF || pc[2] == IL_OP_LDIND_REF ||
	   pc[2] == IL_OP_STIND_I1  || pc[2] == IL_OP_STIND_I2  ||
	   pc[2] == IL_OP_STIND_I4  || pc[2] == IL_OP_STIND_I8  ||
	   pc[2] == IL_OP_STIND_I   || pc[2] == IL_OP_STIND_R4  ||
	   pc[2] == IL_OP_STIND_R8  || pc[2] == IL_OP_STIND_REF ||
	   pc[2] == IL_OP_LDFLD     || pc[2] == IL_OP_STFLD     ||
	   pc[2] == IL_OP_LDOBJ     || pc[2] == IL_OP_STOBJ     ||
	   pc[2] == IL_OP_LDSFLD    || pc[2] == IL_OP_STSFLD    ||
	   (pc[2] == IL_OP_PREFIX && pc[3] == IL_PREFIX_OP_INITBLK) ||
	   (pc[2] == IL_OP_PREFIX && pc[3] == IL_PREFIX_OP_CPBLK) ||
	   (pc[2] == IL_OP_PREFIX && pc[3] == IL_PREFIX_OP_UNALIGNED) ||
	   (pc[2] == IL_OP_PREFIX && pc[3] == IL_PREFIX_OP_VOLATILE))
	{
		ILCoderPtrPrefix(coder, 0);
	}
	else
	{
		VERIFY_INSN_ERROR();
	}
}
break;

case IL_OP_NEWARR:
{
	/* Create a new array */
	classType = GetTypeToken(method, pc);
	if(classType != 0 &&
	   (STK_UNARY == ILEngineType_I || STK_UNARY == ILEngineType_I4))
	{
		classType = ILTypeFindOrCreateArray
				(ILImageToContext(ILProgramItem_Image(method)), 1, classType);
		if(!classType)
		{
			VERIFY_MEMORY_ERROR();
		}
		classInfo = ILClassFromType(ILProgramItem_Image(method),
									0, classType, 0);
		if(!classInfo)
		{
			VERIFY_MEMORY_ERROR();
		}
		classInfo = ILClassResolve(classInfo);
		ILCoderNewArray(coder, classType, classInfo, STK_UNARY);
		stack[stackSize - 1].engineType = ILEngineType_O;
		stack[stackSize - 1].typeInfo = classType;
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_LDLEN:
{
	/* Get the length of a zero-based, single-dimensional array */
	if(STK_UNARY == ILEngineType_O &&
	   ArrayElementType(stack[stackSize - 1].typeInfo) != 0)
	{
		ILCoderArrayLength(coder);
		STK_UNARY = ILEngineType_I;
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

VERIFY_LDELEM(I1, ILType_Int8,    ILEngineType_I4);
VERIFY_LDELEM(U1, ILType_UInt8,   ILEngineType_I4);
VERIFY_LDELEM(I2, ILType_Int16,   ILEngineType_I4);
VERIFY_LDELEM(U2, ILType_UInt16,  ILEngineType_I4);
VERIFY_LDELEM(I4, ILType_Int32,   ILEngineType_I4);
VERIFY_LDELEM(U4, ILType_UInt32,  ILEngineType_I4);
VERIFY_LDELEM(I8, ILType_Int64,   ILEngineType_I8);
VERIFY_LDELEM(I,  ILType_Int,     ILEngineType_I);
VERIFY_LDELEM(R4, ILType_Float32, ILEngineType_F);
VERIFY_LDELEM(R8, ILType_Float64, ILEngineType_F);

VERIFY_STELEM(I1, ILType_Int8,    ILEngineType_I4);
VERIFY_STELEM(I2, ILType_Int16,   ILEngineType_I4);
VERIFY_STELEM(I4, ILType_Int32,   ILEngineType_I4);
VERIFY_STELEM(I8, ILType_Int64,   ILEngineType_I8);
VERIFY_STELEM(I,  ILType_Int,     ILEngineType_I);
VERIFY_STELEM(R4, ILType_Float32, ILEngineType_F);
VERIFY_STELEM(R8, ILType_Float64, ILEngineType_F);

case IL_OP_LDELEM_REF:
{
	/* Load an object reference from an array element */
	if(STK_BINARY_1 == ILEngineType_O &&
	   (STK_BINARY_2 == ILEngineType_I4 || STK_BINARY_2 == ILEngineType_I) &&
	   (elemType = ArrayElementType(stack[stackSize - 2].typeInfo)) != 0)
	{
		if(elemType == ILType_Void || IsObjectRef(elemType))
		{
			ILCoderArrayAccess(coder, opcode, STK_BINARY_2, elemType);
			stack[stackSize - 2].engineType = ILEngineType_O;
			stack[stackSize - 2].typeInfo = elemType;
			--stackSize;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_LDELEMA:
{
	/* Load the address of an array element */
	if(STK_BINARY_1 == ILEngineType_O &&
	   (STK_BINARY_2 == ILEngineType_I4 || STK_BINARY_2 == ILEngineType_I) &&
	   (elemType = ArrayElementType(stack[stackSize - 2].typeInfo)) != 0)
	{
		classType = GetTypeToken(method, pc);
		if(classType &&
		   (elemType == ILType_Void || ILTypeIdentical(elemType, classType)))
		{
			ILCoderArrayAccess(coder, opcode, STK_BINARY_2, classType);
			stack[stackSize - 2].engineType = ILEngineType_M;
			stack[stackSize - 2].typeInfo = classType;
			--stackSize;
		}
		else
		{
			/* The ECMA instruction specification says that this instruction
			   should throw a run-time exception if the type is incorrect,
			   rather than bailing out during method verification */
			ThrowSystem("System", "ArrayTypeMismatchException");
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_STELEM_REF:
{
	/* Store an object reference to an array element */
	if(STK_TERNARY_1 == ILEngineType_O &&
	   (STK_TERNARY_2 == ILEngineType_I4 ||
	    STK_TERNARY_2 == ILEngineType_I) &&
	   (elemType = ArrayElementType(stack[stackSize - 3].typeInfo)) != 0 &&
	   STK_TERNARY_3 == ILEngineType_O)
	{
		if(elemType == ILType_Void ||
		   AssignCompatible(method, &(stack[stackSize - 1]),
		   					elemType, unsafeAllowed))
		{
			ILCoderArrayAccess(coder, opcode, STK_TERNARY_2, elemType);
			stackSize -= 3;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

#endif /* IL_VERIFY_CODE */
