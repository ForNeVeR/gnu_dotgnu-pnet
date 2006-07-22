/*
 * jitc_pinvoke.c - Handle marshaling within the JIT.
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

#define MARSHAL_FIRST_LEVEL_VALUE		0
#define MARSHAL_ITEM_OF_STRUCTURE		1
#define MARSHAL_ITEM_OF_ARRAY			2


static int NeedMarshalValue(ILType *type);
static int NeedMarshalStruct(ILType *structureILType);

static ILJitValue MarshalValue(jit_function_t function, ILJitValue in, ILType *type, ILUInt32 current, unsigned int addressKind, jit_nint offset, ILJitValue outAddress);
static ILJitValue MarshalStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind);

static ILJitValue MarshalReturnValue(jit_function_t function, ILJitValue in, ILType *type, unsigned int addressKind, jit_nint offset, ILJitValue outAddress);
static ILJitValue MarshalReturnStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind);

static void MarshalByRefValue(jit_function_t function, ILJitValue in, ILType *type, ILUInt32 current, unsigned int addressKind, jit_nint offset, ILJitValue outAddress);
static void MarshalByRefStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind);

void *ILJitDelegateGetClosure(ILExecThread *currentThread, ILObject *del, ILType *delType);

static ILJitValue MarshalDelegateValue(jit_function_t function, ILJitValue in, ILType *type, ILUInt32 current, unsigned int addressKind, jit_nint offset, ILJitValue outAddress, ILExecThread *thread);
static ILJitValue MarshalDelegateStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind, ILExecThread *thread);

static ILJitValue MarshalDelegateReturnValue(jit_function_t function, ILJitValue in, ILType *type, unsigned int addressKind, jit_nint offset, ILJitValue outAddress, ILExecThread *thread);
static ILJitValue MarshalDelegateReturnStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind, ILExecThread *thread);

static void MarshalDelegateByRefValue(jit_function_t function, ILJitValue in, ILType *type, ILUInt32 current, unsigned int addressKind, jit_nint offset, ILJitValue outAddress, ILExecThread *thread);
static void MarshalDelegateByRefStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind, ILExecThread *thread);

/*
 * On demand code generator.for functions implemented in IL code.
 */
