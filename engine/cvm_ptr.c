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

/**
 * <opcode name="bread" group="Pointer handling">
 *   <operation>Read <code>int8</code> from pointer</operation>
 *
 *   <format>bread</format>
 *
 *   <form name="bread" code="COP_BREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read the 8 bit value at <i>pointer</i> in
 *   memory, sign-extend it to <code>int32</code> and push it onto
 *   the stack.</description>
 * </opcode>
 */
case COP_BREAD:
{
	/* Read a signed byte quantity from a pointer */
	stacktop[-1].intValue = (ILInt32)(*((ILInt8 *)(stacktop[-1].ptrValue)));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

/**
 * <opcode name="ubread" group="Pointer handling">
 *   <operation>Read <code>uint8</code> from pointer</operation>
 *
 *   <format>ubread</format>
 *
 *   <form name="ubread" code="COP_UBREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read the 8 bit value at <i>pointer</i> in
 *   memory, zero-extend it to <code>int32</code> and push it onto
 *   the stack.</description>
 * </opcode>
 */
case COP_UBREAD:
{
	/* Read an unsigned byte quantity from a pointer */
	stacktop[-1].uintValue = (ILUInt32)(*((ILUInt8 *)(stacktop[-1].ptrValue)));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

/**
 * <opcode name="sread" group="Pointer handling">
 *   <operation>Read <code>int16</code> from pointer</operation>
 *
 *   <format>sread</format>
 *
 *   <form name="sread" code="COP_SREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read the 16 bit value at <i>pointer</i> in
 *   memory, sign-extend it to <code>int32</code> and push it onto
 *   the stack.</description>
 * </opcode>
 */
case COP_SREAD:
{
	/* Read a signed short quantity from a pointer */
	stacktop[-1].intValue = (ILInt32)(*((ILInt16 *)(stacktop[-1].ptrValue)));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

/**
 * <opcode name="usread" group="Pointer handling">
 *   <operation>Read <code>uint16</code> from pointer</operation>
 *
 *   <format>usread</format>
 *
 *   <form name="usread" code="COP_USREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read the 16 bit value at <i>pointer</i> in
 *   memory, zero-extend it to <code>int32</code> and push it onto
 *   the stack.</description>
 * </opcode>
 */
case COP_USREAD:
{
	/* Read an unsigned short quantity from a pointer */
	stacktop[-1].uintValue = (ILUInt32)(*((ILUInt16 *)(stacktop[-1].ptrValue)));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

/**
 * <opcode name="iread" group="Pointer handling">
 *   <operation>Read <code>int32</code> from pointer</operation>
 *
 *   <format>iread</format>
 *
 *   <form name="iread" code="COP_IREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read the 32 bit <code>int32</code> value
 *   at <i>pointer</i> in memory, and push it onto the stack.</description>
 *
 *   <notes>This instruction can also be used to read <code>uint32</code>
 *   values from memory.</notes>
 * </opcode>
 */
case COP_IREAD:
{
	/* Read an integer quantity from a pointer */
	stacktop[-1].intValue = *((ILInt32 *)(stacktop[-1].ptrValue));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

/**
 * <opcode name="fread" group="Pointer handling">
 *   <operation>Read <code>float32</code> from pointer</operation>
 *
 *   <format>fread</format>
 *
 *   <form name="fread" code="COP_FREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read the 32 bit <code>float32</code> value
 *   at <i>pointer</i> in memory, extend it to <code>native float</code>,
 *   and push it onto the stack.</description>
 * </opcode>
 */
case COP_FREAD:
{
	/* Read a float quantity from a pointer and push
	   it onto the stack as a "native float" value */
	WriteFloat(&(stacktop[-1]),
		(ILNativeFloat)(*((ILFloat *)(stacktop[-1].ptrValue))));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT - 1);
}
break;

/**
 * <opcode name="dread" group="Pointer handling">
 *   <operation>Read <code>float64</code> from pointer</operation>
 *
 *   <format>dread</format>
 *
 *   <form name="dread" code="COP_DREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read the 64 bit <code>float64</code> value
 *   at <i>pointer</i> in memory, extend it to <code>native float</code>,
 *   and push it onto the stack.</description>
 * </opcode>
 */
case COP_DREAD:
{
	/* Read a double quantity from a pointer and push
	   it onto the stack as a "native float" value */
	WriteFloat(&(stacktop[-1]),
		(ILNativeFloat)(*((ILDouble *)(stacktop[-1].ptrValue))));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT - 1);
}
break;

/**
 * <opcode name="pread" group="Pointer handling">
 *   <operation>Read <code>ptr</code> from pointer</operation>
 *
 *   <format>pread</format>
 *
 *   <form name="pread" code="COP_PREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read the <code>ptr</code> value
 *   at <i>pointer</i> in memory, and push it onto the stack.</description>
 *
 *   <notes>This instruction must not be confused with <i>iread</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of memory space on all
 *   platforms.</notes>
 * </opcode>
 */
case COP_PREAD:
{
	/* Read a pointer quantity from a pointer */
	stacktop[-1].ptrValue = *((void **)(stacktop[-1].ptrValue));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

/**
 * <opcode name="mread" group="Pointer handling">
 *   <operation>Read multiple bytes from pointer</operation>
 *
 *   <format>mread<fsep/>N[1]</format>
 *   <format>wide<fsep/>mread<fsep/>N[4]</format>
 *
 *   <form name="mread" code="COP_MREAD"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Read <i>N</i> bytes from <i>pointer</i>
 *   in memory, and push them onto the stack, aligned on a word
 *   boundary.</description>
 *
 *   <notes>The exact number of stack words that is pushed depends
 *   upon the underlying platform.</notes>
 * </opcode>
 */
case COP_MREAD:
{
	/* Read a multi-byte value from a pointer */
	IL_MEMCPY(&(stacktop[-1]), stacktop[-1].ptrValue, (unsigned)(pc[1]));
	stacktop += (((((unsigned)(pc[1])) + sizeof(CVMWord) - 1)
						/ sizeof(CVMWord)) - 1);
	pc += 2;
}
break;

/**
 * <opcode name="bwrite" group="Pointer handling">
 *   <operation>Write <code>int8</code> to pointer</operation>
 *
 *   <format>bwrite</format>
 *
 *   <form name="bwrite" code="COP_BWRITE"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>int32</code> respectively.
 *   Truncate <i>value</i> to 8 bits and write it to <i>pointer</i> in
 *   memory.</description>
 *
 *   <notes>This instruction can also be used to write values of
 *   type <code>uint8</code> to memory.</notes>
 * </opcode>
 */
case COP_BWRITE:
{
	/* Write a byte quantity to a pointer */
	*((ILInt8 *)(stacktop[-2].ptrValue)) = (ILInt8)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="swrite" group="Pointer handling">
 *   <operation>Write <code>int16</code> to pointer</operation>
 *
 *   <format>swrite</format>
 *
 *   <form name="swrite" code="COP_SWRITE"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>int32</code> respectively.
 *   Truncate <i>value</i> to 16 bits and write it to <i>pointer</i> in
 *   memory.</description>
 *
 *   <notes>This instruction can also be used to write values of
 *   type <code>uint16</code> to memory.</notes>
 * </opcode>
 */
case COP_SWRITE:
{
	/* Write a short quantity to a pointer */
	*((ILInt16 *)(stacktop[-2].ptrValue)) = (ILInt16)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="iwrite" group="Pointer handling">
 *   <operation>Write <code>int32</code> to pointer</operation>
 *
 *   <format>iwrite</format>
 *
 *   <form name="iwrite" code="COP_IWRITE"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>int32</code> respectively.
 *   Write <i>value</i> to <i>pointer</i> in memory.</description>
 *
 *   <notes>This instruction can also be used to write values of
 *   type <code>uint32</code> to memory.</notes>
 * </opcode>
 */
case COP_IWRITE:
{
	/* Write an integer quantity to a pointer */
	*((ILInt32 *)(stacktop[-2].ptrValue)) = (ILInt32)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="fwrite" group="Pointer handling">
 *   <operation>Write <code>float32</code> to pointer</operation>
 *
 *   <format>fwrite</format>
 *
 *   <form name="fwrite" code="COP_FWRITE"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>native float</code> respectively.
 *   Truncate <i>value</i> to <code>float32</code> and write it
 *   to <i>pointer</i> in memory.</description>
 * </opcode>
 */
case COP_FWRITE:
{
	/* Write a "native float" value to a pointer as a "float" */
	*((ILFloat *)(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT + 1)] .ptrValue)) =
		(ILFloat)ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT + 1));
}
break;

/**
 * <opcode name="dwrite" group="Pointer handling">
 *   <operation>Write <code>float64</code> to pointer</operation>
 *
 *   <format>dwrite</format>
 *
 *   <form name="dwrite" code="COP_DWRITE"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>native float</code> respectively.
 *   Truncate <i>value</i> to <code>float64</code> and write it
 *   to <i>pointer</i> in memory.</description>
 * </opcode>
 */
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

/**
 * <opcode name="pwrite" group="Pointer handling">
 *   <operation>Write <code>ptr</code> to pointer</operation>
 *
 *   <format>pwrite</format>
 *
 *   <form name="pwrite" code="COP_PWRITE"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack as
 *   type <code>ptr</code>.  Write <i>value</i> to <i>pointer</i>
 *   in memory.</description>
 *
 *   <notes>This instruction must not be confused with <i>iwrite</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of memory space on all
 *   platforms.</notes>
 * </opcode>
 */
case COP_PWRITE:
{
	/* Write a pointer quantity to a pointer */
	*((void **)(stacktop[-2].ptrValue)) = stacktop[-1].ptrValue;
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="mwrite" group="Pointer handling">
 *   <operation>Write multiple bytes to pointer</operation>
 *
 *   <format>mwrite<fsep/>N[1]</format>
 *   <format>wide<fsep/>mwrite<fsep/>N[4]</format>
 *
 *   <form name="mwrite" code="COP_MWRITE"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>uint8[N]</code> (i.e. <i>N</i>
 *   bytes of data, aligned on a stack word) respectively.  Write
 *   <i>value</i> to <i>pointer</i> in memory.</description>
 * </opcode>
 */
case COP_MWRITE:
{
	/* Write a multi-byte value to a pointer */
	stacktop -= (((((unsigned)(pc[1])) + sizeof(CVMWord) - 1)
						/ sizeof(CVMWord)) + 1);
	IL_MEMCPY(stacktop[0].ptrValue, &(stacktop[1]), (unsigned)(pc[1]));
	pc += 2;
}
break;

/**
 * <opcode name="bwrite_r" group="Pointer handling">
 *   <operation>Write <code>int8</code> to pointer with reversed
 *				arguments</operation>
 *
 *   <format>bwrite_r</format>
 *
 *   <form name="bwrite_r" code="COP_BWRITE_R"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack as
 *   the types <code>int32</code> and <code>ptr</code> respectively.
 *   Truncate <i>value</i> to 8 bits and write it to <i>pointer</i> in
 *   memory.</description>
 *
 *   <notes>This instruction can also be used to write values of
 *   type <code>uint8</code> to memory.</notes>
 * </opcode>
 */
case COP_BWRITE_R:
{
	/* Write a byte quantity to a pointer with reversed args */
	*((ILInt8 *)(stacktop[-1].ptrValue)) = (ILInt8)(stacktop[-2].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="swrite_r" group="Pointer handling">
 *   <operation>Write <code>int16</code> to pointer with reversed
 *				arguments</operation>
 *
 *   <format>swrite_r</format>
 *
 *   <form name="swrite_r" code="COP_SWRITE_R"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack as
 *   the types <code>int32</code> and <code>ptr</code> respectively.
 *   Truncate <i>value</i> to 16 bits and write it to <i>pointer</i> in
 *   memory.</description>
 *
 *   <notes>This instruction can also be used to write values of
 *   type <code>uint16</code> to memory.</notes>
 * </opcode>
 */
case COP_SWRITE_R:
{
	/* Write a short quantity to a pointer with reversed args */
	*((ILInt16 *)(stacktop[-1].ptrValue)) = (ILInt16)(stacktop[-2].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="iwrite_r" group="Pointer handling">
 *   <operation>Write <code>int32</code> to pointer with reversed
 *				arguments</operation>
 *
 *   <format>iwrite_r</format>
 *
 *   <form name="iwrite_r" code="COP_IWRITE_R"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack as
 *   the types <code>int32</code> and <code>ptr</code> respectively.
 *   Write <i>value</i> to <i>pointer</i> in memory.</description>
 *
 *   <notes>This instruction can also be used to write values of
 *   type <code>uint32</code> to memory.</notes>
 * </opcode>
 */
case COP_IWRITE_R:
{
	/* Write an integer quantity to a pointer with reversed args */
	*((ILInt32 *)(stacktop[-1].ptrValue)) = (ILInt32)(stacktop[-2].intValue);
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="fwrite_r" group="Pointer handling">
 *   <operation>Write <code>float32</code> to pointer with reversed
 *				arguments</operation>
 *
 *   <format>fwrite_r</format>
 *
 *   <form name="fwrite_r" code="COP_FWRITE_R"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack as
 *   the types <code>native float</code> and <code>ptr</code> respectively.
 *   Truncate <i>value</i> to <code>float32</code> and write it
 *   to <i>pointer</i> in memory.</description>
 * </opcode>
 */
case COP_FWRITE_R:
{
	/* Write a "native float" value to a pointer as a "float",
	   with reversed arguments */
	*((ILFloat *)(stacktop[-1].ptrValue)) = (ILFloat)
		ReadFloat(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT + 1)]));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT + 1));
}
break;

/**
 * <opcode name="dwrite_r" group="Pointer handling">
 *   <operation>Write <code>float64</code> to pointer with reversed
 *				arguments</operation>
 *
 *   <format>dwrite_r</format>
 *
 *   <form name="dwrite_r" code="COP_DWRITE_R"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack as
 *   the types <code>native float</code> and <code>ptr</code> respectively.
 *   Truncate <i>value</i> to <code>float64</code> and write it
 *   to <i>pointer</i> in memory.</description>
 * </opcode>
 */
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

/**
 * <opcode name="pwrite_r" group="Pointer handling">
 *   <operation>Write <code>ptr</code> to pointer with reversed
 *				arguments</operation>
 *
 *   <format>pwrite_r</format>
 *
 *   <form name="pwrite_r" code="COP_PWRITE_R"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack as
 *   type <code>ptr</code>.  Write <i>value</i> to <i>pointer</i>
 *   in memory.</description>
 *
 *   <notes>This instruction must not be confused with <i>iwrite_r</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of memory space on all
 *   platforms.</notes>
 * </opcode>
 */
case COP_PWRITE_R:
{
	/* Write a pointer quantity to a pointer with reversed args */
	*((void **)(stacktop[-1].ptrValue)) = stacktop[-2].ptrValue;
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="mwrite_r" group="Pointer handling">
 *   <operation>Write multiple bytes to pointer with reversed
 *				arguments</operation>
 *
 *   <format>mwrite_r<fsep/>N[1]</format>
 *   <format>wide<fsep/>mwrite_r<fsep/>N[4]</format>
 *
 *   <form name="mwrite_r" code="COP_MWRITE_R"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack as
 *   the types <code>uint8[N]</code> (i.e. <i>N</i> bytes of data,
 *   aligned on a stack word) and <code>ptr</code> respectively.
 *   Write <i>value</i> to <i>pointer</i> in memory.</description>
 * </opcode>
 */
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

/**
 * <opcode name="padd_offset" group="Pointer handling">
 *   <operation>Add a literal byte offset to a pointer</operation>
 *
 *   <format>padd_offset<fsep/>N[1]</format>
 *
 *   <form name="padd_offset" code="COP_PADD_OFFSET"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., newpointer</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type <code>ptr</code>.
 *   Compute <i>newpointer = pointer + N</i> and push <i>newpointer</i>
 *   onto the stack.</description>
 * </opcode>
 */
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

/**
 * <opcode name="padd_offset_n" group="Pointer handling">
 *   <operation>Add a literal byte offset to a pointer that is
 *              several words down the stack</operation>
 *
 *   <format>padd_offset_n<fsep/>N[1]<fsep/>M[1]</format>
 *   <format>wide<fsep/>padd_offset_n<fsep/>N[4]<fsep/>M[4]</format>
 *
 *   <form name="padd_offset_n" code="COP_PADD_OFFSET_N"/>
 *
 *   <before>..., pointer, val1, ..., valN</before>
 *   <after>..., newpointer, val1, ..., valN</after>
 *
 *   <description>Read <i>pointer</i> from the <i>N</i> positions
 *   down the stack as type <code>ptr</code>.  Compute
 *   <i>newpointer = pointer + M</i> and replace <i>pointer</i>
 *   with <i>newpointer</i>.  A value of <i>N == 0</i> indicates the
 *   top-most stack word.</description>
 *
 *   <notes>The <i>padd_offset</i> instruction is more efficient if
 *   <i>N == 0</i> and <i>M &lt; 256</i>.</notes>
 * </opcode>
 */
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

/**
 * <opcode name="padd_i4" group="Pointer handling">
 *   <operation>Add <code>int32</code> value to pointer</operation>
 *
 *   <format>padd_i4</format>
 *
 *   <form name="padd_i4" code="COP_PADD_I4"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>..., newpointer</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack
 *   as the types <code>ptr</code> and <code>int32</code> respectively.
 *   Compute <i>newpointer = pointer + value</i> and push
 *   <i>newpointer</i> onto the stack.</description>
 *
 *   <notes>The <i>padd_offset</i> instruction is more efficient if
 *   <i>value</i> is constant and less than 256.</notes>
 * </opcode>
 */
case COP_PADD_I4:
{
	/* Add a 32-bit integer to a pointer */
	stacktop[-2].ptrValue = (void *)
		(((unsigned char *)(stacktop[-2].ptrValue)) +
		 ((ILInt32)(stacktop[-1].intValue)));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

/**
 * <opcode name="padd_i4_r" group="Pointer handling">
 *   <operation>Add <code>int32</code> value to pointer with
 *				reversed arguments</operation>
 *
 *   <format>padd_i4_r</format>
 *
 *   <form name="padd_i4_r" code="COP_PADD_I4_R"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>..., newpointer</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack
 *   as the types <code>int32</code> and <code>ptr</code> respectively.
 *   Compute <i>newpointer = pointer + value</i> and push
 *   <i>newpointer</i> onto the stack.</description>
 * </opcode>
 */
case COP_PADD_I4_R:
{
	/* Add a 32-bit integer to a pointer with reversed args */
	stacktop[-2].ptrValue = (void *)
		(((unsigned char *)(stacktop[-1].ptrValue)) +
		 ((ILInt32)(stacktop[-2].intValue)));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

/**
 * <opcode name="padd_i8" group="Pointer handling">
 *   <operation>Add <code>int64</code> value to pointer</operation>
 *
 *   <format>padd_i8</format>
 *
 *   <form name="padd_i8" code="COP_PADD_I8"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>..., newpointer</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack
 *   as the types <code>ptr</code> and <code>int64</code> respectively.
 *   Compute <i>newpointer = pointer + value</i> and push
 *   <i>newpointer</i> onto the stack.</description>
 *
 *   <notes>The <i>value</i> will be truncated to 32 bits on platforms
 *   with 32 bit pointers.</notes>
 * </opcode>
 */
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

/**
 * <opcode name="padd_i8_r" group="Pointer handling">
 *   <operation>Add <code>int64</code> value to pointer with
 *				reversed arguments</operation>
 *
 *   <format>padd_i8</format>
 *
 *   <form name="padd_i8" code="COP_PADD_I8"/>
 *
 *   <before>..., value, pointer</before>
 *   <after>..., newpointer</after>
 *
 *   <description>Pop <i>value</i> and <i>pointer</i> from the stack
 *   as the types <code>int64</code> and <code>ptr</code> respectively.
 *   Compute <i>newpointer = pointer + value</i> and push
 *   <i>newpointer</i> onto the stack.</description>
 *
 *   <notes>The <i>value</i> will be truncated to 32 bits on platforms
 *   with 32 bit pointers.</notes>
 * </opcode>
 */
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

/**
 * <opcode name="psub" group="Pointer handling">
 *   <operation>Subtract pointer values</operation>
 *
 *   <format>psub</format>
 *
 *   <form name="psub" code="COP_PSUB"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Pop <i>value1</i> and <i>value2</i> from the stack
 *   as type <code>ptr</code>.  Compute <i>result = value1 - value2</i>
 *   and push <i>result</i> onto the stack.  The type of <i>result</i>
 *   will be either <code>int32</code> or <code>int64</code>, depending
 *   upon the platform.</description>
 * </opcode>
 */
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

/**
 * <opcode name="psub_i4" group="Pointer handling">
 *   <operation>Subtract <code>int32</code> from pointer</operation>
 *
 *   <format>psub_i4</format>
 *
 *   <form name="psub_i4" code="COP_PSUB_I4"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>..., newpointer</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack
 *   as the types <code>ptr</code> and <code>int32</code> respectively.
 *   Compute <i>newpointer = pointer - value</i> and push
 *   <i>newpointer</i> onto the stack.</description>
 * </opcode>
 */
case COP_PSUB_I4:
{
	/* Subtract a 32-bit integer from a pointer */
	stacktop[-2].ptrValue = (void *)
		(((unsigned char *)(stacktop[-2].ptrValue)) -
		 ((ILInt32)(stacktop[-1].intValue)));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

/**
 * <opcode name="psub_i8" group="Pointer handling">
 *   <operation>Subtract <code>int64</code> from pointer</operation>
 *
 *   <format>psub_i8</format>
 *
 *   <form name="psub_i8" code="COP_PSUB_I8"/>
 *
 *   <before>..., pointer, value</before>
 *   <after>..., newpointer</after>
 *
 *   <description>Pop <i>pointer</i> and <i>value</i> from the stack
 *   as the types <code>ptr</code> and <code>int64</code> respectively.
 *   Compute <i>newpointer = pointer - value</i> and push
 *   <i>newpointer</i> onto the stack.</description>
 *
 *   <notes>The <i>value</i> will be truncated to 32 bits on platforms
 *   with 32 bit pointers.</notes>
 * </opcode>
 */
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

/**
 * <opcode name="cknull" group="Object handling">
 *   <operation>Check pointer for <code>null</code></operation>
 *
 *   <format>cknull</format>
 *
 *   <form name="cknull" code="COP_CKNULL"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., pointer</after>
 *
 *   <description>Throw <code>System.NullReferenceException</code> if
 *   the <code>ptr</code> value on the top of the stack is <code>null</code>.
 *   Otherwise do nothing.</description>
 * </opcode>
 */
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

/**
 * <opcode name="cknull_n" group="Object handling">
 *   <operation>Check pointer down the stack for <code>null</code></operation>
 *
 *   <format>cknull_n<fsep/>N[1]</format>
 *   <format>wide<fsep/>cknull_n<fsep/>N[4]</format>
 *
 *   <form name="cknull_n" code="COP_CKNULL_N"/>
 *
 *   <before>..., pointer, val1, ..., valN</before>
 *   <after>..., pointer, val1, ..., valN</after>
 *
 *   <description>Throw <code>System.NullReferenceException</code> if
 *   the <code>ptr</code> value <i>N</i> words down the stack is
 *   <code>null</code>.  Otherwise do nothing.  <i>N == 0</i> indicates
 *   the top of the stack.</description>
 *
 *   <notes>The <i>cknull</i> instruction is more efficient if
 *   <i>N == 0</i>.</notes>
 * </opcode>
 */
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

/**
 * <opcode name="bread_elem" group="Array handling">
 *   <operation>Read <code>int8</code> value from array</operation>
 *
 *   <format>bread_elem</format>
 *
 *   <form name="bread_elem" code="COP_BREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the 8 bit value from position <i>index</i>
 *   in <i>array</i>, sign-extend it to <code>int32</code>,
 *   and push it onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
SIMPLE_READ_ELEM(COP_BREAD_ELEM,  ILInt8);

/**
 * <opcode name="ubread_elem" group="Array handling">
 *   <operation>Read <code>uint8</code> value from array</operation>
 *
 *   <format>ubread_elem</format>
 *
 *   <form name="ubread_elem" code="COP_UBREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the 8 bit value from position <i>index</i>
 *   in <i>array</i>, zero-extend it to <code>int32</code>,
 *   and push it onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
SIMPLE_READ_ELEM(COP_UBREAD_ELEM, ILUInt8);

/**
 * <opcode name="sread_elem" group="Array handling">
 *   <operation>Read <code>int16</code> value from array</operation>
 *
 *   <format>sread_elem</format>
 *
 *   <form name="sread_elem" code="COP_SREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the 16 bit value from position <i>index</i>
 *   in <i>array</i>, sign-extend it to <code>int32</code>,
 *   and push it onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
SIMPLE_READ_ELEM(COP_SREAD_ELEM,  ILInt16);

/**
 * <opcode name="usread_elem" group="Array handling">
 *   <operation>Read <code>uint16</code> value from array</operation>
 *
 *   <format>usread_elem</format>
 *
 *   <form name="usread_elem" code="COP_USREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the 16 bit value from position <i>index</i>
 *   in <i>array</i>, zero-extend it to <code>int32</code>,
 *   and push it onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
SIMPLE_READ_ELEM(COP_USREAD_ELEM, ILUInt16);

/**
 * <opcode name="iread_elem" group="Array handling">
 *   <operation>Read <code>int32</code> value from array</operation>
 *
 *   <format>iread_elem</format>
 *
 *   <form name="iread_elem" code="COP_IREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the <code>int32</code> value from position
 *   <i>index</i> and push it onto the stack.</description>
 *
 *   <notes>This instruction can also be used to read values of
 *   type <code>uint32</code> from an array.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
SIMPLE_READ_ELEM(COP_IREAD_ELEM,  ILInt32);

/**
 * <opcode name="pread_elem" group="Array handling">
 *   <operation>Read <code>ptr</code> value from array</operation>
 *
 *   <format>pread_elem</format>
 *
 *   <form name="pread_elem" code="COP_PREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the <code>ptr</code> value from position
 *   <i>index</i> and push it onto the stack.</description>
 *
 *   <notes>This instruction must not be confused with <i>iread_elem</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of memory space on all
 *   platforms.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="bwrite_elem" group="Array handling">
 *   <operation>Write <code>int8</code> value to array</operation>
 *
 *   <format>bwrite_elem</format>
 *
 *   <form name="bwrite_elem" code="COP_BWRITE_ELEM"/>
 *
 *   <before>..., array, index, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>array</i>, <i>index</i>, and
 *   <i>value</i> from the stack as the types <code>ptr</code>,
 *   <code>int32</code>, and <code>int32</code> respectively.
 *   The <i>value</i> is truncated to 8 bits and written at
 *   position <i>index</i> in <i>array</i>.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
SIMPLE_WRITE_ELEM(COP_BWRITE_ELEM, ILInt8);

/**
 * <opcode name="swrite_elem" group="Array handling">
 *   <operation>Write <code>int16</code> value to array</operation>
 *
 *   <format>swrite_elem</format>
 *
 *   <form name="swrite_elem" code="COP_SWRITE_ELEM"/>
 *
 *   <before>..., array, index, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>array</i>, <i>index</i>, and
 *   <i>value</i> from the stack as the types <code>ptr</code>,
 *   <code>int32</code>, and <code>int32</code> respectively.
 *   The <i>value</i> is truncated to 16 bits and written at
 *   position <i>index</i> in <i>array</i>.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
SIMPLE_WRITE_ELEM(COP_SWRITE_ELEM, ILInt16);

/**
 * <opcode name="iwrite_elem" group="Array handling">
 *   <operation>Write <code>int32</code> value to array</operation>
 *
 *   <format>iwrite_elem</format>
 *
 *   <form name="iwrite_elem" code="COP_IWRITE_ELEM"/>
 *
 *   <before>..., array, index, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>array</i>, <i>index</i>, and
 *   <i>value</i> from the stack as the types <code>ptr</code>,
 *   <code>int32</code>, and <code>int32</code> respectively.
 *   The <i>value</i> is written at position <i>index</i>
 *   in <i>array</i>.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
SIMPLE_WRITE_ELEM(COP_IWRITE_ELEM, ILInt32);

/**
 * <opcode name="pwrite_elem" group="Array handling">
 *   <operation>Write <code>ptr</code> value to array</operation>
 *
 *   <format>pwrite_elem</format>
 *
 *   <form name="pwrite_elem" code="COP_PWRITE_ELEM"/>
 *
 *   <before>..., array, index, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>array</i>, <i>index</i>, and
 *   <i>value</i> from the stack as the types <code>ptr</code>,
 *   <code>int32</code>, and <code>ptr</code> respectively.
 *   The <i>value</i> is written at position <i>index</i>
 *   in <i>array</i>.</description>
 *
 *   <notes>This instruction must not be confused with <i>iwrite_elem</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of memory space on all
 *   platforms.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="ckarray_load_i4" group="Array handling">
 *   <operation>Check an array load with an <code>int32</code>
 *				index</operation>
 *
 *   <format>ckarray_load_i4</format>
 *
 *   <form name="ckarray_load_i4" code="COP_CKARRAY_LOAD_I4"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., pointer, index</after>
 *
 *   <description>Retrieve <i>array</i> and <i>index</i>
 *   from the stack (without popping them) as the types <code>ptr</code>
 *   and <code>int32</code> respectively.  Throw a
 *   <code>System.IndexOutOfRangeException</code> if <i>index</i> is
 *   out of range.  Otherwise set <i>pointer</i> to the address of
 *   the first element in the array.</description>
 *
 *   <notes>This instruction is used to assist in obtaining the address
 *   of an array element.  The program will normally follow this
 *   instruction with an <i>imul</i> operation to adjust the index
 *   for the size of the elements, followed by <i>padd_i4</i> to compute
 *   the final element address.  This instruction sequence can also be
 *   used in combination with <i>mread</i> to fetch odd-sized array
 *   elements by pointer.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="ckarray_load_i8" group="Array handling">
 *   <operation>Check an array load with an <code>int64</code>
 *				index</operation>
 *
 *   <format>ckarray_load_i8</format>
 *
 *   <form name="ckarray_load_i8" code="COP_CKARRAY_LOAD_I8"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., pointer, index</after>
 *
 *   <description>Retrieve <i>array</i> and <i>index</i>
 *   from the stack (without popping them) as the types <code>ptr</code>
 *   and <code>int64</code> respectively.  Throw a
 *   <code>System.IndexOutOfRangeException</code> if <i>index</i> is
 *   out of range.  Otherwise set <i>pointer</i> to the address of
 *   the first element in the array.</description>
 *
 *   <notes>This instruction is used to assist in obtaining the address
 *   of an array element.  The program will normally follow this
 *   instruction with an <i>lmul</i> operation to adjust the index
 *   for the size of the elements, followed by <i>padd_i8</i> to compute
 *   the final element address.  This instruction sequence can also be
 *   used in combination with <i>mread</i> to fetch odd-sized array
 *   elements by pointer.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="ckarray_store_i8" group="Array handling">
 *   <operation>Check an array store that uses an <code>int64</code>
 *				index</operation>
 *
 *   <format>ckarray_store_i8<fsep/>N[1]<fsep/>M[1]</format>
 *
 *   <form name="ckarray_store_i8" code="COP_CKARRAY_STORE_I8"/>
 *
 *   <before>..., array, index, value</before>
 *   <after>..., pointer, value</after>
 *
 *   <description>Pop <i>array</i>, <i>index</i>, and <i>value</i>
 *   from the stack as the types <code>ptr</code>, <code>int64</code>,
 *   and <code>word[N]</code> respectively (where <code>word</code>
 *   is the type of a stack word).  Throw a
 *   <code>System.IndexOutOfRangeException</code> if <i>index</i> is
 *   out of range.  Otherwise set <i>pointer</i> to the address of
 *   the <i>index</i>'th element in the array.  The size of each
 *   array element is <i>M</i> bytes.  The <i>pointer</i> and
 *   <i>value</i> are pushed onto the stack.</description>
 *
 *   <notes>This instruction is used to assist in storing an element
 *   to an array when the CIL index had the type I on a 64-bit platform.
 *   This instruction sequence is typically followed by a <i>*write</i>
 *   instruction to store <i>value</i> at <i>pointer</i>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="array_len" group="Array handling">
 *   <operation>Get the length of an array</operation>
 *
 *   <format>array_len</format>
 *
 *   <form name="array_len" code="COP_ARRAY_LEN"/>
 *
 *   <before>..., array</before>
 *   <after>..., length</after>
 *
 *   <description>Pop <i>array</i> from the stack as type <code>ptr</code>.
 *   Fetch the <i>length</i> of this array and push it onto the stack
 *   as type <code>native int</code>.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="bread_field" group="Object handling">
 *   <operation>Read <code>int8</code> field</operation>
 *
 *   <format>bread_field<fsep/>N[1]</format>
 *
 *   <form name="bread_field" code="COP_BREAD_FIELD"/>
 *
 *   <before>..., object</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>.
 *   Fetch the 8 bit value at <i>object + N</i>, sign-extend it to
 *   <code>int32</code>, and push it onto the stack.</description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull, ldc_i4 N, padd_i4, bread</i>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="ubread_field" group="Object handling">
 *   <operation>Read <code>uint8</code> field</operation>
 *
 *   <format>ubread_field<fsep/>N[1]</format>
 *
 *   <form name="ubread_field" code="COP_UBREAD_FIELD"/>
 *
 *   <before>..., object</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>.
 *   Fetch the 8 bit value at <i>object + N</i>, zero-extend it to
 *   <code>int32</code>, and push it onto the stack.</description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull, ldc_i4 N, padd_i4, ubread</i>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="sread_field" group="Object handling">
 *   <operation>Read <code>int16</code> field</operation>
 *
 *   <format>sread_field<fsep/>N[1]</format>
 *
 *   <form name="sread_field" code="COP_SREAD_FIELD"/>
 *
 *   <before>..., object</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>.
 *   Fetch the 16 bit value at <i>object + N</i>, sign-extend it to
 *   <code>int32</code>, and push it onto the stack.</description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull, ldc_i4 N, padd_i4, sread</i>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="usread_field" group="Object handling">
 *   <operation>Read <code>uint16</code> field</operation>
 *
 *   <format>usread_field<fsep/>N[1]</format>
 *
 *   <form name="usread_field" code="COP_USREAD_FIELD"/>
 *
 *   <before>..., object</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>.
 *   Fetch the 16 bit value at <i>object + N</i>, zero-extend it to
 *   <code>int32</code>, and push it onto the stack.</description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull, ldc_i4 N, padd_i4, usread</i>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="iread_field" group="Object handling">
 *   <operation>Read <code>int32</code> field</operation>
 *
 *   <format>iread_field<fsep/>N[1]</format>
 *
 *   <form name="iread_field" code="COP_IREAD_FIELD"/>
 *
 *   <before>..., object</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>.
 *   Fetch the <code>int32</code> value at <i>object + N</i>, and
 *   push it onto the stack.</description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull, ldc_i4 N, padd_i4, iread</i>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="pread_field" group="Object handling">
 *   <operation>Read <code>ptr</code> field</operation>
 *
 *   <format>pread_field<fsep/>N[1]</format>
 *
 *   <form name="pread_field" code="COP_PREAD_FIELD"/>
 *
 *   <before>..., object</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>.
 *   Fetch the <code>ptr</code> value at <i>object + N</i>, and
 *   push it onto the stack.</description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull, ldc_i4 N, padd_i4, pread</i>.<p/>
 *
 *   This instruction must not be confused with <i>iread_field</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of memory space on all
 *   platforms.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="bwrite_field" group="Object handling">
 *   <operation>Write <code>int8</code> field</operation>
 *
 *   <format>bwrite_field<fsep/>N[1]</format>
 *
 *   <form name="bwrite_field" code="COP_BWRITE_FIELD"/>
 *
 *   <before>..., object, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>object</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>int32</code> respectively.
 *   Truncate <i>value</i> to 8 bits and store it at <i>object + N</i>.
 *   </description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull_n 1, padd_offset_n 1 N, bwrite</i>.<p/>
 *
 *   This instruction can also be used to write values of type
 *   <code>uint8</code>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="swrite_field" group="Object handling">
 *   <operation>Write <code>int16</code> field</operation>
 *
 *   <format>pwrite_field<fsep/>N[1]</format>
 *
 *   <form name="swrite_field" code="COP_SWRITE_FIELD"/>
 *
 *   <before>..., object, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>object</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>int32</code> respectively.
 *   Truncate <i>value</i> to 16 bits and store it at <i>object + N</i>.
 *   </description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull_n 1, padd_offset_n 1 N, swrite</i>.<p/>
 *
 *   This instruction can also be used to write values of type
 *   <code>uint16</code>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="iwrite_field" group="Object handling">
 *   <operation>Write <code>int32</code> field</operation>
 *
 *   <format>iwrite_field<fsep/>N[1]</format>
 *
 *   <form name="iwrite_field" code="COP_IWRITE_FIELD"/>
 *
 *   <before>..., object, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>object</i> and <i>value</i> from the stack as
 *   the types <code>ptr</code> and <code>int32</code> respectively.
 *   Store <i>value</i> at <i>object + N</i>.</description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull_n 1, padd_offset_n 1 N, iwrite</i>.<p/>
 *
 *   This instruction can also be used to write values of type
 *   <code>uint32</code>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="pwrite_field" group="Object handling">
 *   <operation>Write <code>int32</code> field</operation>
 *
 *   <format>pwrite_field<fsep/>N[1]</format>
 *
 *   <form name="pwrite_field" code="COP_PWRITE_FIELD"/>
 *
 *   <before>..., object, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>object</i> and <i>value</i> from the stack
 *   as type <code>ptr</code>.  Store <i>value</i> at
 *   <i>object + N</i>.</description>
 *
 *   <notes>If the offset <i>N</i> is greater than 255, then use
 *   the sequence <i>cknull_n 1, padd_offset_n 1 N, pwrite</i>.<p/>
 *
 *   This instruction must not be confused with <i>iwrite_field</i>.
 *   Values of type <code>int32</code> and <code>ptr</code> do not
 *   necessarily occupy the same amount of memory space on all
 *   platforms.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>object</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="castclass" group="Object handling">
 *   <operation>Cast an object to a new class</operation>
 *
 *   <format>castclass<fsep/>class</format>
 *
 *   <form name="castclass" code="COP_CASTCLASS"/>
 *
 *   <before>..., object</before>
 *   <after>..., object</after>
 *
 *   <description>Inspect the <i>object</i> on the top of the stack.
 *   If <i>object</i> is <code>null</code> or its class inherits
 *   from <i>class</i>, execution continues with the next instruction.
 *   Otherwise, <code>System.InvalidCastException</code>
 *   is thrown.</description>
 *
 *   <notes>The <i>class</i> value is a native pointer to the class
 *   descriptor, which may 32 or 64 bits in size.<p/>
 *
 *   This instruction can only be used to test for normal inheritance.
 *   Use <i>castinterface</i> to cast objects to interfaces.</notes>
 *
 *   <exceptions>
 *     <exception name="System.InvalidCastException">Raised if
 *     <i>object</i>'s class does not inherit from <i>class</i>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="isinst" group="Object handling">
 *   <operation>Determine if an object is an instance of a class</operation>
 *
 *   <format>isinst<fsep/>class</format>
 *
 *   <form name="isinst" code="COP_ISINST"/>
 *
 *   <before>..., object</before>
 *   <after>..., newobject</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>.
 *   If <i>object</i> is <code>null</code> or its class inherits
 *   from <i>class</i>, then set <i>newobject</i> to <i>object</i>.
 *   Otherwise, set <i>newobject</i> to <code>null</code>.  Push
 *   <i>newobject</i> onto the stack.</description>
 *
 *   <notes>The <i>class</i> value is a native pointer to the class
 *   descriptor, which may 32 or 64 bits in size.<p/>
 *
 *   This instruction can only be used to test for normal inheritance.
 *   Use <i>isinterface</i> to test for interface membership.</notes>
 * </opcode>
 */
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

/**
 * <opcode name="castinterface" group="Object handling">
 *   <operation>Cast an object to a new interface</operation>
 *
 *   <format>castinterface<fsep/>interface</format>
 *
 *   <form name="castinterface" code="COP_CASTINTERFACE"/>
 *
 *   <before>..., object</before>
 *   <after>..., object</after>
 *
 *   <description>Inspect the <i>object</i> on the top of the stack.
 *   If <i>object</i> is <code>null</code> or its class implements
 *   <i>interface</i>, execution continues with the next instruction.
 *   Otherwise, <code>System.InvalidCastException</code>
 *   is thrown.</description>
 *
 *   <notes>The <i>interface</i> value is a native pointer to the class
 *   descriptor, which may 32 or 64 bits in size.<p/>
 *
 *   This instruction can only be used to test for interface inheritance.
 *   Use <i>castclass</i> to cast objects to parent classes.</notes>
 *
 *   <exceptions>
 *     <exception name="System.InvalidCastException">Raised if
 *     <i>object</i>'s class does not implement <i>interface</i>.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="isinterface" group="Object handling">
 *   <operation>Determine if an object is an instance of
 *              an interface</operation>
 *
 *   <format>isinterface<fsep/>interface</format>
 *
 *   <form name="isinterface" code="COP_ISINTERFACE"/>
 *
 *   <before>..., object</before>
 *   <after>..., newobject</after>
 *
 *   <description>Pop <i>object</i> from the stack as type <code>ptr</code>.
 *   If <i>object</i> is <code>null</code> or its class implements
 *   <i>interface</i>, then set <i>newobject</i> to <i>object</i>.
 *   Otherwise, set <i>newobject</i> to <code>null</code>.  Push
 *   <i>newobject</i> onto the stack.</description>
 *
 *   <notes>The <i>interface</i> value is a native pointer to the class
 *   descriptor, which may 32 or 64 bits in size.<p/>
 *
 *   This instruction can only be used to test for interface inheritance.
 *   Use <i>isinst</i> to test for normal inheritance.</notes>
 * </opcode>
 */
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

/**
 * <opcode name="get_static" group="Object handling">
 *   <operation>Get a pointer to the static data area of a class</operation>
 *
 *   <format>get_static<fsep/>class</format>
 *
 *   <form name="get_static" code="COP_GET_STATIC"/>
 *
 *   <before>...</before>
 *   <after>..., pointer</after>
 *
 *   <description>If <i>class</i> currently has a static data area,
 *   then push the data area's <i>pointer</i> onto the stack.
 *   Otherwise, allocate a new static data area for <i>class</i>
 *   and push it onto the stack.</description>
 *
 *   <notes>The <i>class</i> value is a native pointer to the class
 *   descriptor, which may 32 or 64 bits in size.<p/>
 *
 *   This instruction is used in combination with the
 *   <i>*read</i> and <i>*write</i> instructions to access static
 *   fields within a class.</notes>
 *
 *   <exceptions>
 *     <exception name="System.OutOfMemoryException">Raised if
 *     there is insufficient memory to allocate the static data
 *     area.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="new" group="Object handling">
 *   <operation>Allocate an instance of the current class</operation>
 *
 *   <format>new</format>
 *
 *   <form name="new" code="COP_NEW"/>
 *
 *   <before>...</before>
 *   <after>..., pointer</after>
 *
 *   <description>Allocates an instance of the class associated with
 *   the currently executing method, and pushes a <i>pointer</i> to
 *   this instance onto the stack.</description>
 *
 *   <notes>This instruction is used inside constructors to allocate
 *   memory for a new object.  The fields of the new object are
 *   initialized to zero.</notes>
 *
 *   <exceptions>
 *     <exception name="System.OutOfMemoryException">Raised if
 *     there is insufficient memory to allocate the object.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="new_value" group="Object handling">
 *   <operation>Allocate a new value type instance and push it down</operation>
 *
 *   <format>new_value<fsep/>N[1]<fsep/>M[1]</format>
 *   <format>wide<fsep/>new_value<fsep/>N[4]<fsep/>M[4]</format>
 *
 *   <form name="new_value" code="COP_NEW_VALUE"/>
 *
 *   <before>..., val1, ..., valN</before>
 *   <after>..., value, pointer, val1, ..., valN</after>
 *
 *   <description>Creates a new value type instance of <i>M</i>
 *   stack words in size and places it, and a <i>pointer</i> to it,
 *   <i>N</i> positions down the stack.  The fields of the new
 *   instance are initialized to zero.  <i>N == 0</i> to place
 *   the new instance at the top of the stack.</description>
 *
 *   <notes>This instruction is used inside value type constructors to
 *   insert the return value and the <code>this</code> pointer into the
 *   stack prior to running the main constructor code.</notes>
 * </opcode>
 */
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

/**
 * <opcode name="ldstr" group="Constant loading">
 *   <operation>Load a string constant onto the stack</operation>
 *
 *   <format>ldstr<fsep/>token[4]</format>
 *
 *   <form name="ldstr" code="COP_LDSTR"/>
 *
 *   <before>...</before>
 *   <after>..., string</after>
 *
 *   <description>Loads the string <i>token</i> from the current
 *   method's image and pushes the corresponding <i>string</i>
 *   object onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OutOfMemoryException">Raised if
 *     there is insufficient memory to allocate the string.</exception>
 *   </exceptions>
 * </opcode>
 */
case COP_LDSTR:
{
	/* Load a string constant onto the stack */
	COPY_STATE_TO_THREAD();
	stacktop[0].ptrValue = _ILStringInternFromImage
			(thread, ILProgramItem_Image(method), IL_READ_UINT32(pc + 1));
	RESTORE_STATE_FROM_THREAD();
	pc = thread->pc;
	MODIFY_PC_AND_STACK(5, 1);
}
break;

/**
 * <opcode name="ldtoken" group="Miscellaneous instructions">
 *   <operation>Load a token pointer onto the stack</operation>
 *
 *   <format>ldstr<fsep/>pointer</format>
 *
 *   <form name="ldtoken" code="COP_LDTOKEN"/>
 *
 *   <before>...</before>
 *   <after>..., pointer</after>
 *
 *   <description>Push <i>pointer</i> onto the stack as a
 *   <code>ptr</code> value.</description>
 *
 *   <notes>The <i>pointer</i> value is a native pointer to the token
 *   descriptor, which may 32 or 64 bits in size.<p/>
 *
 *   This instruction is used to load type, field, and method
 *   descriptors onto the stack as handle instances.</notes>
 * </opcode>
 */
case COP_LDTOKEN:
{
	/* Load a token handle onto the stack */
	stacktop[0].ptrValue = ReadPointer(pc + 1);
	MODIFY_PC_AND_STACK(1 + sizeof(void *), 1);
}
break;

/**
 * <opcode name="box" group="Object handling">
 *   <operation>Box a value type instance</operation>
 *
 *   <format>box<fsep/>N[1]<fsep/>class</format>
 *   <format>wide<fsep/>box<fsep/>N[4]<fsep/>class</format>
 *
 *   <form name="box" code="COP_BOX"/>
 *
 *   <before>..., value</before>
 *   <after>..., object</after>
 *
 *   <description>Pop the managed <i>value</i> from the stack.  The size
 *   of <i>value</i> is <i>N</i> stack words.  Allocate a new <i>object</i>
 *   instance of <i>class</i>, copy <i>value</i> into it, and then push
 *   <i>object</i> onto the stack.</description>
 *
 *   <notes>The <i>class</i> value is a native pointer to the class
 *   descriptor, which may 32 or 64 bits in size.<p/>
 *
 *   There is no <i>unbox</i> instruction.  The object layout is defined
 *   so that an object reference can also be used as a managed pointer
 *   to the contents of the object.</notes>
 *
 *   <exceptions>
 *     <exception name="System.OutOfMemoryException">Raised if
 *     there is insufficient memory to allocate the new object.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="box_ptr" group="Object handling">
 *   <operation>Box a value type instance at a pointer</operation>
 *
 *   <format>box_ptr<fsep/>N[1]<fsep/>class</format>
 *   <format>wide<fsep/>box_ptr<fsep/>N[4]<fsep/>class</format>
 *
 *   <form name="box_ptr" code="COP_BOX_PTR"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., object</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type <code>ptr</code>.
 *   Allocate a new <i>object</i> of <i>N</i> bytes in size as an instance
 *   of <i>class</i>.  Copy the <i>N</i> bytes of memory from <i>pointer</i>
 *   to the object.  Push <i>object</i> onto the stack.</description>
 *
 *   <notes>The <i>class</i> value is a native pointer to the class
 *   descriptor, which may 32 or 64 bits in size.</notes>
 *
 *   <exceptions>
 *     <exception name="System.OutOfMemoryException">Raised if
 *     there is insufficient memory to allocate the new object.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="memcpy" group="Miscellaneous instructions">
 *   <operation>Copy a fixed-size block of non-overlapping memory</operation>
 *
 *   <format>memcpy<fsep/>N[1]</format>
 *   <format>wide<fsep/>memcpy<fsep/>N[4]</format>
 *
 *   <form name="memcpy" code="COP_MEMCPY"/>
 *
 *   <before>..., dest, src</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>dest</i> and <i>src</i> from the stack as type
 *   <code>ptr</code>.  Copy <i>N</i> bytes of memory from <i>src</i>
 *   to <i>dest</i>.</description>
 *
 *   <notes>It is assumed that the source and destination regions do
 *   not overlap.<p/>
 *
 *   This instruction is typically used to copy value type instances
 *   from one location to another.<p/>
 *
 *   Use <i>memmove</i> for overlapping memory regions, and for regions
 *   that do not have a fixed size.</notes>
 * </opcode>
 */
case COP_MEMCPY:
{
	/* Copy a fixed-size memory block */
	IL_MEMCPY(stacktop[-2].ptrValue, stacktop[-1].ptrValue, (unsigned)(pc[1]));
	MODIFY_PC_AND_STACK(2, -2);
}
break;

/**
 * <opcode name="memmove" group="Miscellaneous instructions">
 *   <operation>Move a block of memory</operation>
 *
 *   <format>memmove</format>
 *
 *   <form name="memmove" code="COP_MEMMOVE"/>
 *
 *   <before>..., dest, src, length</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>dest</i>, <i>src</i>, and <i>length</i>
 *   from the stack as the types <code>ptr</code>, <code>ptr</code>,
 *   and <code>uint32</code> respectively.  Move <i>length</i> bytes
 *   of memory from <i>src</i> to <i>dest</i>.</description>
 *
 *   <notes>If the source and destination regions overlap, this instruction
 *   will guarantee to produce the correct result.</notes>
 * </opcode>
 */
case COP_MEMMOVE:
{
	/* Move a variable-size memory block */
	IL_MEMMOVE(stacktop[-3].ptrValue, stacktop[-2].ptrValue,
			   stacktop[-1].uintValue);
	MODIFY_PC_AND_STACK(1, -3);
}
break;

/**
 * <opcode name="memzero" group="Miscellaneous instructions">
 *   <operation>Fill a fixed-size block of memory with zeroes</operation>
 *
 *   <format>memzero<fsep/>N[1]</format>
 *   <format>wide<fsep/>memzero<fsep/>N[4]</format>
 *
 *   <form name="memzero" code="COP_MEMZERO"/>
 *
 *   <before>..., dest</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>dest</i> from the stack as type
 *   <code>ptr</code>.  Fill <i>N</i> bytes of memory at <i>dest</i>
 *   with zero bytes.</description>
 *
 *   <notes>This instruction is typically used to initialize value type
 *   instances.<p/>
 *
 *   Use <i>memset</i> if the region does not have a fixed size, or the
 *   fill value is something other than zero.</notes>
 * </opcode>
 */
case COP_MEMZERO:
{
	/* Fill a fixed-size memory block with zeroes */
	IL_MEMZERO(stacktop[-1].ptrValue, (unsigned)(pc[1]));
	MODIFY_PC_AND_STACK(2, -1);
}
break;

/**
 * <opcode name="memset" group="Miscellaneous instructions">
 *   <operation>Fill a block of memory with a byte value</operation>
 *
 *   <format>memset</format>
 *
 *   <form name="memset" code="COP_MEMSET"/>
 *
 *   <before>..., dest, value, length</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>dest</i>, <i>value</i>, and <i>length</i>
 *   from the stack as the types <code>ptr</code>, <code>int32</code>,
 *   and <code>uint32</code>.  Fill <i>length</i> bytes of memory at
 *   <i>dest</i> with <i>value</i>.</description>
 * </opcode>
 */
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

/**
 * <opcode name="lread_elem" group="Array handling">
 *   <operation>Read <code>int64</code> value from array</operation>
 *
 *   <format>prefix<fsep/>lread_elem</format>
 *
 *   <form name="lread_elem" code="COP_PREFIX_LREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the <code>int64</code> value from position
 *   <i>index</i> in <i>array</i>, and push it onto the stack.</description>
 *
 *   <notes>This instruction can also be used to read values of
 *   type <code>uint64</code>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
LARGE_READ_ELEM(COP_PREFIX_LREAD_ELEM, ILInt64, CVM_WORDS_PER_LONG,
				ReadLong, WriteLong);

/**
 * <opcode name="fread_elem" group="Array handling">
 *   <operation>Read <code>float32</code> value from array</operation>
 *
 *   <format>prefix<fsep/>fread_elem</format>
 *
 *   <form name="fread_elem" code="COP_PREFIX_FREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the <code>float32</code> value from position
 *   <i>index</i> in <i>array</i>, extend it to <code>native float</code>,
 *   and push it onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
LARGE_READ_ELEM(COP_PREFIX_FREAD_ELEM, ILFloat, CVM_WORDS_PER_NATIVE_FLOAT,
				(ILNativeFloat)*(ILFloat *), WriteFloat);

/**
 * <opcode name="dread_elem" group="Array handling">
 *   <operation>Read <code>float64</code> value from array</operation>
 *
 *   <format>prefix<fsep/>dread_elem</format>
 *
 *   <form name="dread_elem" code="COP_PREFIX_DREAD_ELEM"/>
 *
 *   <before>..., array, index</before>
 *   <after>..., value</after>
 *
 *   <description>Pop <i>array</i> and <i>index</i> from
 *   the stack as the types <code>ptr</code> and <code>int32</code>
 *   respectively.  Load the <code>float64</code> value from position
 *   <i>index</i> in <i>array</i>, extend it to <code>native float</code>,
 *   and push it onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="lwrite_elem" group="Array handling">
 *   <operation>Write <code>int64</code> value to array</operation>
 *
 *   <format>prefix<fsep/>lwrite_elem</format>
 *
 *   <form name="lwrite_elem" code="COP_PREFIX_LWRITE_ELEM"/>
 *
 *   <before>..., array, index, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>array</i>, <i>index</i>, and
 *   <i>value</i> from the stack as the types <code>ptr</code>,
 *   <code>int32</code>, and <code>int64</code> respectively.
 *   The <i>value</i> is written at position <i>index</i>
 *   in <i>array</i>.</description>
 *
 *   <notes>This instruction can also be used to write values of
 *   type <code>uint64</code>.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
LARGE_WRITE_ELEM(COP_PREFIX_LWRITE_ELEM, ILInt64, CVM_WORDS_PER_LONG,
				 ReadLong, WriteHardLong);

/**
 * <opcode name="fwrite_elem" group="Array handling">
 *   <operation>Write <code>float32</code> value to array</operation>
 *
 *   <format>prefix<fsep/>fwrite_elem</format>
 *
 *   <form name="fwrite_elem" code="COP_PREFIX_FWRITE_ELEM"/>
 *
 *   <before>..., array, index, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>array</i>, <i>index</i>, and
 *   <i>value</i> from the stack as the types <code>ptr</code>,
 *   <code>int32</code>, and <code>native float</code> respectively.
 *   The <i>value</i> is truncated to <code>float32</code> and written
 *   at position <i>index</i> in <i>array</i>.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
LARGE_WRITE_ELEM(COP_PREFIX_FWRITE_ELEM, ILFloat, CVM_WORDS_PER_NATIVE_FLOAT,
				 ReadFloat, WriteFloat32);

/**
 * <opcode name="dwrite_elem" group="Array handling">
 *   <operation>Write <code>float64</code> value to array</operation>
 *
 *   <format>prefix<fsep/>dwrite_elem</format>
 *
 *   <form name="dwrite_elem" code="COP_PREFIX_DWRITE_ELEM"/>
 *
 *   <before>..., array, index, value</before>
 *   <after>...</after>
 *
 *   <description>Pop <i>array</i>, <i>index</i>, and
 *   <i>value</i> from the stack as the types <code>ptr</code>,
 *   <code>int32</code>, and <code>native float</code> respectively.
 *   The <i>value</i> is truncated to <code>float64</code> and written
 *   at position <i>index</i> in <i>array</i>.</description>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>array</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>index</i> is not within the array's bounds.</exception>
 *   </exceptions>
 * </opcode>
 */
LARGE_WRITE_ELEM(COP_PREFIX_DWRITE_ELEM, ILDouble, CVM_WORDS_PER_NATIVE_FLOAT,
				 ReadFloat, WriteDouble);

/**
 * <opcode name="mkrefany" group="Object handling">
 *   <operation>Make a <code>typedref</code></operation>
 *
 *   <format>prefix<fsep/>mkrefany<fsep/>class</format>
 *
 *   <form name="mkrefany" code="COP_PREFIX_MKREFANY"/>
 *
 *   <before>..., pointer</before>
 *   <after>..., reference</after>
 *
 *   <description>Pop <i>pointer</i> from the stack as type
 *   <code>ptr</code>.  Make a <code>typedref</code> <i>reference</i> from
 *   <i>pointer</i> and <i>class</i>.  Push <i>reference</i> onto
 *   the stack.</description>
 *
 *   <notes>The <i>class</i> is a native pointer, which may be either
 *   32 or 64 bits in size, depending upon the platform.</notes>
 * </opcode>
 */
case COP_PREFIX_MKREFANY:
{
	/* Make a typedref from an address and class information block */
	((ILTypedRef *)(stacktop - 1))->value = stacktop[-1].ptrValue;
	((ILTypedRef *)(stacktop - 1))->type = ReadPointer(pc + 2);
	MODIFY_PC_AND_STACK(2 + sizeof(void *), CVM_WORDS_PER_TYPED_REF - 1);
}
break;

/**
 * <opcode name="refanyval" group="Object handling">
 *   <operation>Extract the value from a <code>typedref</code></operation>
 *
 *   <format>prefix<fsep/>refanyval<fsep/>class</format>
 *
 *   <form name="refanyval" code="COP_PREFIX_REFANYVAL"/>
 *
 *   <before>..., reference</before>
 *   <after>..., pointer</after>
 *
 *   <description>Pop <i>reference</i> from the stack as type
 *   <code>typedref</code>.  If <i>reference</i> refers to an instance
 *   of <i>class</i>, then extract the <i>pointer</i> from the
 *   <i>reference</i> and push it onto the stack.  Otherwise, throw
 *   <code>System.InvalidCastException</code>.</description>
 *
 *   <notes>The <i>class</i> is a native pointer, which may be either
 *   32 or 64 bits in size, depending upon the platform.</notes>
 *
 *   <exceptions>
 *     <exception name="System.InvalidCastException">Raised if
 *     <i>reference</i> does not refer to an instance of <i>class</i>.
 *     </exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="refanytype" group="Object handling">
 *   <operation>Extract the type from a <code>typedref</code></operation>
 *
 *   <format>prefix<fsep/>refanytype</format>
 *
 *   <form name="refanytype" code="COP_PREFIX_REFANYTYPE"/>
 *
 *   <before>..., reference</before>
 *   <after>..., type</after>
 *
 *   <description>Pop <i>reference</i> from the stack as type
 *   <code>typedref</code>.  Extract the <i>type</i> from
 *   <i>reference</i> and push it onto the stack.</description>
 * </opcode>
 */
case COP_PREFIX_REFANYTYPE:
{
	/* Extract the type part of a typedref */
	stacktop[-CVM_WORDS_PER_TYPED_REF].ptrValue =
		((ILTypedRef *)(stacktop - CVM_WORDS_PER_TYPED_REF))->type;
	MODIFY_PC_AND_STACK(2, -(CVM_WORDS_PER_TYPED_REF - 1));
}
break;

#endif /* IL_CVM_PREFIX */
