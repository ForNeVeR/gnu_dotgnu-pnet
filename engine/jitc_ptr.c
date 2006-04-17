/*
 * jitc_ptr.c - Coder implementation for JIT pointers and arrays.
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
 * Get the base pointer to the array data.
 */
static ILJitValue GetArrayBase(ILJITCoder *coder, ILJitValue array)
{
	return jit_insn_add_relative(coder->jitFunction, array, sizeof(System_Array));
}

/*
 * Get the array length.
 */
static ILJitValue GetArrayLength(ILJITCoder *coder, ILJitValue array)
{
	ILJitValue len;

	len = jit_insn_load_relative(coder->jitFunction, array, 0,
								 _IL_JIT_TYPE_UINT32);
	return len;
}

/*
 * Validate the array index.
 */
static void ValidateArrayIndex(ILJITCoder *coder, ILJitValue length,
												  ILJitValue index)
{
	jit_label_t label = jit_label_undefined;
	ILJitValue temp;

	/* Make both values unsigned. We can save one compare this way. */
	AdjustSign(coder->jitFunction, &length, 1, 0);
	AdjustSign(coder->jitFunction, &index, 1, 0);

	temp = jit_insn_lt(coder->jitFunction, index, length);
	jit_insn_branch_if(coder->jitFunction, temp, &label);
	/* throw the System.IndexOutOfRange exception. */
	_ILJitThrowInternal(coder->jitFunction, JIT_RESULT_OUT_OF_BOUNDS);
	jit_insn_label(coder->jitFunction, &label);
}

/*
 * Handle the ldelem* instructions.
 */
static void LoadArrayElem(ILJITCoder *coder, ILJitType type)
{
	ILJitValue array = coder->jitStack[coder->stackTop - 2];
	ILJitValue index = coder->jitStack[coder->stackTop - 1];
	ILJitValue length;
	ILJitValue value;
	ILJitValue arrayBase;

	_ILJitCheckNull(coder, array);
	length = GetArrayLength(coder, array);
	ValidateArrayIndex(coder, length, index);
	arrayBase = GetArrayBase(coder, array);
	value = jit_insn_load_elem(coder->jitFunction, arrayBase, index,
							   type);
	coder->jitStack[coder->stackTop - 2] = 
		_ILJitValueConvertToStackType(coder->jitFunction, value);
	JITC_ADJUST(coder, -1);
}

/*
 * Handle the stelem* instructions.
 */
static void StoreArrayElem(ILJITCoder *coder, ILJitType type)
{
	ILJitValue array = coder->jitStack[coder->stackTop - 3];
	ILJitValue index = coder->jitStack[coder->stackTop - 2];
	ILJitValue value = coder->jitStack[coder->stackTop - 1];
	ILJitValue length;
	ILJitValue arrayBase;
	ILJitType valueType = jit_value_get_type(value);

	_ILJitCheckNull(coder, array);
	length = GetArrayLength(coder, array);
	ValidateArrayIndex(coder, length, index);
	arrayBase = GetArrayBase(coder, array);

	/* Convert the value to the array type when needed. */
	if(valueType != type)
	{
		value = _ILJitValueConvertImplicit(coder->jitFunction, value, type);
	}
	jit_insn_store_elem(coder->jitFunction, arrayBase, index, value);
	JITC_ADJUST(coder, -3);
}

/*
 * Handle the ldind* instructions.
 */
static void LoadRelative(ILJITCoder *coder, ILJitType type)
{
	ILJitValue ptr = coder->jitStack[coder->stackTop - 1];
	ILJitValue value;

	_ILJitCheckNull(coder, ptr);
	value = jit_insn_load_relative(coder->jitFunction, ptr, (jit_nint)0, type);
	coder->jitStack[coder->stackTop - 1] = 
		_ILJitValueConvertToStackType(coder->jitFunction, value);
}

/*
 * Handle the stind* instructions.
 */
