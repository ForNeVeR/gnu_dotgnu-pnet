/*
 * cvmc_setup.c - Coder implementation for CVM method entry setup.
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

#ifdef IL_CVMC_CODE

/*
 * Set up a CVM coder instance to process a specific method.
 */
static int CVMCoder_Setup(ILCoder *_coder, ILMethod *method, ILMethodCode *code)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	ILUInt32 offset;
	ILUInt32 maxArgOffset;
	ILUInt32 num;
	ILUInt32 current;
	ILUInt32 argIndex;
	ILType *signature;
	ILType *type;

	/* Clear the label pool */
	ILMemPoolClear(&(coder->labelPool));
	coder->labelList = 0;
	coder->labelOutOfMemory = 0;

	/* Determine where the new method will start output within the buffer */
	if(coder->posn >= coder->len)
	{
		/* Flush the current contents of the method buffer */
		coder->posn = 0;
		coder->start = 0;
	}
	else
	{
		/* The new method starts here */
		coder->start = coder->posn;
	}

	/* Allocate offsets for the arguments */
	signature = ILMethod_Signature(method);
	num = (ILUInt32)(signature->num);
	if((signature->kind & (IL_META_CALLCONV_HASTHIS << 8)) != 0 &&
	   (signature->kind & (IL_META_CALLCONV_EXPLICITTHIS << 8)) == 0)
	{
		/* Allocate an argument slot for the "this" parameter */
		coder->argOffsets[0] = 0;
		argIndex = 1;
		offset = 1;
	}
	else
	{
		/* No "this" parameter, or it is explicitly provided in the signature */
		argIndex = 0;
		offset = 0;
	}
	for(current = 1; current <= num; ++num)
	{
		type = ILTypeGetEnumType(ILTypeGetParam(signature, current));
		coder->argOffsets[argIndex++] = offset;
		if(type == ILType_Float32 || type == ILType_Float64)
		{
			offset += CVM_WORDS_PER_NATIVE_FLOAT;
		}
		else
		{
			offset += GetTypeSize(coder->coder.thread, type);
		}
	}
	maxArgOffset = offset;

	/* Set the number of arguments, which initialize's the method's frame */
	CVM_WIDE(COP_SET_NUM_ARGS, offset);

	/* Fix up any arguments that are larger than their true types */
	if((signature->kind & (IL_META_CALLCONV_HASTHIS << 8)) != 0 &&
	   (signature->kind & (IL_META_CALLCONV_EXPLICITTHIS << 8)) == 0)
	{
		/* Allocate an argument slot for the "this" parameter */
		argIndex = 1;
	}
	else
	{
		/* No "this" parameter, or it is explicitly provided in the signature */
		argIndex = 0;
	}
	for(current = 1; current <= num; ++num)
	{
		type = ILTypeGetEnumType(ILTypeGetParam(signature, current));
		offset = coder->argOffsets[argIndex++];
		switch((unsigned long)type)
		{
		#ifndef CVM_X86		/* Not needed for 32-bit little-endian machines */
			case (unsigned long)ILType_Boolean:
			case (unsigned long)ILType_Int8:
			case (unsigned long)ILType_UInt8:
			{
				CVM_WIDE(COP_BFIXUP, offset);
			}
			break;

			case (unsigned long)ILType_Int16:
			case (unsigned long)ILType_UInt16:
			case (unsigned long)ILType_Char:
			{
				CVM_WIDE(COP_SFIXUP, offset);
			}
			break;
		#endif

			case (unsigned long)ILType_Float32:
			{
				CVM_WIDE(COP_FFIXUP, offset);
			}
			break;

			case (unsigned long)ILType_Float64:
			{
				CVM_WIDE(COP_DFIXUP, offset);
			}
			break;

			default: break;
		}
	}

	/* Allocate offsets for the local variables */
	offset = maxArgOffset;
	if(code->localVarSig)
	{
		signature = ILStandAloneSigGetType(code->localVarSig);
		num = ILTypeNumLocals(signature);
		for(current = 0; current < num; ++num)
		{
			type = ILTypeGetLocal(signature, current);
			coder->localOffsets[current] = offset;
			offset += GetTypeSize(coder->coder.thread, type);
		}
	}

	/* Output the stack height check instruction, which is fixed
	   up at the end of the method with the maximum height */
	coder->stackCheck = coder->posn;
	CVM_BYTE(COP_CKHEIGHT_N);
	CVM_BYTE(0);
	CVM_BYTE(0);
	CVM_BYTE(0);
	CVM_BYTE(0);

	/* Output CVM code to allocate space for the local variables */
	offset -= maxArgOffset;
	if(offset == 1)
	{
		CVM_BYTE(COP_MK_LOCAL_1);
	}
	else if(offset == 2)
	{
		CVM_BYTE(COP_MK_LOCAL_2);
	}
	else if(offset == 3)
	{
		CVM_BYTE(COP_MK_LOCAL_3);
	}
	else
	{
		CVM_WIDE(COP_MK_LOCAL_N, offset);
	}

	/* Set the current stack height */
	coder->height = (long)offset;
	coder->minHeight = (long)offset;
	coder->maxHeight = (long)offset;

	/* Done */
	return 1;
}

/*
 * Finish processing a method using a CVM coder instance.
 */
static int CVMCoder_Finish(ILCoder *_coder)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;

	/* Clear the label pool */
	ILMemPoolClear(&(coder->labelPool));
	coder->labelList =0;
	if(coder->labelOutOfMemory)
	{
		/* We ran out of memory trying to allocate labels */
		return 0;
	}

	if(coder->posn < coder->len)
	{
		/* Back-patch the stack height check instruction */
		if(coder->maxHeight <= 8)
		{
			coder->buffer[coder->stackCheck] = COP_CKHEIGHT;
		}
		else
		{
			coder->buffer[coder->stackCheck + 1] =
					(unsigned char)(coder->maxHeight);
			coder->buffer[coder->stackCheck + 2] =
					(unsigned char)(coder->maxHeight >> 8);
			coder->buffer[coder->stackCheck + 3] =
					(unsigned char)(coder->maxHeight >> 16);
			coder->buffer[coder->stackCheck + 4] =
					(unsigned char)(coder->maxHeight >> 24);
		}
		return 1;
	}
	else
	{
		/* The buffer overflowed while outputting the code */
		return 0;
	}
}

#endif	/* IL_CVMC_CODE */
