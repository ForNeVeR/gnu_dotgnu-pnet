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
 * Allocate enough space for "n" arguments.
 */
#define	ALLOC_ARGS(n)	\
			do { \
				ILUInt32 temp = (ILUInt32)(((n) + 7) & ~7); \
				if(temp > coder->maxArgs) \
				{ \
					ILUInt32 *newArgs = \
						(ILUInt32 *)ILRealloc(coder->argOffsets, \
											  temp * sizeof(ILUInt32)); \
					if(!newArgs) \
					{ \
						return 0; \
					} \
					coder->argOffsets = newArgs; \
					coder->maxArgs = temp; \
				} \
			} while (0)

/*
 * Allocate enough space for "n" locals.
 */
#define	ALLOC_LOCALS(n)	\
			do { \
				ILUInt32 temp = (ILUInt32)(((n) + 7) & ~7); \
				if(temp > coder->maxLocals) \
				{ \
					ILUInt32 *newLocals = \
						(ILUInt32 *)ILRealloc(coder->localOffsets, \
											  temp * sizeof(ILUInt32)); \
					if(!newLocals) \
					{ \
						return 0; \
					} \
					coder->localOffsets = newLocals; \
					coder->maxLocals = temp; \
				} \
			} while (0)

/*
 * Context structure for outputting entry point code.
 */
typedef struct _tagCVMEntryContext
{
	ILUInt32	thisAdjust;			/* Value to adjust for "this" words */
	ILUInt32	numArgWords;		/* Number of argument words */
	ILUInt32	numLocals;			/* Number of declared local variables */
	ILUInt32	numLocalWords;		/* Number of declared local words */
	ILUInt32	extraLocals;		/* Extra locals for special purposes */
	ILUInt32	extraOffset;		/* Allocation offset for extra locals */
	int			firstExtraIsTop;	/* First extra is top-most on stack */
	int			nativeArg;			/* Current native argument */
	ILUInt32	nativeArgWords;		/* Number of native arguments on stack */
	ILUInt32	returnOffset;		/* Offset of native return value */

} CVMEntryContext;

/*
 * Initialize an entry context.
 */
static void CVMEntryInit(CVMEntryContext *ctx)
{
	ctx->thisAdjust = 0;
	ctx->numArgWords = 0;
	ctx->numLocals = 0;
	ctx->numLocalWords = 0;
	ctx->extraLocals = 0;
	ctx->extraOffset = 0;
	ctx->firstExtraIsTop = 0;
	ctx->nativeArg = -1;
	ctx->nativeArgWords = 0;
	ctx->returnOffset = 0;
}

/*
 * Allocate slots for the method arguments in an entry context.
 * Returns zero if out of memory.
 */
static int CVMEntryAllocArgs(CVMEntryContext *ctx, ILCVMCoder *coder,
							 ILType *signature, int suppressThis)
{
	ILUInt32 num;
	ILUInt32 current;
	ILUInt32 total;
	ILUInt32 argIndex;
	ILUInt32 offset;
	ILType *type;

	/* Handle the "this" parameter as necessary */
	num = ILTypeNumParams(signature);
	if(ILType_HasThis(signature) && !suppressThis)
	{
		/* Allocate an argument slot for the "this" parameter */
		total = num + 1;
		ALLOC_ARGS(total);
		coder->argOffsets[0] = 0;
		argIndex = 1;
		offset = 1;
	}
	else
	{
		/* No "this" parameter, or it is explicitly provided in the
		   signature, or this is an allocating constructor which must
		   create its own "this" parameter during execution */
		total = num;
		ALLOC_ARGS(total);
		argIndex = 0;
		offset = 0;
	}
	ctx->thisAdjust = argIndex;

	/* Allocate slots for the regular arguments */
	for(current = 1; current <= num; ++current)
	{
		type = ILTypeGetEnumType(ILTypeGetParam(signature, current));
		coder->argOffsets[argIndex++] = offset;
		if(type == ILType_Float32 || type == ILType_Float64)
		{
			offset += CVM_WORDS_PER_NATIVE_FLOAT;
		}
		else
		{
			offset += GetTypeSize(type);
		}
	}

	/* Vararg methods have an extra parameter to pass the variable
	   arguments within an "Object[]" array */
	if((ILType_CallConv(signature) & IL_META_CALLCONV_MASK) ==
			IL_META_CALLCONV_VARARG)
	{
		ALLOC_ARGS(total + 1);
		coder->varargIndex = argIndex;
		coder->argOffsets[argIndex++] = offset;
		++offset;
	}

	/* Set the argument word count and return */
	ctx->numArgWords = offset;
	return 1;
}

