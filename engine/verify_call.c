/*
 * verify_call.c - Verify instructions related to method calls.
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
 * Get a method token from within a method's code.
 */
static ILMethod *GetMethodToken(ILMethod *method, unsigned char *pc)
{
	ILUInt32 token;
	ILMethod *methodInfo;

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
	methodInfo = ILProgramItemToMethod((ILProgramItem *)
						ILImageTokenInfo(ILProgramItem_Image(method), token));
	if(!methodInfo)
	{
		return 0;
	}
	methodInfo = (ILMethod *)ILMemberResolve((ILMember *)methodInfo);

	/* We have the requested method */
	return methodInfo;
}

/*
 * Get a constructor token from within a method's code.
 */
static ILMethod *GetConstructorToken(ILMethod *method, unsigned char *pc)
{
	ILMethod *methodInfo = GetMethodToken(method, pc);

	/* Bail out if no method found */
	if(!methodInfo)
	{
		return 0;
	}

	/* Bail out if the method is abstract */
	if(ILMethod_IsAbstract(methodInfo))
	{
		return 0;
	}

	/* Bail out if the method is not accessible */
	if(!ILMemberAccessible((ILMember *)methodInfo, ILMethod_Owner(method)))
	{
		return 0;
	}

	/* Bail out if the method is not a constructor */
	if(!ILMethod_IsConstructor(methodInfo))
	{
		return 0;
	}

	/* We have a legitimate constructor */
	return methodInfo;
}

/*
 * Match a method signature against the contents of the stack.
 * Returns -1 if a type error has been detected, or the number
 * of parameters to be popped otherwise.
 *
 * TODO: handle vararg method calls.
 */
