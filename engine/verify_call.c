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
 * Get a call site signature.  Returns NULL if invalid.
 */
static ILType *GetCallSiteSig(ILMethod *method, unsigned char *pc)
{
	ILUInt32 token;
	ILStandAloneSig *sig;
	ILType *type;
	ILUInt32 callConv;

	/* Fetch the token from the instruction's arguments */
	if(pc[0] != IL_OP_PREFIX)
	{
		token = IL_READ_UINT32(pc + 1);
	}
	else
	{
		token = IL_READ_UINT32(pc + 2);
	}

	/* Resolve the token to a stand-alone signature */
	sig = ILProgramItemToStandAloneSig((ILProgramItem *)
						ILImageTokenInfo(ILProgramItem_Image(method), token));
	if(!sig)
	{
		return 0;
	}

	/* Extract the type component of the signature and verify
	   that it is indeed a method signature */
	type = ILStandAloneSig_Type(sig);
	if(!ILType_IsMethod(type))
	{
		return 0;
	}

	/* If the type specifies "this", then it must be "explicit this" */
	callConv = ILType_CallConv(type);
	if((callConv & IL_META_CALLCONV_HASTHIS) != 0)
	{
		if((callConv & IL_META_CALLCONV_EXPLICITTHIS) == 0)
		{
			return 0;
		}
	}
	else if((callConv & IL_META_CALLCONV_EXPLICITTHIS) != 0)
	{
		return 0;
	}

	/* We have a valid signature */
	return type;
}

/*
 * Match a method signature against the contents of the stack.
 * Returns -1 if a type error has been detected, or the number
 * of parameters to be popped otherwise.
 */
static ILInt32 MatchSignature(ILCoder *coder, ILEngineStackItem *stack,
						      ILUInt32 stackSize, ILType *signature,
						      ILMethod *method, int unsafeAllowed,
							  int suppressThis, int indirectCall,
							  ILCoderMethodInfo *callInfo, int tailCall)
{
	ILClass *owner = (method ? ILMethod_Owner(method) : 0);
	ILUInt32 numParams;
	ILUInt32 totalParams;
	ILUInt32 param;
	ILType *paramType;
	ILEngineStackItem *item;
	int hasThis;
	int isValueThis;
	ILType *thisType;
#ifdef IL_CONFIG_VARARGS
	int isVarArg;
#endif

	/* TODO: match explicit this information for indirect calls */

	/* Check the vararg vs non-vararg conventions, and get the
	   number of non-vararg parameters */
#ifdef IL_CONFIG_VARARGS
	if(indirectCall)
	{
		/* We don't have a method to compare against for indirect calls */
		if((ILType_CallConv(signature) & IL_META_CALLCONV_MASK) ==
				IL_META_CALLCONV_VARARG)
		{
			numParams = ILTypeNumParams(signature);
			totalParams = numParams;
			isVarArg = 1;
			if((ILType_Kind(signature) & IL_TYPE_COMPLEX_METHOD_SENTINEL) != 0)
			{
				while(numParams > 0 &&
					  !ILType_IsSentinel(ILTypeGetParam(signature, numParams)))
				{
					--numParams;
				}
				if(numParams > 0)
				{
					--numParams;
				}
			}
		}
		else
		{
			numParams = ILTypeNumParams(signature);
			totalParams = numParams;
			isVarArg = 0;
		}
	}
	else if((ILType_CallConv(signature) & IL_META_CALLCONV_MASK) ==
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
#else /* !IL_CONFIG_VARARGS */
	if((ILType_CallConv(signature) & IL_META_CALLCONV_MASK) ==
			IL_META_CALLCONV_VARARG)
	{
		return -1;
	}
	if(method)
	{
		if((ILMethodGetCallConv(method) & IL_META_CALLCONV_MASK) ==
				IL_META_CALLCONV_VARARG)
		{
			return -1;
		}
	}
	numParams = ILTypeNumParams(signature);
	totalParams = numParams;
#endif /* !IL_CONFIG_VARARGS */

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
						 ILType_Kind(paramType) == IL_TYPE_COMPLEX_BYREF ||
						 ILType_Kind(paramType) == IL_TYPE_COMPLEX_METHOD))
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
				        ILType_Kind(paramType) == IL_TYPE_COMPLEX_PTR)
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
				if(tailCall && !unsafeAllowed)
				{
					/* Cannot use managed pointers in tail calls */
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

	/* Initialize the call argument information */
	callInfo->args = stack;
	callInfo->numBaseArgs = numParams;
	callInfo->numVarArgs = totalParams - numParams;
	callInfo->hasParamArray = 0;
	callInfo->tailCall = tailCall;

#ifdef IL_CONFIG_VARARGS
	/* Convert the vararg parameters into an "Object[]" array */
	if(isVarArg)
	{
		ILCoderPackVarArgs(coder, signature,
						   (hasThis ? numParams + 1 : numParams + 2),
						   stack + numParams, totalParams - numParams);
		callInfo->hasParamArray = 1;
	}
#endif /* IL_CONFIG_VARARGS */

	/* If we get here, then a match has occurred */
	return (ILInt32)totalParams;
}