static void StoreRelative(ILJITCoder *coder, ILJitType type)
{
	ILJitValue ptr = coder->jitStack[coder->stackTop - 2];
	ILJitValue value  = coder->jitStack[coder->stackTop - 1];
	ILJitType valueType = jit_value_get_type(value);

	_ILJitCheckNull(coder, ptr);
	if(valueType != type)
	{
		value = _ILJitValueConvertImplicit(coder->jitFunction, value, type);
	}
	jit_insn_store_relative(coder->jitFunction, ptr, (jit_nint)0, value);
	JITC_ADJUST(coder, -2);
}

/*
 * Handle an array access opcode.
 */
static void JITCoder_ArrayAccess(ILCoder *coder, int opcode,
								 ILEngineType indexType, ILType *elemType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue array;
	ILJitValue index;
	ILJitValue len;
	ILJitValue value;
	ILJitValue arrayBase;

	switch(opcode)
	{
		case IL_OP_LDELEMA:
		{
			array = jitCoder->jitStack[jitCoder->stackTop - 2];
			index = jitCoder->jitStack[jitCoder->stackTop - 1];

			_ILJitCheckNull(jitCoder, array);
			len = GetArrayLength(jitCoder, array);
			ValidateArrayIndex(jitCoder, array, index);
			arrayBase = GetArrayBase(jitCoder, array);

			ILJitType type = _ILJitGetReturnType(elemType, jitCoder->process);

			value = jit_insn_load_elem_address(jitCoder->jitFunction, arrayBase,
									    index, type);
			jitCoder->jitStack[jitCoder->stackTop - 2] = value;
			JITC_ADJUST(jitCoder, -1);
		}
		break;

		case IL_OP_LDELEM_I1:
		{
			/* Load a signed byte from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_SBYTE);
		}
		break;

		case IL_OP_LDELEM_I2:
		{
			/* Load a signed short from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_INT16);
		}
		break;

		case IL_OP_LDELEM_I4:
	#ifdef IL_NATIVE_INT32
		case IL_OP_LDELEM_I:
	#endif
		{
			/* Load an integer from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_INT32);
		}
		break;

		case IL_OP_LDELEM_I8:
	#ifdef IL_NATIVE_INT64
		case IL_OP_LDELEM_I:
	#endif
		{
			/* Load a long from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_INT64);
		}
		break;

		case IL_OP_LDELEM_U1:
		{
			/* Load an unsigned byte from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_BYTE);
		}
		break;

		case IL_OP_LDELEM_U2:
		{
			/* Load an unsigned short from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_UINT16);
		}
		break;

		case IL_OP_LDELEM_U4:
		{
			/* Load an unsigned integer from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_UINT32);
		}
		break;

		case IL_OP_LDELEM_R4:
		{
			/* Load a 32-bit float from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_SINGLE);
		}
		break;

		case IL_OP_LDELEM_R8:
		{
			/* Load a 64-bit float from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_DOUBLE);
		}
		break;

		case IL_OP_LDELEM_REF:
		{
			/* Load a pointer from an array */
			LoadArrayElem(jitCoder, _IL_JIT_TYPE_VPTR);
		}
		break;

		case IL_OP_LDELEM:
		{
			ILJitType type = _ILJitGetReturnType(elemType, jitCoder->process);
			LoadArrayElem(jitCoder, type);
		}
		break;

		case IL_OP_STELEM_I1:
		{
			/* Store a byte value to an array */
			StoreArrayElem(jitCoder, _IL_JIT_TYPE_SBYTE);
		}
		break;

		case IL_OP_STELEM_I2:
		{
			/* Store a short value to an array */
			StoreArrayElem(jitCoder, _IL_JIT_TYPE_INT16);
		}
		break;

		case IL_OP_STELEM_I4:
	#ifdef IL_NATIVE_INT32
		case IL_OP_STELEM_I:
	#endif
		{
			/* Store an integer value to an array */
			StoreArrayElem(jitCoder, _IL_JIT_TYPE_INT32);
		}
		break;

		case IL_OP_STELEM_I8:
	#ifdef IL_NATIVE_INT64
		case IL_OP_STELEM_I:
	#endif
		{
			/* Store a long value to an array */
			StoreArrayElem(jitCoder, _IL_JIT_TYPE_INT64);
		}
		break;

		case IL_OP_STELEM_R4:
		{
			/* Store a 32-bit floating point value to an array */
			StoreArrayElem(jitCoder, _IL_JIT_TYPE_SINGLE);
		}
		break;

		case IL_OP_STELEM_R8:
		{
			/* Store a 64-bit floating point value to an array */
			StoreArrayElem(jitCoder, _IL_JIT_TYPE_DOUBLE);
		}
		break;

		case IL_OP_STELEM_REF:
		{
			/* Store a pointer to an array */
			/* TODO: Check if the types are assignmentcompatible. */
			StoreArrayElem(jitCoder, _IL_JIT_TYPE_VPTR);
		}
		break;

		case IL_OP_STELEM:
		{
			/* Store a pointer to an array */
			/* TODO: Check if the types are assignmentcompatible. */
			ILJitType type = _ILJitGetReturnType(elemType, jitCoder->process);
			StoreArrayElem(jitCoder, type);
		}
		break;
	}
}

