/*
 * verify_obj.c - Verify instructions related to objects.
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
 * Get a field token from within a method's code.
 */
static ILField *GetFieldToken(ILMethod *method, unsigned char *pc)
{
	ILUInt32 token;
	ILField *fieldInfo;

	/* Fetch the token from the instruction's arguments */
	if(pc[0] != IL_OP_PREFIX)
	{
		token = IL_READ_UINT32(pc + 1);
	}
	else
	{
		token = IL_READ_UINT32(pc + 2);
	}

	/* Get the token and resolve it */
	fieldInfo = ILProgramItemToField((ILProgramItem *)
						ILImageTokenInfo(ILProgramItem_Image(method), token));
	while(fieldInfo != 0 &&
		  ILMemberGetKind((ILMember *)fieldInfo) == IL_META_MEMBERKIND_REF)
	{
		fieldInfo = (ILField *)ILMemberResolveRef((ILMember *)fieldInfo);
	}
	if(!fieldInfo)
	{
		return 0;
	}

	/* Check the accessibility of the field */
	if(!ILMemberAccessible((ILMember *)fieldInfo, ILMethod_Owner(method)))
	{
		return 0;
	}

	/* Literal fields can never be used with IL instructions because
	   they don't occupy any physical space at runtime.  Their values
	   are supposed to have been expanded by the compiler */
	if(ILField_IsLiteral(fieldInfo))
	{
		return 0;
	}

	/* We have the requested field */
	return fieldInfo;
}

/*
 * Determine if a type is a sub-class of a specific class.
 */
static int IsSubClass(ILType *type, ILClass *classInfo)
{
	ILClass *typeClass;
	if(type == 0)
	{
		/* The type is "null", which is always a sub-class */
		return 1;
	}
	else if(ILType_IsClass(type) || ILType_IsValueType(type))
	{
		typeClass = ILType_ToClass(type);
		if(ILClassInheritsFrom(typeClass, classInfo) ||
		   ILClassImplements(typeClass, classInfo))
		{
			return 1;
		}
		return 0;
	}
	else
	{
		/* TODO: handle array and other types */
		return 0;
	}
}

#elif defined(IL_VERIFY_LOCALS)

ILField *fieldInfo;

#else /* IL_VERIFY_CODE */

case IL_OP_CPOBJ:
{
	/* TODO */
}
break;

