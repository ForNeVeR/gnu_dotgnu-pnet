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

static void CVMCoder_ValueCtorArgs(ILCoder *coder, ILClass *classInfo,
								   ILEngineStackItem *args, ILUInt32 numArgs)
{
	ILUInt32 posn = ComputeStackSize(coder, args, numArgs);
	ILUInt32 size = GetTypeSize(ILType_FromValueType(classInfo));
	CVM_DWIDE(COP_NEW_VALUE, posn, size);
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
				CVM_BYTE(COP_CALL_EXTERN);
				CVM_WORD(0);
				CVM_PTR(cctor);
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
	CVM_BYTE(COP_CALL_EXTERN);
	CVM_WORD(0);
	CVM_PTR(methodInfo);
	AdjustForCall(coder, args, numArgs, returnItem, methodInfo);
}

static void CVMCoder_CallCtor(ILCoder *coder, ILEngineStackItem *args,
					   		  ILUInt32 numArgs, ILMethod *methodInfo)
{
	CallStaticConstructor(coder, ILMethod_Owner(methodInfo), 1);
	CVM_BYTE(COP_CALL_CTOR);
	CVM_WORD(0);
	CVM_PTR(methodInfo);
	CVM_ADJUST(-(ILInt32)ComputeStackSize(coder, args, numArgs));
	CVM_ADJUST(1);
}

static void CVMCoder_CallVirtual(ILCoder *coder, ILEngineStackItem *args,
					    	     ILUInt32 numArgs,
								 ILEngineStackItem *returnItem,
								 ILMethod *methodInfo)
{
	ILUInt32 argSize = ComputeStackSize(coder, args, numArgs);
	CVM_DWIDE(COP_CALL_VIRTUAL, argSize, methodInfo->index);
	AdjustForCall(coder, args, numArgs, returnItem, methodInfo);
}

static void CVMCoder_CallInterface(ILCoder *coder, ILEngineStackItem *args,
					      		   ILUInt32 numArgs,
								   ILEngineStackItem *returnItem,
								   ILMethod *methodInfo)
{
	void *ptr = ILMethod_Owner(methodInfo);
	ILUInt32 argSize = ComputeStackSize(coder, args, numArgs);
	CVM_DWIDE(COP_CALL_INTERFACE, argSize, methodInfo->index);
	CVM_PTR(ptr);
	AdjustForCall(coder, args, numArgs, returnItem, methodInfo);
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
			CVM_BYTE(COP_RETURN);
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
			CVM_BYTE(COP_RETURN_1);
			CVM_ADJUST(-1);
		}
		break;

		case ILEngineType_I8:
	#ifdef IL_NATIVE_INT64
		case ILEngineType_I:
	#endif
		{
			CVM_RETURN(CVM_WORDS_PER_LONG);
			CVM_ADJUST(-CVM_WORDS_PER_LONG);
		}
		break;

		case ILEngineType_F:
		{
			CVM_RETURN(CVM_WORDS_PER_NATIVE_FLOAT);
			CVM_ADJUST(-CVM_WORDS_PER_NATIVE_FLOAT);
		}
		break;

		case ILEngineType_MV:
		{
			ILUInt32 size = GetTypeSize(returnType);
			CVM_RETURN(size);
			CVM_ADJUST(-(ILInt32)size);
		}
		break;

		case ILEngineType_TypedRef:
		{
			CVM_RETURN(CVM_WORDS_PER_TYPED_REF);
			CVM_ADJUST(-CVM_WORDS_PER_TYPED_REF);
		}
		break;
	}
}

static void CVMCoder_LoadFuncAddr(ILCoder *coder, ILMethod *methodInfo)
{
	/* TODO */
}

static void CVMCoder_LoadVirtualAddr(ILCoder *coder, ILMethod *methodInfo)
{
	/* TODO */
}

static void CVMCoder_LoadInterfaceAddr(ILCoder *coder, ILMethod *methodInfo)
{
	/* TODO */
}

#endif	/* IL_CVMC_CODE */
