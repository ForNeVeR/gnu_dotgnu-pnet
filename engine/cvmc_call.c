/*
 * cvmc_call.c - Coder implementation for CVM method calls.
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

#ifdef IL_CVMC_CODE

static void CVMCoder_UpConvertArg(ILCoder *coder, ILEngineStackItem *args,
						          ILUInt32 numArgs, ILUInt32 param,
						          ILType *paramType)
{
	/* TODO */
}

static void CVMCoder_DownConvertArg(ILCoder *coder, ILEngineStackItem *args,
						            ILUInt32 numArgs, ILUInt32 param,
						            ILType *paramType)
{
	/* TODO */
}

static void CVMCoder_PackVarArgs(ILCoder *coder, ILType *callSiteSig,
					             ILUInt32 firstParam, ILEngineStackItem *args,
						         ILUInt32 numArgs)
{
#ifdef IL_CONFIG_VARARGS
	CVMP_OUT_WORD2_PTR(COP_PREFIX_PACK_VARARGS,
					   firstParam, numArgs, callSiteSig);
	CVM_ADJUST(-(ILInt32)(ComputeStackSize(coder, args, numArgs)));
	CVM_ADJUST(1);
#endif
}

static void CVMCoder_ValueCtorArgs(ILCoder *coder, ILClass *classInfo,
								   ILEngineStackItem *args, ILUInt32 numArgs)
{
	ILUInt32 posn = ComputeStackSize(coder, args, numArgs);
	ILUInt32 size = GetTypeSize(ILType_FromValueType(classInfo));
	CVM_OUT_DWIDE(COP_NEW_VALUE, posn, size);
	CVM_ADJUST(size + 1);
}

/*
 * Call the static constructor for a class if necessary.
 */