/*
 * Match a delegate signature against the contents of the stack.
 * Returns -1 if a type error has been detected, or the number
 * of parameters to be popped otherwise (normally 2).
 */
static ILInt32 MatchDelegateSignature(ILCoder *coder, ILEngineStackItem *stack,
						              ILUInt32 stackSize, ILType *signature,
									  ILClass *classInfo)
{
	ILMethod *method;
	ILType *methodSignature;

	/* The constructor must have two parameters: Object and IntPtr */
	if(ILTypeNumParams(signature) != 2)
	{
		return -1;
	}
	if(!ILTypeIsObjectClass(ILTypeGetParam(signature, 1)))
	{
		return -1;
	}
	if(ILTypeGetParam(signature, 2) != ILType_Int)
	{
		return -1;
	}

	/* Check the stack contents for correct engine types */
	if(stackSize < 2)
	{
		return -1;
	}
	if(stack[stackSize - 2].engineType != ILEngineType_O)
	{
		return -1;
	}
	if(stack[stackSize - 1].engineType != ILEngineType_I)
	{
		return -1;
	}

	/* Convert the method reference type into the underlying method */
	method = MethodRefToMethod(stack[stackSize - 1].typeInfo);
	if(!method)
	{
		return -1;
	}

	/* Check that the delegate signatures match */
	if(!ILTypeDelegateSignatureMatch(ILType_FromClass(classInfo), method))
	{
		return -1;
	}

	/* Validate the "this" parameter value, which we ignore if
	   the method is "static" */
	methodSignature = ILMethod_Signature(method);
	if(ILType_HasThis(methodSignature))
	{
		if(!IsSubClass(stack[stackSize - 2].typeInfo,
					   ILMethod_Owner(method)))
		{
			return -1;
		}
	}

	/* Done */
	return 2;
}

/*
 * Determine if two method signatures are identical for the
 * purposes of indirect method calls.
 */
