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
 * Get a method token from within a method's code.  If "callSiteSig"
 * is not NULL, then write the call site signature to it.  If it is
 * NULL, then vararg method calls are disallowed.
 */
static ILMethod *GetMethodToken(ILMethod *method, unsigned char *pc,
								ILType **callSiteSig)
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

	/* Make sure that the method's class has been laid out */
	if(!_ILLayoutClass(ILMethod_Owner(methodInfo)))
	{
		return 0;
	}

	/* Process call site information */
	if(callSiteSig)
	{
		*callSiteSig = ILMethod_Signature(methodInfo);
		methodInfo = ILMethodResolveCallSite(methodInfo);
	}
	else if(ILType_Kind(ILMethod_Signature(methodInfo)) ==
				(IL_TYPE_COMPLEX_METHOD | IL_TYPE_COMPLEX_METHOD_SENTINEL))
	{
		return 0;
	}

	/* We have the requested method */
	return methodInfo;
}

/*
 * Get a constructor token from within a method's code.  If "callSiteSig"
 * is not NULL, then write the call site signature to it.  If it is
 * NULL, then vararg method calls are disallowed.
 */
static ILMethod *GetConstructorToken(ILMethod *method, unsigned char *pc,
									 ILType **callSiteSig)
{
	ILMethod *methodInfo = GetMethodToken(method, pc, callSiteSig);

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
 */
static ILInt32 MatchSignature(ILCoder *coder, ILEngineStackItem *stack,
						      ILUInt32 stackSize, ILType *signature,
						      ILMethod *method, int unsafeAllowed,
							  int suppressThis)
{
	ILClass *owner = (method ? ILMethod_Owner(method) : 0);
	ILUInt32 numParams;
	ILUInt32 totalParams;
	ILUInt32 param;
	ILType *paramType;
	ILEngineStackItem *item;
	int hasThis;
	int isValueThis;
	int isVarArg;
	ILType *thisType;

	/* Check the vararg vs non-vararg conventions, and get the
	   number of non-vararg parameters */
	if((ILType_CallConv(signature) & IL_META_CALLCONV_MASK) ==
			IL_META_CALLCONV_VARARG)
	{
		if((ILMethodGetCallConv(method) & IL_META_CALLCONV_MASK) !=
				IL_META_CALLCONV_VARARG)
		{
			/* The call site signature is vararg, but not the method */
			return -1;
		}
		numParams = ILTypeNumParams(ILMethod_Signature(method));
		if(ILType_Kind(signature) == IL_TYPE_COMPLEX_METHOD)
		{
			/* Calling the method using its own signature */
			totalParams = ILTypeNumParams(signature);
		}
		else
		{
			/* Calling the method using a signature containing a sentinel */
			totalParams = ILTypeNumParams(signature) - 1;
		}
		isVarArg = 1;
	}
	else
	{
		if((ILMethodGetCallConv(method) & IL_META_CALLCONV_MASK) ==
				IL_META_CALLCONV_VARARG)
		{
			/* The method is vararg, but not the call site signature */
			return -1;
		}
		numParams = ILTypeNumParams(signature);
		totalParams = numParams;
		isVarArg = 0;
	}

	/* Determine if the signature needs an extra "this" parameter */
	hasThis = (ILType_HasThis(signature) && !suppressThis);
	if(hasThis)
	{
		++numParams;
		++totalParams;
		if(!method)
		{
			/* Call site signatures need "explicit this" */
			return -1;
		}
		if(ILClassIsValueType(owner))
		{
			/* The "this" parameter is a value type, which must be
			   passed as either a managed or transient pointer */
			isValueThis = 1;
			thisType = ILClassToType(owner);
		}
		else
		{
			/* The "this" parameter is an object reference */
			isValueThis = 0;
			thisType = ILClassGetSynType(owner);
			if(!thisType)
			{
				thisType = ILType_FromClass(owner);
			}
		}
	}
	else
	{
		isValueThis = 0;
		thisType = 0;
	}

	/* Validate the stack size */
	if(stackSize < totalParams)
	{
		/* Insufficient parameters to the call */
		return -1;
	}

	/* Find the base of the parameters on the stack */
	stack += (stackSize - totalParams);

	/* Match the argument signature */
	for(param = 1; param <= totalParams; ++param)
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
					paramType = ILClassToType(owner);
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
					if(item->engineType != ILEngineType_O ||
					   (item->typeInfo != 0 &&
					    !AssignCompatible(method, item, thisType,
										  unsafeAllowed)))
					{
						return -1;
					}
				}
				continue;
			}
			else if(param <= numParams)
			{
				/* Parameter passed before the vararg sentinel */
				paramType = ILTypeGetParam(signature, param - 1);
			}
			else
			{
				/* Parameter passed after the vararg sentinel */
				paramType = ILTypeGetParam(signature, param);
			}
		}
		else if(param <= numParams)
		{
			/* Parameter passed before the vararg sentinel */
			paramType = ILTypeGetParam(signature, param);
		}
		else
		{
			/* Parameter passed after the vararg sentinel */
			paramType = ILTypeGetParam(signature, param + 1);
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
					ILCoderUpConvertArg(coder, stack, totalParams,
										param, paramType);
					item->engineType = ILEngineType_I;
				}
				else if(paramType == ILType_Int64 ||
						paramType == ILType_UInt64)
				{
					/* We must up-convert from I4 to I8 */
					ILCoderUpConvertArg(coder, stack, totalParams,
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
					ILCoderDownConvertArg(coder, stack, totalParams,
										  param, ILType_Int32);
					item->engineType = ILEngineType_I4;
				}
				else if(paramType == ILType_Int ||
						paramType == ILType_UInt)
				{
					/* We must down-convert from I8 to I */
					ILCoderDownConvertArg(coder, stack, totalParams,
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
					ILCoderDownConvertArg(coder, stack, totalParams,
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
					ILCoderUpConvertArg(coder, stack, totalParams,
										param, paramType);
					item->engineType = ILEngineType_I8;
				}
				else if(paramType != 0 && ILType_IsComplex(paramType) &&
				        (ILType_Kind(paramType) == IL_TYPE_COMPLEX_PTR ||
						 ILType_Kind(paramType) == IL_TYPE_COMPLEX_BYREF))
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
				   ILType_Kind(paramType) == IL_TYPE_COMPLEX_BYREF &&
				   ILTypeIdentical(ILType_Ref(paramType), item->typeInfo))
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
				        ILType_Kind(paramType) == IL_TYPE_COMPLEX_PTR &&
				        ILTypeIdentical(ILType_Ref(paramType), item->typeInfo))
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
				   (item->typeInfo == 0 ||
				    AssignCompatible(method, item, paramType,
									 unsafeAllowed)))
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

	/* Convert the vararg parameters into an "Object[]" array */
	if(isVarArg)
	{
		ILCoderPackVarArgs(coder, signature, numParams + 2,
						   stack + numParams, totalParams - numParams);
	}

	/* If we get here, then a match has occurred */
	return (ILInt32)totalParams;
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

