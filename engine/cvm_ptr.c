/*
 * cvm_ptr.c - Opcodes for accessing pointers.
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

#if defined(IL_CVM_GLOBALS)

static IL_INLINE int CkArrayStoreI8(CVMWord *posn, void *tempptr,
									ILUInt32 valueSize, ILUInt32 elemSize)
{
	ILUInt64 index = ReadULong(posn);
	if(index < (ILUInt64)(ILUInt32)(((System_Array *)tempptr)->length))
	{
		/* Convert the array pointer into an element pointer.
		   Note: this assumes that the array can never be
		   greater than 4 Gb in size */
		posn[-1].ptrValue = (void *)(((unsigned char *)tempptr) +
									 sizeof(System_Array) +
									 (ILUInt32)(index * elemSize));

		/* Shift the value down on the stack to remove the index */
		IL_MEMMOVE(posn, posn + CVM_WORDS_PER_LONG,
				   valueSize * sizeof(CVMWord));
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Write a 32-bit float value to a memory location.
 */
static IL_INLINE void WriteFloat32(CVMWord *ptr, ILFloat value)
{
	*((ILFloat *)ptr) = value;
}

/*
 * Write a long value to an unaligned pointer, but don't inline it.
 * This is needed in "lread_elem" to prevent a register spill on x86.
 */
static void WriteHardLong(CVMWord *ptr, ILInt64 value)
{
#ifdef CVM_LONGS_ALIGNED_WORD
	*((ILInt64 *)ptr) = value;
#else
	ILMemCpy(ptr, &value, sizeof(ILInt64));
#endif
}

#elif defined(IL_CVM_LOCALS)

void *tempptr;
ILClass *classInfo;
ILUInt32 tempSize;

#elif defined(IL_CVM_MAIN)

case COP_BREAD:
{
	/* Read a signed byte quantity from a pointer */
	stacktop[-1].intValue = (ILInt32)(*((ILInt8 *)(stacktop[-1].ptrValue)));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_UBREAD:
{
	/* Read an unsigned byte quantity from a pointer */
	stacktop[-1].uintValue = (ILUInt32)(*((ILUInt8 *)(stacktop[-1].ptrValue)));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_SREAD:
{
	/* Read a signed short quantity from a pointer */
	stacktop[-1].intValue = (ILInt32)(*((ILInt16 *)(stacktop[-1].ptrValue)));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_USREAD:
{
	/* Read an unsigned short quantity from a pointer */
	stacktop[-1].uintValue = (ILUInt32)(*((ILUInt16 *)(stacktop[-1].ptrValue)));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_IREAD:
{
	/* Read an integer quantity from a pointer */
	stacktop[-1].intValue = *((ILInt32 *)(stacktop[-1].ptrValue));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_FREAD:
{
	/* Read a float quantity from a pointer and push
	   it onto the stack as a "native float" value */
	WriteFloat(&(stacktop[-1]),
		(ILNativeFloat)(*((ILFloat *)(stacktop[-1].ptrValue))));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT - 1);
}
break;

case COP_DREAD:
{
	/* Read a double quantity from a pointer and push
	   it onto the stack as a "native float" value */
	WriteFloat(&(stacktop[-1]),
		(ILNativeFloat)(*((ILDouble *)(stacktop[-1].ptrValue))));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT - 1);
}
break;

case COP_PREAD:
{
	/* Read a pointer quantity from a pointer */
	stacktop[-1].ptrValue = *((void **)(stacktop[-1].ptrValue));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_MREAD:
{
	/* Read a multi-byte value from a pointer */
	IL_MEMCPY(&(stacktop[-1]), stacktop[-1].ptrValue, (unsigned)(pc[1]));
	stacktop += (((((unsigned)(pc[1])) + sizeof(CVMWord) - 1)
						/ sizeof(CVMWord)) - 1);
	pc += 2;
}
break;

case COP_BWRITE:
{
	/* Write a byte quantity to a pointer */
	*((ILInt8 *)(stacktop[-2].ptrValue)) = (ILInt8)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_SWRITE:
{
	/* Write a short quantity to a pointer */
	*((ILInt16 *)(stacktop[-2].ptrValue)) = (ILInt16)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_IWRITE:
{
	/* Write an integer quantity to a pointer */
	*((ILInt32 *)(stacktop[-2].ptrValue)) = (ILInt32)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_FWRITE:
{
	/* Write a "native float" value to a pointer as a "float" */
	*((ILFloat *)(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT + 1)] .ptrValue)) =
		(ILFloat)ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT + 1));
}
break;

case COP_DWRITE:
{
	/* Write a "native float" value to a pointer as a "double" */
	WriteDouble((CVMWord *)
		(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT + 1)].ptrValue),
		(ILDouble)(ReadFloat
					(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]))));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT + 1));
}
break;