static int SameSignature(ILType *ptrSig, ILType *callSiteSig)
{
	unsigned long ptrParams;
	unsigned long callSiteParams;
	unsigned long param;

	/* The pointer signature must be a method
	   (callSiteSig was already checked) */
	if(!ILType_IsMethod(ptrSig))
	{
		return 0;
	}

	/* Check the return types */
	if(!ILTypeIdentical(ILTypeGetReturn(ptrSig), ILTypeGetReturn(callSiteSig)))
	{
		return 0;
	}

	/* Check the calling conventions */
	ptrParams = ILTypeNumParams(ptrSig);
	callSiteParams = ILTypeNumParams(callSiteSig);
	if(ILType_HasThis(ptrSig))
	{
		if((ILType_CallConv(callSiteSig) & IL_META_CALLCONV_EXPLICITTHIS) == 0)
		{
			return 0;
		}
	}
	else if((ILType_CallConv(callSiteSig) & IL_META_CALLCONV_EXPLICITTHIS) != 0)
	{
		return 0;
	}
	if((ILType_CallConv(ptrSig) & IL_META_CALLCONV_MASK) ==
			IL_META_CALLCONV_VARARG)
	{
		if((ILType_CallConv(callSiteSig) & IL_META_CALLCONV_MASK) !=
				IL_META_CALLCONV_VARARG)
		{
			return 0;
		}
		if((ILType_Kind(callSiteSig) & IL_TYPE_COMPLEX_METHOD_SENTINEL) != 0)
		{
			/* The call site has a sentinel, so reduce the number
			   of parameters that we need to check for identity */
			callSiteParams = 0;
			while(!ILType_IsSentinel(ILTypeGetParam
						(callSiteSig, callSiteParams + 1)))
			{
				++callSiteSig;
			}
		}
	}
	else if((ILType_CallConv(callSiteSig) & IL_META_CALLCONV_MASK) ==
				IL_META_CALLCONV_VARARG)
	{
		return 0;
	}

	/* TODO: match explicit this information */

	/* Check the number of parameters */
	if(ptrParams != callSiteParams)
	{
		return 0;
	}

	/* Check that the non-vararg parameters are identical */
	for(param = 1; param <= ptrParams; ++param)
	{
		if(!ILTypeIdentical(ILTypeGetParam(ptrSig, param),
						    ILTypeGetParam(callSiteSig, param)))
		{
			return 0;
		}
	}
	return 1;
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
	if(typeInfo1 == ILType_Int || typeInfo1 == ILType_UInt)
	{
		/* Convert MV into I for the IntPtr and UIntPtr types */
		stack[insertPosn].engineType = ILEngineType_I;
	}
	else
	{
		stack[insertPosn].engineType = engineType1;
	}
	stack[insertPosn].typeInfo = typeInfo1;
	stack[insertPosn + 1].engineType = engineType2;
	stack[insertPosn + 1].typeInfo = typeInfo2;
}

/*
 * Determine if a class corresponds to a synthetic 2-D array type.
 */
static int Is2DArrayClass(ILClass *classInfo)
{
	ILType *type;
	type = ILClassGetSynType(ILClassResolve(classInfo));
	if(type && ILType_IsArray(type))
	{
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_ARRAY_CONTINUE &&
		   ILType_Kind(ILType_ElemType(type)) == IL_TYPE_COMPLEX_ARRAY)
		{
			return 1;
		}
	}
	return 0;
}

/*
 * Table of inline methods.
 */
typedef struct
{
	const char *name;
	const char *namespace;
	const char *methodName;
	const char *signature;
	int         inlineType;

} InlineMethodInfo;
static InlineMethodInfo const InlineMethods[] = {
	{"String", "System", "get_Length", "(T)i", IL_INLINEMETHOD_STRING_LENGTH},
	{"String", "System", "Concat",
	 "(oSystem.String;oSystem.String;)oSystem.String;",
	 IL_INLINEMETHOD_STRING_CONCAT_2},
	{"String", "System", "Concat",
	 "(oSystem.String;oSystem.String;oSystem.String;)oSystem.String;",
	 IL_INLINEMETHOD_STRING_CONCAT_3},
	{"String", "System", "Concat",
	 "(oSystem.String;oSystem.String;oSystem.String;oSystem.String;)"
	 		"oSystem.String;",
	 IL_INLINEMETHOD_STRING_CONCAT_4},
	{"String", "System", "op_Equality", "(oSystem.String;oSystem.String;)Z",
	 IL_INLINEMETHOD_STRING_EQUALS},
	{"String", "System", "Equals", "(oSystem.String;oSystem.String;)Z",
	 IL_INLINEMETHOD_STRING_EQUALS},
	{"String", "System", "op_Inequality", "(oSystem.String;oSystem.String;)Z",
	 IL_INLINEMETHOD_STRING_NOT_EQUALS},
	{"String", "System", "get_Chars", "(Ti)c",
	 IL_INLINEMETHOD_STRING_GET_CHAR},

	{"Monitor", "System.Threading", "Enter", "(oSystem.Object;)V",
	 IL_INLINEMETHOD_MONITOR_ENTER},
	{"Monitor", "System.Threading", "Exit", "(oSystem.Object;)V",
	 IL_INLINEMETHOD_MONITOR_EXIT},

	{"Type", "System", "GetTypeFromHandle",
	 "(vSystem.RuntimeTypeHandle;)oSystem.Type;",
	 IL_INLINEMETHOD_TYPE_FROM_HANDLE},

	{"StringBuilder", "System.Text", "Append",
	 "(Tc)oSystem.Text.StringBuilder;", IL_INLINEMETHOD_BUILDER_APPEND_CHAR},

	{"Char", "System", "IsWhiteSpace", "(c)Z", IL_INLINEMETHOD_IS_WHITE_SPACE},
};
#define	NumInlineMethods	(sizeof(InlineMethods) / sizeof(InlineMethodInfo))

