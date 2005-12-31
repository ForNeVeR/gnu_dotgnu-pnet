/*
 * jitc_call.c - Coder implementation for JIT method calls.
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
 * Call the static constructor for a class if necessary.
 */
static void _ILJitCallStaticConstructor(ILCoder *_coder, ILClass *classInfo,
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
		ILJITCoder *coder = _ILCoderToILJITCoder(_coder);

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
			if(cctor != coder->currentMethod)
			{
				/* Output a call to the static constructor */
				jit_value_t thread = jit_value_get_param(coder->jitFunction, 0);

				jit_insn_call(coder->jitFunction, "cctor",
								ILJitFunctionFromILMethod(cctor), 0,
								&thread, 1, 0);
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
 * Create a new object and push it on the stack.
 */
static void _ILJitNewObj(ILJITCoder *coder, ILClass *info, ILJitValue *newArg)
{
	ILJitValue newObj;
	ILJitValue args[3];
	jit_label_t label1 = jit_label_undefined;
	
	args[0] = jit_value_get_param(coder->jitFunction, 0);
	args[1] = jit_value_create_nint_constant(coder->jitFunction,
												jit_type_void_ptr, (jit_nint)info);
	args[2] = jit_value_create_nint_constant(coder->jitFunction,
												jit_type_int, 0);
	newObj = jit_insn_call_native(coder->jitFunction, 0, _ILEngineAlloc,
									_ILJitSignature_ILEngineAlloc, args, 3, 0);
	jit_insn_branch_if(coder->jitFunction, newObj, &label1);
	_ILJitThrowCurrentException(coder);
	jit_insn_label(coder->jitFunction, &label1);
	*newArg = newObj;
}

/*
 * Call a Method.
 */
static void _ILJitCallMethod(ILJITCoder *coder, ILJitValue *stackTop, int isCtor)
{

}

static void JITCoder_UpConvertArg(ILCoder *coder, ILEngineStackItem *args,
						          ILUInt32 numArgs, ILUInt32 param,
						          ILType *paramType)
{
}

static void JITCoder_DownConvertArg(ILCoder *coder, ILEngineStackItem *args,
						            ILUInt32 numArgs, ILUInt32 param,
						            ILType *paramType)
{
}

static void JITCoder_PackVarArgs(ILCoder *coder, ILType *callSiteSig,
					             ILUInt32 firstParam, ILEngineStackItem *args,
						         ILUInt32 numArgs)
{
}

static void JITCoder_ValueCtorArgs(ILCoder *coder, ILClass *classInfo,
								   ILEngineStackItem *args, ILUInt32 numArgs)
{
}

static void JITCoder_CheckCallNull(ILCoder *coder, ILCoderMethodInfo *info)
{
}

static void JITCoder_CallMethod(ILCoder *coder, ILCoderMethodInfo *info,
								ILEngineStackItem *returnItem,
								ILMethod *methodInfo)
{
}

static void JITCoder_CallIndirect(ILCoder *coder, ILCoderMethodInfo *info,
								  ILEngineStackItem *returnItem)
{
}

static void JITCoder_CallCtor(ILCoder *coder, ILCoderMethodInfo *info,
					   		  ILMethod *methodInfo)
{
}

static void JITCoder_CallVirtual(ILCoder *coder, ILCoderMethodInfo *info,
								 ILEngineStackItem *returnItem,
								 ILMethod *methodInfo)
{
}

static void JITCoder_CallInterface(ILCoder *coder, ILCoderMethodInfo *info,
								   ILEngineStackItem *returnItem,
								   ILMethod *methodInfo)
{
}

static int JITCoder_CallInlineable(ILCoder *coder, int inlineType,
								   ILMethod *methodInfo)
{
	return 0;
}

static void JITCoder_JumpMethod(ILCoder *coder, ILMethod *methodInfo)
{
}

static void JITCoder_ReturnInsn(ILCoder *coder, ILEngineType engineType,
							    ILType *returnType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	if(engineType == ILEngineType_Invalid)
	{
	       jit_insn_return(jitCoder->jitFunction, 0);
	}
	else
	{
		jit_insn_return(jitCoder->jitFunction,
					jitCoder->jitStack[jitCoder->stackTop - 1]);
		JITC_ADJUST(jitCoder, -1);
	}
}

static void JITCoder_LoadFuncAddr(ILCoder *coder, ILMethod *methodInfo)
{
}

static void JITCoder_LoadVirtualAddr(ILCoder *coder, ILMethod *methodInfo)
{
}

static void JITCoder_LoadInterfaceAddr(ILCoder *coder, ILMethod *methodInfo)
{
}

#endif	/* IL_JITC_CODE */