static ILInt32 MatchSignature(ILCoder *coder, ILEngineStackItem *stack,
						      ILUInt32 stackSize, ILType *signature,
						      ILMethod *method, int unsafeAllowed,
							  int suppressThis)
{
	ILClass *owner = (method ? ILMethod_Owner(method) : 0);
	ILUInt32 numParams = signature->num;
	ILUInt32 param;
	ILType *paramType;
	ILEngineStackItem *item;
	int hasThis;
	int isValueThis;

	/* Determine if the signature needs an extra "this" parameter */
	hasThis = (ILType_HasThis(signature) && !suppressThis);
	if(hasThis)
	{
		++numParams;
		if(!method)
		{
			/* Call site signatures need "explicit this" */
			return -1;
		}
		if(ILClassIsValueType(owner) && !ILMethod_IsVirtual(method))
		{
			/* The "this" parameter is a value type, which must be
			   passed as either a managed or transient pointer */
			isValueThis = 1;
		}
		else
		{
			/* The "this" parameter is an object reference */
			isValueThis = 0;
		}
	}
	else
	{
		isValueThis = 0;
	}

	/* Validate the stack size */
	if(stackSize < numParams)
	{
		/* Insufficient parameters to the call */
		return -1;
	}

	/* Find the base of the parameters */
	stack += (stackSize - numParams);

	/* Match the argument signature */
	for(param = 1; param <= numParams; ++param)
	{
		/* Get the stack item corresponding to the parameter */
		item = &(stack[param - 1]);

		/* Get the parameter type and expand out enumerations */
		if(hasThis)
		{
			if(param == 1)
			{
				/* The "this" parameter needs to be handled specially */
				if(isValueThis)
				{
					/* The "this" parameter must be a pointer */
					paramType = ILType_FromValueType(owner);
					if(item->engineType == ILEngineType_T ||
					   item->engineType == ILEngineType_M)
					{
						if(!ILTypeIdentical(item->typeInfo, paramType))
						{
							return -1;
						}
					}
					else if(!unsafeAllowed ||
							item->engineType != ILEngineType_I)
					{
						return -1;
					}
				}
				else
				{
					/* The "this" parameter must be an object reference */
					paramType = ILType_FromClass(owner);
					if(item->engineType != ILEngineType_O ||
					   (item->typeInfo != 0 &&
					    !AssignCompatible(item, paramType)))
					{
						return -1;
					}
				}
				continue;
			}
			else
			{
				paramType = ILTypeGetParam(signature, param - 1);
			}
		}
		else
		{
			paramType = ILTypeGetParam(signature, param);
		}
		paramType = ILTypeGetEnumType(paramType);

		/* Determine what to do based on the supplied stack item */
		switch(item->engineType)
		{
			case ILEngineType_I4:
			{
				/* The supplied value is I4 */
				if(paramType == ILType_Int8 ||
				   paramType == ILType_UInt8 ||
				   paramType == ILType_Boolean ||
				   paramType == ILType_Char ||
				   paramType == ILType_Int16 ||
				   paramType == ILType_UInt16)
				{
					/* The method itself is responsible for
					   down-converting to these types */
				}
				else if(paramType == ILType_Int32 ||
						paramType == ILType_UInt32)
				{
					/* Direct 32-bit conversion is OK */
				}
				else if(paramType == ILType_Int ||
						paramType == ILType_UInt)
				{
					/* We must up-convert from I4 to I */
					ILCoderUpConvertArg(coder, stack, numParams,
										param, paramType);
					item->engineType = ILEngineType_I;
				}
				else if(paramType == ILType_Int64 ||
						paramType == ILType_UInt64)
				{
					/* We must up-convert from I4 to I8 */
					ILCoderUpConvertArg(coder, stack, numParams,
										param, paramType);
					item->engineType = ILEngineType_I8;
				}
				else
				{
					return -1;
				}
			}
			break;

			case ILEngineType_I8:
			{
				/* The supplied value is I8 */
				if(paramType == ILType_Int8 ||
				   paramType == ILType_UInt8 ||
				   paramType == ILType_Boolean ||
				   paramType == ILType_Char ||
				   paramType == ILType_Int16 ||
				   paramType == ILType_UInt16 ||
				   paramType == ILType_Int32 ||
				   paramType == ILType_UInt32)
				{
					/* We must down-convert from I8 to I4.  The method
					   itself will down-convert further if necessary */
					ILCoderDownConvertArg(coder, stack, numParams,
										  param, ILType_Int32);
					item->engineType = ILEngineType_I4;
				}
				else if(paramType == ILType_Int ||
						paramType == ILType_UInt)
				{
					/* We must down-convert from I8 to I */
					ILCoderDownConvertArg(coder, stack, numParams,
										  param, ILType_Int);
					item->engineType = ILEngineType_I;
				}
				else if(paramType == ILType_Int64 ||
						paramType == ILType_UInt64)
				{
					/* Direct 64-bit conversion is OK */
				}
				else
				{
					return -1;
				}
			}
			break;

			case ILEngineType_I:
			{
				/* The supplied value is I */
				if(paramType == ILType_Int8 ||
				   paramType == ILType_UInt8 ||
				   paramType == ILType_Boolean ||
				   paramType == ILType_Char ||
				   paramType == ILType_Int16 ||
				   paramType == ILType_UInt16 ||
				   paramType == ILType_Int32 ||
				   paramType == ILType_UInt32)
				{
					/* We must down-convert from I to I4.  The method
					   itself will down-convert further if necessary */
					ILCoderDownConvertArg(coder, stack, numParams,
										  param, ILType_Int32);
					item->engineType = ILEngineType_I4;
				}
				else if(paramType == ILType_Int ||
						paramType == ILType_UInt)
				{
					/* Direct native conversion is OK */
				}
				else if(paramType == ILType_Int64 ||
						paramType == ILType_UInt64)
				{
					/* We must up-convert from I to I8 */
					ILCoderUpConvertArg(coder, stack, numParams,
										param, paramType);
					item->engineType = ILEngineType_I8;
				}
				else if(paramType != 0 && ILType_IsComplex(paramType) &&
				        (paramType->kind == IL_TYPE_COMPLEX_PTR ||
						 paramType->kind == IL_TYPE_COMPLEX_BYREF))
				{
					/* Can pass unmanaged pointers by pointer or reference
					   only if unsafe IL code is permitted */
					if(!unsafeAllowed)
					{
						return -1;
					}
				}
				else
				{
					return -1;
				}
			}
			break;

			case ILEngineType_F:
			{
				/* The supplied value is F */
				if(paramType == ILType_Float32 ||
				   paramType == ILType_Float64 ||
				   paramType == ILType_Float)
				{
					/* Pass as native float and let the method
					   down-convert further if necessary */
				}
				else
				{
					return -1;
				}
			}
			break;

			case ILEngineType_M:
			case ILEngineType_T:
			{
				/* The supplied value is M or T.  Note: we assume
				   that the garbage collector does not need to be
				   told to start or stop GC tracking on the stack
				   for transient pointers because it will scan the
				   stack regardless */
				if(paramType != 0 && ILType_IsComplex(paramType) &&
				   paramType->kind == IL_TYPE_COMPLEX_BYREF &&
				   ILTypeIdentical(paramType->un.refType, item->typeInfo))
				{
					/* Passing a managed pointer to a reference parameter */
				}
				else if(paramType == ILType_Int ||
				        paramType == ILType_UInt)
				{
					/* Probably an attempt to convert to an unmanaged pointer */
					if(!unsafeAllowed)
					{
						return -1;
					}
				}
				else if(paramType != 0 && ILType_IsComplex(paramType) &&
				        paramType->kind == IL_TYPE_COMPLEX_PTR &&
				        ILTypeIdentical(paramType->un.refType, item->typeInfo))
				{
					/* Converting from a managed to an unmanaged pointer */
					if(!unsafeAllowed)
					{
						return -1;
					}
				}
				else
				{
					return -1;
				}
			}
			break;

			case ILEngineType_O:
			{
				/* The supplied value is O */
				if(IsObjectRef(paramType) &&
				   (item->typeInfo == 0 || AssignCompatible(item, paramType)))
				{
					/* Valid object reference passing */
				}
				else
				{
					return -1;
				}
			}
			break;

			case ILEngineType_MV:
			{
				/* The supplied value is MV */
				if(!ILTypeIdentical(paramType, item->typeInfo))
				{
					return -1;
				}
			}
			break;

			case ILEngineType_Invalid: break;

			case ILEngineType_TypedRef:
			{
				/* The supplied value is a typed reference */
				if(paramType != ILType_TypedRef)
				{
					return -1;
				}
			}
			break;
		}
	}

	/* If we get here, then a match has occurred */
	return (ILInt32)numParams;
}