/*
 * Handle a pointer indirection opcode.
 */
static void JITCoder_PtrAccess(ILCoder *coder, int opcode)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	switch(opcode)
	{
		case IL_OP_LDIND_I1:
		{
			/* Load a signed byte from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_SBYTE);
		}
		break;

		case IL_OP_LDIND_U1:
		{
			/* Load an unsigned byte from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_BYTE);
		}
		break;

		case IL_OP_LDIND_I2:
		{
			/* Load a signed short from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_INT16);
		}
		break;

		case IL_OP_LDIND_U2:
		{
			/* Load an unsigned short from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_UINT16);
		}
		break;

		case IL_OP_LDIND_I4:
	#ifdef IL_NATIVE_INT32
		case IL_OP_LDIND_I:
	#endif
		{
			/* Load an integer from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_INT32);
		}
		break;

		case IL_OP_LDIND_U4:
		{
			/* Load an unsigned integer from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_UINT32);
		}
		break;

		case IL_OP_LDIND_I8:
	#ifdef IL_NATIVE_INT64
		case IL_OP_LDIND_I:
	#endif
		{
			/* Load a long from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_INT64);
		}
		break;

		case IL_OP_LDIND_R4:
		{
			/* Load a 32-bit float from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_SINGLE);
		}
		break;

		case IL_OP_LDIND_R8:
		{
			/* Load a 64-bit float from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_DOUBLE);
		}
		break;

		case IL_OP_LDIND_REF:
		{
			/* Load a pointer from a pointer */
			LoadRelative(jitCoder, _IL_JIT_TYPE_VPTR);
		}
		break;

		case IL_OP_STIND_REF:
		{
			/* Store a pointer to a pointer */
			StoreRelative(jitCoder, _IL_JIT_TYPE_VPTR);
		}
		break;

		case IL_OP_STIND_I1:
		{
			/* Store a byte to a pointer */
			StoreRelative(jitCoder, _IL_JIT_TYPE_SBYTE);
		}
		break;

		case IL_OP_STIND_I2:
		{
			/* Store a short to a pointer */
			StoreRelative(jitCoder, _IL_JIT_TYPE_INT16);
		}
		break;

		case IL_OP_STIND_I4:
	#ifdef IL_NATIVE_INT32
		case IL_OP_STIND_I:
	#endif
		{
			/* Store an integer to a pointer */
			StoreRelative(jitCoder, _IL_JIT_TYPE_INT32);
		}
		break;

		case IL_OP_STIND_I8:
	#ifdef IL_NATIVE_INT64
		case IL_OP_STIND_I:
	#endif
		{
			/* Store a long to a pointer */
			StoreRelative(jitCoder, _IL_JIT_TYPE_INT64);
		}
		break;

		case IL_OP_STIND_R4:
		{
			/* Store a 32-bit float to a pointer */
			StoreRelative(jitCoder, _IL_JIT_TYPE_SINGLE);
		}
		break;

		case IL_OP_STIND_R8:
		{
			/* Store a 64-bit float to a pointer */
			StoreRelative(jitCoder, _IL_JIT_TYPE_DOUBLE);
		}
		break;
	}
}

/*
 * Handle a pointer indirection opcode for a managed value.
 */