/*
 * Allocate slots for the declared local variables in an entry context.
 * Returns zero if out of memory.
 */
static int CVMEntryAllocLocals(CVMEntryContext *ctx, ILCVMCoder *coder,
							   ILStandAloneSig *localVarSig)
{
	ILType *signature;
	ILType *type;
	ILUInt32 num;
	ILUInt32 current;
	ILUInt32 offset;

	if(localVarSig)
	{
		/* Determine the number of locals to allocate */
		signature = ILStandAloneSigGetType(localVarSig);
		ctx->numLocals = num = ILTypeNumLocals(signature);
		offset = ctx->numArgWords;

		/* Allocate the "localOffsets" array for the variables */
		ALLOC_LOCALS(num);

		/* Set the offsets for each of the local variables */
		for(current = 0; current < num; ++current)
		{
			type = ILTypeGetLocal(signature, current);
			coder->localOffsets[current] = offset;
			offset += GetTypeSize(type);
		}

		/* Record the number of words used for local variables */
		ctx->numLocalWords = offset - ctx->numArgWords;
	}

	return 1;
}

/*
 * Make space for an extra local.  If "type" is NULL, then allocate
 * a single word for a pointer argument.
 */
static void CVMEntryNeedExtraLocal(CVMEntryContext *ctx, ILType *type)
{
	ctx->extraLocals += (type ? GetTypeSize(type) : 1);
}

/*
 * Allocate an extra local variable and return its offset.
 * If "type" is NULL, then allocate a single word for a
 * pointer argument.
 */
static ILUInt32 CVMEntryAllocExtraLocal(CVMEntryContext *ctx, ILType *type)
{
	ILUInt32 size = (type ? GetTypeSize(type) : 1);
	ILUInt32 offset = ctx->extraOffset;
	ctx->firstExtraIsTop = (offset == 0 && ctx->extraLocals == size);
	ctx->extraOffset += size;
	return offset + ctx->numArgWords + ctx->numLocalWords;
}

/*
 * Generate the main entry point code for a method, based on
 * the information in an entry context.
 */
static int CVMEntryGen(CVMEntryContext *ctx, ILCVMCoder *coder,
					   ILMethod *method, ILType *signature,
					   int isConstructor, int newStart,
					   unsigned char **start)
{
	unsigned char *pushDown;
	ILUInt32 numLocals;
	ILUInt32 offset;
	ILUInt32 current;
	ILUInt32 num;
	ILType *type;

	/* Determine where the new method will start output within the buffer */
	if(newStart)
	{
		coder->start = ILCacheStartMethod(coder->cache, &(coder->codePosn),
										  1, method);
		if(!(coder->start))
		{
			return 0;
		}
	}
	else
	{
		coder->start = CVM_POSN();
	}

	/* If this method is a constructor, then we need to output
	   some extra entry point code just before the main entry */
	if(isConstructor)
	{
		CVM_OUT_NONE(COP_NEW);
		pushDown = CVM_POSN();
		CVM_OUT_PUSHDOWN();
		coder->start = CVM_POSN();
	}
	else
	{
		pushDown = 0;
	}

	/* Return the start of the method code to the caller */
	*start = coder->start;

	/* Set the number of arguments, which initialize's the method's frame */
	CVM_OUT_WIDE(COP_SET_NUM_ARGS, ctx->numArgWords);

	/* Is this a static constructor? */
	if(ILMethod_IsStaticConstructor(method))
	{
		/* Output a "cctor_once" instruction to ensure that this
		   method's body can only be executed once */
		CVM_OUT_NONE(COP_CCTOR_ONCE);
	}

	/* If this is a constructor, then back-patch the push down size,
	   which is one less than the number of argument words */
	if(isConstructor)
	{
		CVM_BACKPATCH_PUSHDOWN(pushDown, ctx->numArgWords - 1);
	}

	/* Fix up any arguments that are larger than their true types */
	num = ILTypeNumParams(signature);
	for(current = 0; current < num; ++current)
	{
		type = ILTypeGetEnumType(ILTypeGetParam(signature, current + 1));
		offset = coder->argOffsets[current + ctx->thisAdjust];
		switch((unsigned long)type)
		{
			/* Not needed for little-endian machines */
		#ifndef CVM_LITTLE_ENDIAN
			case (unsigned long)ILType_Boolean:
			case (unsigned long)ILType_Int8:
			case (unsigned long)ILType_UInt8:
			{
				CVM_OUT_WIDE(COP_BFIXUP, offset);
			}
			break;

			case (unsigned long)ILType_Int16:
			case (unsigned long)ILType_UInt16:
			case (unsigned long)ILType_Char:
			{
				CVM_OUT_WIDE(COP_SFIXUP, offset);
			}
			break;
		#endif

			case (unsigned long)ILType_Float32:
			{
				CVM_OUT_WIDE(COP_FFIXUP, offset);
			}
			break;

			case (unsigned long)ILType_Float64:
			{
				CVM_OUT_WIDE(COP_DFIXUP, offset);
			}
			break;

			default: break;
		}
	}

	/* Output the stack height check instruction, which is fixed
	   up at the end of the method with the maximum height */
	coder->stackCheck = CVM_POSN();
	CVM_OUT_CKHEIGHT();

	/* Output CVM code to allocate space for the local variables */
	numLocals = ctx->numLocalWords + ctx->extraLocals;
	if(numLocals == 1)
	{
		CVM_OUT_NONE(COP_MK_LOCAL_1);
	}
	else if(numLocals == 2)
	{
		CVM_OUT_NONE(COP_MK_LOCAL_2);
	}
	else if(numLocals == 3)
	{
		CVM_OUT_NONE(COP_MK_LOCAL_3);
	}
	else if(numLocals != 0)
	{
		CVM_OUT_WIDE(COP_MK_LOCAL_N, numLocals);
	}

	/* Set the current stack height */
	coder->height = (long)(ctx->numArgWords + numLocals);
	coder->minHeight = coder->height;
	coder->maxHeight = coder->height;

	/* Done */
	return 1;
}

