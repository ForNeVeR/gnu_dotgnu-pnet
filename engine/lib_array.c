/*
 * lib_array.c - Internalcall methods for "System.Array" and subclasses.
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

#include "engine.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Support for walking the argument stack in methods that
 * require a variable number of arguments.  This is CVM
 * specific and will need to be updated if we change the
 * engine architecture in the future.  We have to do this
 * because "libffi" will pack the arguments onto the
 * stack in a way which cannot reliably be extracted
 * with "va_arg" on all platforms.
 */
typedef struct
{
	CVMWord	*posn;

} ArgWalker;
#define	ArgWalkerInit(args)	\
			do { \
				(args)->posn = thread->stackBase + thread->frame; \
			} while (0)
#define	ArgWalkerInitThis(args)	\
			do { \
				(args)->posn = thread->stackBase + thread->frame + 1; \
			} while (0)
#define	ArgWalkerGetShortInt(args,type) \
			(*((type *)(((args)->posn)++)))
#define	ArgWalkerGetInt(args) \
			((((args)->posn)++)->intValue)
#define	ArgWalkerGetUInt(args) \
			((((args)->posn)++)->uintValue)
#define	ArgWalkerGetPtr(args) \
			((((args)->posn)++)->ptrValue)
#define	ArgWalkerGetAddr(args) \
			((args)->posn)
#define	ArgWalkerAdvance(args,nwords) \
			((args)->posn += (nwords))

/*
 * Allocation constructor for single-dimensional arrays.
 *
 * public T[](uint size);
 */
static System_Array *System_SArray_ctor(ILExecThread *thread,
										ILUInt32 length)
{
	ILClass *classInfo;
	ILType *type;
	ILUInt32 elemSize;
	ILUInt64 totalSize;

	/* Get the synthetic class and type for the array */
	classInfo = ILMethod_Owner(thread->method);
	type = ILClassGetSynType(classInfo);

	/* Compute the element size */
	elemSize = ILSizeOfType(type->un.array.elemType);

	/* Determine the total size of the array in bytes */
	totalSize = ((ILUInt64)elemSize) * ((ILUInt64)length);
	if(totalSize > (ILUInt64)IL_MAX_INT32)
	{
		ILExecThreadThrowOutOfMemory(thread);
		return 0;
	}

	/* Allocate the array and return it */
	return (System_Array *)_ILEngineAlloc
			(thread, classInfo, sizeof(System_Array) + (ILUInt32)totalSize);
}

/*
 * Construct the header part of a multi-dimensional array.
 */
