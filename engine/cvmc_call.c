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
	ILUInt32 size = GetTypeSize(coder->thread, ILType_FromValueType(classInfo));
	CVM_DWIDE(COP_NEW_VALUE, posn, size);
	CVM_ADJUST(size + 1);
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
	CVM_BYTE(COP_CALL_EXTERN);
	CVM_WORD(0);
	CVM_PTR(methodInfo);
	AdjustForCall(coder, args, numArgs, returnItem, methodInfo);
}

static void CVMCoder_CallCtor(ILCoder *coder, ILEngineStackItem *args,
					   		  ILUInt32 numArgs, ILMethod *methodInfo)
{
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
	CVM_WIDE(COP_CALL_VIRTUAL, methodInfo->index);
	AdjustForCall(coder, args, numArgs, returnItem, methodInfo);
}

static void CVMCoder_CallInterface(ILCoder *coder, ILEngineStackItem *args,
					      		   ILUInt32 numArgs,
								   ILEngineStackItem *returnItem,
								   ILMethod *methodInfo)
{
	void *ptr = ILMethod_Owner(methodInfo);
	CVM_WIDE(COP_CALL_INTERFACE, methodInfo->index);
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
			ILUInt32 size = GetTypeSize(coder->thread, returnType);
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

#endif	/* IL_CVMC_CODE */
