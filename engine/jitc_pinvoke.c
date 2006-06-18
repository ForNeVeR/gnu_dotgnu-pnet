/*
 * jitc_pinvoke.c - Handle pinvokes and marshaling within the JIT.
 *
 * Copyright (C) 2006  Southern Storm Software, Pty Ltd.
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

/*
 * On demand code generator.for functions implemented in IL code.
 */
static int _ILJitCompilePinvoke(jit_function_t func)
{
	ILMethod *method = (ILMethod *)jit_function_get_meta(func, IL_JIT_META_METHOD);
	ILPInvoke *pinv = ILPInvokeFind(method);
	jit_abi_t jitAbi;
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	ILClass *info = ILMethod_Owner(method);
	ILClassPrivate *classPrivate = (ILClassPrivate *)info->userData;
	ILJITCoder *jitCoder = (ILJITCoder *)(classPrivate->process->coder);
	char *methodName = _ILJitFunctionGetMethodName(func);
#endif
	ILJitMethodInfo *jitMethodInfo = (ILJitMethodInfo *)(method->userData);
	ILJitType signature = jit_function_get_signature(func);
	ILJitType returnType = jit_type_get_return(signature);
	unsigned int numParams = jit_type_num_params(signature);
	ILJitType callSignature;
	ILJitValue returnValue;
	ILUInt32 current;
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	unsigned int totalParams = numParams - 1;
#else
	unsigned int totalParams = numParams;
#endif

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout, "CompilePinvoke: %s\n", methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	/* TODO */
	if(!pinv)
	{
		/* The pinvoke record could not be found. */
		return JIT_RESULT_COMPILE_ERROR;
	}

	/* determine which calling convention to use. */
	switch(pinv->member.attributes & IL_META_PINVOKE_CALL_CONV_MASK)
	{
		case IL_META_PINVOKE_CALL_CONV_WINAPI:
		{
			/* TODO: There is no winapi calling convention in libjit. */
			jitAbi = IL_JIT_CALLCONV_STDCALL;
		}
		break;

		case IL_META_PINVOKE_CALL_CONV_CDECL:
		{
			jitAbi = IL_JIT_CALLCONV_CDECL;
		}
		break;

		case IL_META_PINVOKE_CALL_CONV_STDCALL:
		{
			jitAbi = IL_JIT_CALLCONV_STDCALL;
		}
		break;

		case IL_META_PINVOKE_CALL_CONV_FASTCALL:
		{
			jitAbi = IL_JIT_CALLCONV_FASTCALL;
		}
		break;

		default:
		{
			/* There is an invalid calling convention inthe metadata. */
			return JIT_RESULT_COMPILE_ERROR;
		}
	}

	if(totalParams > 0)
	{
		ILJitType paramType;
		ILJitType jitParamTypes[totalParams];
		ILJitValue jitParams[totalParams];
		ILUInt32 param = 0;

#ifdef IL_JIT_THREAD_IN_SIGNATURE
		for(current = 1; current < numParams; ++current)
#else
		for(current = 0; current < numParams; ++current)
#endif
		{
			paramType = jit_type_get_param(signature, current);
			jitParams[param] = jit_value_get_param(func, current);
			jitParamTypes[param] = paramType;
			++param;
		}
		callSignature = jit_type_create_signature(jitAbi,
												  returnType,
												  jitParamTypes,
												  totalParams, 1);

		returnValue = jit_insn_call_native(func, 0, jitMethodInfo->fnInfo.func,
								 		   callSignature,
										   jitParams, totalParams, 0);
	}
	else
	{
		callSignature = jit_type_create_signature(jitAbi,
												  returnType,
												  0, 0, 1);

		returnValue = jit_insn_call_native(func, 0, jitMethodInfo->fnInfo.func,
										   callSignature, 0, 0, 0);
	}
	jit_type_free(callSignature);
	jit_insn_return(func, returnValue);	

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
#ifdef _IL_JIT_DUMP_FUNCTION
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		jit_dump_function(stdout, func, methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#ifdef _IL_JIT_DISASSEMBLE_FUNCTION
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		if(!jit_function_compile(func))
		{
			return JIT_RESULT_COMPILE_ERROR;
		}
		ILMutexLock(globalTraceMutex);
		jit_dump_function(stdout, func, methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#endif
	return JIT_RESULT_OK;
}