/*
 * Push the "lead-in" values for a native method call.  These
 * include the thread argument and the return value pointer.
 * If "coder" is NULL, then calculate the number of extra locals
 * that are required.
 */
static void CVMEntryPushLeadIn(CVMEntryContext *ctx, ILCVMCoder *coder,
							   ILType *signature, int useRawCalls,
							   int isInternal, int isConstructor)
{
	ILType *returnType;
	ILUInt32 offset;

	/* We need the thread argument for "internalcall" methods */
	if(isInternal && coder)
	{
		if(useRawCalls)
		{
			CVM_OUT_NONE(COP_PUSH_THREAD_RAW);
			CVM_ADJUST(1);
			++(ctx->nativeArgWords);
		}
		else
		{
			CVM_OUT_NONE(COP_PUSH_THREAD);
			++(ctx->nativeArg);
		}
	}

	/* Process value type return values */
	returnType = ILTypeGetEnumType(ILTypeGetReturn(signature));
	if(ILType_IsValueType(returnType) && isInternal)
	{
		/* Value types are passed by reference to "internalcall" methods */
		if(coder)
		{
			/* Allocate space for the return value */
			offset = CVMEntryAllocExtraLocal(ctx, returnType);
			ctx->returnOffset = offset;

			/* Pass the pointer to the native method */
			if(useRawCalls)
			{
				CVM_OUT_WIDE(COP_WADDR, offset);
				CVM_ADJUST(1);
				++(ctx->nativeArgWords);
			}
			else
			{
				CVM_OUT_WIDE(COP_WADDR, offset);
				CVM_ADJUST(1);
				offset = CVMEntryAllocExtraLocal(ctx, 0);
				if(offset < 4)
				{
					CVM_OUT_NONE(COP_PSTORE_0 + offset);
				}
				else
				{
					CVM_OUT_WIDE(COP_PSTORE, offset);
				}
				CVM_ADJUST(-1);
				CVM_OUT_WIDE(COP_WADDR_NATIVE_0 + ctx->nativeArg, offset);
				++(ctx->nativeArg);
			}
		}
		else
		{
			/* Inform the context that we need space for the return value */
			CVMEntryNeedExtraLocal(ctx, returnType);
			if(!useRawCalls)
			{
				/* We also need space for the pointer to the value */
				CVMEntryNeedExtraLocal(ctx, 0);
			}
		}
	}
	else if(returnType != ILType_Void)
	{
		/* Allocate space for the return value */
		if(coder)
		{
			ctx->returnOffset = CVMEntryAllocExtraLocal(ctx, returnType);
		}
		else
		{
			CVMEntryNeedExtraLocal(ctx, returnType);
		}
	}
	else if(isConstructor)
	{
		/* We need space to return the object pointer */
		if(coder)
		{
			ctx->returnOffset = CVMEntryAllocExtraLocal(ctx, 0);
		}
		else
		{
			CVMEntryNeedExtraLocal(ctx, 0);
		}
	}

	/* Push the "this" value onto the stack */
	if(ILType_HasThis(signature) && !isConstructor && coder)
	{
		if(isInternal)
		{
			/* Check the "this" value for NULL first, so that the
			   engine "internalcalls" don't have to */
			CVM_OUT_NONE(COP_PLOAD_0);
			CVM_ADJUST(1);
			CVM_OUT_NONE(COP_CKNULL);
			if(useRawCalls)
			{
				++(ctx->nativeArgWords);
			}
			else
			{
				CVM_OUT_NONE(COP_POP);
				CVM_ADJUST(-1);
				CVM_OUT_WIDE(COP_WADDR_NATIVE_0 + ctx->nativeArg, 0);
				++(ctx->nativeArg);
			}
		}
		else if(useRawCalls)
		{
			/* Push the "this" value onto the stack directly */
			CVM_OUT_NONE(COP_PLOAD_0);
			CVM_ADJUST(1);
			++(ctx->nativeArgWords);
		}
		else
		{
			/* Push the address of the "this" value onto the native
			   argument stack */
			CVM_OUT_WIDE(COP_WADDR_NATIVE_0 + ctx->nativeArg, 0);
			++(ctx->nativeArg);
		}
	}
}

