/*
 * jitc_obj.c - Coder implementation for JIT object operations.
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
 * Get a ILJitValue with a pointer to the class' static area.
 */
static ILJitValue _ILJitGetClassStaticArea(ILExecThread *thread,
										   ILJitFunction func,
										   ILField *field)
{
	/* Get the static data area for a particular class */
	ILClass *classInfo = ILField_Owner(field);
	void *classStaticData = ((ILClassPrivate *)(classInfo->userData))->staticData;
	ILJitValue staticData;

	if(!classStaticData)
	{
		if(((ILClassPrivate *)(classInfo->userData))->managedStatic)
		{
			classStaticData = _ILEngineAlloc(thread, 0,
			   ((ILClassPrivate *)(classInfo->userData))->staticSize);
		}
		else
		{
			/* There are no managed fields in the static area,
		   	so use atomic allocation */
			classStaticData = _ILEngineAllocAtomic(thread, 0,
			   ((ILClassPrivate *)(classInfo->userData))->staticSize);
		}
		if(!classStaticData)
		{
			jit_exception_builtin(JIT_RESULT_OUT_OF_MEMORY);
		}
		else
		{
			((ILClassPrivate *)(classInfo->userData))->staticData = classStaticData;
		}
	}
	staticData = jit_value_create_nint_constant(func, _IL_JIT_TYPE_VPTR,
												(jit_nint)classStaticData);
	return staticData;
}

static void JITCoder_CastClass(ILCoder *coder, ILClass *classInfo,
							   int throwException)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue classTo = jit_value_create_nint_constant(jitCoder->jitFunction,
														_IL_JIT_TYPE_VPTR,
														(jit_nint)classInfo);
	ILJitValue object = jitCoder->jitStack[jitCoder->stackTop - 1];
	jit_label_t label = jit_label_undefined;
	ILJitValue args[3];
	ILJitValue returnValue;

	jit_insn_branch_if_not(jitCoder->jitFunction, object, &label);
	if(ILClass_IsInterface(classInfo))
	{
		/* We are casting to an interface */
		args[0] = object;
		args[1] = classTo;
		returnValue = jit_insn_call_native(jitCoder->jitFunction, 0,
										   ILRuntimeClassImplements,
										   _ILJitSignature_ILRuntimeClassImplements,
										   args, 2, 0);
	}
	else
	{
		args[0] = jit_value_get_param(jitCoder->jitFunction, 0);
		args[1] = object;
		args[2] = classTo;
		returnValue = jit_insn_call_native(jitCoder->jitFunction, 0,
										   ILRuntimeCanCastClass,
										   _ILJitSignature_ILRuntimeCanCastClass,
										   args, 3, 0);
	}
	if(throwException)
	{
		jit_insn_branch_if(jitCoder->jitFunction, returnValue, &label);
		/* TODO: Throw an InvalidCastException here. */

	}
	else
	{
		jit_label_t label1 = jit_label_undefined;
		ILJitValue temp = jit_value_create(jitCoder->jitFunction, _IL_JIT_TYPE_VPTR);
		ILJitValue nullPointer = 
				jit_value_create_nint_constant(jitCoder->jitFunction,
											   _IL_JIT_TYPE_VPTR,
											   (jit_nint)0);
	
		jit_insn_branch_if(jitCoder->jitFunction, returnValue, &label1);
		jit_insn_store(jitCoder->jitFunction, temp, nullPointer);
		jit_insn_branch(jitCoder->jitFunction, &label);
		jit_insn_label(jitCoder->jitFunction, &label1);
		jit_insn_store(jitCoder->jitFunction, temp, jitCoder->jitStack[jitCoder->stackTop - 1]);
		jitCoder->jitStack[jitCoder->stackTop - 1] = temp;
	}
	jit_insn_label(jitCoder->jitFunction, &label);
}

static void JITCoder_LoadField(ILCoder *coder, ILEngineType ptrType,
							   ILType *objectType, ILField *field,
							   ILType *fieldType)
{
}

static void JITCoder_LoadThisField(ILCoder *coder, ILField *field,
							   	   ILType *fieldType)
{
}

static void JITCoder_LoadStaticField(ILCoder *coder, ILField *field,
							         ILType *fieldType)
{
}

static void JITCoder_LoadFieldAddr(ILCoder *coder, ILEngineType ptrType,
							       ILType *objectType, ILField *field,
							       ILType *fieldType)
{
}

static void JITCoder_LoadStaticFieldAddr(ILCoder *coder, ILField *field,
					 ILType *fieldType)
{
}

static void JITCoder_StoreField(ILCoder *coder, ILEngineType ptrType,
							    ILType *objectType, ILField *field,
							    ILType *fieldType, ILEngineType valueType)
{
}

static void JITCoder_StoreStaticField(ILCoder *coder, ILField *field,
				      ILType *fieldType, ILEngineType valueType)
{
}

static void JITCoder_CopyObject(ILCoder *coder, ILEngineType destPtrType,
							    ILEngineType srcPtrType, ILClass *classInfo)
{
}

static void JITCoder_CopyBlock(ILCoder *coder, ILEngineType destPtrType,
							   ILEngineType srcPtrType)
{
}

static void JITCoder_InitObject(ILCoder *coder, ILEngineType ptrType,
				ILClass *classInfo)
{
}

static void JITCoder_InitBlock(ILCoder *coder, ILEngineType ptrType)
{
}

static void JITCoder_Box(ILCoder *coder, ILClass *boxClass,
					     ILEngineType valueType, ILUInt32 size)
{
}

static void JITCoder_BoxSmaller(ILCoder *coder, ILClass *boxClass,
					   		    ILEngineType valueType, ILType *smallerType)
{
}

static void JITCoder_Unbox(ILCoder *coder, ILClass *boxClass)
{
}

static void JITCoder_MakeTypedRef(ILCoder *coder, ILClass *classInfo)
{
}

static void JITCoder_RefAnyVal(ILCoder *coder, ILClass *classInfo)
{
}

static void JITCoder_RefAnyType(ILCoder *coder)
{
}

static void JITCoder_PushToken(ILCoder *coder, ILProgramItem *item)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	jitCoder->jitStack[jitCoder->stackTop] = 
		jit_value_create_nint_constant(jitCoder->jitFunction,
									   _IL_JIT_TYPE_VPTR, 
							    	   (jit_nint)item);

	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_SizeOf(ILCoder *coder, ILType *type)
{
}

static void JITCoder_ArgList(ILCoder *coder)
{
}

#endif	/* IL_JITC_CODE */