case COP_PWRITE:
{
	/* Write a pointer quantity to a pointer */
	*((void **)(stacktop[-2].ptrValue)) = stacktop[-1].ptrValue;
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_MWRITE:
{
	/* Write a multi-byte value to a pointer */
	stacktop -= (((((unsigned)(pc[1])) + sizeof(CVMWord) - 1)
						/ sizeof(CVMWord)) + 1);
	IL_MEMCPY(stacktop[0].ptrValue, &(stacktop[1]), (unsigned)(pc[1]));
	pc += 2;
}
break;

case COP_BWRITE_R:
{
	/* Write a byte quantity to a pointer with reversed args */
	*((ILInt8 *)(stacktop[-1].ptrValue)) = (ILInt8)(stacktop[-2].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_SWRITE_R:
{
	/* Write a short quantity to a pointer with reversed args */
	*((ILInt16 *)(stacktop[-1].ptrValue)) = (ILInt16)(stacktop[-2].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_IWRITE_R:
{
	/* Write an integer quantity to a pointer with reversed args */
	*((ILInt32 *)(stacktop[-1].ptrValue)) = (ILInt32)(stacktop[-2].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_FWRITE_R:
{
	/* Write a "native float" value to a pointer as a "float",
	   with reversed arguments */
	*((ILFloat *)(stacktop[-1].ptrValue)) = (ILFloat)
		ReadFloat(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT + 1)]));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT + 1));
}
break;

case COP_DWRITE_R:
{
	/* Write a "native float" value to a pointer as a "double",
	   with reversed arguments */
	WriteDouble((CVMWord *)(stacktop[-1].ptrValue),
		(ILDouble)(ReadFloat
				(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT + 1)]))));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT + 1));
}
break;

case COP_PWRITE_R:
{
	/* Write a pointer quantity to a pointer with reversed args */
	*((void **)(stacktop[-1].ptrValue)) = stacktop[-2].ptrValue;
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_MWRITE_R:
{
	/* Write a multi-byte value to a pointer with reversed args */
	tempptr = stacktop[-1].ptrValue;
	stacktop -= (((((unsigned)(pc[1])) + sizeof(CVMWord) - 1)
						/ sizeof(CVMWord)) + 1);
	IL_MEMCPY(tempptr, &(stacktop[0]), (unsigned)(pc[1]));
	pc += 2;
}
break;

case COP_PADD_OFFSET:
{
	/* Add an explicit byte offset to a pointer.  This is used
	   to obtain the address of a field within a managed value */
	stacktop[-1].ptrValue = (void *)
		(((unsigned char *)(stacktop[-1].ptrValue)) +
				(unsigned)(pc[1]));
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PADD_OFFSET_N:
{
	/* Add an explicit byte offset to a pointer that is N
	   values down the stack */
	stacktop[-(((ILInt32)(pc[1])) + 1)].ptrValue = (void *)
		(((unsigned char *)(stacktop[-(((ILInt32)(pc[1])) + 1)]
				.ptrValue)) + (ILUInt32)(pc[2]));
	MODIFY_PC_AND_STACK(3, 0);
}
break;

case COP_PADD_I4:
{
	/* Add a 32-bit integer to a pointer */
	stacktop[-2].ptrValue = (void *)
		(((unsigned char *)(stacktop[-2].ptrValue)) +
		 ((ILInt32)(stacktop[-1].intValue)));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_PADD_I4_R:
{
	/* Add a 32-bit integer to a pointer with reversed args */
	stacktop[-2].ptrValue = (void *)
		(((unsigned char *)(stacktop[-1].ptrValue)) +
		 ((ILInt32)(stacktop[-2].intValue)));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_PADD_I8:
{
	/* Add a 64-bit integer to a pointer */
#if SIZEOF_LONG != 4
	stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue = (void *)
		(((unsigned char *)
		     (stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue)) +
		 ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
#else
	/* This looks like a 32-bit platform, so truncate
	   the 64-bit value before we use it */
	stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue = (void *)
		(((unsigned char *)
		     (stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue)) +
		 (ILInt32)ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
#endif
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_PADD_I8_R:
{
	/* Add a 64-bit integer to a pointer with reversed args */
#if SIZEOF_LONG != 4
	stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue = (void *)
		(((unsigned char *)(stacktop[-1].ptrValue)) +
		 ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)])));
#else
	/* This looks like a 32-bit platform, so truncate
	   the 64-bit value before we use it */
	stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue = (void *)
		(((unsigned char *)(stacktop[-1].ptrValue)) +
		 (ILInt32)ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)])));