static int _ILJitCompilePinvoke(jit_function_t func)
{
	ILMethod *method = (ILMethod *)jit_function_get_meta(func, IL_JIT_META_METHOD);
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	ILClass *info = ILMethod_Owner(method);
	ILClassPrivate *classPrivate = (ILClassPrivate *)info->userData;
	ILJITCoder *jitCoder = (ILJITCoder*)(classPrivate->process->coder);
	char *methodName = _ILJitFunctionGetMethodName(func);
#endif
	ILJitMethodInfo *jitMethodInfo = (ILJitMethodInfo *)(method->userData);
	ILType *ilSignature = ILMethod_Signature(method);
	ILType *type;
	ILJitType signature = jit_function_get_signature(func);
	ILPInvoke *pinv = ILPInvokeFind(method);
	jit_abi_t jitAbi;
	ILUInt32 numParams = jit_type_num_params(signature);
	ILJitType returnType = jit_type_get_return(signature);
	ILJitValue returnValue = jit_value_create(func, jit_type_void_ptr);
	unsigned int current;
	unsigned int typeFlag;
	ILJitValue newValue;
	ILType *valueType;

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout, "CompilePinvoke: %s\n", methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

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


	ILJitType paramType;
	ILJitType jitParamTypes[numParams + 1];
	ILJitValue jitParams[numParams + 1];
	ILJitValue tempParams[numParams + 1];
	ILUInt32 param = 0;

	for(current = 1; current < numParams; ++current)
	{

		paramType = jit_type_get_param(signature, current);
		valueType = ILTypeGetParam(ilSignature, current);
		valueType = ILTypeGetEnumType(valueType);
		tempParams[param] = jit_insn_dup(func, jit_value_get_param(func, current));
		jitParams[param] = (ILJitValue)MarshalValue(func, tempParams[param], valueType, current, 0, 0, 0);
		jitParamTypes[param] = paramType;
		++param;
	}
	ILJitType callSignature = jit_type_create_signature(jitAbi,
								   returnType,
								   jitParamTypes,
								   numParams - 1, 1);
	if(numParams > 1)
	{														   
		type = ILTypeGetEnumType(ILTypeGetReturn(ilSignature));
		returnValue = jit_insn_call_native(func, 0, jitMethodInfo->fnInfo.func,
								 callSignature,
								 jitParams, numParams - 1, 0);
		jit_insn_store(func, returnValue, (ILJitValue)MarshalReturnValue(func, jit_insn_dup(func, returnValue), type, 0, 0, 0));
		param = 0;
		for(current = 1; current < numParams; ++current)
		{
			newValue = jit_insn_dup(func, jitParams[param]);
			type = ILTypeGetParam(ilSignature, current);
			type = ILTypeGetEnumType(type);

			if(type!=0 && ILType_IsSimpleArray(type))
			{
				MarshalByRefValue(func, newValue, type, current, 0, 0, tempParams[param]);
			}
			else if(type!=0 && ILType_IsComplex(type))
			{
				typeFlag = ILType_Kind(type);
		    		if(typeFlag==IL_TYPE_COMPLEX_BYREF || typeFlag == IL_TYPE_COMPLEX_PTR)
				{
					MarshalByRefValue(func, newValue, type,
								    current, 0,
								    0, tempParams[param]);
				}
			}
			++param;
		}
		jit_insn_return(func, returnValue);

	}
	else
	{
		jit_insn_call_native(func, 0, jitMethodInfo->fnInfo.func,
									callSignature, 0, 0, 0);
		jit_insn_return(func, 0);
	}
	jit_type_free(callSignature);	

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

static int NeedMarshalValue(ILType *type)
{
    type = ILTypeGetEnumType(type);
    if(ILType_IsPrimitive(type))
    {
	switch(ILType_ToElement(type))
	{
		case IL_META_ELEMTYPE_VOID:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_BOOLEAN:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_I1:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_U1:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_I2:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_U2:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_CHAR:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_I4:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_U4:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_I8:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_U8:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_R4:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_R8:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_I:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_U:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_TYPEDBYREF:
		{
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_OBJECT:
		{	
			return 0;
		}
		break;
		default: break;
	}			
    }
    else if(ILType_IsValueType(type))
    {	
	    return NeedMarshalStruct(type);
    }
    else if(ILTypeIsStringClass(type))
    {
	    return 1;
    }
    else if(ILTypeIsDelegateSubClass(type))
    {
	    return 1;
    }
    else if(ILType_IsClass(type))
    {
	    return 0;
    }
    else if(type !=0 && ILType_IsComplex(type))
    {
	    
	    switch(ILType_Kind(type))
	    {
		    case IL_TYPE_COMPLEX_PTR:
		    case IL_TYPE_COMPLEX_BYREF:
		    {
			    return 1;
		    }
		    break;
		    default:
		    {

		    }
		    break;
	    }
    }

    if(ILType_IsSimpleArray(type))
    {
	    return 1;
    }
    else if(ILType_IsArray(type))
    {
    	    return 1;
    }

    return 1;
}

static int NeedMarshalStruct(ILType *structureILType)
{
	ILClass *classInfo = ILClassResolve(ILType_ToValueType(structureILType));
	ILField *field = 0;
	ILType *type;

	while((field = (ILField *)ILClassNextMemberByKind
			(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if(!ILField_IsStatic(field))
		{
			type = ILTypeGetEnumType(ILField_Type(field));
			if(NeedMarshalValue(type))return 1;
		}
	}
	return 0;
}


static ILJitValue MarshalValue(jit_function_t function, ILJitValue in, ILType *type, ILUInt32 current, unsigned int addressKind, jit_nint offset, ILJitValue outAddress)
{
    ILJitValue outAddressValue = outAddress;
    ILJitValue temp = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue ptr, valuePtr;
    ILType *refType;
    ILJitValue srcArray = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue args[3];
    ILJitValue srcElemSize;
    ILType *elemType;
    ILExecThread *thread;
    ILJitValue newElemSize, newArraySize;
    ILJitValue newArray = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    jit_label_t startLoop = jit_label_undefined;
    jit_label_t endLoop = jit_label_undefined;
    jit_label_t endLabel = jit_label_undefined;
    ILJitValue srcElemAddress;
    ILJitValue newElemAddress;
    ILJitValue counter;
    ILJitValue arraySize;
    ILUInt32 marshalType;
    char *customName;
    int customNameLen;
    char *customCookie;
    int customCookieLen;
    ILMethod *method = (ILMethod *)jit_function_get_meta(function, IL_JIT_META_METHOD);
    ILPInvoke *pinv = ILPInvokeFind(method);
    marshalType = ILPInvokeGetMarshalType(pinv, method, current, &customName, &customNameLen,
							&customCookie,	&customCookieLen, type);
    if(marshalType==IL_META_MARSHAL_DIRECT && addressKind==MARSHAL_FIRST_LEVEL_VALUE) 
    {
    	return in;
    }
    else if(marshalType==IL_META_MARSHAL_CUSTOM)
    {
	    fprintf(stderr, "Custom marshaling not yet supported\n");
	    return 0;
    }

    if(!NeedMarshalValue(type) && addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    return in;
    }    
    type = ILTypeGetEnumType(type);
    if(addressKind==MARSHAL_ITEM_OF_ARRAY || addressKind == MARSHAL_ITEM_OF_STRUCTURE) outAddressValue = outAddress;
    else if(addressKind!=MARSHAL_FIRST_LEVEL_VALUE && addressKind!=MARSHAL_ITEM_OF_STRUCTURE)
    {
	    fprintf(stderr, "addressKind has an invalid value of %d\n", addressKind);
	    return 0;
    }
    if(ILType_IsPrimitive(type))
    {
	switch(ILType_ToElement(type))
	{
		case IL_META_ELEMTYPE_VOID:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_BOOLEAN:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_BYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I1:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_SBYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U1:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_BYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I2:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT16);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U2:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT16);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_CHAR:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_CHAR);				
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT32);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT32);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT64);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT64);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;			
		}
		break;
		case IL_META_ELEMTYPE_R4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_SINGLE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_R8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_DOUBLE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I:
		{			
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_NINT);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_NUINT);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;			
		}
		break;
		case IL_META_ELEMTYPE_TYPEDBYREF:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) temp = in;
			else temp = jit_insn_load_relative(function, in, offset, _ILJitTypedRef);
			ptr = jit_insn_address_of(function, temp);			
			valuePtr = jit_insn_load_relative(function, 
								ptr,
								offsetof(ILTypedRef, value),
								_IL_JIT_TYPE_VPTR);
			refType = ILType_Ref(type);
			switch(addressKind)
			{
				case MARSHAL_ITEM_OF_ARRAY:
				{
					return (ILJitValue)MarshalStruct(function, valuePtr, refType, outAddressValue, offset, MARSHAL_ITEM_OF_ARRAY);
				}
				break;
				case MARSHAL_ITEM_OF_STRUCTURE:
				{
					return (ILJitValue)MarshalStruct(function, valuePtr, refType, outAddressValue, offset, MARSHAL_ITEM_OF_STRUCTURE);
				}
				break;
				default:
				{
					return (ILJitValue)MarshalStruct(function, valuePtr, refType, 0, 0, MARSHAL_FIRST_LEVEL_VALUE);
				}
				break;
			}
		}
		break;
		case IL_META_ELEMTYPE_OBJECT:
		{	
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		default:  break;
	}			
    }
    else if (ILType_IsValueType(type))
    {
	    switch(addressKind)
	    {
		    case MARSHAL_ITEM_OF_STRUCTURE:
		    case MARSHAL_ITEM_OF_ARRAY:
		    {
			    return (ILJitValue)MarshalStruct(function, in, type, outAddressValue, offset, MARSHAL_ITEM_OF_ARRAY);
		    }
		    break;
		    default:
		    {
			    return (ILJitValue)MarshalStruct(function, jit_insn_address_of(function, in), type, 0, 0, MARSHAL_FIRST_LEVEL_VALUE);
		    }
		    break;
	    }
    }
    else if(ILTypeIsStringClass(type))
    {
	    args[0] = _ILJitFunctionGetThread(function);
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) args[1] = in;
	    else args[1] = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);

	    switch(marshalType)
	    {
			case IL_META_MARSHAL_ANSI_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToAnsi",
						    ILStringToAnsi,
						    _ILJitSignature_ILStringToAnsi,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF8_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToUTF8",
						    ILStringToUTF8,
						    _ILJitSignature_ILStringToUTF8,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF16_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToUTF16",
						    ILStringToUTF16,
						    _ILJitSignature_ILStringToUTF16,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			default:
			{
				fprintf(stderr, "Unsupported string char set in marshaling\n");
				return 0;
			};
	    }

	    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
		    jit_insn_store_relative(function, outAddressValue, offset, temp);
	    return temp;
    }

    else if(type!=0 && ILTypeIsDelegate(type))
    {
	    temp = jit_insn_dup(function, in);
	    if(addressKind!=MARSHAL_FIRST_LEVEL_VALUE)
	    {
		temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    }
	    ILJitValue closure = jit_insn_load_relative(function, temp, offsetof(System_Delegate, closure), _IL_JIT_TYPE_VPTR);
	    jit_insn_branch_if(function, jit_insn_to_bool(function, closure), &endLabel);
	    args[0] = _ILJitFunctionGetThread(function);
	    args[1] = jit_insn_dup(function, temp);
	    args[2] = jit_value_create_nint_constant(function, _IL_JIT_TYPE_VPTR, (jit_nint)type);
	    jit_insn_store(function, closure, jit_insn_call_native(function,
					    "ILJitDelegateGetClosure",
					    ILJitDelegateGetClosure,
					    _ILJitSignature_ILJitDelegateGetClosure,
					    args, 3, JIT_CALL_NOTHROW));
	    jit_insn_label(function, &endLabel);
	    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
		    jit_insn_store_relative(function, outAddressValue, offset, closure);
	    return closure; 
    }
    else if(type!=0 && ILType_IsComplex(type))
    {
	    switch(ILType_Kind(type))
	    {
		    case IL_TYPE_COMPLEX_PTR:
		    {
			    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			    jit_insn_store_relative(function, outAddressValue, offset, temp);
			    return temp;
		    }
		    break;
		    case IL_TYPE_COMPLEX_BYREF:
		    {
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    jit_insn_store(function, srcArray, jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR));
			    else jit_insn_store(function, srcArray, in);
			    elemType = ILType_Ref(type);
			    if(NeedMarshalValue(elemType))
			    {
				        MarshalValue(function, jit_insn_dup(function, srcArray),
						    	    elemType, 0, 
							    MARSHAL_ITEM_OF_STRUCTURE,
							    0,
							    jit_insn_dup(function, srcArray));
			    }
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY) 
		    		jit_insn_store_relative(function, outAddressValue, offset, srcArray);
			    return srcArray;
		    }
		    break;
		    default: break;
		    }

		    if(ILType_IsSimpleArray(type))
		    {
			    srcArray = in;
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    srcArray = jit_insn_load_relative(function, srcArray, offset, _IL_JIT_TYPE_VPTR);
			    elemType = ILType_ElemType(type);
			    if(!NeedMarshalValue(elemType))
			    {
				    jit_insn_store(function, newArray, srcArray);
				    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)
				    {
					    jit_insn_store(function, newArray, jit_insn_add_relative(function,
				    	    					newArray,
				    						(jit_nint)(sizeof(ILUInt32))));
				    }
				    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    	    jit_insn_store_relative(function, outAddressValue, offset, newArray);
				    return newArray;
			    }

			    // The size of a simple array can be known only on run-time.
			    arraySize = jit_insn_load_relative(function, srcArray, 0, _IL_JIT_TYPE_UINT32);
			    thread = ILExecThreadCurrent();
			    srcElemSize = jit_value_create_nint_constant(function,
				    						_IL_JIT_TYPE_INT32,
										(jit_nint)(ILSizeOfType(thread, elemType)));
			    newElemSize = jit_value_create_nint_constant(function,
										_IL_JIT_TYPE_INT32,
									    	(jit_nint)(ILSizeOfType(thread, elemType)));
			    newArraySize = jit_insn_mul(function, arraySize, newElemSize);
			    newArraySize = jit_insn_add_relative(function, newArraySize, (jit_nint)sizeof(ILUInt32));
		 	    args[0] = newArraySize;
			    jit_insn_store(function, newArray, jit_insn_call_native(function, "_ILMalloc",
			    			    			ILGCAlloc,
			    			    			_ILJitSignature_malloc,
			    						args, 1, 0));

			    jit_insn_branch_if(function,
						jit_insn_eq(function, arraySize,
									jit_value_create_nint_constant
										(function, _IL_JIT_TYPE_UINT32, 0)),
										&endLoop);
			    srcElemAddress = jit_insn_add_relative(function,
									srcArray,
									(jit_nint)(sizeof(ILUInt32)));
			    newElemAddress = jit_insn_dup(function, newArray);
			    counter = arraySize;
			    jit_insn_label(function, &startLoop);
			    jit_insn_store(function, counter, jit_insn_sub(function, counter,
		    									jit_value_create_nint_constant(function,
											_IL_JIT_TYPE_INT32,
											1)));
			    MarshalValue(function, jit_insn_dup(function, srcElemAddress),
							elemType, 0,
							MARSHAL_ITEM_OF_ARRAY,
							0,
							jit_insn_dup(function, newElemAddress));
			    jit_insn_store(function, srcElemAddress,
							    jit_insn_add(function,
							    srcElemAddress, srcElemSize));
			    jit_insn_store(function, newElemAddress,
							jit_insn_add(function,
							newElemAddress, newElemSize));
			    jit_insn_branch_if(function,
					        jit_insn_to_bool(function, counter),
					        &(startLoop));
			    jit_insn_label(function, &endLoop);
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
					jit_insn_store_relative(function, outAddressValue, offset, newArray);
			    return newArray;
		    }
		    else if (ILType_IsArray(type))
		    {
			    fprintf(stderr, "Multidimential arrays are not supported for marshaling\n");
    		    	    return 0;
    		    }
    }

    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
    jit_insn_store_relative(function, outAddressValue, offset, temp);
    return temp;
}