static void CallStaticConstructor(ILCoder *coder, ILClass *classInfo,
								  int isCtor)
{
	if((classInfo->attributes & IL_META_TYPEDEF_CCTOR_ONCE) != 0)
	{
		/* We already know that the static constructor has been called,
		   so there is no point outputting a call to it again */
		return;
	}
	if(isCtor ||
	   (classInfo->attributes & IL_META_TYPEDEF_BEFORE_FIELD_INIT) == 0)
	{
		/* We must call the static constructor before instance
		   constructors, or before static methods when the
		   "beforefieldinit" attribute is not present */
		ILMethod *cctor = 0;
		while((cctor = (ILMethod *)ILClassNextMemberByKind
					(classInfo, (ILMember *)cctor,
					 IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if(ILMethod_IsStaticConstructor(cctor))
			{
				break;
			}
		}
		if(cctor != 0)
		{
			/* Don't call it if we are within the constructor already */
			if(cctor != ((ILCVMCoder *)coder)->currentMethod)
			{
				/* Output a call to the static constructor */
				CVM_OUT_PTR(COP_CALL, cctor);
			}
		}
		else
		{
			/* This class does not have a static constructor,
			   so mark it so that we never do this test again */
			classInfo->attributes |= IL_META_TYPEDEF_CCTOR_ONCE;
		}
	}
}

/*
 * Adjust the position of the stack for a call.
 */
static void AdjustForCall(ILCoder *coder, ILEngineStackItem *args,
						  ILUInt32 numArgs, ILEngineStackItem *returnItem,
						  ILMethod *methodInfo)
{
	CVM_ADJUST(-(ILInt32)ComputeStackSize(coder, args, numArgs));
	if(returnItem != 0 && returnItem->engineType != ILEngineType_Invalid)
	{
		CVM_ADJUST(ComputeStackSize(coder, returnItem, 1));
	}
}

static void CVMCoder_CallMethod(ILCoder *coder, ILEngineStackItem *args,
					   		    ILUInt32 numArgs,
								ILEngineStackItem *returnItem,
								ILMethod *methodInfo)
{
	CallStaticConstructor(coder, ILMethod_Owner(methodInfo), 0);
	if(((ILCVMCoder *)coder)->tailCallFlag)
	{
		CVMP_OUT_PTR(COP_PREFIX_TAIL_CALL, methodInfo);
	}
	else
	{
		CVM_OUT_PTR(COP_CALL, methodInfo);
	}
	AdjustForCall(coder, args, numArgs, returnItem, methodInfo);
	((ILCVMCoder *)coder)->tailCallFlag = 0;
}

static void CVMCoder_CallIndirect(ILCoder *coder, ILEngineStackItem *args,
					   		      ILUInt32 numArgs,
								  ILEngineStackItem *returnItem)
{
	CVM_OUT_NONE(COP_CALLI);
	CVM_ADJUST(-1);	/* The function pointer was popped */
	AdjustForCall(coder, args, numArgs, returnItem, 0);
	((ILCVMCoder *)coder)->tailCallFlag = 0;
}

static void CVMCoder_CallCtor(ILCoder *coder, ILEngineStackItem *args,
					   		  ILUInt32 numArgs, ILMethod *methodInfo)
{
	CallStaticConstructor(coder, ILMethod_Owner(methodInfo), 1);
	CVM_OUT_PTR(COP_CALL_CTOR, methodInfo);
	CVM_ADJUST(-(ILInt32)ComputeStackSize(coder, args, numArgs));
	CVM_ADJUST(1);
	((ILCVMCoder *)coder)->tailCallFlag = 0;
}

static void CVMCoder_CallVirtual(ILCoder *coder, ILEngineStackItem *args,
					    	     ILUInt32 numArgs,
								 ILEngineStackItem *returnItem,
								 ILMethod *methodInfo)
{
	ILUInt32 argSize = ComputeStackSize(coder, args, numArgs);
	CVM_OUT_DWIDE(COP_CALL_VIRTUAL, argSize, methodInfo->index);
	AdjustForCall(coder, args, numArgs, returnItem, methodInfo);
	((ILCVMCoder *)coder)->tailCallFlag = 0;
}

static void CVMCoder_CallInterface(ILCoder *coder, ILEngineStackItem *args,
					      		   ILUInt32 numArgs,
								   ILEngineStackItem *returnItem,
								   ILMethod *methodInfo)
{
	void *ptr = ILMethod_Owner(methodInfo);
	ILUInt32 argSize = ComputeStackSize(coder, args, numArgs);
	CVM_OUT_DWIDE_PTR(COP_CALL_INTERFACE, argSize, methodInfo->index, ptr);
	AdjustForCall(coder, args, numArgs, returnItem, methodInfo);
	((ILCVMCoder *)coder)->tailCallFlag = 0;
}

static int CVMCoder_CallInlineable(ILCoder *coder, int inlineType)
{
	/* Inline methods cannot be tail calls */
	((ILCVMCoder *)coder)->tailCallFlag = 0;

	/* Determine what to do for the inlineable method type */
	switch(inlineType)
	{
		case IL_INLINEMETHOD_MONITOR_ENTER:
		{
			/* We don't support threads yet, so just pop the object */
			/* TODO: support for real threads */
			CVM_OUT_NONE(COP_POP);
			CVM_ADJUST(-1);
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_MONITOR_EXIT:
		{
			/* We don't support threads yet, so just pop the object */
			/* TODO: support for real threads */
			CVM_OUT_NONE(COP_POP);
			CVM_ADJUST(-1);
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_STRING_LENGTH:
		{
			/* The string length is at a fixed offset from the pointer */
			CVM_OUT_BYTE(COP_IREAD_FIELD,
						 (unsigned)&(((System_String *)0)->length));
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_STRING_CONCAT_2:
		{
			/* Concatenate two string objects */
			CVMP_OUT_NONE(COP_PREFIX_STRING_CONCAT_2);
			CVM_ADJUST(-1);
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_STRING_CONCAT_3:
		{
			/* Concatenate three string objects */
			CVMP_OUT_NONE(COP_PREFIX_STRING_CONCAT_3);
			CVM_ADJUST(-2);
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_STRING_CONCAT_4:
		{
			/* Concatenate four string objects */
			CVMP_OUT_NONE(COP_PREFIX_STRING_CONCAT_4);
			CVM_ADJUST(-3);
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_STRING_EQUALS:
		{
			/* Compare two string objects for equality */
			CVMP_OUT_NONE(COP_PREFIX_STRING_EQ);
			CVM_ADJUST(-1);
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_STRING_NOT_EQUALS:
		{
			/* Compare two string objects for inequality */
			CVMP_OUT_NONE(COP_PREFIX_STRING_NE);
			CVM_ADJUST(-1);
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_STRING_GET_CHAR:
		{
			/* Compare two string objects for equality */
			CVMP_OUT_NONE(COP_PREFIX_STRING_GET_CHAR);
			CVM_ADJUST(-1);
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_TYPE_FROM_HANDLE:
		{
			/* Convert a RuntimeTypeHandle into a Type object */
			CVMP_OUT_NONE(COP_PREFIX_TYPE_FROM_HANDLE);
			return 1;
		}
		/* Not reached */
	}

	/* If we get here, then we don't know how to inline the method */
	return 0;
}

static void CVMCoder_JumpMethod(ILCoder *coder, ILMethod *methodInfo)
{
	/* TODO */
}

static void CVMCoder_ReturnInsn(ILCoder *coder, ILEngineType engineType,
							    ILType *returnType)
{
	switch(engineType)
	{
		case ILEngineType_Invalid:
		{
			CVM_OUT_NONE(COP_RETURN);
		}
		break;

		case ILEngineType_M:
		case ILEngineType_O:
		case ILEngineType_T:
		case ILEngineType_I4:
	#ifdef IL_NATIVE_INT32
		case ILEngineType_I:
	#endif
		{
			CVM_OUT_NONE(COP_RETURN_1);
			CVM_ADJUST(-1);
		}
		break;

		case ILEngineType_I8:
	#ifdef IL_NATIVE_INT64
		case ILEngineType_I:
	#endif
		{
			CVM_OUT_RETURN(CVM_WORDS_PER_LONG);
			CVM_ADJUST(-CVM_WORDS_PER_LONG);
		}
		break;

		case ILEngineType_F:
		{
			CVM_OUT_RETURN(CVM_WORDS_PER_NATIVE_FLOAT);
			CVM_ADJUST(-CVM_WORDS_PER_NATIVE_FLOAT);
		}
		break;

		case ILEngineType_MV:
		{
			ILUInt32 size = GetTypeSize(returnType);
			CVM_OUT_RETURN(size);
			CVM_ADJUST(-(ILInt32)size);
		}
		break;

		case ILEngineType_TypedRef:
		{
			CVM_OUT_RETURN(CVM_WORDS_PER_TYPED_REF);
			CVM_ADJUST(-CVM_WORDS_PER_TYPED_REF);
		}
		break;
	}
}

static void CVMCoder_LoadFuncAddr(ILCoder *coder, ILMethod *methodInfo)
{
	CVMP_OUT_PTR(COP_PREFIX_LDFTN, methodInfo);
	CVM_ADJUST(1);
}

static void CVMCoder_LoadVirtualAddr(ILCoder *coder, ILMethod *methodInfo)
{
	CVMP_OUT_WORD(COP_PREFIX_LDVIRTFTN, methodInfo->index);
}

static void CVMCoder_LoadInterfaceAddr(ILCoder *coder, ILMethod *methodInfo)
{
	CVMP_OUT_WORD_PTR(COP_PREFIX_LDINTERFFTN, methodInfo->index,
					  methodInfo->member.owner);
}

static void CVMCoder_TailCall(ILCoder *coder, ILMethod *methodInfo)
{
	((ILCVMCoder *)coder)->tailCallFlag = 1;
}

#endif	/* IL_CVMC_CODE */