#endif
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_PSUB:
{
	/* Subtract two pointers with a "native int" result */
#ifdef IL_NATIVE_INT64
	WriteLong(&(stacktop[-2]),
		(ILInt64)(((unsigned char *)(stacktop[-2].ptrValue)) -
				  ((unsigned char *)(stacktop[-1].ptrValue))));
#else
	stacktop[-2].intValue =
		(ILInt32)(((unsigned char *)(stacktop[-2].ptrValue)) -
				  ((unsigned char *)(stacktop[-1].ptrValue)));
#endif
	MODIFY_PC_AND_STACK(1, -2 + CVM_WORDS_PER_NATIVE_INT);
}
break;

case COP_PSUB_I4:
{
	/* Subtract a 32-bit integer from a pointer */
	stacktop[-2].ptrValue = (void *)
		(((unsigned char *)(stacktop[-2].ptrValue)) -
		 ((ILInt32)(stacktop[-1].intValue)));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_PSUB_I8:
{
	/* Subtract a 64-bit integer from a pointer */
#if SIZEOF_LONG != 4
	stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue = (void *)
		(((unsigned char *)
		     (stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue)) -
		 ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
#else
	/* This looks like a 32-bit platform, so truncate
	   the 64-bit value before we use it */
	stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue = (void *)
		(((unsigned char *)
		     (stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue)) -
		 (ILInt32)ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
#endif
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_CKNULL:
{
	/* Check the stack top for "null" */
	if(stacktop[-1].ptrValue != 0)
	{
		MODIFY_PC_AND_STACK(1, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_CKNULL_N:
{
	/* Check a value some way down the stack for "null" */
	if(stacktop[-(((int)(pc[1])) + 1)].ptrValue != 0)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_LDRVA:
{
	/* Load a relative virtual address (RVA) onto the stack */
	/* TODO */
	stacktop[0].ptrValue = 0;
	MODIFY_PC_AND_STACK(5, 1);
}
break;

/* Read a simple value from an array element */
#define	SIMPLE_READ_ELEM(name,type) \
case name: \
{ \
	if((tempptr = stacktop[-2].ptrValue) != 0) \
	{ \
		if(stacktop[-1].uintValue < \
				((ILUInt32)(((System_Array *)tempptr)->length))) \
		{ \
			stacktop[-2].intValue = \
				(ILInt32)(((type *)(ArrayToBuffer(tempptr))) \
							[stacktop[-1].uintValue]); \
			MODIFY_PC_AND_STACK(1, -1); \
		} \
		else \
		{ \
			ARRAY_INDEX_EXCEPTION(); \
		} \
	} \
	else \
	{ \
		NULL_POINTER_EXCEPTION(); \
	} \
} \
break

SIMPLE_READ_ELEM(COP_BREAD_ELEM,  ILInt8);
SIMPLE_READ_ELEM(COP_UBREAD_ELEM, ILUInt8);
SIMPLE_READ_ELEM(COP_SREAD_ELEM,  ILInt16);
SIMPLE_READ_ELEM(COP_USREAD_ELEM, ILUInt16);
SIMPLE_READ_ELEM(COP_IREAD_ELEM,  ILInt32);

case COP_PREAD_ELEM:
{
	/* Read a pointer value from an array element */
	if((tempptr = stacktop[-2].ptrValue) != 0)
	{
		if(stacktop[-1].uintValue <
				((ILUInt32)(((System_Array *)tempptr)->length)))
		{
			stacktop[-2].ptrValue =
				((void **)(ArrayToBuffer(tempptr)))[stacktop[-1].uintValue];
			MODIFY_PC_AND_STACK(1, -1);
		}
		else
		{
			ARRAY_INDEX_EXCEPTION();
		}
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

/* Write a simple value to an array element */
#define	SIMPLE_WRITE_ELEM(name,type) \
case name: \
{ \
	if((tempptr = stacktop[-3].ptrValue) != 0) \
	{ \
		if(stacktop[-2].uintValue < \
				((ILUInt32)(((System_Array *)tempptr)->length))) \
		{ \
			((type *)(ArrayToBuffer(tempptr)))[stacktop[-2].uintValue] = \
					(type)(stacktop[-1].intValue); \
			MODIFY_PC_AND_STACK(1, -3); \
		} \
		else \
		{ \
			ARRAY_INDEX_EXCEPTION(); \
		} \
	} \
	else \
	{ \
		NULL_POINTER_EXCEPTION(); \
	} \
} \
break

SIMPLE_WRITE_ELEM(COP_BWRITE_ELEM, ILInt8);
SIMPLE_WRITE_ELEM(COP_SWRITE_ELEM, ILInt16);
SIMPLE_WRITE_ELEM(COP_IWRITE_ELEM, ILInt32);

case COP_PWRITE_ELEM:
{
	/* Write a pointer value to an array element */
	if((tempptr = stacktop[-3].ptrValue) != 0)
	{
		if(stacktop[-2].uintValue <
				((ILUInt32)(((System_Array *)tempptr)->length)))
		{
			((void **)(ArrayToBuffer(tempptr)))[stacktop[-2].uintValue] =
					stacktop[-1].ptrValue;
			MODIFY_PC_AND_STACK(1, -3);
		}
		else
		{
			ARRAY_INDEX_EXCEPTION();
		}
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_CKARRAY_LOAD_I4:
{
	/* Check an array load that uses an I4 index */
	if((tempptr = stacktop[-2].ptrValue) != 0)
	{
		if(stacktop[-1].uintValue <
				((ILUInt32)(((System_Array *)tempptr)->length)))
		{
			/* Adjust the pointer to address the first array element */
			stacktop[-2].ptrValue = (void *)(((unsigned char *)tempptr) +
											 sizeof(System_Array));
			MODIFY_PC_AND_STACK(1, 0);
		}
		else
		{
			ARRAY_INDEX_EXCEPTION();
		}
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_CKARRAY_LOAD_I8:
{
	/* Check an array load that uses an I8 index */
	if((tempptr = stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue) != 0)
	{
		if(ReadULong(&(stacktop[-CVM_WORDS_PER_LONG])) <
				((ILUInt64)(ILUInt32)(((System_Array *)tempptr)->length)))
		{
			/* Adjust the pointer to address the first array element */
			stacktop[-(CVM_WORDS_PER_LONG + 1)].ptrValue =
				(void *)(((unsigned char *)tempptr) + sizeof(System_Array));
			MODIFY_PC_AND_STACK(1, 0);
		}
		else
		{
			ARRAY_INDEX_EXCEPTION();
		}
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_CKARRAY_STORE_I8:
{
	/* Check an array store that uses an I8 index */
	tempNum = ((ILUInt32)(pc[1]));
	if((tempptr = (stacktop - tempNum - CVM_WORDS_PER_LONG - 1)->ptrValue) != 0)
	{
		if(CkArrayStoreI8(stacktop - tempNum - CVM_WORDS_PER_LONG,
						  tempptr, tempNum, (ILUInt32)(pc[2])))
		{
			MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
		}
		else
		{
			ARRAY_INDEX_EXCEPTION();
		}
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_ARRAY_LEN:
{
	/* Get the length of an array */
	if(stacktop[-1].ptrValue != 0)
	{
	#ifdef IL_NATIVE_INT32
		stacktop[-1].intValue =
			((System_Array *)(stacktop[-1].ptrValue))->length;
	#else
		WriteLong(&(stacktop[-1]),
				  (ILInt64)(((System_Array *)(stacktop[-1].ptrValue))->length));
	#endif
		MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_INT - 1);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

#define GET_FIELD(type)	\
(*((type *)(((unsigned char *)(stacktop[-1].ptrValue)) + \
				(ILUInt32)(pc[1]))))
#define PUT_FIELD(type)	\
(*((type *)(((unsigned char *)(stacktop[-2].ptrValue)) + \
				(ILUInt32)(pc[1]))))

case COP_BREAD_FIELD:
{
	/* Read a byte value from an object field */
	if(stacktop[-1].ptrValue != 0)
	{
		stacktop[-1].intValue = (ILInt32)(GET_FIELD(ILInt8));
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_UBREAD_FIELD:
{
	/* Read an unsigned byte value from an object field */
	if(stacktop[-1].ptrValue != 0)
	{
		stacktop[-1].uintValue = (ILUInt32)(GET_FIELD(ILUInt8));
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_SREAD_FIELD:
{
	/* Read a short value from an object field */
	if(stacktop[-1].ptrValue != 0)
	{
		stacktop[-1].intValue = (ILInt32)(GET_FIELD(ILInt16));
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_USREAD_FIELD:
{
	/* Read an unsigned short value from an object field */
	if(stacktop[-1].ptrValue != 0)
	{
		stacktop[-1].uintValue = (ILUInt32)(GET_FIELD(ILUInt16));
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_IREAD_FIELD:
{
	/* Read an integer value from an object field */
	if(stacktop[-1].ptrValue != 0)
	{
		stacktop[-1].intValue = GET_FIELD(ILInt32);
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_PREAD_FIELD:
{
	/* Read a pointer value from an object field */
	if(stacktop[-1].ptrValue != 0)
	{
		stacktop[-1].ptrValue = GET_FIELD(void *);
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_BWRITE_FIELD:
{
	/* Write a byte value to an object field */
	if(stacktop[-2].ptrValue != 0)
	{
		PUT_FIELD(ILInt8) = (ILInt8)(stacktop[-1].intValue);
		MODIFY_PC_AND_STACK(2, -2);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_SWRITE_FIELD:
{
	/* Write a short value to an object field */
	if(stacktop[-2].ptrValue != 0)
	{
		PUT_FIELD(ILInt16) = (ILInt16)(stacktop[-1].intValue);
		MODIFY_PC_AND_STACK(2, -2);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_IWRITE_FIELD:
{
	/* Write an integer value to an object field */
	if(stacktop[-2].ptrValue != 0)
	{
		PUT_FIELD(ILInt32) = stacktop[-1].intValue;
		MODIFY_PC_AND_STACK(2, -2);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_PWRITE_FIELD:
{
	/* Write a pointer value to an object field */
	if(stacktop[-2].ptrValue != 0)
	{
		PUT_FIELD(void *) = stacktop[-1].ptrValue;
		MODIFY_PC_AND_STACK(2, -2);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_CASTCLASS:
{
	/* Cast the object on the stack top to a new class */
	classInfo = (ILClass *)(ReadPointer(pc + 1));
	if(!stacktop[-1].ptrValue ||
	   ILClassInheritsFrom(GetObjectClass(stacktop[-1].ptrValue), classInfo))
	{
		MODIFY_PC_AND_STACK(1 + sizeof(void *), 0);
	}
	else
	{
		INVALID_CAST_EXCEPTION();
	}
}
break;

case COP_ISINST:
{
	/* Determine if the object on the stack top is an
	   instance of a particular class */
	classInfo = (ILClass *)(ReadPointer(pc + 1));
	if(stacktop[-1].ptrValue != 0 &&
	   !ILClassInheritsFrom(GetObjectClass(stacktop[-1].ptrValue), classInfo))
	{
		stacktop[-1].ptrValue = 0;
	}
	MODIFY_PC_AND_STACK(1 + sizeof(void *), 0);
}
break;

case COP_CASTINTERFACE:
{
	/* Cast the object on the stack top to a new interface */
	classInfo = (ILClass *)(ReadPointer(pc + 1));
	if(!stacktop[-1].ptrValue ||
	   ILClassImplements(GetObjectClass(stacktop[-1].ptrValue), classInfo))
	{
		MODIFY_PC_AND_STACK(1 + sizeof(void *), 0);
	}
	else
	{
		INVALID_CAST_EXCEPTION();
	}
}
break;

case COP_ISINTERFACE:
{
	/* Determine if the object on the stack top is an
	   instance of a particular interface */
	classInfo = (ILClass *)(ReadPointer(pc + 1));
	if(stacktop[-1].ptrValue != 0 &&
	   !ILClassImplements(GetObjectClass(stacktop[-1].ptrValue), classInfo))
	{
		stacktop[-1].ptrValue = 0;
	}
	MODIFY_PC_AND_STACK(1 + sizeof(void *), 0);
}
break;

case COP_GET_STATIC:
{
	/* Get the static data area for a particular class */
	classInfo = (ILClass *)(ReadPointer(pc + 1));
	if(((ILClassPrivate *)(classInfo->userData))->staticData)
	{
		stacktop[0].ptrValue =
			((ILClassPrivate *)(classInfo->userData))->staticData;
		MODIFY_PC_AND_STACK(1 + sizeof(void *), 1);
		break;
	}
	COPY_STATE_TO_THREAD();
	((ILClassPrivate *)(classInfo->userData))->staticData =
		_ILEngineAlloc(thread, 0,
		   ((ILClassPrivate *)(classInfo->userData))->staticSize);
	RESTORE_STATE_FROM_THREAD();
	stacktop[0].ptrValue =
		((ILClassPrivate *)(classInfo->userData))->staticData;
	MODIFY_PC_AND_STACK(1 + sizeof(void *), 1);
}
break;

case COP_NEW:
{
	/* Create a new object of the current method's class */
	if(((ILUInt32)(stackmax - stacktop)) >= 1)
	{
		/* Allocate the object and push it onto the stack */
		classInfo = method->member.owner;
		COPY_STATE_TO_THREAD();
		tempptr = _ILEngineAlloc(thread, classInfo,
								 ((ILClassPrivate *)(classInfo->userData))
								 		->size);
		RESTORE_STATE_FROM_THREAD();
		stacktop[0].ptrValue = tempptr;
		MODIFY_PC_AND_STACK(1, 1);
	}
	else
	{
		STACK_OVERFLOW_EXCEPTION();
	}
}
break;

case COP_NEW_VALUE:
{
	/* Create a new value type and insert it below the constructors */
	tempNum = (ILUInt32)(pc[1]);
	tempSize = (ILUInt32)(pc[2]);
	IL_MEMMOVE(stacktop - tempNum + tempSize + 1, stacktop - tempNum,
			   tempNum * sizeof(CVMWord));
	IL_MEMZERO(stacktop - tempNum, tempSize * sizeof(CVMWord));
	(stacktop - tempNum + tempSize)->ptrValue = (void *)(stacktop - tempNum);
	MODIFY_PC_AND_STACK(3, tempSize + 1);
}
break;

case COP_LDSTR:
{
	/* Load a string constant onto the stack */
	COPY_STATE_TO_THREAD();
	stacktop[0].ptrValue = _ILStringInternFromImage
			(thread, ILProgramItem_Image(method), IL_READ_UINT32(pc + 1));
	RESTORE_STATE_FROM_THREAD();
	pcstart = thread->pcstart;
	pc = pcstart + thread->pc;
	MODIFY_PC_AND_STACK(5, 1);
}
break;

case COP_LDTOKEN:
{
	/* Load a token handle onto the stack */
	stacktop[0].ptrValue = ReadPointer(pc + 1);
	MODIFY_PC_AND_STACK(1 + sizeof(void *), 1);
}
break;

case COP_BOX:
{
	/* Box a managed value */
	classInfo = (ILClass *)ReadPointer(pc + 2);
	tempNum = (((ILUInt32)(pc[1])) + sizeof(CVMWord) - 1) / sizeof(CVMWord);
	COPY_STATE_TO_THREAD();
	tempptr = (void *)_ILEngineAlloc(thread, classInfo, (ILUInt32)(pc[1]));
	RESTORE_STATE_FROM_THREAD();
	IL_MEMCPY(tempptr, stacktop - tempNum, (ILUInt32)(pc[1]));
	stacktop[-((ILInt32)tempNum)].ptrValue = tempptr;
	MODIFY_PC_AND_STACK(2 + sizeof(void *), -((ILInt32)(tempNum - 1)));
}
break;

case COP_BOX_PTR:
{
	/* Box a managed pointer */
	classInfo = (ILClass *)ReadPointer(pc + 2);
	COPY_STATE_TO_THREAD();
	tempptr = (void *)_ILEngineAlloc(thread, classInfo, (ILUInt32)(pc[1]));
	RESTORE_STATE_FROM_THREAD();
	IL_MEMCPY(tempptr, stacktop[-1].ptrValue, (ILUInt32)(pc[1]));
	stacktop[-1].ptrValue = tempptr;
	MODIFY_PC_AND_STACK(2 + sizeof(void *), 0);
}
break;

case COP_MEMCPY:
{
	/* Copy a fixed-size memory block */
	IL_MEMCPY(stacktop[-2].ptrValue, stacktop[-1].ptrValue, (unsigned)(pc[1]));
	MODIFY_PC_AND_STACK(2, -2);
}
break;

case COP_MEMMOVE:
{
	/* Move a variable-size memory block */
	IL_MEMMOVE(stacktop[-3].ptrValue, stacktop[-2].ptrValue,
			   stacktop[-1].uintValue);
	MODIFY_PC_AND_STACK(1, -3);
}
break;

case COP_MEMZERO:
{
	/* Fill a fixed-size memory block with zeroes */
	IL_MEMZERO(stacktop[-1].ptrValue, (unsigned)(pc[1]));
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_MEMSET:
{
	/* Set the contents of a memory block to the same byte value */
	IL_MEMSET(stacktop[-3].ptrValue, (int)(stacktop[-2].intValue),
			  stacktop[-1].uintValue);
	MODIFY_PC_AND_STACK(1, -3);
}
break;

#elif defined(IL_CVM_WIDE)

case COP_MREAD:
{
	/* Wide version of "mread" */
	tempNum = IL_READ_UINT32(pc + 2);
	IL_MEMCPY(&(stacktop[-1]), stacktop[-1].ptrValue, tempNum);
	stacktop += (((tempNum + sizeof(CVMWord) - 1) / sizeof(CVMWord)) - 1);
	pc += 6;
}
break;

case COP_MWRITE:
{
	/* Wide version of "mwrite" */
	tempNum = IL_READ_UINT32(pc + 2);
	stacktop -= (((tempNum + sizeof(CVMWord) - 1) / sizeof(CVMWord)) + 1);
	IL_MEMCPY(stacktop[0].ptrValue, &(stacktop[1]), tempNum);
	pc += 6;
}
break;

case COP_MWRITE_R:
{
	/* Wide version of "mwrite_r" */
	tempNum = IL_READ_UINT32(pc + 2);
	tempptr = stacktop[-1].ptrValue;
	stacktop -= (((tempNum + sizeof(CVMWord) - 1) / sizeof(CVMWord)) + 1);
	IL_MEMCPY(tempptr, &(stacktop[0]), tempNum);
	pc += 6;
}
break;

case COP_CKNULL_N:
{
	/* Wide version of "cknull_n" */
	if(stacktop[-(((int)IL_READ_UINT32(pc + 2)) + 1)].ptrValue != 0)
	{
		MODIFY_PC_AND_STACK(6, 0);
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

case COP_PADD_OFFSET_N:
{
	/* Wide version of "padd_offset_n" */
	tempNum = IL_READ_UINT32(pc + 2);
	stacktop[-((ILInt32)(tempNum + 1))].ptrValue = (void *)
		(((unsigned char *)(stacktop[-((ILInt32)(tempNum + 1))]
				.ptrValue)) + IL_READ_UINT32(pc + 6));
	MODIFY_PC_AND_STACK(10, 0);
}
break;

case COP_NEW_VALUE:
{
	/* Wide version of "new_value" */
	tempNum = IL_READ_UINT32(pc + 2);
	tempSize = IL_READ_UINT32(pc + 6);
	IL_MEMMOVE(stacktop - tempNum + tempSize + 1, stacktop - tempNum,
			   tempNum * sizeof(CVMWord));
	IL_MEMZERO(stacktop - tempNum, tempSize * sizeof(CVMWord));
	(stacktop - tempNum + tempSize)->ptrValue = (void *)(stacktop - tempNum);
	MODIFY_PC_AND_STACK(10, tempSize + 1);
}
break;

case COP_BOX:
{
	/* Wide version of "box" */
	classInfo = (ILClass *)ReadPointer(pc + 6);
	tempSize = IL_READ_UINT32(pc + 2);
	tempNum = (tempSize + sizeof(CVMWord) - 1) / sizeof(CVMWord);
	COPY_STATE_TO_THREAD();
	tempptr = (void *)_ILEngineAlloc(thread, classInfo, tempSize);
	RESTORE_STATE_FROM_THREAD();
	IL_MEMCPY(tempptr, stacktop - tempNum, tempSize);
	stacktop[-((ILInt32)tempNum)].ptrValue = tempptr;
	MODIFY_PC_AND_STACK(6 + sizeof(void *), -((ILInt32)(tempNum - 1)));
}
break;

case COP_BOX_PTR:
{
	/* Wide version of "box_ptr" */
	classInfo = (ILClass *)ReadPointer(pc + 6);
	tempSize = IL_READ_UINT32(pc + 2);
	COPY_STATE_TO_THREAD();
	tempptr = (void *)_ILEngineAlloc(thread, classInfo, tempSize);
	RESTORE_STATE_FROM_THREAD();
	IL_MEMCPY(tempptr, stacktop[-1].ptrValue, tempSize);
	stacktop[-1].ptrValue = tempptr;
	MODIFY_PC_AND_STACK(6 + sizeof(void *), 0);
}
break;

case COP_MEMCPY:
{
	/* Wide version of "memcpy" */
	IL_MEMCPY(stacktop[-2].ptrValue, stacktop[-1].ptrValue,
			  IL_READ_UINT32(pc + 2));
	MODIFY_PC_AND_STACK(6, -2);
}
break;

case COP_MEMZERO:
{
	/* Wide version of "memzero" */
	IL_MEMZERO(stacktop[-1].ptrValue, IL_READ_UINT32(pc + 2));
	MODIFY_PC_AND_STACK(6, -1);
}
break;

#elif defined(IL_CVM_PREFIX)

/* Read a large value from an array element */
#define	LARGE_READ_ELEM(name,type,size,read,write) \
case name: \
{ \
	if((tempptr = stacktop[-2].ptrValue) != 0) \
	{ \
		if(stacktop[-1].uintValue < \
				((ILUInt32)(((System_Array *)tempptr)->length))) \
		{ \
			write(&(stacktop[-2]), \
				  read((CVMWord *)&(((type *)(ArrayToBuffer(tempptr))) \
							        	[stacktop[-1].uintValue]))); \
			MODIFY_PC_AND_STACK(1, -2 + size); \
		} \
		else \
		{ \
			ARRAY_INDEX_EXCEPTION(); \
		} \
	} \
	else \
	{ \
		NULL_POINTER_EXCEPTION(); \
	} \
} \
break

LARGE_READ_ELEM(COP_PREFIX_LREAD_ELEM, ILInt64, CVM_WORDS_PER_LONG,
				ReadLong, WriteLong);
LARGE_READ_ELEM(COP_PREFIX_FREAD_ELEM, ILFloat, CVM_WORDS_PER_NATIVE_FLOAT,
				(ILNativeFloat)*(ILFloat *), WriteFloat);
LARGE_READ_ELEM(COP_PREFIX_DREAD_ELEM, ILDouble, CVM_WORDS_PER_NATIVE_FLOAT,
				ReadDouble, WriteFloat);

/* Write a large value to an array element */
#define	LARGE_WRITE_ELEM(name,type,size,read,write) \
case name: \
{ \
	if((tempptr = stacktop[-(size + 2)].ptrValue) != 0) \
	{ \
		if(stacktop[-(size + 1)].uintValue < \
				((ILUInt32)(((System_Array *)tempptr)->length))) \
		{ \
			write(((CVMWord *)&(((type *)(ArrayToBuffer(tempptr))) \
						[stacktop[-(size + 1)].uintValue])), \
				  (type)read(&(stacktop[-(size)]))); \
			MODIFY_PC_AND_STACK(1, -(size + 2)); \
		} \
		else \
		{ \
			ARRAY_INDEX_EXCEPTION(); \
		} \
	} \
	else \
	{ \
		NULL_POINTER_EXCEPTION(); \
	} \
} \
break

LARGE_WRITE_ELEM(COP_PREFIX_LWRITE_ELEM, ILInt64, CVM_WORDS_PER_LONG,
				 ReadLong, WriteHardLong);
LARGE_WRITE_ELEM(COP_PREFIX_FWRITE_ELEM, ILFloat, CVM_WORDS_PER_NATIVE_FLOAT,
				 ReadFloat, WriteFloat32);
LARGE_WRITE_ELEM(COP_PREFIX_DWRITE_ELEM, ILDouble, CVM_WORDS_PER_NATIVE_FLOAT,
				 ReadFloat, WriteDouble);

case COP_PREFIX_MKREFANY:
{
	/* Make a typedref from an address and class information block */
	((ILTypedRef *)(stacktop - 1))->value = stacktop[-1].ptrValue;
	((ILTypedRef *)(stacktop - 1))->type = ReadPointer(pc + 2);
	MODIFY_PC_AND_STACK(2 + sizeof(void *), CVM_WORDS_PER_TYPED_REF - 1);
}
break;

case COP_PREFIX_REFANYVAL:
{
	/* Extract the value part of a typedref */
	if(((ILTypedRef *)(stacktop - CVM_WORDS_PER_TYPED_REF))->type ==
			ReadPointer(pc + 2))
	{
		stacktop[-CVM_WORDS_PER_TYPED_REF].ptrValue =
			((ILTypedRef *)(stacktop - CVM_WORDS_PER_TYPED_REF))->value;
		MODIFY_PC_AND_STACK(2 + sizeof(void *),
							-(CVM_WORDS_PER_TYPED_REF - 1));
	}
	else
	{
		INVALID_CAST_EXCEPTION();
	}
}
break;

case COP_PREFIX_REFANYTYPE:
{
	/* Extract the type part of a typedref */
	stacktop[-CVM_WORDS_PER_TYPED_REF].ptrValue =
		((ILTypedRef *)(stacktop - CVM_WORDS_PER_TYPED_REF))->type;
	MODIFY_PC_AND_STACK(2 + sizeof(void *), -(CVM_WORDS_PER_TYPED_REF - 1));
}
break;

#endif /* IL_CVM_PREFIX */