static ILJitValue MarshalStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind)
{
        ILClass *classInfo = ILClassResolve(ILType_ToValueType(structureILType));
	ILExecThread *thread = ILExecThreadCurrent();
        unsigned int structSize = ILSizeOfType(thread, structureILType);
	ILField *field = 0;
	ILType *type;

	switch(addressKind)
	{
		case MARSHAL_FIRST_LEVEL_VALUE:
		{
			ILJitValue sizeValue = jit_value_create_nint_constant(function, _IL_JIT_TYPE_UINT32, structSize);
			outAddress = jit_insn_call_native(function, "_ILMalloc",
								    ILGCAlloc,
								    _ILJitSignature_malloc,
								    &sizeValue, 1, 0);
		}
		break;
		case MARSHAL_ITEM_OF_STRUCTURE:
		case MARSHAL_ITEM_OF_ARRAY:
		{	    
			
		}
		break;
		default:
		{
			fprintf(stderr, "addressKind is an invalid value of %d\n", addressKind);
			return 0;
		}
		break;
	}

	while((field = (ILField *)ILClassNextMemberByKind
			(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if(!ILField_IsStatic(field))
		{
			type = ILTypeGetEnumType(ILField_Type(field));
			switch(addressKind)
			{
				case MARSHAL_FIRST_LEVEL_VALUE:
				{
					MarshalValue(function, inAddress, type, 0, MARSHAL_ITEM_OF_STRUCTURE, field->offset + offset, outAddress);
				}
			        break;
				case MARSHAL_ITEM_OF_STRUCTURE:
				{
					MarshalValue(function, inAddress, type, 0, MARSHAL_ITEM_OF_STRUCTURE, field->offset + offset, outAddress);
				}
				break;
				default:
				{
					MarshalValue(function, inAddress, type, 0, MARSHAL_ITEM_OF_ARRAY, field->offset + offset, outAddress);
				}
				break;
			}
		}
	}
	
	return outAddress;
}


static ILJitValue MarshalReturnValue(jit_function_t function, ILJitValue in, ILType *type, unsigned int addressKind, jit_nint offset, ILJitValue outAddress)
{
    ILJitValue retValue;    
    ILJitValue outAddressValue = outAddress;
    ILJitValue temp = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue args[3];
    ILJitValue srcArray = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILUInt32 marshalType;
    char *customName;
    int customNameLen;
    char *customCookie;
    int customCookieLen;
    ILMethod *method = (ILMethod *)jit_function_get_meta(function, IL_JIT_META_METHOD);
    ILPInvoke *pinv = ILPInvokeFind(method);
    marshalType = ILPInvokeGetMarshalType(pinv, method, 1, &customName, &customNameLen,
							&customCookie,	&customCookieLen, type);
    if(marshalType == IL_META_MARSHAL_DIRECT && addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    return in;
    }
    else if(marshalType == IL_META_MARSHAL_CUSTOM)
    {
	    fprintf(stderr, "custom marshaling of return value is not supported\n");
	    return 0;
    }
    type = ILTypeGetEnumType(type);

    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE && !NeedMarshalValue(type))
    {
	    return in;
    }

    switch(addressKind)
    {    
	    case MARSHAL_FIRST_LEVEL_VALUE:
	    case MARSHAL_ITEM_OF_STRUCTURE:
	    case MARSHAL_ITEM_OF_ARRAY:
	    {
	    }
	    break;
	    default:
	    {
		    fprintf(stderr, "addressKind has an invalid %d value\n", addressKind);
		    return 0;
	    }
	    break;
    }

    if(ILType_IsPrimitive(type))
    {
	switch(ILType_ToElement(type))
	{
		case IL_META_ELEMTYPE_VOID:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_BOOLEAN:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_BYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I1:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_SBYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U1:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_BYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I2:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT16);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U2:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT16);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_CHAR:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_CHAR);				
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT32);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT32);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT64);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT64);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_R4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_SINGLE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_R8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_DOUBLE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_NINT);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_NUINT);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_TYPEDBYREF:
		{	
			fprintf(stderr, "Marshaling of TypedByRef as a return value is not supported\n");
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_OBJECT:
		{	
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		default: break;
	}			
    }
    else if(ILType_IsValueType(type))
    {
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return (ILJitValue)MarshalReturnStruct(function, jit_insn_address_of(function, in), type, 0, 0, MARSHAL_FIRST_LEVEL_VALUE);
	    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    retValue = (ILJitValue)MarshalReturnStruct(function, temp, type, outAddressValue, offset, MARSHAL_ITEM_OF_STRUCTURE);
	    jit_insn_store_relative(function, outAddressValue, offset, retValue);
	    return retValue;
    }
    else if(ILTypeIsStringClass(type))
    {
	    args[0] = _ILJitFunctionGetThread(function);
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) args[1] = in;
	    else args[1] = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    switch(marshalType)
	    {
			case IL_META_MARSHAL_ANSI_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringCreate",
						    ILStringCreate,
						    _ILJitSignature_ILStringCreate,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF8_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringCreateUTF8",
						    ILStringCreateUTF8,
						    _ILJitSignature_ILStringCreateUTF8,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF16_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringWCreate",
						    ILStringWCreate,
						    _ILJitSignature_ILStringWCreate,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
	    }
	    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
		    jit_insn_store_relative(function, outAddressValue, offset, temp);
	    return temp;
    }
    else if(ILTypeIsDelegateSubClass(type))
    {
	    fprintf(stderr, "Marshaling of a delegate as a return value is not supported\n");
	    return 0;
    }
    else if(ILType_IsClass(type))
    {
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
	    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    jit_insn_store_relative(function, outAddressValue, offset, temp);
	    retValue = temp;
	    return retValue;
    }
    else if(type !=0 && ILType_IsComplex(type))
    {
	    switch(ILType_Kind(type))
	    {
		    case IL_TYPE_COMPLEX_PTR:
		    {
			    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)return in;
			    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			    jit_insn_store_relative(function, outAddressValue, offset, temp);
			    return temp;
		    };
		    break;
		    case IL_TYPE_COMPLEX_BYREF:
		    {
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    jit_insn_store(function, srcArray, jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR));
			    else jit_insn_store(function, srcArray, in);
			    ILType *elemType = ILType_Ref(type);
			    if(NeedMarshalValue(elemType))
			    {
				    MarshalReturnValue(function, jit_insn_dup(function, srcArray),
								    elemType,
								    MARSHAL_ITEM_OF_STRUCTURE,
								    0,
								    jit_insn_dup(function, srcArray));
			    }
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    jit_insn_store_relative(function, outAddressValue, offset, srcArray);
			    return srcArray;
		    }
		    break;
		    default: break;
	    }

	    if(ILType_IsSimpleArray(type))
	    {
			// The size of a simple array can be known only on run-time 
			// and we cannot use the same return memory as it is allocated in C code.
			fprintf(stderr, "Simple arrays not supported for marshaling as a return value");
			return 0;
			
	    }
	    else if(ILType_IsArray(type))
	    {
		    fprintf(stderr, "Multidimential arrays are not supported for marshaling as a return value\n");
		    return 0;
	    }

    }

    // Do something smart.
    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
    jit_insn_store_relative(function, outAddressValue, offset, temp);
    return temp;
}