/*
 * Push native arguments onto the stack.  If "coder" is NULL
 * then determine the number of extra locals that we need only.
 */
static void CVMEntryPushNativeArgs(CVMEntryContext *ctx, ILCVMCoder *coder,
								   ILMethod *method, ILType *signature,
								   int useRawCalls, int isInternal)
{
	unsigned long num;
	unsigned long param;
	ILType *paramType;
	unsigned long thisAdjust;
	ILUInt32 offset;
	ILUInt32 size;
	ILPInvoke *pinv;
	ILUInt32 pinvAttrs;

	/* Get the PInvoke information */
	if(!isInternal)
	{
		pinv = ILPInvokeFind(method);
		if(pinv)
		{
			pinvAttrs = ILPInvoke_Attrs(pinv);
		}
		else
		{
			pinvAttrs = 0;
		}
	}
	else
	{
		pinvAttrs = 0;
	}

	/* Process each of the arguments in turn */
	num = ILTypeNumParams(signature);
	thisAdjust = 1 - ctx->thisAdjust;
	for(param = 1; param <= num; ++param)
	{
		/* Get the parameter and then check if we are in codegen mode or not */
		paramType = ILTypeGetEnumType(ILTypeGetParam(signature, param));
		if(!coder)
		{
			/* We need extra locals for value type parameters when
			   we use the non-raw version of function invocation */
			if(!useRawCalls && isInternal && ILType_IsValueType(paramType))
			{
				CVMEntryNeedExtraLocal(ctx, 0);
			}
			continue;
		}

		/* Perform PInvoke expansions on the parameter */
		if(!isInternal && ILTypeIsStringClass(paramType))
		{
			/* String value parameter: convert into a "const char *" */
			offset = coder->argOffsets[param - thisAdjust];
			if(offset < 4)
			{
				CVM_OUT_NONE(COP_PLOAD_0 + offset);
			}
			else
			{
				CVM_OUT_WIDE(COP_PLOAD, offset);
			}
			CVM_ADJUST(1);
			switch(pinvAttrs & IL_META_PINVOKE_CHAR_SET_MASK)
			{
				case IL_META_PINVOKE_CHAR_SET_NOT_SPEC:
				{
					CVMP_OUT_NONE(COP_PREFIX_STR2UTF8);
				}
				break;

				case IL_META_PINVOKE_CHAR_SET_ANSI:
				case IL_META_PINVOKE_CHAR_SET_AUTO:
				{
					CVMP_OUT_NONE(COP_PREFIX_STR2ANSI);
				}
				break;

				case IL_META_PINVOKE_CHAR_SET_UNICODE:
				{
					/* TODO: convert the string into "const wchar_t *" */
				}
				break;
			}
			if(offset < 4)
			{
				CVM_OUT_NONE(COP_PSTORE_0 + offset);
			}
			else
			{
				CVM_OUT_WIDE(COP_PSTORE, offset);
			}
			CVM_ADJUST(-1);
		}

		/* Generate the code for the parameter */
		if(useRawCalls)
		{
			/* Push the parameter value onto the stack directly */
			offset = coder->argOffsets[param - thisAdjust];
			if(ILType_IsPrimitive(paramType))
			{
				/* Push a primitive value in its best "raw" form */
				switch(ILType_ToElement(paramType))
				{
					case IL_META_ELEMTYPE_BOOLEAN:
					case IL_META_ELEMTYPE_I1:
					{
						if(offset < 256)
						{
							CVM_OUT_BYTE(COP_BLOAD, offset);
						}
						else
						{
							CVM_OUT_WIDE(COP_WADDR, offset);
							CVM_OUT_NONE(COP_BREAD);
						}
						CVM_ADJUST(1);
						++(ctx->nativeArgWords);
					}
					break;

					case IL_META_ELEMTYPE_U1:
					{
						CVM_OUT_WIDE(COP_WADDR, offset);
						CVM_OUT_NONE(COP_UBREAD);
						CVM_ADJUST(1);
						++(ctx->nativeArgWords);
					}
					break;

					case IL_META_ELEMTYPE_I2:
					{
						CVM_OUT_WIDE(COP_WADDR, offset);
						CVM_OUT_NONE(COP_SREAD);
						CVM_ADJUST(1);
						++(ctx->nativeArgWords);
					}
					break;

					case IL_META_ELEMTYPE_U2:
					case IL_META_ELEMTYPE_CHAR:
					{
						CVM_OUT_WIDE(COP_WADDR, offset);
						CVM_OUT_NONE(COP_USREAD);
						CVM_ADJUST(1);
						++(ctx->nativeArgWords);
					}
					break;

					case IL_META_ELEMTYPE_I4:
					case IL_META_ELEMTYPE_U4:
				#ifdef IL_NATIVE_INT32
					case IL_META_ELEMTYPE_I:
					case IL_META_ELEMTYPE_U:
				#endif
					{
						if(offset < 4)
						{
							CVM_OUT_NONE(COP_ILOAD_0 + offset);
						}
						else
						{
							CVM_OUT_WIDE(COP_ILOAD, offset);
						}
						CVM_ADJUST(1);
						++(ctx->nativeArgWords);
					}
					break;

					default:
					{
						size = GetTypeSize(paramType);
						CVM_OUT_DWIDE(COP_MLOAD, offset, size);
						CVM_ADJUST(size);
						ctx->nativeArgWords += size;
					}
					break;
				}
			}
			else if(ILType_IsValueType(paramType))
			{
				/* Push a pointer to the value type instance.  Note: this
				   will never be used in the method is PInvoke, because
				   PInvoke value types never use the raw call form */
				CVM_OUT_WIDE(COP_WADDR, offset);
				CVM_ADJUST(1);
				++(ctx->nativeArgWords);
			}
			else
			{
				/* Assume that everything else is a pointer */
				if(offset < 4)
				{
					CVM_OUT_NONE(COP_PLOAD_0 + offset);
				}
				else
				{
					CVM_OUT_WIDE(COP_PLOAD, offset);
				}
				CVM_ADJUST(1);
				++(ctx->nativeArgWords);
			}
		}
		else if(ctx->nativeArg < CVM_MAX_NATIVE_ARGS)
		{
			if(isInternal && ILType_IsValueType(paramType))
			{
				/* Push a pointer to the value type onto the native stack */
				CVM_OUT_WIDE(COP_WADDR, coder->argOffsets[param - thisAdjust]);
				CVM_ADJUST(1);
				offset = CVMEntryAllocExtraLocal(ctx, 0);
				if(offset < 4)
				{
					CVM_OUT_NONE(COP_PSTORE_0 + offset);
				}
				else
				{
					CVM_OUT_WIDE(COP_PSTORE, offset);
				}
				CVM_ADJUST(-1);
				CVM_OUT_WIDE(COP_WADDR_NATIVE_0 + ctx->nativeArg, offset);
			}
			else
			{
				/* Push a pointer to the actual arg onto the native stack */
				CVM_OUT_WIDE(COP_WADDR_NATIVE_0 + ctx->nativeArg,
							 coder->argOffsets[param - thisAdjust]);
			}
			++(ctx->nativeArg);
		}
	}
}

