/*
 * cvm_var.c - Opcodes for accessing variables.
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

/* No globals required */

#elif defined(IL_CVM_LOCALS)

ILUInt32 tempNum;

#elif defined(IL_CVM_MAIN)

#define	COP_LOAD_N(n)	\
case COP_ILOAD_##n: \
{ \
	stacktop[0].intValue = frame[(n)].intValue; \
	MODIFY_PC_AND_STACK(1, 1); \
} \
break; \
case COP_PLOAD_##n: \
{ \
	stacktop[0].ptrValue = frame[(n)].ptrValue; \
	MODIFY_PC_AND_STACK(1, 1); \
} \
break

/* Load integer or pointer values from the frame onto the stack */
COP_LOAD_N(0);
COP_LOAD_N(1);
COP_LOAD_N(2);
COP_LOAD_N(3);

case COP_ILOAD:
{
	/* Load an integer value from the frame */
	stacktop[0].intValue = frame[pc[1]].intValue;
	MODIFY_PC_AND_STACK(2, 1);
}
break;

case COP_PLOAD:
{
	/* Load a pointer value from the frame */
	stacktop[0].ptrValue = frame[pc[1]].ptrValue;
	MODIFY_PC_AND_STACK(2, 1);
}
break;

#define	COP_STORE_N(n)	\
case COP_ISTORE_##n: \
{ \
	frame[(n)].intValue = stacktop[-1].intValue; \
	MODIFY_PC_AND_STACK(1, -1); \
} \
break; \
case COP_PSTORE_##n: \
{ \
	frame[(n)].ptrValue = stacktop[-1].ptrValue; \
	MODIFY_PC_AND_STACK(1, -1); \
} \
break

/* Store integer or pointer values from the stack to the frame */
COP_STORE_N(0);
COP_STORE_N(1);
COP_STORE_N(2);
COP_STORE_N(3);

case COP_ISTORE:
{
	/* Store an integer value to the frame */
	frame[pc[1]].intValue = stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_PSTORE:
{
	/* Store a pointer value to the frame */
	frame[pc[1]].ptrValue = stacktop[-1].ptrValue;
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_MLOAD:
{
	/* Load a value consisting of multiple stack words */
	IL_MEMCPY(stacktop, &(frame[pc[1]]), sizeof(CVMWord) * (unsigned)(pc[2]));
	stacktop += (unsigned)(pc[2]);
	pc += 3;
}
break;

case COP_MSTORE:
{
	/* Store a value consisting of multiple stack words */
	stacktop -= (unsigned)(pc[2]);
	IL_MEMCPY(&(frame[pc[1]]), stacktop, sizeof(CVMWord) * (unsigned)(pc[2]));
	pc += 3;
}
break;

case COP_WADDR:
{
	/* Get the address of the value starting at a frame word */
	stacktop[0].ptrValue = (void *)(&(frame[pc[1]]));
	MODIFY_PC_AND_STACK(2, 1);
}
break;

case COP_MADDR:
{
	/* Get the address of a managed value N words down the stack */
	stacktop[0].ptrValue = (void *)(stacktop - ((int)(pc[1])));
	MODIFY_PC_AND_STACK(2, 1);
}
break;

case COP_BFIXUP:
{
	/* Perform a byte fixup on a frame offset that corresponds
	   to an argument.  Byte arguments are passed from the caller
	   as int parameters, but inside the method we need to access
	   them by an address that is always aligned on the start
	   of a stack word.  This operation fixes address mismatches
	   on CPU's that aren't little-endian */
	*((ILInt8 *)(&(frame[pc[1]]))) = (ILInt8)(frame[pc[1]].intValue);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_SFIXUP:
{
	/* Perform a short fixup on a frame offset that corresponds
	   to an argument.  See above for the rationale */
	*((ILInt16 *)(&(frame[pc[1]]))) = (ILInt16)(frame[pc[1]].intValue);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_FFIXUP:
{
	/* Perform a float fixup on a frame offset that corresponds
	   to an argument.  Float arguments are passed from the caller
	   as "native float" values, but inside the method we need to
	   access them by an address that is a pointer to "float" */
	*((ILFloat *)(&(frame[pc[1]]))) = (ILFloat)ReadFloat(&(frame[pc[1]]));
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_DFIXUP:
{
	/* Perform a double fixup on a frame offset that corresponds
	   to an argument.  Double arguments are passed from the caller
	   as "native float" values, but inside the method we need to
	   access them by an address that is a pointer to "double" */
	WriteDouble(&(frame[pc[1]]), (ILDouble)ReadFloat(&(frame[pc[1]])));
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_MK_LOCAL_1:
{
	/* Make a single local variable slot on the stack */
#ifdef CVM_WORDS_AND_PTRS_SAME_SIZE
	stacktop[0].ptrValue = 0;
#else
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord));
#endif
	MODIFY_PC_AND_STACK(1, 1);
}
break;

case COP_MK_LOCAL_2:
{
	/* Make two local variable slots on the stack */
#ifdef CVM_WORDS_AND_PTRS_SAME_SIZE
	stacktop[0].ptrValue = 0;
	stacktop[1].ptrValue = 0;
#else
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord) * 2);
#endif
	MODIFY_PC_AND_STACK(1, 2);
}
break;

case COP_MK_LOCAL_3:
{
	/* Make three local variable slots on the stack */
#ifdef CVM_WORDS_AND_PTRS_SAME_SIZE
	stacktop[0].ptrValue = 0;
	stacktop[1].ptrValue = 0;
	stacktop[2].ptrValue = 0;
#else
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord) * 3);
#endif
	MODIFY_PC_AND_STACK(1, 3);
}
break;