static ILJitValue MarshalReturnStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind)
{
    ILClass *classInfo = ILClassResolve(ILType_ToValueType(structureILType));
    ILExecThread *thread = ILExecThreadCurrent();
    unsigned int structSize = ILSizeOfType(thread, structureILType);
    ILField *field = 0;
    ILJitValue newStruct = 0;
    ILType *type;
    ILJitType structType;	

    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    structType = jit_type_create_struct(0, 0, 0);
	    jit_type_set_size_and_alignment(structType, structSize, 0);
	    newStruct = jit_value_create(function, structType);
	    outAddress = jit_insn_address_of(function, newStruct);
    }

    while((field = (ILField *)ILClassNextMemberByKind
    		(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
    {
	    if(!ILField_IsStatic(field))
	    {
		    type = ILTypeGetEnumType(ILField_Type(field));
		    MarshalReturnValue(function, inAddress, type, MARSHAL_ITEM_OF_STRUCTURE, field->offset, outAddress);
	    }
    }
    return jit_insn_dup(function, newStruct);
}

static void MarshalByRefValue(jit_function_t function, ILJitValue in, ILType *type, ILUInt32 current, unsigned int addressKind, jit_nint offset, ILJitValue outAddress)
{
    ILJitValue outAddressValue = outAddress;
    ILJitValue temp = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue args[3];
    ILJitValue srcArray = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue srcElemSize;
    ILType *elemType;
    ILExecThread *thread;
    ILJitValue newElemSize;
    ILJitValue newArray;
    jit_label_t startLoop = jit_label_undefined;
    jit_label_t endLoop = jit_label_undefined;
    ILJitValue srcElemAddress;
    ILJitValue newElemAddress;
    ILJitValue counter;
    ILJitValue arraySize;    
    ILUInt32 marshalType;
    char *customName;
    int customNameLen;
    char *customCookie;
    int customCookieLen;
    ILMethod *method = (ILMethod *)jit_function_get_meta(function, IL_JIT_META_METHOD);
    ILPInvoke *pinv = ILPInvokeFind(method);
    marshalType = ILPInvokeGetMarshalType(pinv, method, current, &customName, &customNameLen,
							&customCookie,	&customCookieLen, type);
	
    if(marshalType == IL_META_MARSHAL_DIRECT && addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    return;
    }
    type = ILTypeGetEnumType(type);

    if(!NeedMarshalValue(type)) return;

    switch(addressKind)
    {    
	    case MARSHAL_FIRST_LEVEL_VALUE:
	    case MARSHAL_ITEM_OF_STRUCTURE:
	    case MARSHAL_ITEM_OF_ARRAY:
	    {
		    outAddressValue = outAddress;
	    }
	    break;
	    default:
	    {
		    fprintf(stderr, "addressKind has an invalid %d value\n", addressKind);
		    return;
	    }
	    break;
    }

    if(ILType_IsPrimitive(type))
    {
	switch(ILType_ToElement(type))
	{
		case IL_META_ELEMTYPE_VOID:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_BOOLEAN:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I1:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U1:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I2:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U2:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_CHAR:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I4:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U4:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I8:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U8:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_R4:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_R8:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_TYPEDBYREF:
		{	
			fprintf(stderr, "Marshaling of TypedByRef as a byref is not supported\n");
			return;
		}
		break;
		case IL_META_ELEMTYPE_OBJECT:
		{	
			return;
		}
		break;
		default: break;
	}			
    }
    else if(ILType_IsValueType(type))
    {	
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)
	    {
		    MarshalByRefStruct(function, jit_insn_address_of(function, in), type, jit_insn_address_of(function, in), 0, MARSHAL_ITEM_OF_STRUCTURE);
		    return;
	    }
	    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    MarshalByRefStruct(function, temp, type, temp, offset, MARSHAL_ITEM_OF_STRUCTURE);
	    return;
    }
    else if(ILTypeIsStringClass(type))
    {	   
	    args[0] = _ILJitFunctionGetThread(function);
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) args[1] = in;
	    else args[1] = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    switch(marshalType)
	    {
			case IL_META_MARSHAL_ANSI_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringCreate",
						    ILStringCreate,
						    _ILJitSignature_ILStringCreate,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF8_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringCreateUTF8",
						    ILStringCreateUTF8,
						    _ILJitSignature_ILStringCreateUTF8,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF16_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringWCreate",
						    ILStringWCreate,
						    _ILJitSignature_ILStringWCreate,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
	    }
	    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
		    jit_insn_store_relative(function, in, offset, temp);
	    return;
    }
    else if(ILTypeIsDelegate(type))
    {
	    return;
    }
    else if(ILType_IsClass(type))
    {
	    return;
    }
    else if(type !=0 && ILType_IsComplex(type))
    {
	    switch(ILType_Kind(type))
	    {
		    case IL_TYPE_COMPLEX_PTR:
		    {
			    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)return;
			    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			    jit_insn_store_relative(function, outAddressValue, offset, temp);
			    return;
		    };
		    break;
		    case IL_TYPE_COMPLEX_BYREF:
		    {
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
			    {
				    jit_insn_store(function, srcArray, jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR));
			    }
			    else jit_insn_store(function, srcArray, in);
			    ILType *elemType = ILType_Ref(type);
			    if(NeedMarshalValue(elemType))
			    {
				    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    {
				    
					    MarshalByRefValue(function, jit_insn_dup(function, srcArray),
										    elemType, current,
										    MARSHAL_ITEM_OF_STRUCTURE,
										    0,
										    jit_insn_load_relative(function, outAddressValue, offset, _IL_JIT_TYPE_VPTR));
				    }
				    else MarshalByRefValue(function, jit_insn_dup(function, srcArray),
										    elemType, current,
										    MARSHAL_ITEM_OF_STRUCTURE,
										    0,
										    srcArray);
			    }
			    return;
		    }
		    break;
		    default: break;
	    }

	    if(ILType_IsSimpleArray(type))
	    {
			    srcArray = in;
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    srcArray = jit_insn_load_relative(function, srcArray, offset, _IL_JIT_TYPE_VPTR);
			    elemType = ILType_ElemType(type);

			    if(!NeedMarshalValue(elemType))
			    {
				    return;
			    }

			    // The size of a simple array can be known only on run-time.
			    arraySize = jit_insn_load_relative(function, outAddressValue, 0, _IL_JIT_TYPE_UINT32);
			    thread = ILExecThreadCurrent();
			    srcElemSize = jit_value_create_nint_constant(function,
				    						_IL_JIT_TYPE_INT32,
										(jit_nint)(ILSizeOfType(thread, elemType)));
			    newElemSize = jit_value_create_nint_constant(function,
										_IL_JIT_TYPE_INT32,
									    	(jit_nint)(ILSizeOfType(thread, elemType)));

			    newArray = jit_insn_add_relative(function, outAddressValue, (jit_nint)(sizeof(ILUInt32)));
			    jit_insn_branch_if(function,
						jit_insn_eq(function, arraySize,
									jit_value_create_nint_constant
										(function, _IL_JIT_TYPE_UINT32, 0)),
										&endLoop);
			    srcElemAddress = jit_insn_dup(function, srcArray);
			    newElemAddress = jit_insn_dup(function, newArray);
			    counter = arraySize;
			    jit_insn_label(function, &startLoop);
			    jit_insn_store(function, counter, jit_insn_sub(function, counter,
		    									jit_value_create_nint_constant(function,
											_IL_JIT_TYPE_INT32,
											1)));
			    MarshalByRefValue(function, srcElemAddress,
								elemType, current,
								MARSHAL_ITEM_OF_ARRAY,
								0,
								newElemAddress);
			    jit_insn_store(function, srcElemAddress,
							    jit_insn_add(function,
							    srcElemAddress, srcElemSize));
			    jit_insn_store(function, newElemAddress,
							jit_insn_add(function,
							newElemAddress, newElemSize));
			    jit_insn_branch_if(function,
					        jit_insn_to_bool(function, counter),
					        &(startLoop));
			    jit_insn_label(function, &endLoop);
			    return;
	    }
	    else if(ILType_IsArray(type))
	    {
		    fprintf(stderr, "Multidimential arrays are not supported for marshaling as a return value\n");
		    return;
	    }

    }
    // Should not reach here but, make the compiler happy.
    return;
}