/*
 * Call a native method.
 */
static void CVMEntryCallNative(CVMEntryContext *ctx, ILCVMCoder *coder,
							   ILType *signature, void *fn, void *cif,
							   int useRawCalls, int isInternal,
							   int isConstructor)
{
	ILType *returnType = ILTypeGetEnumType(ILTypeGetReturn(signature));
	int hasReturn;
	ILUInt32 size;

	/* Push the address of the argument array onto the stack */
	if(useRawCalls)
	{
		CVM_OUT_WIDE(COP_MADDR, ctx->nativeArgWords);
		CVM_ADJUST(1);
	}

	/* Push the address of the return value onto the stack */
	if(returnType != ILType_Void || isConstructor)
	{
		if(!isInternal || !ILType_IsValueType(returnType))
		{
			CVM_OUT_WIDE(COP_WADDR, ctx->returnOffset);
			CVM_ADJUST(1);
			hasReturn = 1;
		}
		else
		{
			/* Value type return for "internalcall" method */
			hasReturn = 0;
		}
	}
	else
	{
		hasReturn = 0;
	}

	/* Make the call */
	if(useRawCalls)
	{
		if(hasReturn)
		{
			CVM_OUT_PTR2(COP_CALL_NATIVE_RAW, fn, cif);
			CVM_ADJUST(-2);
		}
		else
		{
			CVM_OUT_PTR2(COP_CALL_NATIVE_VOID_RAW, fn, cif);
			CVM_ADJUST(-1);
		}
	}
	else
	{
		if(hasReturn)
		{
			CVM_OUT_PTR2(COP_CALL_NATIVE, fn, cif);
			CVM_ADJUST(-1);
		}
		else
		{
			CVM_OUT_PTR2(COP_CALL_NATIVE_VOID, fn, cif);
		}
	}

	/* Return the native value to the caller */
	if(useRawCalls && ctx->nativeArgWords != 0)
	{
		/* We still have the native argument values on the stack */
		ctx->firstExtraIsTop = 0;
	}
	if(returnType == ILType_Void)
	{
		/* Handle "void" or constructor returns */
		if(isConstructor)
		{
			if(ctx->returnOffset != 0 || !(ctx->firstExtraIsTop))
			{
				if(ctx->returnOffset < 4)
				{
					CVM_OUT_NONE(COP_PLOAD_0 + ctx->returnOffset);
				}
				else
				{
					CVM_OUT_WIDE(COP_PLOAD, ctx->returnOffset);
				}
				CVM_ADJUST(1);
			}
			CVM_OUT_NONE(COP_RETURN_1);
		}
		else
		{
			CVM_OUT_NONE(COP_RETURN);
		}
	}
	else if(ILType_IsPrimitive(returnType))
	{
		/* Handle primitive type returns */
		switch(ILType_ToElement(returnType))
		{
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
		#endif
			{
				/* Return values of "int" or less in size are returned
				   to us as native integers by libffi */
			#ifdef IL_NATIVE_INT32
				if(ctx->returnOffset != 0 || !(ctx->firstExtraIsTop))
				{
					if(ctx->returnOffset < 4)
					{
						CVM_OUT_NONE(COP_ILOAD_0 + ctx->returnOffset);
					}
					else
					{
						CVM_OUT_WIDE(COP_ILOAD, ctx->returnOffset);
					}
					CVM_ADJUST(1);
				}
				CVM_OUT_NONE(COP_RETURN_1);
			#else
				CVM_OUT_DWIDE(COP_MLOAD, ctx->returnOffset, CVM_WORDS_PER_LONG);
				CVM_ADJUST(CVM_WORDS_PER_LONG);
				CVM_OUT_NONE(CVM_L2I);
				CVM_OUT_NONE(COP_RETURN_1);
				CVM_ADJUST(-CVM_WORDS_PER_LONG);
			#endif
			}
			break;

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
		#ifdef IL_NATIVE_INT64
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
		#endif
			{
				if(ctx->returnOffset != 0 || !(ctx->firstExtraIsTop))
				{
					CVM_OUT_DWIDE(COP_MLOAD, ctx->returnOffset,
								  CVM_WORDS_PER_LONG);
					CVM_ADJUST(CVM_WORDS_PER_LONG);
				}
				CVM_OUT_RETURN(CVM_WORDS_PER_LONG);
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
				CVM_OUT_WIDE(COP_WADDR, ctx->returnOffset);
				CVM_ADJUST(1);
				CVM_OUT_NONE(COP_FREAD);
				CVM_ADJUST(CVM_WORDS_PER_NATIVE_FLOAT - 1);
				CVM_OUT_RETURN(CVM_WORDS_PER_NATIVE_FLOAT);
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				CVM_OUT_WIDE(COP_WADDR, ctx->returnOffset);
				CVM_ADJUST(1);
				CVM_OUT_NONE(COP_DREAD);
				CVM_ADJUST(CVM_WORDS_PER_NATIVE_FLOAT - 1);
				CVM_OUT_RETURN(CVM_WORDS_PER_NATIVE_FLOAT);
			}
			break;

			case IL_META_ELEMTYPE_R:
			{
				if(ctx->returnOffset != 0 || !(ctx->firstExtraIsTop))
				{
					CVM_OUT_DWIDE(COP_MLOAD, ctx->returnOffset,
								  CVM_WORDS_PER_NATIVE_FLOAT);
					CVM_ADJUST(CVM_WORDS_PER_NATIVE_FLOAT);
				}
				CVM_OUT_RETURN(CVM_WORDS_PER_NATIVE_FLOAT);
			}
			break;

			default:
			{
				size = GetTypeSize(returnType);
				if(ctx->returnOffset != 0 || !(ctx->firstExtraIsTop))
				{
					CVM_OUT_DWIDE(COP_MLOAD, ctx->returnOffset, size);
					CVM_ADJUST(size);
				}
				CVM_OUT_RETURN(size);
			}
			break;
		}
	}
	else if(ILType_IsValueType(returnType))
	{
		/* Load the value type onto the stack and return */
		size = GetTypeSize(returnType);
		if(ctx->returnOffset != 0 || !(ctx->firstExtraIsTop))
		{
			CVM_OUT_DWIDE(COP_MLOAD, ctx->returnOffset, size);
			CVM_ADJUST(size);
		}
		CVM_OUT_RETURN(size);
	}
	else
	{
		/* Assume that everything else is a pointer */
		if(ctx->returnOffset != 0 || !(ctx->firstExtraIsTop))
		{
			if(ctx->returnOffset < 4)
			{
				CVM_OUT_NONE(COP_PLOAD_0 + ctx->returnOffset);
			}
			else
			{
				CVM_OUT_WIDE(COP_PLOAD, ctx->returnOffset);
			}
			CVM_ADJUST(1);
		}
		CVM_OUT_NONE(COP_RETURN_1);
	}
}