case IL_OP_CASTCLASS:
{
	/* Cast an object to a specific class */
	classInfo = GetClassToken(method, pc);
	if(STK_UNARY == ILEngineType_O)
	{
		if(classInfo != 0)
		{
			ILCoderCastClass(coder, classInfo, 1);
			stack[stackSize - 1].typeInfo = ILType_FromClass(classInfo);
		}
		else
		{
			ThrowSystem(coder, method, "TypeLoadException");
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_ISINST:
{
	/* Determine if an object belongs to a specific class */
	classInfo = GetClassToken(method, pc);
	if(STK_UNARY == ILEngineType_O)
	{
		if(classInfo != 0)
		{
			ILCoderCastClass(coder, classInfo, 0);
			stack[stackSize - 1].typeInfo = ILType_FromClass(classInfo);
		}
		else
		{
			ThrowSystem(coder, method, "TypeLoadException");
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_BOX:
{
	/* TODO */
}
break;

case IL_OP_UNBOX:
{
	/* TODO */
}
break;

case IL_OP_LDFLD:
{
	/* Load the contents of an object field.  Note: according to the
	   ECMA spec, it is possible to access a static field by way of
	   "ldfld", "stfld", and "ldflda", even though there are other
	   instructions that are normally used for that.  The only difference
	   is that the type of the object is verified to ensure that it
	   is consistent with the field's type */
	fieldInfo = GetFieldToken(method, pc);
	if(fieldInfo)
	{
		classType = ILField_Type(fieldInfo);
		if(STK_UNARY == ILEngineType_O)
		{
			/* Accessing a field within an object reference */
			if(IsSubClass(stack[stackSize - 1].typeInfo,
						  ILField_Owner(fieldInfo)))
			{
				if(!ILField_IsStatic(fieldInfo))
				{
					ILCoderLoadField(coder, ILEngineType_O,
									 stack[stackSize - 1].typeInfo,
									 fieldInfo, classType);
				}
				else
				{
					ILCoderPop(coder, ILEngineType_O, ILType_Invalid);
					ILCoderLoadStaticField(coder, fieldInfo, classType);
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else if(STK_UNARY == ILEngineType_M ||
				STK_UNARY == ILEngineType_T)
		{
			/* Accessing a field within a pointer to a managed value */
			if(IsSubClass(stack[stackSize - 1].typeInfo,
						  ILField_Owner(fieldInfo)) &&
			   ILTypeIdentical(stack[stackSize - 1].typeInfo, classType))
			{
				if(!ILField_IsStatic(fieldInfo))
				{
					ILCoderLoadField(coder, STK_UNARY,
									 stack[stackSize - 1].typeInfo,
									 fieldInfo, classType);
				}
				else
				{
					ILCoderPop(coder, STK_UNARY, ILType_Invalid);
					ILCoderLoadStaticField(coder, fieldInfo, classType);
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else if(STK_UNARY == ILEngineType_MV)
		{
			/* Accessing a field within a managed value */
			if(IsSubClass(stack[stackSize - 1].typeInfo,
						  ILField_Owner(fieldInfo)) &&
			   ILTypeIdentical(stack[stackSize - 1].typeInfo, classType))
			{
				if(!ILField_IsStatic(fieldInfo))
				{
					ILCoderLoadField(coder, ILEngineType_MV,
									 stack[stackSize - 1].typeInfo,
									 fieldInfo, classType);
				}
				else
				{
					ILCoderPop(coder, ILEngineType_MV,
							   stack[stackSize - 1].typeInfo);
					ILCoderLoadStaticField(coder, fieldInfo, classType);
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else if(unsafeAllowed &&
				(STK_UNARY == ILEngineType_I ||
				 STK_UNARY == ILEngineType_I4))
		{
			/* Accessing a field within an unmanaged pointer.
			   We assume that the types are consistent */
			if(!ILField_IsStatic(fieldInfo))
			{
				ILCoderLoadField(coder, STK_UNARY,
								 stack[stackSize - 1].typeInfo,
								 fieldInfo, classType);
			}
			else
			{
				ILCoderPop(coder, STK_UNARY, ILType_Invalid);
				ILCoderLoadStaticField(coder, fieldInfo, classType);
			}
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
		stack[stackSize - 1].engineType = TypeToEngineType(classType);
		stack[stackSize - 1].typeInfo = classType;
	}
	else
	{
		/* The ECMA spec specifies that an exception should be thrown
		   if the field cannot be found */
		ThrowSystem(coder, method, "MissingFieldException");
	}
}
break;

case IL_OP_LDFLDA:
{
	/* Load the address of an object field */
	fieldInfo = GetFieldToken(method, pc);
	if(fieldInfo)
	{
		classType = ILField_Type(fieldInfo);
		if(STK_UNARY == ILEngineType_O)
		{
			/* Accessing a field within an object reference */
			if(IsSubClass(stack[stackSize - 1].typeInfo,
						  ILField_Owner(fieldInfo)))
			{
				if(!ILField_IsStatic(fieldInfo))
				{
					ILCoderLoadFieldAddr(coder, ILEngineType_O,
									     stack[stackSize - 1].typeInfo,
									     fieldInfo, classType);
				}
				else
				{
					ILCoderPop(coder, ILEngineType_O, ILType_Invalid);
					ILCoderLoadStaticFieldAddr(coder, fieldInfo, classType);
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else if(STK_UNARY == ILEngineType_M ||
				STK_UNARY == ILEngineType_T)
		{
			/* Accessing a field within a pointer to a managed value */
			if(IsSubClass(stack[stackSize - 1].typeInfo,
						  ILField_Owner(fieldInfo)) &&
			   ILTypeIdentical(stack[stackSize - 1].typeInfo, classType))
			{
				if(!ILField_IsStatic(fieldInfo))
				{
					ILCoderLoadFieldAddr(coder, STK_UNARY,
									     stack[stackSize - 1].typeInfo,
									     fieldInfo, classType);
				}
				else
				{
					ILCoderPop(coder, STK_UNARY, ILType_Invalid);
					ILCoderLoadStaticFieldAddr(coder, fieldInfo, classType);
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else if(unsafeAllowed &&
				(STK_UNARY == ILEngineType_I ||
				 STK_UNARY == ILEngineType_I4))
		{
			/* Accessing a field within an unmanaged pointer.
			   We assume that the types are consistent */
			if(!ILField_IsStatic(fieldInfo))
			{
				ILCoderLoadFieldAddr(coder, STK_UNARY,
								     stack[stackSize - 1].typeInfo,
								     fieldInfo, classType);
			}
			else
			{
				ILCoderPop(coder, STK_UNARY, ILType_Invalid);
				ILCoderLoadStaticFieldAddr(coder, fieldInfo, classType);
			}

			/* Taking the address of a field within an unmanaged
			   pointer always returns an unmanaged pointer */
			stack[stackSize - 1].engineType = ILEngineType_I;
			stack[stackSize - 1].typeInfo = 0;
			continue;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
		stack[stackSize - 1].engineType = ILEngineType_M;
		stack[stackSize - 1].typeInfo = classType;
	}
	else
	{
		/* The ECMA spec specifies that an exception should be thrown
		   if the field cannot be found */
		ThrowSystem(coder, method, "MissingFieldException");
	}
}
break;

case IL_OP_STFLD:
{
	/* Store a value into an object field */
	fieldInfo = GetFieldToken(method, pc);
	if(fieldInfo)
	{
		classType = ILField_Type(fieldInfo);
		if(STK_BINARY_1 == ILEngineType_O)
		{
			/* Accessing a field within an object reference */
			if(IsSubClass(stack[stackSize - 2].typeInfo,
						  ILField_Owner(fieldInfo)) &&
			   AssignCompatible(&(stack[stackSize - 1]), classType))
			{
				if(!ILField_IsStatic(fieldInfo))
				{
					ILCoderStoreField(coder, ILEngineType_O,
									  stack[stackSize - 2].typeInfo,
									  fieldInfo, classType,
									  STK_BINARY_2);
				}
				else
				{
					ILCoderStoreStaticField(coder, fieldInfo, classType,
											STK_BINARY_2);
					ILCoderPop(coder, ILEngineType_O, ILType_Invalid);
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else if(STK_BINARY_1 == ILEngineType_M ||
				STK_BINARY_1 == ILEngineType_T)
		{
			/* Accessing a field within a pointer to a managed value */
			if(IsSubClass(stack[stackSize - 2].typeInfo,
						  ILField_Owner(fieldInfo)) &&
			   ILTypeIdentical(stack[stackSize - 2].typeInfo, classType))
			{
				if(!ILField_IsStatic(fieldInfo))
				{
					ILCoderStoreField(coder, STK_BINARY_1,
									  stack[stackSize - 2].typeInfo,
									  fieldInfo, classType, STK_BINARY_2);
				}
				else
				{
					ILCoderStoreStaticField(coder, fieldInfo, classType,
											STK_BINARY_2);
					ILCoderPop(coder, STK_BINARY_2, ILType_Invalid);
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else if(unsafeAllowed &&
				(STK_BINARY_1 == ILEngineType_I ||
				 STK_BINARY_1 == ILEngineType_I4))
		{
			/* Accessing a field within an unmanaged pointer.
			   We assume that the types are consistent */
			if(!ILField_IsStatic(fieldInfo))
			{
				ILCoderStoreField(coder, STK_BINARY_1,
								  stack[stackSize - 2].typeInfo,
								  fieldInfo, classType, STK_BINARY_2);
			}
			else
			{
				ILCoderStoreStaticField(coder, fieldInfo, classType,
										STK_BINARY_2);
				ILCoderPop(coder, STK_BINARY_1, ILType_Invalid);
			}
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
		stackSize -= 2;
	}
	else
	{
		/* The ECMA spec specifies that an exception should be thrown
		   if the field cannot be found */
		ThrowSystem(coder, method, "MissingFieldException");
	}
}
break;

case IL_OP_LDSFLD:
{
	/* Load the contents of a static field */
	fieldInfo = GetFieldToken(method, pc);
	if(fieldInfo)
	{
		classType = ILField_Type(fieldInfo);
		ILCoderLoadStaticField(coder, fieldInfo, classType);
		stack[stackSize - 1].engineType = TypeToEngineType(classType);
		stack[stackSize - 1].typeInfo = classType;
	}
	else
	{
		/* The ECMA spec specifies that an exception should be thrown
		   if the field cannot be found */
		ThrowSystem(coder, method, "MissingFieldException");
	}
}
break;

case IL_OP_LDSFLDA:
{
	/* Load the address of a static field */
	fieldInfo = GetFieldToken(method, pc);
	if(fieldInfo)
	{
		classType = ILField_Type(fieldInfo);
		ILCoderLoadStaticFieldAddr(coder, fieldInfo, classType);
		stack[stackSize - 1].engineType = ILEngineType_M;
		stack[stackSize - 1].typeInfo = classType;
	}
	else
	{
		/* The ECMA spec specifies that an exception should be thrown
		   if the field cannot be found */
		ThrowSystem(coder, method, "MissingFieldException");
	}
}
break;

case IL_OP_STSFLD:
{
	/* Store a value into a static field */
	fieldInfo = GetFieldToken(method, pc);
	if(fieldInfo)
	{
		classType = ILField_Type(fieldInfo);
		if(AssignCompatible(&(stack[stackSize - 1]), classType))
		{
			ILCoderStoreStaticField(coder, fieldInfo, classType, STK_UNARY);
			stackSize -= 2;
		}
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else
	{
		/* The ECMA spec specifies that an exception should be thrown
		   if the field cannot be found */
		ThrowSystem(coder, method, "MissingFieldException");
	}
}
break;

case IL_OP_REFANYVAL:
{
	/* TODO */
}
break;

case IL_OP_MKREFANY:
{
	/* TODO */
}
break;

case IL_OP_LDTOKEN:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_ARGLIST:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_INITOBJ:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_CPBLK:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_INITBLK:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_SIZEOF:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_REFANYTYPE:
{
	/* TODO */
}
break;

#endif /* IL_VERIFY_CODE */
