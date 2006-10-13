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
								 _IL_JIT_TYPE_INT32);
	return len;
}

/*
 * Validate the array index.
 */
#define JITC_START_CHECK_ARRAY_INDEX(jitCoder, length, index) \
	{ \
		jit_label_t __label = jit_label_undefined; \
		jit_label_t __okLabel = jit_label_undefined; \
		ILJitValue  __length = (length); \
		ILJitValue  __index = (index); \
		ILJitValue  __temp; \
		\
		AdjustSign(jitCoder->jitFunction, &(__length), 1, 0); \
		AdjustSign(jitCoder->jitFunction, &(__index), 1, 0); \
		\
		__temp = jit_insn_lt(jitCoder->jitFunction, __index, __length); \
		jit_insn_branch_if_not(jitCoder->jitFunction, __temp, &__label);

#define JITC_END_CHECK_ARRAY_INDEX(jitCoder) \
		jit_insn_branch(jitCoder->jitFunction, &__okLabel); \
		jit_insn_label(jitCoder->jitFunction, &__label); \
		_ILJitThrowSystem(jitCoder, _IL_JIT_INDEX_OUT_OF_RANGE); \
		jit_insn_label(jitCoder->jitFunction, &__okLabel); \
	}

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
	/* Throw a System.IndexOutOfRange exception. */
	_ILJitThrowSystem(coder, _IL_JIT_INDEX_OUT_OF_RANGE);
	jit_insn_label(coder->jitFunction, &label);
}

/*
 * Handle the ldelem* instructions.
 */
static void LoadArrayElem(ILJITCoder *coder, ILJitType type)
{
	_ILJitStackItemNew(index);
	_ILJitStackItemNew(array);
	ILJitValue length;
	ILJitValue value;
	ILJitValue arrayBase;

	_ILJitStackPop(coder, index);
	_ILJitStackPop(coder, array);
	_ILJitStackItemCheckNull(coder, array);
	length = GetArrayLength(coder, _ILJitStackItemValue(array));
	JITC_START_CHECK_ARRAY_INDEX(coder, length, _ILJitStackItemValue(index))
	arrayBase = GetArrayBase(coder, _ILJitStackItemValue(array));
	value = jit_insn_load_elem(coder->jitFunction,
							   arrayBase,
							   _ILJitStackItemValue(index),
							   type);
	_ILJitStackPushValue(coder, value);
	JITC_END_CHECK_ARRAY_INDEX(coder)
}

/*
 * Handle the stelem* instructions.
 */
static void StoreArrayElem(ILJITCoder *coder, ILJitType type)
{
	_ILJitStackItemNew(value);
	_ILJitStackItemNew(index);
	_ILJitStackItemNew(array);
	ILJitValue length;
	ILJitValue arrayBase;
	ILJitValue temp;
	ILJitType valueType;

	_ILJitStackPop(coder, value);
	_ILJitStackPop(coder, index);
	_ILJitStackPop(coder, array);
	valueType = jit_value_get_type(_ILJitStackItemValue(value));
	_ILJitStackItemCheckNull(coder, array);
	length = GetArrayLength(coder, _ILJitStackItemValue(array));
	ValidateArrayIndex(coder, length, _ILJitStackItemValue(index));
	arrayBase = GetArrayBase(coder, _ILJitStackItemValue(array));

	/* Convert the value to the array type when needed. */
	if(valueType != type)
	{
		int valueIsStruct = (jit_type_is_struct(valueType) || jit_type_is_union(valueType));
		int destIsStruct = (jit_type_is_struct(type) || jit_type_is_union(type));

		if(valueIsStruct || destIsStruct)
		{
			int valueSize = jit_type_get_size(valueType);
			int destSize = jit_type_get_size(type);

			if(destSize == valueSize)
			{
				/* The sizes match so we can safely use store element. */
				temp = _ILJitStackItemValue(value);
			}
			else
			{
				/* We assume that destSize is smaller than valueSize because */
				/* the values have to be assignment compatible. */
				/* But we have to use memcpy instead. */
				_ILJitStackItemNew(dest);
				ILJitValue destPtr = jit_insn_load_elem_address(coder->jitFunction,
																arrayBase,
																_ILJitStackItemValue(index),
																type);
				ILJitValue srcPtr = jit_insn_address_of(coder->jitFunction,
														_ILJitStackItemValue(value));
				ILJitValue size = jit_value_create_nint_constant(coder->jitFunction,
																 _IL_JIT_TYPE_NINT,
																 (jit_nint)destSize);

				_ILJitStackItemInitWithNotNullValue(dest, destPtr);
				_ILJitStackItemMemCpy(coder, dest, srcPtr, size);
				return;
			}
		}
		else
		{
			temp = _ILJitValueConvertImplicit(coder->jitFunction,
											  _ILJitStackItemValue(value),
											  type);
		}
	}
	else
	{
		temp = _ILJitStackItemValue(value);
	}
	jit_insn_store_elem(coder->jitFunction,
						arrayBase,
						_ILJitStackItemValue(index),
						temp);
}