/*
 * Set up a CVM coder instance to process a specific method.
 */
static int CVMCoder_Setup(ILCoder *_coder, unsigned char **start,
						  ILMethod *method, ILMethodCode *code)
{
	ILCVMCoder *coder = ((ILCVMCoder *)_coder);
	ILType *signature = ILMethod_Signature(method);
	CVMEntryContext ctx;

	/* Initialize the entry point context */
	CVMEntryInit(&ctx);

	/* Allocate the arguments */
	if(!CVMEntryAllocArgs(&ctx, coder, signature, 0))
	{
		return 0;
	}

	/* Allocate the local variables */
	if(!CVMEntryAllocLocals(&ctx, coder, code->localVarSig))
	{
		return 0;
	}

	/* Generate the entry point code */
	return CVMEntryGen(&ctx, coder, method, signature,
					   ILMethod_IsConstructor(method), 1, start);
}

/*
 * Determine if we can use raw libffi calls for a particular method.
 */
int _ILCVMCanUseRawCalls(ILMethod *method, int isInternal)
{
#if defined(HAVE_LIBFFI)
	#if !FFI_NO_RAW_API && FFI_NATIVE_RAW_API
		ILType *signature;
		unsigned long num;
		unsigned long param;
		ILType *type;

		/* Check the size of the stack word against libffi's expectations */
		if(sizeof(CVMWord) != sizeof(ffi_raw))
		{
			return 0;
		}

		/* The method's signature must not include ILType_TypedRef, or value
		   types because raw mode doesn't support structures very well */
		signature = ILMethod_Signature(method);
		type = ILTypeGetEnumType(ILTypeGetReturn(signature));
		if(type == ILType_TypedRef ||
		   (ILType_IsValueType(type) && !isInternal))
		{
			return 0;
		}
		num = ILTypeNumParams(signature);
		for(param = 1; param <= num; ++param)
		{
			type = ILTypeGetEnumType(ILTypeGetParam(signature, param));
			if(type == ILType_TypedRef ||
			   (ILType_IsValueType(type) && !isInternal))
			{
				return 0;
			}
		}

		/* If we get here, then we can use raw calls */
		return 1;
	#else
		return 0;
	#endif
#else
	return 0;
#endif
}