static void MarshalByRefStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind)
{
    ILClass *classInfo = ILClassResolve(ILType_ToValueType(structureILType));
    ILField *field = 0;
    ILType *type;

    while((field = (ILField *)ILClassNextMemberByKind
    		(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
    {
	    if(!ILField_IsStatic(field))
	    {
		    type = ILTypeGetEnumType(ILField_Type(field));
		    MarshalByRefValue(function, inAddress, type, 1, MARSHAL_ITEM_OF_STRUCTURE, field->offset + offset, outAddress);
	    }
    }
    return;
}



void *ILJitDelegateGetClosure(ILExecThread *currentThread, ILObject *del, ILType *delType)
{
	ILType *invokeSignature = ILMethod_Signature(((System_Delegate*)del)->methodInfo);
	int paramCount = ILTypeNumParams(invokeSignature);
	ILType *returnILType = ILTypeGetReturn(invokeSignature);
	ILJitType returnType = _ILJitGetReturnType(returnILType, ILExecThreadGetProcess(currentThread));
	ILJitType jitParamTypes[paramCount];
	ILJitType jitInvokeTypes[paramCount + 2];
	ILJitValue jitParams[paramCount + 2];
	int current;
	ILJitType newSignature;
	ILType *valueType;	
	ILJitType delInvokeSignature;
	ILJitFunction function;
	ILMethod *method;
	ILClass *info;
	ILClassPrivate *classPrivate;
	ILJITCoder *jitCoder;
	ILJitFunction delFunction;
	ILJitValue returnValue = 0;
	ILJitValue tempParams[paramCount + 2];
	unsigned int typeFlag;
	ILType *type;
	ILJitValue newValue;
    	for(current = 0; current < paramCount; ++current)
	{
		valueType = ILTypeGetParam(invokeSignature, current + 1);
		jitParamTypes[current] = _ILJitGetArgType(valueType, ILExecThreadGetProcess(currentThread));
		jitInvokeTypes[current + 2] = jitParamTypes[current];
	}
	newSignature = jit_type_create_signature(jit_abi_cdecl,
						    returnType,
						    jitParamTypes,
						    paramCount, 1);
	jitInvokeTypes[0] = jit_type_void_ptr;
	jitInvokeTypes[1] = jit_type_void_ptr;
	delInvokeSignature = jit_type_create_signature(jit_abi_cdecl,
							returnType,
							jitInvokeTypes,
							paramCount + 2, 1);
	method = ILTypeGetDelegateMethod(delType);
	info = ILMethod_Owner(method);
	classPrivate = (ILClassPrivate *)info->userData;
	jitCoder = (ILJITCoder *)(classPrivate->process->coder);

	function = jit_function_create(jitCoder->context, newSignature);
	delFunction = ILJitFunctionFromILMethod(method);

	jit_function_set_meta(function, IL_JIT_META_METHOD, method, 0, 0);

	for(current = 0; current < paramCount; ++current)
	{
		tempParams[current] = jit_value_get_param(function, current);
		jitParams[current + 2] = (ILJitValue)MarshalDelegateValue(function, jit_value_get_param(function, current), ILTypeGetParam(invokeSignature, current + 1), current + 1, 0, 0, 0, currentThread);
	}

	jitParams[0] = jit_value_create_nint_constant(function, jit_type_void_ptr, (jit_nint)currentThread);
	jitParams[1] = jit_value_create_nint_constant(function, jit_type_void_ptr, (jit_nint)del);
	if(returnType==jit_type_void) jit_insn_call(function, "callDelegatePinvokeMethod", delFunction, delInvokeSignature, jitParams, paramCount + 2, JIT_CALL_NOTHROW);
	else
	{
		returnValue = jit_insn_call(function, "callDelegatePinvokeMethod", delFunction, delInvokeSignature, jitParams, paramCount + 2, JIT_CALL_NOTHROW);
		returnValue = (ILJitValue)MarshalDelegateReturnValue(function, returnValue, returnILType, 0, 0, 0, currentThread);
	}
	for(current = 0; current < paramCount; ++current)
	{
		newValue = jitParams[current + 2];
		type = ILTypeGetParam(invokeSignature, current + 1);
		type = ILTypeGetEnumType(type);

		if(type!=0 && ILType_IsSimpleArray(type))
		{
			MarshalDelegateByRefValue(function, newValue, type, current + 1, 0, 0, tempParams[current], currentThread);
		}
		else if(type!=0 && ILType_IsComplex(type))
		{
			typeFlag = ILType_Kind(type);
			if(typeFlag==IL_TYPE_COMPLEX_BYREF || typeFlag==IL_TYPE_COMPLEX_PTR)
			{
				MarshalDelegateByRefValue(function, newValue, type, current + 1, 0, 0, tempParams[current], currentThread);
			}
		}
	}
	if(returnType==jit_type_void)jit_insn_return(function, 0);
	else jit_insn_return(function, returnValue);
	jit_function_compile(function);
	((System_Delegate*)del)->closure = jit_function_to_closure(function);
	return ((System_Delegate*)del)->closure;
}


static ILJitValue MarshalDelegateValue(jit_function_t function, ILJitValue in, ILType *type, ILUInt32 current, unsigned int addressKind, jit_nint offset, ILJitValue outAddress, ILExecThread *thread)
{
    ILJitValue retValue;
    ILJitValue outAddressValue = outAddress;
    ILJitValue temp = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue args[3];
    ILJitValue srcArray = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue srcElemSize;
    ILType *elemType;
    ILJitValue newElemSize, newArraySize;
    ILJitValue newArray;
    jit_label_t startLoop = jit_label_undefined;
    jit_label_t endLoop = jit_label_undefined;
    ILJitValue srcElemAddress;
    ILJitValue newElemAddress;
    ILJitValue counter;
    ILJitValue arraySize;    
    ILUInt32 marshalType;
    char *customName;
    int customNameLen;
    char *customCookie;
    int customCookieLen;
    ILMethod *method = (ILMethod *)jit_function_get_meta(function, IL_JIT_META_METHOD);
    ILPInvoke *pinv = ILPInvokeFind(method);
    marshalType = ILPInvokeGetMarshalType(pinv, method, current, &customName, &customNameLen,
							&customCookie,	&customCookieLen, type);
    if(marshalType == IL_META_MARSHAL_DIRECT && addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    return in;
    }
    else if(marshalType == IL_META_MARSHAL_CUSTOM)
    {
	    fprintf(stderr, "Custom marshaling not supported in delegate value\n");
	    return 0;
    }
    type = ILTypeGetEnumType(type);

    switch(addressKind)
    {    
	    case MARSHAL_FIRST_LEVEL_VALUE:
	    case MARSHAL_ITEM_OF_STRUCTURE:
	    case MARSHAL_ITEM_OF_ARRAY:
	    {
		    outAddressValue = outAddress;
	    }
	    break;
	    default:
	    {
		    fprintf(stderr, "addressKind has an invalid %d value\n", addressKind);
		    return 0;
	    }
	    break;
    }

    if(ILType_IsPrimitive(type))
    {
	switch(ILType_ToElement(type))
	{
		case IL_META_ELEMTYPE_VOID:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_BOOLEAN:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_BYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I1:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_SBYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U1:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_BYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I2:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT16);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U2:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT16);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_CHAR:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_CHAR);				
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT32);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT32);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT64);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT64);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_R4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_SINGLE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_R8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_DOUBLE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_NINT);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_NUINT);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_TYPEDBYREF:
		{	
			fprintf(stderr, "Marshaling of TypedByRef as a value is not supported");
			return 0;
		}
		break;
		case IL_META_ELEMTYPE_OBJECT:
		{	
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		default: break;
	}			
    }
    else if(ILType_IsValueType(type))
    {	
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return (ILJitValue)MarshalDelegateStruct(function, jit_insn_address_of(function, in), type, 0, 0, MARSHAL_FIRST_LEVEL_VALUE, thread);
	    retValue = (ILJitValue)MarshalDelegateStruct(function, in, type, outAddressValue, offset, MARSHAL_ITEM_OF_STRUCTURE, thread);
	    jit_insn_store_relative(function, outAddressValue, offset, retValue);
	    return retValue;
    }
    else if(ILTypeIsStringClass(type))
    {
	    args[0] = jit_value_create_nint_constant(function, _IL_JIT_TYPE_VPTR, (jit_nint)thread);
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) args[1] = in;
	    else args[1] = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    switch(marshalType)
	    {
			case IL_META_MARSHAL_ANSI_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringCreate",
						    ILStringCreate,
						    _ILJitSignature_ILStringCreate,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF8_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringCreateUTF8",
						    ILStringCreateUTF8,
						    _ILJitSignature_ILStringCreateUTF8,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF16_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringWCreate",
						    ILStringWCreate,
						    _ILJitSignature_ILStringWCreate,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
	    }
	    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
		    jit_insn_store_relative(function, outAddressValue, offset, temp);
	    return temp;
    }
    else if(ILTypeIsDelegateSubClass(type))
    {
	    fprintf(stderr, "Marshaling in a delegate of a delegate as a value is not supported\n");
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
	    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    jit_insn_store_relative(function, outAddressValue, offset, temp);
	    return temp;
    }
    else if(ILType_IsClass(type))
    {
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
	    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    jit_insn_store_relative(function, outAddressValue, offset, temp);
	    retValue = temp;
	    return retValue;
    }
    else if(type !=0 && ILType_IsComplex(type))
    {
	    switch(ILType_Kind(type))
	    {
		    case IL_TYPE_COMPLEX_PTR:
		    {
			    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)return in;
			    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			    jit_insn_store_relative(function, outAddressValue, offset, temp);
			    return temp;
		    };
		    break;
		    case IL_TYPE_COMPLEX_BYREF:
		    {
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    jit_insn_store(function, srcArray, jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR));
			    else jit_insn_store(function, srcArray, in);
			    ILType *elemType = ILType_Ref(type);
			    if(NeedMarshalValue(elemType))
			    {
				    jit_insn_store(function, temp, MarshalDelegateStruct(function, jit_insn_dup(function, srcArray),
									    elemType,
									    0,
									    0,
									    MARSHAL_FIRST_LEVEL_VALUE,
									    thread));
			    }
			    else jit_insn_store(function, temp, srcArray);
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    jit_insn_store_relative(function, outAddressValue, offset, srcArray);
			    return srcArray;
		    }
		    break;
		    case IL_TYPE_COMPLEX_PINNED:
		    {
			    fprintf(stderr, "Marshaling in a delegate a COMPLEX_PINNED type is not yet supported\n");
			    return 0;
		    }
		    break;
		    default: break;
	    }

	    if(ILType_IsSimpleArray(type))
	    {
			    srcArray = in;
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    srcArray = jit_insn_load_relative(function, srcArray, offset, _IL_JIT_TYPE_VPTR);
			    elemType = ILType_ElemType(type);

			    // The size of a simple array can be known only on run-time.
			    arraySize = jit_insn_load_relative(function, srcArray, 0, _IL_JIT_TYPE_UINT32);
			    srcElemSize = jit_value_create_nint_constant(function,
				    						_IL_JIT_TYPE_INT32,
										(jit_nint)(ILSizeOfType(thread, elemType)));
			    newElemSize = jit_value_create_nint_constant(function,
										_IL_JIT_TYPE_INT32,
									    	(jit_nint)(ILSizeOfType(thread, elemType)));
			    newArraySize = jit_insn_mul(function, arraySize, newElemSize);
			    newArraySize = jit_insn_add_relative(function, newArraySize, (jit_nint)(sizeof(ILUInt32)));
			    args[0] = newArraySize;
			    newArray = jit_insn_call_native(function, "_ILMalloc",
			    		    				ILGCAlloc,
			    		    				_ILJitSignature_malloc,
									args, 1, 0);
			    jit_insn_store_relative(function, newArray, 0, arraySize);
			    newArray = jit_insn_add_relative(function, newArray, (jit_nint)(sizeof(ILUInt32)));
			    jit_insn_branch_if(function,
						jit_insn_eq(function, arraySize,
									jit_value_create_nint_constant
										(function, _IL_JIT_TYPE_UINT32, 0)),
										&endLoop);
			    srcElemAddress = jit_insn_add(function,
								srcArray,
								jit_value_create_nint_constant(function,
												_IL_JIT_TYPE_INT32,
												(jit_nint)(sizeof(ILUInt32))));
			    newElemAddress = jit_insn_dup(function, newArray);
			    counter = arraySize;
			    jit_insn_label(function, &startLoop);
			    jit_insn_store(function, counter, jit_insn_sub(function, counter,
		    									jit_value_create_nint_constant(function,
											_IL_JIT_TYPE_INT32,
											1)));
			    MarshalDelegateValue(function, srcElemAddress,
							elemType, current,
							MARSHAL_ITEM_OF_ARRAY,
							0,
							newElemAddress,
							thread);
			    jit_insn_store(function, srcElemAddress,
							    jit_insn_add(function,
							    srcElemAddress, srcElemSize));
			    jit_insn_store(function, newElemAddress,
							jit_insn_add(function,
							newElemAddress, newElemSize));
			    jit_insn_branch_if(function,
					        jit_insn_to_bool(function, counter),
					        &(startLoop));
			    jit_insn_label(function, &endLoop);
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
					jit_insn_store_relative(function, outAddressValue, offset, newArray);
			    return newArray;
	    }
	    else if(ILType_IsArray(type))
	    {
		    fprintf(stderr, "Multidimential arrays are not supported for marshaling in delegates as a value\n");
		    return 0;
	    }

    }

    // Should not reach here if all every case is above, however do something smart.
    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
    jit_insn_store_relative(function, outAddressValue, offset, temp);
    return temp;
}