static void JITCoder_PtrAccessManaged(ILCoder *coder, int opcode,
									  ILClass *classInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILType *type = ILClassToType(classInfo);
	ILJitType jitType = _ILJitGetReturnType(type, jitCoder->process);
	ILJitValue address;

	if(opcode == IL_OP_LDOBJ)
	{
		/* Load from a pointer */
		address = jitCoder->jitStack[jitCoder->stackTop - 1];
		_ILJitCheckNull(jitCoder, address);
		jitCoder->jitStack[jitCoder->stackTop - 1] =
		_ILJitValueConvertToStackType(jitCoder->jitFunction,
									  jit_insn_load_relative(jitCoder->jitFunction,
															 address, (jit_nint)0,
								   							 jitType));
	}
	else
	{
		/* Store to a pointer */
		ILJitValue object = jitCoder->jitStack[jitCoder->stackTop - 1];;

		address = jitCoder->jitStack[jitCoder->stackTop - 2];
		_ILJitCheckNull(jitCoder, address);
		
		if(jit_value_get_type(object) != jitType)
		{
			object = _ILJitValueConvertImplicit(jitCoder->jitFunction, object,
												jitType);
		}
		jit_insn_store_relative(jitCoder->jitFunction, address, (jit_nint)0, object);
		JITC_ADJUST(jitCoder, -2);
	}

}

/*
 * Process a pointer alignment prefix.  An "alignment" value
 * of zero indicates a "volatile" prefix.
 */
static void JITCoder_PtrPrefix(ILCoder *coder, int alignment)
{
}

/*
 * Get the length of an array.
 */
static void JITCoder_ArrayLength(ILCoder *coder)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue array = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue len;

	_ILJitCheckNull(jitCoder, array);
	len = GetArrayLength(jitCoder, array);
	jitCoder->jitStack[jitCoder->stackTop - 1] = 
		_ILJitValueConvertToStackType(jitCoder->jitFunction,
									  jit_insn_convert(jitCoder->jitFunction,
													   len,
													   _IL_JIT_TYPE_NUINT, 0));
}

/*
 * Construct a new array, given a type and length value.
 */
static void JITCoder_NewArray(ILCoder *coder, ILType *arrayType,
					 		  ILClass *arrayClass, ILEngineType lengthType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue length = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue args[2];
	ILMethod *ctor;
	ILJitFunction jitFunction;
	ILJitValue returnValue;
	jit_label_t label = jit_label_undefined;
	ILInternalInfo fnInfo;

	/* Find the allocation constructor within the array class.
	   We know that the class only has one method, so it must
	   be the constructor that we are looking for */
	ctor = (ILMethod *)ILClassNextMemberByKind
					(arrayClass, 0, IL_META_MEMBERKIND_METHOD);

	jitFunction = ILJitFunctionFromILMethod(ctor);
	if(!jitFunction)
	{
		/* We have to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), arrayClass))
		{
			return;
		}
		jitFunction = ILJitFunctionFromILMethod(ctor);
	}
	if(_ILJitFunctionIsInternal(jitCoder, ctor, &fnInfo, 1))
	{
		JITC_ADJUST(jitCoder, -1);
		returnValue = _ILJitCallInternal(jitCoder->jitFunction, ctor,
										 fnInfo.func, 0, &length, 1);
	}
	else
	{
		args[0] = jit_value_get_param(jitCoder->jitFunction, 0);
		args[1] = length;
		JITC_ADJUST(jitCoder, -1);
		_ILJitSetMethodInThread(jitCoder->jitFunction, args[0], ctor);
		/* Output code to call the array type's constructor */
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction,
									0, args, 2, 0);
	}
	jit_insn_branch_if(jitCoder->jitFunction, returnValue, &label);
	_ILJitThrowCurrentException(jitCoder);
	jit_insn_label(jitCoder->jitFunction, &label);
	jitCoder->jitStack[jitCoder->stackTop] = returnValue;
	JITC_ADJUST(jitCoder, 1);
}

/*
 * Check the top of stack value for NULL.
 */
static void JITCoder_CheckNull(ILCoder *coder)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	_ILJitCheckNull(jitCoder, jitCoder->jitStack[jitCoder->stackTop - 1]);
}

#endif	/* IL_JITC_CODE */