/*
 * Set up a CVM coder instance to process a specific external method.
 */
static int CVMCoder_SetupExtern(ILCoder *_coder, unsigned char **start,
								ILMethod *method, void *fn, void *cif,
								int isInternal)
{
	ILCVMCoder *coder = ((ILCVMCoder *)_coder);
	ILType *signature = ILMethod_Signature(method);
	CVMEntryContext ctx;
	int useRawCalls;

	/* Initialize the entry point context */
	CVMEntryInit(&ctx);

	/* Allocate the arguments */
	if(!CVMEntryAllocArgs(&ctx, coder, signature, 0))
	{
		return 0;
	}

	/* Determine if we can use raw libffi calls */
	useRawCalls = _ILCVMCanUseRawCalls(method, isInternal);

	/* Compute the number of extra locals that we need */
	CVMEntryPushLeadIn(&ctx, (ILCVMCoder *)0, signature, useRawCalls,
					   isInternal, 0);
	CVMEntryPushNativeArgs(&ctx, (ILCVMCoder *)0, method,
						   signature, useRawCalls, isInternal);

	/* Generate the entry point code */
	if(!CVMEntryGen(&ctx, coder, method, signature,
				    ILMethod_IsConstructor(method), 1, start))
	{
		return 0;
	}

	/* Output the body of the method */
	CVMEntryPushLeadIn(&ctx, coder, signature, useRawCalls, isInternal, 0);
	CVMEntryPushNativeArgs(&ctx, coder, method, signature,
						   useRawCalls, isInternal);
	CVMEntryCallNative(&ctx, coder, signature, fn, cif,
					   useRawCalls, isInternal, 0);
	return 1;
}