ILJitValue MarshalDelegateStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind, ILExecThread *thread)
{
    ILClass *classInfo = ILClassResolve(ILType_ToValueType(structureILType));
    unsigned int structSize = ILSizeOfType(thread, structureILType);
    ILField *field = 0;
    ILJitValue newStruct = 0;
    ILType *type;
    ILJitType structType;	

    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    structType = jit_type_create_struct(0, 0, 0);
	    jit_type_set_size_and_alignment(structType, structSize, 0);
	    newStruct = jit_value_create(function, structType);
	    outAddress = jit_insn_address_of(function, newStruct);
    }

    while((field = (ILField *)ILClassNextMemberByKind
    		(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
    {
	    if(!ILField_IsStatic(field))
	    {
		    type = ILTypeGetEnumType(ILField_Type(field));
		    MarshalDelegateValue(function, inAddress, type, 1, MARSHAL_ITEM_OF_STRUCTURE, field->offset, outAddress, thread);
	    }
    }
    return jit_insn_dup(function, newStruct);
}
	

static ILJitValue MarshalDelegateReturnValue(jit_function_t function, ILJitValue in, ILType *type, unsigned int addressKind, jit_nint offset, ILJitValue outAddress, ILExecThread *thread)
{
    ILJitValue outAddressValue = outAddress;
    ILJitValue temp = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue ptr, valuePtr;
    ILType *refType;
    ILJitValue args[3];
    ILType *elemType;
    jit_label_t endLabel = jit_label_undefined;
    ILJitValue srcArray = jit_value_create(function, _IL_JIT_TYPE_VPTR);    
    ILUInt32 marshalType;
    char *customName;
    int customNameLen;
    char *customCookie;
    int customCookieLen;
    ILMethod *method = (ILMethod *)jit_function_get_meta(function, IL_JIT_META_METHOD);
    ILPInvoke *pinv = ILPInvokeFind(method);
    marshalType = ILPInvokeGetMarshalType(pinv, method, 1, &customName, &customNameLen,
							&customCookie,	&customCookieLen, type);

    if(marshalType == IL_META_MARSHAL_DIRECT && addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    return in;
    }    
    else if(marshalType == IL_META_MARSHAL_CUSTOM)
    {
	    fprintf(stderr, "Custom marshaling not supported in return value\n");
	    return 0;
    }
    type = ILTypeGetEnumType(type);

    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    if(!NeedMarshalValue(type))return in;
    }

    if(addressKind==MARSHAL_ITEM_OF_ARRAY || addressKind == MARSHAL_ITEM_OF_STRUCTURE) outAddressValue = outAddress;
    else if(addressKind!=MARSHAL_FIRST_LEVEL_VALUE && addressKind!=MARSHAL_ITEM_OF_STRUCTURE)
    {
	    fprintf(stderr, "addressKind has an invalid value of %d\n", addressKind);
	    return 0;
    }

    if(ILType_IsPrimitive(type))
    {
	switch(ILType_ToElement(type))
	{
		case IL_META_ELEMTYPE_VOID:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_BOOLEAN:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_BYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I1:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_SBYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U1:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_BYTE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I2:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT16);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U2:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT16);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_CHAR:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_CHAR);				
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT32);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT32);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_INT64);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_UINT64);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;			
		}
		break;
		case IL_META_ELEMTYPE_R4:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_SINGLE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_R8:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_DOUBLE);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_I:
		{			
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_NINT);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		case IL_META_ELEMTYPE_U:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_NUINT);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;			
		}
		break;
		case IL_META_ELEMTYPE_TYPEDBYREF:
		{
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) temp = in;
			else temp = jit_insn_load_relative(function, in, offset, _ILJitTypedRef);
			ptr = jit_insn_address_of(function, temp);			
			valuePtr = jit_insn_load_relative(function, 
								ptr,
								offsetof(ILTypedRef, value),
								_IL_JIT_TYPE_VPTR);
			refType = ILType_Ref(type);
			switch(addressKind)
			{
				case MARSHAL_ITEM_OF_ARRAY:
				{
					return (ILJitValue)MarshalDelegateReturnStruct(function, valuePtr, refType, outAddressValue, offset, MARSHAL_ITEM_OF_ARRAY, thread);
				}
				break;
				case MARSHAL_ITEM_OF_STRUCTURE:
				{
					return (ILJitValue)MarshalDelegateReturnStruct(function, valuePtr, refType, outAddressValue, offset, MARSHAL_ITEM_OF_STRUCTURE, thread);
				}
				break;
				default:
				{
					return (ILJitValue)MarshalDelegateReturnStruct(function, valuePtr, refType, 0, 0, MARSHAL_FIRST_LEVEL_VALUE, thread);
				}
				break;
			}
		}
		break;
		case IL_META_ELEMTYPE_OBJECT:
		{	
			if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
			temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			jit_insn_store_relative(function, outAddressValue, offset, temp);
			return temp;
		}
		break;
		default:  break;
	}			
    }
    else if (ILType_IsValueType(type))
    {
	    switch(addressKind)
	    {
		    case MARSHAL_ITEM_OF_STRUCTURE:
		    case MARSHAL_ITEM_OF_ARRAY:
		    {
			    return (ILJitValue)MarshalDelegateReturnStruct(function, in, type, outAddressValue, offset, MARSHAL_ITEM_OF_ARRAY, thread);
		    }
		    break;
		    default:
		    {
			    return (ILJitValue)MarshalDelegateReturnStruct(function, jit_insn_address_of(function, in), type, outAddressValue, 0, MARSHAL_FIRST_LEVEL_VALUE, thread);
		    }
		    break;
	    }
    }
    else if(ILTypeIsStringClass(type))
    {
	    args[0] = jit_value_create_nint_constant(function, _IL_JIT_TYPE_VPTR, (jit_nint)thread);
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) args[1] = in;
	    else args[1] = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    switch(marshalType)
	    {
			case IL_META_MARSHAL_ANSI_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToAnsi",
						    ILStringToAnsi,
						    _ILJitSignature_ILStringToAnsi,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF8_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToUTF8",
						    ILStringToUTF8,
						    _ILJitSignature_ILStringToUTF8,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF16_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToUTF16",
						    ILStringToUTF16,
						    _ILJitSignature_ILStringToUTF16,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
	    }
	    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
		    jit_insn_store_relative(function, outAddressValue, offset, temp);
	    return temp;
    }
    else if(ILTypeIsDelegateSubClass(type))
    {
	    temp = jit_insn_dup(function, in);
	    if(addressKind!=MARSHAL_FIRST_LEVEL_VALUE) temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    ILJitValue closure = jit_insn_load_relative(function, temp, offsetof(System_Delegate, closure), _IL_JIT_TYPE_VPTR);
	    jit_insn_branch_if(function, jit_insn_to_bool(function, closure), &endLabel);
	    args[0] = jit_value_create_nint_constant(function, _IL_JIT_TYPE_VPTR, (jit_nint)thread);
	    args[1] = jit_insn_dup(function, temp);
	    args[2] = jit_value_create_nint_constant(function, _IL_JIT_TYPE_VPTR, (jit_nint)type);
	    jit_insn_store(function, closure, jit_insn_call_native(function,
					    "ILJitDelegateGetClosure",
					    ILJitDelegateGetClosure,
					    _ILJitSignature_ILJitDelegateGetClosure,
					    args, 3, JIT_CALL_NOTHROW));
	    jit_insn_label(function, &endLabel);
	    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
		    jit_insn_store_relative(function, outAddressValue, offset, closure);
	    return(closure);

    }
    else if(type!=0 && ILType_IsComplex(type))
    {
	    switch(ILType_Kind(type))
	    {
		    case IL_TYPE_COMPLEX_PTR:
		    {
			    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)return in;
			    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
			    jit_insn_store_relative(function, outAddressValue, offset, temp);
			    return temp;
		    }
		    break;
		    case IL_TYPE_COMPLEX_BYREF:
		    {
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    jit_insn_store(function, srcArray, jit_insn_load_relative(function, srcArray, offset, _IL_JIT_TYPE_VPTR));
			    else jit_insn_store(function, srcArray, in);
			    elemType = ILType_Ref(type);
			    if(NeedMarshalValue(elemType))
			    {
				    MarshalDelegateReturnValue(function, jit_insn_dup(function, srcArray),
								elemType,
								MARSHAL_ITEM_OF_ARRAY,
								0,
								jit_insn_dup(function, srcArray),
								thread);
			    }
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    jit_insn_store_relative(function, outAddressValue, offset, srcArray);
			    return srcArray;
		    }
		    break;
		    default: break;
		    }

		    if(ILType_IsSimpleArray(type))
		    {
			    // The size of a simple array can be known only on run-time and we
			    // cannot use the same memory as it is used in the C code.
			    fprintf(stderr, "Simple arrays are not supported for marshaling as a return value\n");
			    return 0;
		    }
		    else if (ILType_IsArray(type))
		    {
			    fprintf(stderr, "Multidimential arrays are not supported for marshaling as a return value\n");
    		    	    return 0;
    		    }
    }

    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) return in;
    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
    jit_insn_store_relative(function, outAddressValue, offset, temp);
    return temp;
}