case COP_MK_LOCAL_N:
{
	/* Make an arbitrary number of local variable slots */
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord) * ((unsigned)(pc[1])));
	stacktop += ((unsigned)(pc[1]));
	pc += 2;
}
break;

#elif defined(IL_CVM_WIDE)

case COP_ILOAD:
{
	/* Wide version of "iload" */
	stacktop[0].intValue = frame[IL_READ_UINT32(pc + 2)].intValue;
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_PLOAD:
{
	/* Wide version of "pload" */
	stacktop[0].ptrValue = frame[IL_READ_UINT32(pc + 2)].ptrValue;
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_ISTORE:
{
	/* Wide version of "istore" */
	frame[IL_READ_UINT32(pc + 2)].intValue = stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(6, -1);
}
break;

case COP_PSTORE:
{
	/* Wide version of "pstore" */
	frame[IL_READ_UINT32(pc + 2)].ptrValue = stacktop[-1].ptrValue;
	MODIFY_PC_AND_STACK(6, -1);
}
break;

case COP_MLOAD:
{
	/* Wide version of "mload" */
	tempNum = IL_READ_UINT32(pc + 6);
	IL_MEMCPY(stacktop, &(frame[IL_READ_UINT32(pc + 2)]),
			  sizeof(CVMWord) * tempNum);
	stacktop += tempNum;
	pc += 10;
}
break;

case COP_MSTORE:
{
	/* Wide version of "mstore" */
	tempNum = IL_READ_UINT32(pc + 6);
	stacktop -= tempNum;
	IL_MEMCPY(&(frame[IL_READ_UINT32(pc + 2)]), stacktop,
			  sizeof(CVMWord) * tempNum);
	pc += 10;
}
break;

case COP_WADDR:
{
	/* Wide version of "waddr" */
	stacktop[0].ptrValue = (void *)(&(frame[IL_READ_UINT32(pc + 2)]));
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_MADDR:
{
	/* Wide version of "maddr" */
	stacktop[0].ptrValue = (void *)(stacktop - IL_READ_UINT32(pc + 2));
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_BFIXUP:
{
	/* Wide version of "bfixup" */
	tempNum = IL_READ_UINT32(pc + 2);
	*((ILInt8 *)(&(frame[tempNum]))) = (ILInt8)(frame[tempNum].intValue);
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_SFIXUP:
{
	/* Wide version of "sfixup" */
	tempNum = IL_READ_UINT32(pc + 2);
	*((ILInt16 *)(&(frame[tempNum]))) = (ILInt16)(frame[tempNum].intValue);
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_FFIXUP:
{
	/* Wide version of "ffixup" */
	tempNum = IL_READ_UINT32(pc + 2);
	*((ILFloat *)(&(frame[tempNum]))) = (ILFloat)ReadFloat(&(frame[tempNum]));
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_DFIXUP:
{
	/* Wide version of "dfixup" */
	tempNum = IL_READ_UINT32(pc + 2);
	WriteDouble(&(frame[tempNum]), (ILDouble)ReadFloat(&(frame[tempNum])));
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_MK_LOCAL_N:
{
	/* Wide version of "mk_local_n" */
	tempNum = IL_READ_UINT32(pc + 2);
	IL_MEMZERO(&(stacktop[0]), sizeof(CVMWord) * tempNum);
	MODIFY_PC_AND_STACK(6, tempNum);
}
break;

#endif /* IL_CVM_WIDE */