/*
 * Determine if a method is inlineable, and return its inline type.
 * Returns -1 if the method is not inlineable.
 */
static int GetInlineMethodType(ILMethod *method)
{
	ILClass *owner;
	const char *name;
	const char *namespace;
	const char *methodName;
	int posn;
	ILImage *image;
	ILImage *systemImage;
	ILType *signature;

	/* Extract the information that we require to find the inline */
	owner = ILMethod_Owner(method);
	name = ILClass_Name(owner);
	namespace = ILClass_Namespace(owner);
	methodName = ILMethod_Name(method);
	signature = ILMethod_Signature(method);

	/* Check for types in the runtime library */
	if(namespace)
	{
		for(posn = 0; posn < NumInlineMethods; ++posn)
		{
			if(!strcmp(name, InlineMethods[posn].name) &&
			   !strcmp(namespace, InlineMethods[posn].namespace) &&
			   !strcmp(methodName, InlineMethods[posn].methodName) &&
			   _ILLookupTypeMatch(signature, InlineMethods[posn].signature))
			{
				image = ILClassToImage(owner);
				systemImage = ILContextGetSystem(ILImageToContext(image));
				if(!systemImage || systemImage == image)
				{
					return InlineMethods[posn].inlineType;
				}
			}
		}
	}

	/* Check for 2D array types */
	if(Is2DArrayClass(owner))
	{
		/* Two-dimensional array operation */
		if(!strcmp(methodName, "Get"))
		{
			signature = ILTypeGetReturn(signature);
			if(signature == ILType_Int32)
			{
				return IL_INLINEMETHOD_GET2D_INT;
			}
			else if(signature == ILType_Float64)
			{
				return IL_INLINEMETHOD_GET2D_DOUBLE;
			}
			else if(ILType_IsClass(signature))
			{
				return IL_INLINEMETHOD_GET2D_OBJECT;
			}
		}
		else if(!strcmp(methodName, "Set") && ILTypeNumParams(signature) > 0)
		{
			signature = ILTypeGetParam(signature, ILTypeNumParams(signature));
			if(signature == ILType_Int32)
			{
				return IL_INLINEMETHOD_SET2D_INT;
			}
			else if(signature == ILType_Float64)
			{
				return IL_INLINEMETHOD_SET2D_DOUBLE;
			}
			else if(ILType_IsClass(signature))
			{
				return IL_INLINEMETHOD_SET2D_OBJECT;
			}
		}
		return -1;
	}

	/* This is not an inlineable method */
	return -1;
}

/*
 * Set return type information within a stack item.
 */
static void SetReturnType(ILEngineStackItem *item, ILType *returnType)
{
	item->engineType = TypeToEngineType(returnType);
	if(item->engineType != ILEngineType_M)
	{
		item->typeInfo = returnType;
	}
	else
	{
		item->typeInfo = ILType_Ref(ILTypeStripPrefixes(returnType));
	}
}