static ILJitValue MarshalDelegateReturnStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind, ILExecThread *thread)
{
        ILClass *classInfo = ILClassResolve(ILType_ToValueType(structureILType));
        unsigned int structSize = ILSizeOfType(thread, structureILType);
	ILField *field = 0;
	ILType *type;

	switch(addressKind)
	{
		case MARSHAL_FIRST_LEVEL_VALUE:
		{
			ILJitValue sizeValue = jit_value_create_nint_constant(function, _IL_JIT_TYPE_UINT32, structSize);
			outAddress = jit_insn_call_native(function, "_ILMalloc",
								    ILGCAlloc,
								    _ILJitSignature_malloc,
								    &sizeValue, 1, 0);
		}
		break;
		case MARSHAL_ITEM_OF_STRUCTURE:
		case MARSHAL_ITEM_OF_ARRAY:
		{	    
			
		}
		break;
		default:
		{
			fprintf(stderr, "addressKind is an invalid value of %d\n", addressKind);
			return 0;
		}
		break;
	}

	while((field = (ILField *)ILClassNextMemberByKind
			(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if(!ILField_IsStatic(field))
		{
			type = ILTypeGetEnumType(ILField_Type(field));
			switch(addressKind)
			{
				case MARSHAL_FIRST_LEVEL_VALUE:
				{
					MarshalDelegateReturnValue(function, inAddress, type, MARSHAL_ITEM_OF_STRUCTURE, field->offset + offset, outAddress, thread);
				}
			        break;
				case MARSHAL_ITEM_OF_STRUCTURE:
				{
					MarshalDelegateReturnValue(function, inAddress, type, MARSHAL_ITEM_OF_STRUCTURE, field->offset + offset, outAddress, thread);
				}
				break;
				default:
				{
					MarshalDelegateReturnValue(function, inAddress, type, MARSHAL_ITEM_OF_ARRAY, field->offset + offset, outAddress, thread);
				}
				break;
			}
		}
	}
	
	return outAddress;
}

static void MarshalDelegateByRefValue(jit_function_t function, ILJitValue in, ILType *type, ILUInt32 current, unsigned int addressKind, jit_nint offset, ILJitValue outAddress, ILExecThread *thread)
{
    ILJitValue outAddressValue;
    ILJitValue temp = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue args[3];
    ILJitValue srcArray = jit_value_create(function, _IL_JIT_TYPE_VPTR);
    ILJitValue srcElemSize;
    ILType *elemType;
    ILJitValue newElemSize;
    jit_label_t startLoop = jit_label_undefined;
    jit_label_t endLoop = jit_label_undefined;
    ILJitValue srcElemAddress;
    ILJitValue newElemAddress;
    ILJitValue counter;
    ILJitValue arraySize;    
    ILUInt32 marshalType;
    char *customName;
    int customNameLen;
    char *customCookie;
    int customCookieLen;
    ILMethod *method = (ILMethod *)jit_function_get_meta(function, IL_JIT_META_METHOD);
    ILPInvoke *pinv = ILPInvokeFind(method);
    marshalType = ILPInvokeGetMarshalType(pinv, method, current, &customName, &customNameLen,
							&customCookie,	&customCookieLen, type);
    if(marshalType == IL_META_MARSHAL_DIRECT && addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    return;
    }
    else if(marshalType == IL_META_MARSHAL_CUSTOM)
    {
	    fprintf(stderr, "Custom marshaling not supported in delegate by ref value\n");
	    return;
    }
    type = ILTypeGetEnumType(type);

    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)
    {
	    if(!NeedMarshalValue(type))return;
    }

    switch(addressKind)
    {    
	    case MARSHAL_FIRST_LEVEL_VALUE:
	    case MARSHAL_ITEM_OF_STRUCTURE:
	    case MARSHAL_ITEM_OF_ARRAY:
	    {
		    outAddressValue = outAddress;
	    }
	    break;
	    default:
	    {
		    fprintf(stderr, "addressKind has an invalid %d value\n", addressKind);
		    return;
	    }
	    break;
    }

    if(ILType_IsPrimitive(type))
    {
	switch(ILType_ToElement(type))
	{
		case IL_META_ELEMTYPE_VOID:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_BOOLEAN:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I1:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U1:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I2:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U2:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_CHAR:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I4:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U4:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I8:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U8:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_R4:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_R8:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_I:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_U:
		{
			return;
		}
		break;
		case IL_META_ELEMTYPE_TYPEDBYREF:
		{	
			fprintf(stderr, "Marshaling of TypedByRef as a ref value in delegate is not supported\n");
			return;
		}
		break;
		case IL_META_ELEMTYPE_OBJECT:
		{	
			return;
		}
		break;
		default: break;
	}			
    }
    else if(ILType_IsValueType(type))
    {
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE)
	    {
		    MarshalDelegateByRefStruct(function, jit_insn_address_of(function, in), type, jit_insn_address_of(function, in), 0, MARSHAL_ITEM_OF_STRUCTURE, thread);
		    return;
	    }
	    temp = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    MarshalDelegateByRefStruct(function, temp, type, temp, 0, MARSHAL_ITEM_OF_STRUCTURE, thread);
	    return;
    }
    else if(ILTypeIsDelegateSubClass(type))
    {
	    return;
    }
    else if(ILTypeIsStringClass(type))
    {
	    args[0] = jit_value_create_nint_constant(function, _IL_JIT_TYPE_VPTR, (jit_nint)thread);
	    if(addressKind==MARSHAL_FIRST_LEVEL_VALUE) args[1] = in;
	    else args[1] = jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR);
	    switch(marshalType)
	    {
			case IL_META_MARSHAL_ANSI_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToAnsi",
						    ILStringToAnsi,
						    _ILJitSignature_ILStringToAnsi,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF8_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToUTF8",
						    ILStringToUTF8,
						    _ILJitSignature_ILStringToUTF8,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
			case IL_META_MARSHAL_UTF16_STRING:
			{
				    jit_insn_store(function, temp, jit_insn_call_native(function,
						    "ILStringToUTF16",
						    ILStringToUTF16,
						    _ILJitSignature_ILStringToUTF16,
						    args, 2, JIT_CALL_NOTHROW));
			}
			break;
	    }
	    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
		    jit_insn_store_relative(function, in, offset, temp);
	    return;
    }
    else if(ILType_IsClass(type))
    {
	    return;
    }
    else if(type !=0 && ILType_IsComplex(type))
    {
	    switch(ILType_Kind(type))
	    {
		    case IL_TYPE_COMPLEX_PTR:
		    case IL_TYPE_COMPLEX_BYREF:
		    {
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    jit_insn_store(function, srcArray, jit_insn_load_relative(function, in, offset, _IL_JIT_TYPE_VPTR));
			    else jit_insn_store(function, srcArray, in);
			    ILType *elemType = ILType_Ref(type);
			    if(NeedMarshalValue(elemType))
			    {
				    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    {
					    MarshalDelegateByRefValue(function, jit_insn_dup(function, srcArray),
										    elemType, current,
										    MARSHAL_ITEM_OF_STRUCTURE,
										    0,
										    jit_insn_load_relative(function, outAddressValue, offset, _IL_JIT_TYPE_VPTR),
										    thread);
				    }
				    else MarshalDelegateByRefValue(function, jit_insn_dup(function, srcArray),
										    elemType, current,
										    MARSHAL_ITEM_OF_STRUCTURE,
										    0,
										    srcArray,
										    thread);
			    }
			    return;
		    }
		    break;
		    case IL_TYPE_COMPLEX_PINNED:
		    {
			    fprintf(stderr, "Marshaling a COMPLEX_PINNED type is not yet supported for marshaling as a return byref value\n");
			    return;
		    }
		    break;
		    default: break;
	    }

	    if(ILType_IsSimpleArray(type))
	    {
			    srcArray = in;
			    if(addressKind==MARSHAL_ITEM_OF_STRUCTURE || addressKind==MARSHAL_ITEM_OF_ARRAY)
				    srcArray = jit_insn_load_relative(function, srcArray, offset, _IL_JIT_TYPE_VPTR);
			    elemType = ILType_ElemType(type);

			    if(!NeedMarshalValue(elemType))
			    {
				    return;
			    }

			    // The size of a simple array can be known only on run-time.
			    arraySize = jit_insn_load_relative(function, srcArray, 0, _IL_JIT_TYPE_UINT32);
			    srcElemSize = jit_value_create_nint_constant(function,
				    						_IL_JIT_TYPE_INT32,
										(jit_nint)(ILSizeOfType(thread, elemType)));
			    newElemSize = jit_value_create_nint_constant(function,
										_IL_JIT_TYPE_INT32,
									    	(jit_nint)(ILSizeOfType(thread, elemType)));

			    jit_insn_branch_if(function,
						jit_insn_eq(function, arraySize,
									jit_value_create_nint_constant
										(function, _IL_JIT_TYPE_UINT32, 0)),
										&endLoop);
			    newElemAddress = jit_insn_dup(function, outAddressValue);
			    srcElemAddress = jit_insn_add_relative(function, srcArray, (jit_nint)(sizeof(ILUInt32)));
			    counter = arraySize;
			    jit_insn_label(function, &startLoop);
			    jit_insn_store(function, counter, jit_insn_sub(function, counter,
		    									jit_value_create_nint_constant(function,
											_IL_JIT_TYPE_INT32,
											1)));
			    MarshalDelegateByRefValue(function, srcElemAddress,
							elemType, current,
							MARSHAL_ITEM_OF_ARRAY,
							0,
							newElemAddress,
							thread);
			    jit_insn_store(function, srcElemAddress,
							    jit_insn_add(function,
							    srcElemAddress, srcElemSize));
			    jit_insn_store(function, newElemAddress,
							jit_insn_add(function,
							newElemAddress, newElemSize));
			    jit_insn_branch_if(function,
					        jit_insn_to_bool(function, counter),
					        &(startLoop));
			    jit_insn_label(function, &endLoop);
			    return;
	    }
	    else if(ILType_IsArray(type))
	    {
		    fprintf(stderr, "Multidimential arrays are not supported for marshaling as a return byref value\n");
		    return;
	    }

    }

    // Should not reach here, but make the compiler happy.
    return;
}

static void MarshalDelegateByRefStruct(jit_function_t function, ILJitValue inAddress, ILType *structureILType, ILJitValue outAddress, jit_nint offset, unsigned int addressKind, ILExecThread *thread)
{
    ILClass *classInfo = ILClassResolve(ILType_ToValueType(structureILType));
    ILField *field = 0;
    ILType *type;

    while((field = (ILField *)ILClassNextMemberByKind
    		(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
    {
	    if(!ILField_IsStatic(field))
	    {
		    type = ILTypeGetEnumType(ILField_Type(field));
		    MarshalDelegateByRefValue(function, inAddress, type, 1, MARSHAL_ITEM_OF_STRUCTURE, field->offset + offset, outAddress, thread);
	    }
    }
    return;
}