/*
 * Determine if a method is inlineable, and return its inline type.
 * Returns -1 if the method is not inlineable.
 */
static int GetInlineMethodType(ILMethod *method)
{
	ILClass *owner;
	const char *name;
	ILImage *image;
	ILImage *systemImage;
	ILType *signature;

	owner = ILMethod_Owner(method);
	name = ILClass_Name(owner);
	if(!strcmp(name, "String"))
	{
		/* Make sure that this is really "System.String" */
		name = ILClass_Namespace(owner);
		if(!name || strcmp(name, "System") != 0)
		{
			return -1;
		}
		image = ILClassToImage(owner);
		systemImage = ILContextGetSystem(ILImageToContext(image));
		if(systemImage && systemImage != image)
		{
			return -1;
		}

		/* Check for known inlineable methods */
		name = ILMethod_Name(method);
		signature = ILMethod_Signature(method);
		if(!strcmp(name, "get_Length") &&
		   _ILLookupTypeMatch(signature, "(T)i"))
		{
			return IL_INLINEMETHOD_STRING_LENGTH;
		}
		else if(!strcmp(name, "Concat"))
		{
			if(_ILLookupTypeMatch(signature,
				"(oSystem.String;oSystem.String;)oSystem.String;"))
			{
				return IL_INLINEMETHOD_STRING_CONCAT_2;
			}
			else if(_ILLookupTypeMatch(signature,
						"(oSystem.String;oSystem.String;"
						"oSystem.String;)oSystem.String;"))
			{
				return IL_INLINEMETHOD_STRING_CONCAT_3;
			}
			else if(_ILLookupTypeMatch(signature,
						"(oSystem.String;oSystem.String;oSystem.String;"
						"oSystem.String;)oSystem.String;"))
			{
				return IL_INLINEMETHOD_STRING_CONCAT_4;
			}
		}
		else if((!strcmp(name, "Equals") || !strcmp(name, "op_Equality")) &&
				_ILLookupTypeMatch(signature,
					"(oSystem.String;oSystem.String;)Z"))
		{
			return IL_INLINEMETHOD_STRING_EQUALS;
		}
		else if(!strcmp(name, "op_Inequality") &&
				_ILLookupTypeMatch(signature,
					"(oSystem.String;oSystem.String;)Z"))
		{
			return IL_INLINEMETHOD_STRING_NOT_EQUALS;
		}
		else if(!strcmp(name, "get_Chars") &&
				_ILLookupTypeMatch(signature, "(Ti)c"))
		{
			return IL_INLINEMETHOD_STRING_GET_CHAR;
		}
		return -1;
	}
	else if(!strcmp(name, "Monitor"))
	{
		/* Make sure that this is really "System.Threading.Monitor" */
		name = ILClass_Namespace(owner);
		if(!name || strcmp(name, "System.Threading") != 0)
		{
			return -1;
		}
		image = ILClassToImage(owner);
		systemImage = ILContextGetSystem(ILImageToContext(image));
		if(systemImage && systemImage != image)
		{
			return -1;
		}

		/* Check for known inlineable methods */
		name = ILMethod_Name(method);
		signature = ILMethod_Signature(method);
		if(!strcmp(name, "Enter") &&
		   _ILLookupTypeMatch(signature, "(oSystem.Object;)V"))
		{
			return IL_INLINEMETHOD_MONITOR_ENTER;
		}
		else if(!strcmp(name, "Exit") &&
		        _ILLookupTypeMatch(signature, "(oSystem.Object;)V"))
		{
			return IL_INLINEMETHOD_MONITOR_EXIT;
		}
		return -1;
	}
	else if(!strcmp(name, "Type"))
	{
		/* Make sure that this is really "System.Type" */
		name = ILClass_Namespace(owner);
		if(!name || strcmp(name, "System") != 0)
		{
			return -1;
		}
		image = ILClassToImage(owner);
		systemImage = ILContextGetSystem(ILImageToContext(image));
		if(systemImage && systemImage != image)
		{
			return -1;
		}

		/* Check for known inlineable methods */
		name = ILMethod_Name(method);
		signature = ILMethod_Signature(method);
		if(!strcmp(name, "GetTypeFromHandle") &&
		   _ILLookupTypeMatch(signature,
		   					  "(vSystem.RuntimeTypeHandle;)oSystem.Type;"))
		{
			return IL_INLINEMETHOD_TYPE_FROM_HANDLE;
		}
		return -1;
	}
	else
	{
		/* This class does not have inlineable methods */
		return -1;
	}
}