/*
 * Insert two copies of a constructed object into the
 * stack below the constructor arguments.
 */
static void InsertCtorArgs(ILEngineStackItem *stack, ILUInt32 stackSize,
						   ILUInt32 insertPosn, ILEngineType engineType1,
						   ILType *typeInfo1, ILEngineType engineType2,
						   ILType *typeInfo2)
{
	while(stackSize > insertPosn)
	{
		--stackSize;
		stack[stackSize + 2] = stack[stackSize];
	}
	stack[insertPosn].engineType = engineType1;
	stack[insertPosn].typeInfo = typeInfo1;
	stack[insertPosn + 1].engineType = engineType2;
	stack[insertPosn + 1].typeInfo = typeInfo2;
}

#elif defined(IL_VERIFY_LOCALS)

ILType *methodSignature;
ILInt32 numParams;

#else /* IL_VERIFY_CODE */

case IL_OP_JMP:
{
	/* Jump to another method with exactly the same set of arguments */
	if(unsafeAllowed)
	{
		methodInfo = GetMethodToken(method, pc);
		if(methodInfo && !ILMethod_IsAbstract(methodInfo))
		{
			if(ILMemberAccessible((ILMember *)methodInfo,
								  ILMethod_Owner(method)))
			{
				methodSignature = ILMethod_Signature(methodInfo);
				if(ILTypeIdentical(methodSignature, ILMethod_Signature(method)))
				{
					ILCoderJumpMethod(coder, methodInfo);
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
		else
		{
			VERIFY_TYPE_ERROR();
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
	lastWasJump = 1;
}
break;

case IL_OP_CALL:
{
	/* Call a particular method directly */
	methodInfo = GetMethodToken(method, pc);
	if(methodInfo && !ILMethod_IsAbstract(methodInfo))
	{
		if(ILMemberAccessible((ILMember *)methodInfo, ILMethod_Owner(method)))
		{
			methodSignature = ILMethod_Signature(methodInfo);
			numParams = MatchSignature(coder, stack, stackSize,
									   methodSignature, methodInfo,
									   unsafeAllowed, 0);
			if(numParams >= 0)
			{
				if(methodSignature->un.method.retType != ILType_Void)
				{
					stack[stackSize].engineType = TypeToEngineType
							(methodSignature->un.method.retType);
					stack[stackSize].typeInfo =
							methodSignature->un.method.retType;
				}
				else
				{
					stack[stackSize].engineType = ILEngineType_Invalid;
				}
				ILCoderCallMethod(coder, stack + stackSize - numParams,
								  (ILUInt32)numParams, &(stack[stackSize]),
								  methodInfo);
				stackSize -= (ILUInt32)numParams;
				if(methodSignature->un.method.retType != ILType_Void)
				{
					if(stackSize < code->maxStack)
					{
						stack[stackSize] =
							stack[stackSize + (ILUInt32)numParams];
						++stackSize;
					}
					else
					{
						VERIFY_STACK_ERROR();
					}
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else
		{
			ThrowSystem(coder, method, "SecurityException");
		}
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_CALLI:
{
	/* TODO */
}
break;

case IL_OP_RET:
{
	/* Return from the current method */
	if(signature->un.method.retType != ILType_Void)
	{
		/* Make sure that we have one item on the stack */
		if(stackSize < 1)
		{
			VERIFY_STACK_ERROR();
		}

		/* Validate the type of the return value */
		if(!AssignCompatible(&(stack[stackSize - 1]),
							 signature->un.method.retType))
		{
			VERIFY_TYPE_ERROR();
		}

		/* Notify the coder of the return instruction */
		ILCoderReturnInsn(coder, stack[stackSize - 1].engineType,
					      signature->un.method.retType);

		/* Pop the item from the stack */
		--stackSize;
	}
	else
	{
		/* Notify the coder of a non-value return instruction */
		ILCoderReturnInsn(coder, ILEngineType_Invalid, ILType_Void);
	}
	lastWasJump = 1;
}
break;

case IL_OP_CALLVIRT:
{
	/* Call a virtual or interface method */
	methodInfo = GetMethodToken(method, pc);
	if(methodInfo)
	{
		classInfo = ILMethod_Owner(method);
		if(ILMemberAccessible((ILMember *)methodInfo, classInfo))
		{
			methodSignature = ILMethod_Signature(methodInfo);
			numParams = MatchSignature(coder, stack, stackSize,
									   methodSignature, methodInfo,
									   unsafeAllowed, 0);
			if(numParams >= 0)
			{
				if(methodSignature->un.method.retType != ILType_Void)
				{
					stack[stackSize].engineType = TypeToEngineType
							(methodSignature->un.method.retType);
					stack[stackSize].typeInfo =
							methodSignature->un.method.retType;
				}
				else
				{
					stack[stackSize].engineType = ILEngineType_Invalid;
				}
				if(!ILMethod_IsVirtual(methodInfo))
				{
					/* It is possible to use "callvirt" to call a
					   non-virtual instance method, even though
					   "call" is probably a better way to do it */
					ILCoderCallMethod(coder, stack + stackSize - numParams,
									  (ILUInt32)numParams,
									  &(stack[stackSize]), methodInfo);
				}
				else if(ILClass_IsInterface(classInfo))
				{
					ILCoderCallInterface(coder, stack + stackSize - numParams,
									     (ILUInt32)numParams,
										 &(stack[stackSize]), methodInfo);
				}
				else
				{
					ILCoderCallVirtual(coder, stack + stackSize - numParams,
									   (ILUInt32)numParams,
									   &(stack[stackSize]), methodInfo);
				}
				stackSize -= (ILUInt32)numParams;
				if(methodSignature->un.method.retType != ILType_Void)
				{
					if(stackSize < code->maxStack)
					{
						stack[stackSize] =
							stack[stackSize + (ILUInt32)numParams];
						++stackSize;
					}
					else
					{
						VERIFY_STACK_ERROR();
					}
				}
			}
			else
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else
		{
			ThrowSystem(coder, method, "SecurityException");
		}
	}
	else
	{
		ThrowSystem(coder, method, "MissingMethodException");
	}
}
break;

case IL_OP_NEWOBJ:
{
	/* Create a new object and call its constructor */
	methodInfo = GetConstructorToken(method, pc);
	if(methodInfo)
	{
		/* The construction sequence is different for objects and values */
		classInfo = ILMethod_Owner(methodInfo);
		methodSignature = ILMethod_Signature(methodInfo);
		if(!ILClassIsValueType(classInfo))
		{
			/* Match the signature for the allocation constructor */
			numParams = MatchSignature(coder, stack, stackSize,
									   methodSignature, methodInfo,
									   unsafeAllowed, 1);
			if(numParams < 0)
			{
				VERIFY_TYPE_ERROR();
			}

			/* Call the allocation constructor for the class */
			ILCoderCallCtor(coder, stack + stackSize - numParams,
						    (ILUInt32)numParams, methodInfo);
			stackSize -= (ILUInt32)numParams;
			if(stackSize < code->maxStack)
			{
				stack[stackSize].engineType = ILEngineType_O;
				stack[stackSize].typeInfo = ILType_FromClass(classInfo);
				++stackSize;
			}
			else
			{
				VERIFY_STACK_ERROR();
			}
		}
		else
		{
			/* Rearrange the stack so that there is a newly initialized
			   value on the stack, followed by a managed pointer to the
			   value, and then the constructor arguments.  We assume
			   that we have 2 "slop" positions, as above. */
			InsertCtorArgs(stack, stackSize,
						   stackSize - (ILUInt32)(methodSignature->num),
						   ILEngineType_MV, ILType_FromValueType(classInfo),
						   ILEngineType_M, ILType_FromValueType(classInfo));
			stackSize += 2;
			ILCoderValueCtorArgs(coder, classInfo,
						    stack + stackSize -
									(ILUInt32)(methodSignature->num),
							(ILUInt32)(methodSignature->num));

			/* Match the constructor signature */
			numParams = MatchSignature(coder, stack, stackSize,
									   methodSignature, methodInfo,
									   unsafeAllowed, 0);
			if(numParams < 0)
			{
				VERIFY_TYPE_ERROR();
			}

			/* Call the constructor and pop all of its arguments */
			ILCoderCallMethod(coder, stack + stackSize - numParams,
							  (ILUInt32)numParams, 0, methodInfo);
			stackSize -= (ILUInt32)numParams;

			/* Make sure that we had at least 1 real stack slot
			   available for the final object value (not counting
			   the 2 "slop" items) */
			if(stackSize > code->maxStack)
			{
				VERIFY_STACK_ERROR();
			}
		}
	}
	else
	{
		ThrowSystem(coder, method, "MissingMethodException");
	}
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_LDFTN:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_LDVIRTFTN:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_JMPI:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_TAIL:
{
	/* TODO: tail calls are not yet supported */
}
break;

#endif /* IL_VERIFY_CODE */