/*
 * Set up a CVM coder instance to process a specific external constructor.
 */
static int CVMCoder_SetupExternCtor(ILCoder *_coder, unsigned char **start,
								    ILMethod *method, void *fn, void *cif,
								    void *ctorfn, void *ctorcif,
									int isInternal)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	ILType *signature = ILMethod_Signature(method);
	ILInt32 dest;
	unsigned char *start2;
	CVMEntryContext ctx;
	int useRawCalls;

	/* Output the code for the main method entry point, which will
	   include the prefix code for allocating fixed-sized objects */
	if(!CVMCoder_SetupExtern(_coder, start, method, fn, cif, isInternal))
	{
		return 0;
	}

	/* If we don't have an allocating constructor, then we are done */
	if(!ctorfn)
	{
		return 1;
	}

	/* If the coder buffer is full, then don't go any further.
	   The "Finish" code will cause a cache flush and restart */
	if(ILCacheIsFull(coder->cache, &(coder->codePosn)))
	{
		return 1;
	}

	/* Back-patch the CKHEIGHT instruction in the main method */
	CVM_BACKPATCH_CKHEIGHT(coder->stackCheck, coder->maxHeight);

	/* Initialize the entry point context for the allocating constructor */
	CVMEntryInit(&ctx);

	/* Allocate the arguments */
	if(!CVMEntryAllocArgs(&ctx, coder, signature, 1))
	{
		return 0;
	}

	/* Determine if we can use raw libffi calls */
	useRawCalls = _ILCVMCanUseRawCalls(method, isInternal);

	/* Compute the number of extra locals that we need */
	CVMEntryPushLeadIn(&ctx, (ILCVMCoder *)0, signature, useRawCalls,
					   isInternal, 1);
	CVMEntryPushNativeArgs(&ctx, (ILCVMCoder *)0, method,
						   signature, useRawCalls, isInternal);

	/* Generate the entry point code */
	if(!CVMEntryGen(&ctx, coder, method, signature, 0, 0, &start2))
	{
		return 0;
	}

	/* Back-patch the prefix code to jump to the allocating constructor */
	dest = (ILInt32)(start2 - (*start - CVM_CTOR_OFFSET));
	CVM_OUT_JUMPOVER(*start - CVM_CTOR_OFFSET, dest);

	/* Output the body of the constructor */
	CVMEntryPushLeadIn(&ctx, coder, signature, useRawCalls, isInternal, 1);
	CVMEntryPushNativeArgs(&ctx, coder, method, signature,
						   useRawCalls, isInternal);
	CVMEntryCallNative(&ctx, coder, signature, ctorfn,
					   ctorcif, useRawCalls, isInternal, 1);
	return 1;
}

/*
 * Get the offset of an allocation constructor entry point
 * relative to the main method entry point.
 */
static int CVMCoder_CtorOffset(ILCoder *coder)
{
	return CVM_CTOR_OFFSET;
}

/*
 * Finish processing a method using a CVM coder instance.
 */
static int CVMCoder_Finish(ILCoder *_coder)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	int result;

	/* End method processing within the cache */
	result = _ILCacheEndMethod(&(coder->codePosn));

	/* Clear the label pool */
	ILMemPoolClear(&(coder->labelPool));
	coder->labelList = 0;
	if(coder->labelOutOfMemory)
	{
		/* We ran out of memory trying to allocate labels */
		return IL_CODER_END_TOO_BIG;
	}

	/* Back-patch the stack height check instruction */
	CVM_BACKPATCH_CKHEIGHT(coder->stackCheck, coder->maxHeight);

	/* Ready to go */
	return result;
}

#endif	/* IL_CVMC_CODE */