/*
 * Handle the ldind* instructions.
 */
static void LoadRelative(ILJITCoder *coder, ILJitType type)
{
	_ILJitStackItemNew(ptr);
	ILJitValue value;

	_ILJitStackPop(coder, ptr);
	_ILJitStackItemCheckNull(coder, ptr);
	value = jit_insn_load_relative(coder->jitFunction,
								   _ILJitStackItemValue(ptr),
								   (jit_nint)0,
								   type);
	_ILJitStackPushValue(coder, value);
}

/*
 * Handle the stind* instructions.
 */
static void StoreRelative(ILJITCoder *coder, ILJitType type)
{
	_ILJitStackItemNew(value);
	_ILJitStackItemNew(ptr);
	ILJitType valueType;
	ILJitValue temp;

	_ILJitStackPop(coder, value);
	_ILJitStackPop(coder, ptr);
	valueType = jit_value_get_type(_ILJitStackItemValue(value));
	_ILJitStackItemCheckNull(coder, ptr);
	if(valueType != type)
	{
		int valueIsStruct = (jit_type_is_struct(valueType) || jit_type_is_union(valueType));
		int destIsStruct = (jit_type_is_struct(type) || jit_type_is_union(type));

		if(valueIsStruct || destIsStruct)
		{
			int valueSize = jit_type_get_size(valueType);
			int destSize = jit_type_get_size(type);

			if(destSize == valueSize)
			{
				/* The sizes match so we can safely use store relative. */
				temp = _ILJitStackItemValue(value);
			}
			else
			{
				/* We assume that destSize is smaller than valueSize because */
				/* the values have to be assignment compatible. */
				/* But we have to use memcpy instead. */
				ILJitValue srcPtr = jit_insn_address_of(coder->jitFunction,
														_ILJitStackItemValue(value));
				ILJitValue size = jit_value_create_nint_constant(coder->jitFunction,
																 _IL_JIT_TYPE_NINT,
																 (jit_nint)destSize);

				_ILJitStackItemMemCpy(coder, ptr, srcPtr, size);
				return;
			}
		}
		else
		{
			temp = _ILJitValueConvertImplicit(coder->jitFunction,
											  _ILJitStackItemValue(value),
											  type);
		}
	}
	else
	{
		temp = _ILJitStackItemValue(value);
	}
	_ILJitStackItemStoreRelative(coder, ptr, 0, temp);
}

/*
 * Handle an array access opcode.
 */