#elif defined(IL_VERIFY_LOCALS)

ILType *methodSignature;
ILType *returnType;
ILCoderMethodInfo callInfo;
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
									   unsafeAllowed, 0, 0,
									   &callInfo, tailCall);
			tailCall = 0;
			if(numParams >= 0)
			{
				returnType = ILTypeGetReturn(methodSignature);
				if(returnType != ILType_Void)
				{
					SetReturnType(&(stack[stackSize]), returnType);
				}
				else
				{
					stack[stackSize].engineType = ILEngineType_Invalid;
				}
				inlineType = GetInlineMethodType(methodInfo);
				if(inlineType == -1 ||
				   !ILCoderCallInlineable(coder, inlineType, methodInfo))
				{
					ILCoderCallMethod(coder, &callInfo, &(stack[stackSize]),
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
			ThrowSystem("System.Security", "SecurityException");
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
	/* Call a method using an indirect method pointer */
	if(stackSize < 1)
	{
		/* We don't have sufficient arguments on the stack */
		VERIFY_STACK_ERROR();
	}
	else if(stack[stackSize - 1].engineType != ILEngineType_I)
	{
		/* The method pointer must be of type "I" */
		VERIFY_TYPE_ERROR();
	}
	else
	{
		/* Get the call site signature and validate it */
		methodSignature = GetCallSiteSig(method, pc);
		if(!methodSignature)
		{
			VERIFY_TYPE_ERROR();
		}

		/* Check for identity between the function pointer
		   and the specified call site signature */
		if(stack[stackSize - 1].typeInfo != 0)
		{
			if((!unsafeAllowed) && (!SameSignature(stack[stackSize - 1].typeInfo, methodSignature)))
			{
				VERIFY_TYPE_ERROR();
			}
		}
		else if(!unsafeAllowed)
		{
			VERIFY_TYPE_ERROR();
		}

		/* Pop the function pointer argument */
		--stackSize;

		/* Match the signature against the current stack contents */
		numParams = MatchSignature(coder, stack, stackSize,
								   methodSignature, 0,
								   unsafeAllowed, 0, 1,
								   &callInfo, tailCall);
		tailCall = 0;
		if(numParams >= 0)
		{
			returnType = ILTypeGetReturn(methodSignature);
			if(returnType != ILType_Void)
			{
				SetReturnType(&(stack[stackSize]), returnType);
			}
			else
			{
				stack[stackSize].engineType = ILEngineType_Invalid;
			}
			ILCoderCallIndirect(coder, &callInfo, &(stack[stackSize]));
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

		if (isSynchronized)
		{
			PUSH_SYNC_OBJECT();
			ILCoderCallInlineable(coder, IL_INLINEMETHOD_MONITOR_EXIT, 0);
		}

		/* Notify the coder of the return instruction */
		ILCoderReturnInsn(coder, stack[stackSize - 1].engineType, returnType);

		/* Pop the item from the stack */
		--stackSize;
	}
	else
	{
		if (isSynchronized)
		{
			PUSH_SYNC_OBJECT();
			ILCoderCallInlineable(coder, IL_INLINEMETHOD_MONITOR_EXIT, 0);
		}

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
									   unsafeAllowed, 0, 0,
									   &callInfo, tailCall);
			tailCall = 0;
			if(numParams >= 0)
			{
				returnType = ILTypeGetReturn(methodSignature);
				if(returnType != ILType_Void)
				{
					SetReturnType(&(stack[stackSize]), returnType);
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
					   !ILCoderCallInlineable(coder, inlineType, methodInfo))
					{
						if(numParams > 0 && !ILMethod_IsStatic(methodInfo) &&
						   (stack + stackSize - numParams)->engineType
						   		== ILEngineType_O)
						{
							/* Check the first parameter against "null" */
							ILCoderCheckCallNull(coder, &callInfo);
						}
						ILCoderCallMethod(coder, &callInfo,
										  &(stack[stackSize]), methodInfo);
					}
				}
				else if(ILClass_IsInterface(ILMethod_Owner(methodInfo)))
				{
					ILCoderCallInterface(coder, &callInfo,
										 &(stack[stackSize]), methodInfo);
				}
				else
				{
					ILCoderCallVirtual(coder, &callInfo,
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
			ThrowSystem("System.Security", "SecurityException");
		}
	}
	else
	{
		ThrowSystem("System", "MissingMethodException");
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
		if(ILTypeIsDelegate(ILType_FromClass(classInfo)))
		{
			/* Match a delegate constructor's signature */
			numParams = MatchDelegateSignature(coder, stack, stackSize,
									           methodSignature, classInfo);
			if(numParams < 0)
			{
				VERIFY_TYPE_ERROR();
			}

			/* Call the allocation constructor for the delegate */
			callInfo.args = stack + stackSize - numParams;
			callInfo.numBaseArgs = (ILUInt32)numParams;
			callInfo.numVarArgs = 0;
			callInfo.hasParamArray = 0;
			ILCoderCallCtor(coder, &callInfo, methodInfo);
			stackSize -= (ILUInt32)numParams;
			stack[stackSize].engineType = ILEngineType_O;
			stack[stackSize].typeInfo = ILClassToType(classInfo);
			++stackSize;
		}
		else if(!ILClassIsValueType(classInfo))
		{
			/* Match the signature for the allocation constructor */
			numParams = MatchSignature(coder, stack, stackSize,
									   methodSignature, methodInfo,
									   unsafeAllowed, 1, 0,
									   &callInfo, tailCall);
			tailCall = 0;
			if(numParams < 0)
			{
				VERIFY_TYPE_ERROR();
			}

			/* Call the allocation constructor for the class */
			ILCoderCallCtor(coder, &callInfo, methodInfo);
			stackSize -= (ILUInt32)numParams;
			if(stackSize < code->maxStack)
			{
				stack[stackSize].engineType = ILEngineType_O;
				stack[stackSize].typeInfo = ILClassToType(classInfo);
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
									   unsafeAllowed, 0, 0,
									   &callInfo, tailCall);
			tailCall = 0;
			if(numParams < 0)
			{
				VERIFY_TYPE_ERROR();
			}

			/* Call the constructor and pop all of its arguments */
			ILCoderCallMethod(coder, &callInfo, 0, methodInfo);
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
		ThrowSystem("System", "MissingMethodException");
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
			stack[stackSize].typeInfo =
				MethodToMethodRef(&allocator, methodInfo);
			if(!(stack[stackSize].typeInfo))
			{
				VERIFY_MEMORY_ERROR();
			}
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
		classInfo = ILMethod_Owner(methodInfo);
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
				stack[stackSize - 1].typeInfo =
					MethodToMethodRef(&allocator, methodInfo);
				if(!(stack[stackSize - 1].typeInfo))
				{
					VERIFY_MEMORY_ERROR();
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
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_JMPI:
{
	/* TODO */
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_TAIL:
{
	/* Need at least 8 bytes to make up the full tail call sequence */
	if(len < 8)
	{
		VERIFY_TRUNCATED();
	}

	/* Check that the instruction after the tail call is not a branch target */
	if(IsJumpTarget(jumpMask, offset + 2))
	{
		VERIFY_BRANCH_ERROR();
	}

	/* Check that the instruction sequence is as expected */
	if((pc[2] != IL_OP_CALL && pc[2] == IL_OP_CALLI &&
	    pc[2] != IL_OP_CALLVIRT) || pc[7] != IL_OP_RET)
	{
		VERIFY_INSN_ERROR();
	}

	/* Set the tail call flag and then advance to the next instruction */
	tailCall = 1;
}
break;

#endif /* IL_VERIFY_CODE */