static System_MArray *ConstructMArrayHeader(ILExecThread *thread,
											ILClass *classInfo)
{
	System_MArray *_this;
	ILType *type;
	ILType *elemType;
	ILInt32 rank;

	/* Get the type descriptor that underlies the class */
	type = ILClassGetSynType(classInfo);

	/* Determine the rank and get the element type */
	rank = 1;
	elemType = type;
	while(elemType != 0 && ILType_IsComplex(type) &&
		  elemType->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
	{
		++rank;
		elemType = elemType->un.array.elemType;
	}
	if(elemType != 0 && ILType_IsComplex(elemType) &&
	   elemType->kind == IL_TYPE_COMPLEX_ARRAY)
	{
		elemType = elemType->un.array.elemType;
	}
	else
	{
		/* Shouldn't happen, but do something sane anyway */
		elemType = ILType_Int32;
	}

	/* Allocate space for the array header */
	_this = (System_MArray *)_ILEngineAlloc
					(thread, classInfo,
					 sizeof(System_MArray) +
					 (rank - 1) * sizeof(ILInt32) * 2);
	if(!_this)
	{
		return 0;
	}

	/* Fill in the array header with the rank and element size values */
	_this->rank = rank;
	_this->elemSize = (ILInt32)ILSizeOfType(elemType);
	return _this;
}

/*
 * Construct the data part of a multi-dimensional array.
 */
static System_MArray *ConstructMArrayData(ILExecThread *thread,
									      System_MArray *array)
{
	ILUInt64 sizeInBytes;
	int dim;
	char name[64];

	/* Compute the size of the array in bytes, and also
	   validate the size values to ensure non-negative */
	sizeInBytes = 1;
	for(dim = 0; dim < array->rank; ++dim)
	{
		if(array->bounds[dim * 2 + 1] < 0)
		{
			sprintf(name, "length%d", dim);
			ILExecThreadThrowArgRange(thread, name, "ArgRange_NonNegative");
			return 0;
		}
		sizeInBytes *= (ILUInt64)(array->bounds[dim * 2 + 1]);
		if(sizeInBytes > (ILUInt64)IL_MAX_INT32)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
	}
	sizeInBytes *= (ILUInt64)(array->elemSize);
	if(sizeInBytes > (ILUInt64)IL_MAX_INT32)
	{
		ILExecThreadThrowOutOfMemory(thread);
		return 0;
	}

	/* Allocate the data part of the array */
	array->data = _ILEngineAlloc(thread, (ILClass *)0,
								 (ILUInt32)sizeInBytes);
	if(!(array->data))
	{
		return 0;
	}

	/* The array is ready to go */
	return array;
}

/*
 * Get the address of a particular array element.
 * Returns NULL if an exception was thrown.
 */
static void *GetElemAddress(ILExecThread *thread,
							System_MArray *_this,
							ArgWalker *args)
{
	ILInt32 offset;
	ILInt32 multiplier;
	ILInt32 dim;
	ILInt32 index;

	/* Find the offset of the element */
	offset = 0;
	multiplier = 1;
	for(dim = 0; dim < _this->rank; ++dim)
	{
		index = ArgWalkerGetInt(args) - _this->bounds[dim * 2];
		if(((ILUInt32)index) >= ((ILUInt32)(_this->bounds[dim * 2 + 1])))
		{
			ILExecThreadThrowSystem(thread, "System.IndexOutOfRangeException",
									"Arg_InvalidArrayIndex");
			return 0;
		}
		offset += index * multiplier;
		multiplier *= _this->bounds[dim * 2 + 1];
	}

	/* Compute the element address */
	offset *= _this->elemSize;
	return (void *)(((unsigned char *)_this->data) + offset);
}

/*
 * Set a multi-dimensional array element to a value that
 * appears after a list of indicies.  "valueType" indicates
 * the type of value.
 */
static void SetElement(ILExecThread *thread, System_MArray *_this,
					   int valueType, ArgWalker *args)
{
	ILInt32 offset;
	ILInt32 multiplier;
	ILInt32 dim;
	ILInt32 index;
	void *address;

	/* Find the offset of the element */
	offset = 0;
	multiplier = 1;
	for(dim = 0; dim < _this->rank; ++dim)
	{
		index = ArgWalkerGetInt(args) - _this->bounds[dim * 2];
		if(((ILUInt32)index) >= ((ILUInt32)(_this->bounds[dim * 2 + 1])))
		{
			ILExecThreadThrowSystem(thread, "System.IndexOutOfRangeException",
									"Arg_InvalidArrayIndex");
			return;
		}
		offset += index * multiplier;
		multiplier *= _this->bounds[dim * 2 + 1];
	}

	/* Compute the element address */
	offset *= _this->elemSize;
	address = (void *)(((unsigned char *)_this->data) + offset);

	/* Extract the value from the parameters and set it.  Values
	   greater than 4 bytes in length are copied using "ILMemCpy"
	   because we cannot guarantee their alignment on the stack */
	switch(valueType)
	{
		case IL_META_ELEMTYPE_I1:
		{
			*((ILInt8 *)address) = ArgWalkerGetShortInt(args, ILInt8);
		}
		break;

		case IL_META_ELEMTYPE_I2:
		{
			*((ILInt16 *)address) = ArgWalkerGetShortInt(args, ILInt16);
		}
		break;

		case IL_META_ELEMTYPE_I4:
		{
			*((ILInt32 *)address) = ArgWalkerGetInt(args);
		}
		break;

		case IL_META_ELEMTYPE_I8:
		{
			ILMemCpy(address, ArgWalkerGetAddr(args), sizeof(ILInt64));
			ArgWalkerAdvance(args, CVM_WORDS_PER_LONG);
		}
		break;

		case IL_META_ELEMTYPE_R4:
		{
			ILMemCpy(address, ArgWalkerGetAddr(args), sizeof(ILFloat));
			ArgWalkerAdvance(args, CVM_WORDS_PER_NATIVE_FLOAT);
		}
		break;

		case IL_META_ELEMTYPE_R8:
		{
			ILMemCpy(address, ArgWalkerGetAddr(args), sizeof(ILDouble));
			ArgWalkerAdvance(args, CVM_WORDS_PER_NATIVE_FLOAT);
		}
		break;

		case IL_META_ELEMTYPE_R:
		{
			ILMemCpy(address, ArgWalkerGetAddr(args), sizeof(ILNativeFloat));
			ArgWalkerAdvance(args, CVM_WORDS_PER_NATIVE_FLOAT);
		}
		break;

		case IL_META_ELEMTYPE_OBJECT:
		{
			*((ILObject **)address) = (ILObject *)ArgWalkerGetPtr(args);
		}
		break;

		case IL_META_ELEMTYPE_VALUETYPE:
		{
			ILMemCpy(address, ArgWalkerGetAddr(args), _this->elemSize);
		}
		break;
	}
}

/*
 * Constructor for multi-dimensional arrays that takes sizes.
 *
 * public T[,,,](int size1, int size2, ..., int sizeN)
 */
static System_MArray *System_MArray_ctor_1(ILExecThread *thread)
{
	System_MArray *_this;
	ILInt32 dim;
	ArgWalker args;

	/* Construct the header part of the array */
	_this = ConstructMArrayHeader(thread, ILMethod_Owner(thread->method));
	if(!_this)
	{
		return 0;
	}

	/* Fill in the array header with the size values */
	ArgWalkerInit(&args);
	for(dim = 0; dim < _this->rank; ++dim)
	{
		_this->bounds[dim * 2]     = 0;		/* Lower bound */
		_this->bounds[dim * 2 + 1] = ArgWalkerGetInt(&args);
	}

	/* Construct the data part of the array */
	return ConstructMArrayData(thread, _this);
}

/*
 * Constructor for multi-dimensional arrays that takes lower bounds and sizes.
 *
 * public T[,,,](int low1, int size1, ..., int lowN, int sizeN)
 */
static System_MArray *System_MArray_ctor_2(ILExecThread *thread)
{
	System_MArray *_this;
	ILInt32 dim;
	ArgWalker args;

	/* Construct the header part of the array */
	_this = ConstructMArrayHeader(thread, ILMethod_Owner(thread->method));
	if(!_this)
	{
		return 0;
	}

	/* Fill in the array header with the lower bound and size values */
	ArgWalkerInit(&args);
	for(dim = 0; dim < _this->rank; ++dim)
	{
		_this->bounds[dim * 2]     = ArgWalkerGetInt(&args);
		_this->bounds[dim * 2 + 1] = ArgWalkerGetInt(&args);
	}

	/* Construct the data part of the array */
	return ConstructMArrayData(thread, _this);
}

/*
 * Get a signed byte value from a multi-dimensional array.
 *
 * public sbyte Get(int index1, ..., int indexN)
 */
static ILInt8 System_MArray_Get_sbyte(ILExecThread *thread,
							          System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILInt8 *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get an unsigned byte value from a multi-dimensional array.
 *
 * public byte Get(int index1, ..., int indexN)
 */
static ILUInt8 System_MArray_Get_byte(ILExecThread *thread,
							          System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILUInt8 *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get a signed short value from a multi-dimensional array.
 *
 * public short Get(int index1, ..., int indexN)
 */
static ILInt16 System_MArray_Get_short(ILExecThread *thread,
							           System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILInt16 *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get an unsigned short value from a multi-dimensional array.
 *
 * public ushort Get(int index1, ..., int indexN)
 */
static ILUInt16 System_MArray_Get_ushort(ILExecThread *thread,
							             System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILUInt16 *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get a signed int value from a multi-dimensional array.
 *
 * public int Get(int index1, ..., int indexN)
 */
static ILInt32 System_MArray_Get_int(ILExecThread *thread,
							         System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILInt32 *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get an unsigned int value from a multi-dimensional array.
 *
 * public uint Get(int index1, ..., int indexN)
 */
static ILUInt32 System_MArray_Get_uint(ILExecThread *thread,
							           System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILUInt32 *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get a signed long value from a multi-dimensional array.
 *
 * public long Get(int index1, ..., int indexN)
 */
static ILInt64 System_MArray_Get_long(ILExecThread *thread,
							          System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILInt64 *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get an unsigned long value from a multi-dimensional array.
 *
 * public ulong Get(int index1, ..., int indexN)
 */
static ILUInt64 System_MArray_Get_ulong(ILExecThread *thread,
							            System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILUInt64 *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get a float value from a multi-dimensional array.
 *
 * public float Get(int index1, ..., int indexN)
 */
static ILFloat System_MArray_Get_float(ILExecThread *thread,
							           System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILFloat *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get a double value from a multi-dimensional array.
 *
 * public double Get(int index1, ..., int indexN)
 */
static ILDouble System_MArray_Get_double(ILExecThread *thread,
							             System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILDouble *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get a native float value from a multi-dimensional array.
 *
 * public native float Get(int index1, ..., int indexN)
 */
static ILNativeFloat System_MArray_Get_nativeFloat(ILExecThread *thread,
							             		   System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILNativeFloat *)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get an object reference value from a multi-dimensional array.
 *
 * public Object Get(int index1, ..., int indexN)
 */
static ILObject *System_MArray_Get_ref(ILExecThread *thread,
							           System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		return *((ILObject **)address);
	}
	else
	{
		return 0;
	}
}

/*
 * Get a managed value from a multi-dimensional array.
 *
 * public type Get(int index1, ..., int indexN)
 */
static void System_MArray_Get_managedValue(ILExecThread *thread,
										   void *result,
							               System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	if(address)
	{
		ILMemCpy(result, address, _this->elemSize);
	}
}

/*
 * Get a signed native int value from a multi-dimensional array.
 *
 * public native int Get(int index1, ..., int indexN)
 */
#ifdef IL_NATIVE_INT32
#define	System_MArray_Get_nativeInt		System_MArray_Get_int
#else
#define	System_MArray_Get_nativeInt		System_MArray_Get_long
#endif

/*
 * Get a unsigned native int value from a multi-dimensional array.
 *
 * public native uint Get(int index1, ..., int indexN)
 */
#ifdef IL_NATIVE_INT32
#define	System_MArray_Get_nativeUInt	System_MArray_Get_uint
#else
#define	System_MArray_Get_nativeUInt	System_MArray_Get_ulong
#endif

/*
 * Get the address of an element within a multi-dimensional array.
 *
 * public type & Address(int index1, ..., int indexN)
 */
static void *System_MArray_Address(ILExecThread *thread,
							       System_MArray *_this)
{
	void *address;
	ArgWalker args;
	ArgWalkerInitThis(&args);
	address = GetElemAddress(thread, _this, &args);
	return address;
}

/*
 * Set a byte value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, sbyte value)
 * public void Set(int index1, ..., int indexN, byte value)
 */
static void System_MArray_Set_byte(ILExecThread *thread,
							       System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_I1, &args);
}

/*
 * Set a short value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, short value)
 * public void Set(int index1, ..., int indexN, ushort value)
 */
static void System_MArray_Set_short(ILExecThread *thread,
							        System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_I2, &args);
}

/*
 * Set an int value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, int value)
 * public void Set(int index1, ..., int indexN, uint value)
 */
static void System_MArray_Set_int(ILExecThread *thread,
							      System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_I4, &args);
}

/*
 * Set a long value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, long value)
 * public void Set(int index1, ..., int indexN, ulong value)
 */
static void System_MArray_Set_long(ILExecThread *thread,
							       System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_I8, &args);
}

/*
 * Set a float value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, float value)
 */
static void System_MArray_Set_float(ILExecThread *thread,
							        System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_R4, &args);
}

/*
 * Set a double value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, double value)
 */
static void System_MArray_Set_double(ILExecThread *thread,
							         System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_R8, &args);
}

/*
 * Set a native float value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, native float value)
 */
static void System_MArray_Set_nativeFloat(ILExecThread *thread,
							              System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_R, &args);
}

/*
 * Set an object reference value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, Object value)
 */
static void System_MArray_Set_ref(ILExecThread *thread,
							      System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_OBJECT, &args);
}

/*
 * Set a managed value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, type value)
 */
static void System_MArray_Set_managedValue(ILExecThread *thread,
							               System_MArray *_this)
{
	ArgWalker args;
	ArgWalkerInitThis(&args);
	SetElement(thread, _this, IL_META_ELEMTYPE_VALUETYPE, &args);
}

/*
 * Set a native int value within a multi-dimensional array.
 *
 * public void Set(int index1, ..., int indexN, native int value)
 * public void Set(int index1, ..., int indexN, native uint value)
 */
#ifdef IL_NATIVE_INT32
#define	System_MArray_Set_nativeInt		System_MArray_Set_int
#else
#define	System_MArray_Set_nativeInt		System_MArray_Set_long
#endif

/*
 * Determine if a class inherits from "$Synthetic.MArray".
 */
static int IsMArrayClass(ILClass *classInfo)
{
	const char *name;
	classInfo = ILClass_Parent(classInfo);
	if(!classInfo)
	{
		return 0;
	}
	name = ILClass_Name(classInfo);
	if(strcmp(name, "MArray") != 0)
	{
		return 0;
	}
	name = ILClass_Namespace(classInfo);
	return (name != 0 && !strcmp(name, "$Synthetic"));
}

/*
 * Get the element type of an array.
 */
static ILType *GetElementType(ILType *array)
{
	while(array != 0 && ILType_IsComplex(array) &&
	      array->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
	{
		array = array->un.array.elemType;
	}
	if(array != 0 && ILType_IsComplex(array) &&
	   array->kind == IL_TYPE_COMPLEX_ARRAY)
	{
		return array->un.array.elemType;
	}
	else
	{
		/* Shouldn't happen, but do something sane anyway */
		return ILType_Int32;
	}
}

/*
 * Get the internal version of a synthetic "SArray" or "MArray" method.
 */
void *_ILGetInternalArray(ILMethod *method, int *isCtor)
{
	ILClass *classInfo;
	const char *name;
	ILType *type;
	classInfo = ILMethod_Owner(method);
	if(!classInfo)
	{
		return 0;
	}
	name = ILMethod_Name(method);
	type = ILClassGetSynType(classInfo);
	if(!type)
	{
		return 0;
	}
	if(type->kind == IL_TYPE_COMPLEX_ARRAY &&
	   type->un.array.lowBound == 0)
	{
		/* Single-dimensional arrays have a simple constructor only */
		if(!strcmp(name, ".ctor"))
		{
			*isCtor = 1;
			return (void *)System_SArray_ctor;
		}
		else
		{
			return 0;
		}
	}
	if(!strcmp(name, ".ctor"))
	{
		/* There are two constructors for multi-dimensional arrays.
		   The first specifies sizes, and the second specifies
		   lower bounds and sizes.  Determine which one this is */
		*isCtor = 1;
		if(ILClassNextMemberByKind(classInfo, 0, IL_META_MEMBERKIND_METHOD)
				== (ILMember *)method)
		{
			/* This is the first constructor */
			return (void *)System_MArray_ctor_1;
		}
		else
		{
			/* This is the second constructor */
			return (void *)System_MArray_ctor_2;
		}
	}
	*isCtor = 0;
	if(!strcmp(name, "Get"))
	{
		/* Determine which get function to use based on the element type */
		type = ILTypeGetEnumType(GetElementType(type));
		if(ILType_IsPrimitive(type))
		{
			switch(ILType_ToElement(type))
			{
				case IL_META_ELEMTYPE_BOOLEAN:
				case IL_META_ELEMTYPE_I1:
				{
					return (void *)System_MArray_Get_sbyte;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_U1:
				{
					return (void *)System_MArray_Get_byte;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I2:
				{
					return (void *)System_MArray_Get_short;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_U2:
				case IL_META_ELEMTYPE_CHAR:
				{
					return (void *)System_MArray_Get_ushort;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I4:
				{
					return (void *)System_MArray_Get_int;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_U4:
				{
					return (void *)System_MArray_Get_uint;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I:
				{
					return (void *)System_MArray_Get_nativeInt;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_U:
				{
					return (void *)System_MArray_Get_nativeUInt;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I8:
				{
					return (void *)System_MArray_Get_long;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_U8:
				{
					return (void *)System_MArray_Get_ulong;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_R4:
				{
					return (void *)System_MArray_Get_float;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_R8:
				{
					return (void *)System_MArray_Get_double;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_R:
				{
					return (void *)System_MArray_Get_nativeFloat;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					return (void *)System_MArray_Get_managedValue;
				}
				/* Not reached */
			}
			return (void *)System_MArray_Get_int;
		}
		else if(ILType_IsValueType(type))
		{
			return (void *)System_MArray_Get_managedValue;
		}
		else
		{
			return (void *)System_MArray_Get_ref;
		}
	}
	else if(!strcmp(name, "Set"))
	{
		/* Determine which set function to use based on the element type */
		type = ILTypeGetEnumType(GetElementType(type));
		if(ILType_IsPrimitive(type))
		{
			switch(ILType_ToElement(type))
			{
				case IL_META_ELEMTYPE_BOOLEAN:
				case IL_META_ELEMTYPE_I1:
				case IL_META_ELEMTYPE_U1:
				{
					return (void *)System_MArray_Set_byte;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I2:
				case IL_META_ELEMTYPE_U2:
				case IL_META_ELEMTYPE_CHAR:
				{
					return (void *)System_MArray_Set_short;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I4:
				case IL_META_ELEMTYPE_U4:
				{
					return (void *)System_MArray_Set_int;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I:
				case IL_META_ELEMTYPE_U:
				{
					return (void *)System_MArray_Set_nativeInt;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_I8:
				case IL_META_ELEMTYPE_U8:
				{
					return (void *)System_MArray_Set_long;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_R4:
				{
					return (void *)System_MArray_Set_float;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_R8:
				{
					return (void *)System_MArray_Set_double;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_R:
				{
					return (void *)System_MArray_Set_nativeFloat;
				}
				/* Not reached */

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					return (void *)System_MArray_Set_managedValue;
				}
				/* Not reached */
			}
			return (void *)System_MArray_Set_int;
		}
		else if(ILType_IsValueType(type))
		{
			return (void *)System_MArray_Set_managedValue;
		}
		else
		{
			return (void *)System_MArray_Set_ref;
		}
	}
	else if(!strcmp(name, "Address"))
	{
		return (void *)System_MArray_Address;
	}
	else
	{
		return 0;
	}
}

/*
 * Determine if an array inherits from "$Synthetic.SArray".
 */
static int IsSArray(System_Array *array)
{
	if(array)
	{
		ILClass *classInfo;
		const char *name;
		classInfo = ILClass_Parent(GetObjectClass(array));
		if(!classInfo)
		{
			return 0;
		}
		name = ILClass_Name(classInfo);
		if(strcmp(name, "SArray") != 0)
		{
			return 0;
		}
		name = ILClass_Namespace(classInfo);
		return (name != 0 && !strcmp(name, "$Synthetic"));
	}
	else
	{
		return 0;
	}
}

/*
 * Determine if an array inherits from "$Synthetic.MArray".
 */
static int IsMArray(System_Array *array)
{
	if(array)
	{
		return IsMArrayClass(GetObjectClass(array));
	}
	else
	{
		return 0;
	}
}

/*
 * private int GetRankNative();
 */
static ILInt32 System_Array_GetRankNative(ILExecThread *thread,
										  System_Array *_this)
{
	if(IsMArray(_this))
	{
		return ((System_MArray *)_this)->rank;
	}
	else if(IsSArray(_this))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * private int GetLengthNative();
 */
static ILInt32 System_Array_GetLengthNative(ILExecThread *thread,
										    System_Array *_this)
{
	if(IsSArray(_this))
	{
		return _this->length;
	}
	else if(IsMArray(_this))
	{
		ILInt32 len = 1;
		ILInt32 dim;
		for(dim = 0; dim < ((System_MArray *)_this)->rank; ++dim)
		{
			len *= ((System_MArray *)_this)->bounds[dim * 2 + 1];
		}
		return len;
	}
	else
	{
		return 0;
	}
}

/*
 * private int GetLength(int dimension);
 */
static ILInt32 System_Array_GetLength(ILExecThread *thread,
									  System_Array *_this,
									  ILInt32 dimension)
{
	if(IsSArray(_this))
	{
		if(dimension == 0)
		{
			return _this->length;
		}
	}
	else if(IsMArray(_this))
	{
		if(dimension >= 0 && dimension < ((System_MArray *)_this)->rank)
		{
			return ((System_MArray *)_this)->bounds[dimension * 2 + 1];
		}
	}
	ILExecThreadThrowSystem(thread, "System.IndexOutOfRangeException",
							"Arg_InvalidArrayIndex");
	return 0;
}

/*
 * Method table for the "System.Array" class.
 */
IL_METHOD_BEGIN(_ILSystemArrayMethods)
	IL_METHOD("GetRankNative",	 "(T)i",  System_Array_GetRankNative)
	IL_METHOD("GetRank",	 	 "(T)i",  System_Array_GetRankNative) /* Mono */
	IL_METHOD("GetLengthNative", "(T)i",  System_Array_GetLengthNative)
	IL_METHOD("GetLength",		 "(Ti)i", System_Array_GetLength)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