static void JITCoder_ArrayAccess(ILCoder *coder, int opcode,
								 ILEngineType indexType, ILType *elemType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	_ILJitStackItemNew(array);
	_ILJitStackItemNew(index);
	ILJitValue len;
	ILJitValue value;
	ILJitValue arrayBase;

	switch(opcode)
	{
		case IL_OP_LDELEMA:
		{
			_ILJitStackPop(jitCoder, index);
			_ILJitStackPop(jitCoder, array);

			_ILJitStackItemCheckNull(jitCoder, array);
			len = GetArrayLength(jitCoder, _ILJitStackItemValue(array));
			ValidateArrayIndex(jitCoder, len, _ILJitStackItemValue(index));
			arrayBase = GetArrayBase(jitCoder, _ILJitStackItemValue(array));

			ILJitType type = _ILJitGetReturnType(elemType, jitCoder->process);

			value = jit_insn_load_elem_address(jitCoder->jitFunction,
											   arrayBase,
											   _ILJitStackItemValue(index),
											   type);
			_ILJitStackPushNotNullValue(jitCoder, value);
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
	_ILJitStackItemNew(address);
	_ILJitStackItemNew(object);
	ILJitValue value;

	if(opcode == IL_OP_LDOBJ)
	{
		/* Load from a pointer */
		/* Pop the address off the evaluation stack. */
		_ILJitStackPop(jitCoder, address);
		_ILJitStackItemCheckNull(jitCoder, address);
		value = jit_insn_load_relative(jitCoder->jitFunction,
									   _ILJitStackItemValue(address),
									   (jit_nint)0,
									   jitType);
		_ILJitStackPushValue(jitCoder, value);
	}
	else
	{
		/* Store to a pointer */
		ILJitType valueType;
		/* Pop the object off the evaluation stack. */
		_ILJitStackPop(jitCoder, object);
		/* Pop the address off the evaluation stack. */
		_ILJitStackPop(jitCoder, address);
		valueType = jit_value_get_type(_ILJitStackItemValue(object));
		_ILJitStackItemCheckNull(jitCoder, address);
		
		if(valueType != jitType)
		{
			int valueIsStruct = (jit_type_is_struct(valueType) || jit_type_is_union(valueType));
			int destIsStruct = (jit_type_is_struct(jitType) || jit_type_is_union(jitType));

			if(valueIsStruct || destIsStruct)
			{
				int valueSize = jit_type_get_size(valueType);
				int destSize = jit_type_get_size(jitType);

				if(destSize == valueSize)
				{
					/* The sizes match so we can safely use store relative. */
					value = _ILJitStackItemValue(object);
				}
				else
				{
					/* We assume that destSize is smaller than valueSize because */
					/* the values have to be assignment compatible. */
					/* But we have to use memcpy instead. */
					ILJitValue srcPtr = jit_insn_address_of(jitCoder->jitFunction,
															_ILJitStackItemValue(object));
					ILJitValue size = jit_value_create_nint_constant(jitCoder->jitFunction,
																	 _IL_JIT_TYPE_NINT,
																	 (jit_nint)destSize);

					_ILJitStackItemMemCpy(jitCoder, address, srcPtr, size);
					return;
				}
			}
			else
			{
				value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
													_ILJitStackItemValue(object),
													jitType);
			}
		}
		else
		{
			value = _ILJitStackItemValue(object);
		}
		_ILJitStackItemStoreRelative(jitCoder, address, 0, value);
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
	_ILJitStackItemNew(array);
	ILJitValue length;

	/* Pop the array off the evaluation stack. */
	_ILJitStackPop(jitCoder, array);

	_ILJitStackItemCheckNull(jitCoder, array);
	length = GetArrayLength(jitCoder, _ILJitStackItemValue(array));
	_ILJitStackPushValue(jitCoder, length);
}

/*
 * Construct a new array, given a type and length value.
 */
static void JITCoder_NewArray(ILCoder *coder, ILType *arrayType,
					 		  ILClass *arrayClass, ILEngineType lengthType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	_ILJitStackItemNew(length);
	ILJitValue returnValue;

#if 1
	ILJitValue args[2];
	ILMethod *ctor;
	ILJitFunction jitFunction;
	jit_label_t label = jit_label_undefined;
	ILInternalInfo fnInfo;

	/* Pop the length off the evaluation stack. */
	_ILJitStackPop(jitCoder, length);

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
		ILJitValue thread = _ILJitCoderGetThread(jitCoder);

		returnValue = _ILJitCallInternal(jitCoder->jitFunction,
										 thread,
										 ctor,
										 fnInfo.func, 0,
										 &_ILJitStackItemValue(length), 1);
	}
	else
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		args[0] = _ILJitCoderGetThread(jitCoder);
		args[1] = _ILJitStackItemValue(length);

		/* Output code to call the array type's constructor */
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction,
									0, args, 2, 0);
	#else
		args[0] = _ILJitStackItemValue(length);
		/* Output code to call the array type's constructor */
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction,
									0, args, 1, 0);
	#endif
	}
	jit_insn_branch_if(jitCoder->jitFunction, returnValue, &label);
	_ILJitThrowCurrentException(jitCoder);
	jit_insn_label(jitCoder->jitFunction, &label);
#else
	/* Pop the length off the evaluation stack. */
	_ILJitStackPop(jitCoder, length);

	returnValue = _ILJitSArrayNew(jitCoder,
								  arrayClass,
								  _ILJitStackItemValue(length));
#endif

	_ILJitStackPushNotNullValue(jitCoder, returnValue);
}

/*
 * Check the top of stack value for NULL.
 */
static void JITCoder_CheckNull(ILCoder *coder)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	_ILJitStackItemNew(value);

	_ILJitStackGetTop(jitCoder, value);
	_ILJitStackItemCheckNull(jitCoder, value);
}

#endif	/* IL_JITC_CODE */