#elif defined(IL_VERIFY_LOCALS)

ILType *methodSignature;
ILType *returnType;
ILInt32 numParams;
int inlineType;

#else /* IL_VERIFY_CODE */

case IL_OP_JMP:
{
	/* Jump to another method with exactly the same set of arguments */
	if(unsafeAllowed)
	{
		methodInfo = GetMethodToken(method, pc, (ILType **)0);
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
	methodInfo = GetMethodToken(method, pc, &methodSignature);
	if(methodInfo && !ILMethod_IsAbstract(methodInfo))
	{
		if(ILMemberAccessible((ILMember *)methodInfo, ILMethod_Owner(method)))
		{
			numParams = MatchSignature(coder, stack, stackSize,
									   methodSignature, methodInfo,
									   unsafeAllowed, 0);
			if(numParams >= 0)
			{
				returnType = ILTypeGetReturn(methodSignature);
				if(returnType != ILType_Void)
				{
					stack[stackSize].engineType = TypeToEngineType(returnType);
					stack[stackSize].typeInfo = returnType;
				}
				else
				{
					stack[stackSize].engineType = ILEngineType_Invalid;
				}
				inlineType = GetInlineMethodType(methodInfo);
				if(inlineType == -1 ||
				   !ILCoderCallInlineable(coder, inlineType))
				{
					ILCoderCallMethod(coder, stack + stackSize - numParams,
									  (ILUInt32)numParams, &(stack[stackSize]),
									  methodInfo);
				}
				stackSize -= (ILUInt32)numParams;
				if(returnType != ILType_Void)
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
	returnType = ILTypeGetReturn(signature);
	if(returnType != ILType_Void)
	{
		/* Make sure that we have one item on the stack */
		if(stackSize < 1)
		{
			VERIFY_STACK_ERROR();
		}

		/* Validate the type of the return value */
		if(!AssignCompatible(method, &(stack[stackSize - 1]),
							 returnType, unsafeAllowed))
		{
			VERIFY_TYPE_ERROR();
		}

		/* Notify the coder of the return instruction */
		ILCoderReturnInsn(coder, stack[stackSize - 1].engineType, returnType);

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
	methodInfo = GetMethodToken(method, pc, &methodSignature);
	if(methodInfo)
	{
		classInfo = ILMethod_Owner(method);
		if(ILMemberAccessible((ILMember *)methodInfo, classInfo))
		{
			numParams = MatchSignature(coder, stack, stackSize,
									   methodSignature, methodInfo,
									   unsafeAllowed, 0);
			if(numParams >= 0)
			{
				returnType = ILTypeGetReturn(methodSignature);
				if(returnType != ILType_Void)
				{
					stack[stackSize].engineType = TypeToEngineType(returnType);
					stack[stackSize].typeInfo = returnType;
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
					inlineType = GetInlineMethodType(methodInfo);
					if(inlineType == -1 ||
					   !ILCoderCallInlineable(coder, inlineType))
					{
						ILCoderCallMethod(coder, stack + stackSize - numParams,
										  (ILUInt32)numParams,
										  &(stack[stackSize]), methodInfo);
					}
				}
				else if(ILClass_IsInterface(ILMethod_Owner(methodInfo)))
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
				if(returnType != ILType_Void)
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
	methodInfo = GetConstructorToken(method, pc, &methodSignature);
	if(methodInfo)
	{
		/* The construction sequence is different for objects and values */
		classInfo = ILMethod_Owner(methodInfo);
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
			classType = ILClassToType(classInfo);
			numParams = ILTypeNumParams(methodSignature);
			InsertCtorArgs(stack, stackSize,
						   stackSize - numParams,
						   ILEngineType_MV, classType,
						   ILEngineType_M, classType);
			stackSize += 2;
			ILCoderValueCtorArgs(coder, classInfo,
						    stack + stackSize - numParams, numParams);

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
	/* Load the address of a function onto the stack as "I" */
	methodInfo = GetMethodToken(method, pc, (ILType **)0);
	if(methodInfo && !ILMethod_IsAbstract(methodInfo))
	{
		if(ILMemberAccessible((ILMember *)methodInfo, ILMethod_Owner(method)))
		{
			ILCoderLoadFuncAddr(coder, methodInfo);
			stack[stackSize].engineType = ILEngineType_I;
			stack[stackSize].typeInfo = ILMethod_Signature(methodInfo);
			++stackSize;
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

case IL_OP_PREFIX + IL_PREFIX_OP_LDVIRTFTN:
{
	/* Load the address of a virtual function onto the stack as "I" */
	methodInfo = GetMethodToken(method, pc, (ILType **)0);
	if(methodInfo && STK_UNARY == ILEngineType_O)
	{
		classInfo = ILMethod_Owner(method);
		if(AssignCompatible(method, &(stack[stackSize - 1]),
							ILType_FromClass(classInfo),
							unsafeAllowed))
		{
			if(ILMemberAccessible((ILMember *)methodInfo, classInfo))
			{
				if(!ILMethod_IsVirtual(methodInfo))
				{
					/* It is possible to use "ldvirtfn" to access a
					   non-virtual instance method, even though
					   "ldftn" is probably a better way to do it */
					if(!ILMethod_IsAbstract(methodInfo))
					{
						ILCoderPop(coder, STK_UNARY, STK_UNARY_TYPEINFO);
						ILCoderLoadFuncAddr(coder, methodInfo);
					}
				}
				else if(ILClass_IsInterface(ILMethod_Owner(methodInfo)))
				{
					ILCoderLoadInterfaceAddr(coder, methodInfo);
				}
				else
				{
					ILCoderLoadVirtualAddr(coder, methodInfo);
				}
				stack[stackSize - 1].engineType = ILEngineType_I;
				stack[stackSize - 1].typeInfo = ILMethod_Signature(methodInfo);
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
break;

case IL_OP_PREFIX + IL_PREFIX_OP_JMPI:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_TAIL:
{
	/* Missing: (?)
	 *
	 * Correct CIL must not branch to the call instruction, but it is
	 * permitted to branch to ret.
	 *
	 * The tail.call (or calli or callvirt) instruction cannot be used
	 * to transfer control out of a try, filter, catch, or finally block.
	 */

	/* Confirm that the .tail precedes a call */
	if (( pc[2] == IL_OP_CALL || 
		pc[2] == IL_OP_CALLI || 
		pc[2] == IL_OP_CALLVIRT
		) && pc[3 + sizeof(void *)] == IL_OP_RET)
	{
		/*  Initialization */
		methodInfo = 0;
		methodSignature = 0;
		numParams = 0;

		/*  Get called method information as appropriate. */
		switch(pc[2]) 
		{
		case IL_OP_CALL:
			/* Test that only the target method's arguments are on the stack */
			methodInfo = GetMethodToken(method, &pc[2], &methodSignature);
			numParams = MatchSignature(coder, stack, stackSize,
								   methodSignature, methodInfo,
								   unsafeAllowed, 0);

		case IL_OP_CALLI:
		case IL_OP_CALLVIRT:
			/*  TODO - Implement CALLI and CALLVIRT tails */
			break;
		}

		/*  A method exists and it's not synchronized? */
		if (methodInfo && !ILMethod_IsSynchronized(methodInfo)) 
		{
			/*  Test that the stack contents are sane */
			if (stackSize == numParams) 
			{
				if (unsafeAllowed) 
				{
					/*  Don't bother checking for pointers */
					ILCoderTailCall(coder, method);
				} 
				else 
				{
					/*  Test for managed pointers on the stack - a no-no */
					int i, mPtrFlag;
					mPtrFlag = 0;
					for (i=0; i<numParams; i++)
					{
						if (stack[i].engineType == ILEngineType_M ||
								stack[i].engineType == ILEngineType_T)
						{
							mPtrFlag = 1;
							break;
						}
					}

					if (mPtrFlag) 
					{
						VERIFY_TYPE_ERROR();
					} 
					else 
					{
						ILCoderTailCall(coder, method);
					}
				}
			}
			else
			{
				VERIFY_STACK_ERROR();
			}
		}
	} 
	else 
	{
		VERIFY_INSN_ERROR();
	}
}
break;

#endif /* IL_VERIFY_CODE */
