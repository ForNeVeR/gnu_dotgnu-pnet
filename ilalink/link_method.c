/*
 * link_method.c - Convert a method and copy it to the final image.
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

#include "linker.h"
#include "il_opcodes.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Determine if a type is a simple array of strings.
 */
static int IsStringArray(ILType *type)
{
	const char *namespace;
	if(type == 0 || !ILType_IsComplex(type) ||
	   type->kind != IL_TYPE_COMPLEX_ARRAY ||
	   type->un.array.size != 0 ||
	   type->un.array.lowBound != 0)
	{
		return 0;
	}
	type = type->un.array.elemType;
	if(!ILType_IsClass(type))
	{
		return 0;
	}
	if(strcmp(ILClass_Name(ILType_ToClass(type)), "String") != 0)
	{
		return 0;
	}
	namespace = ILClass_Namespace(ILType_ToClass(type));
	return (namespace != 0 && !strcmp(namespace, "System"));
}

/*
 * Convert a token that is embedded within a method.
 * Returns 1 if a fixup must be recorded, zero if no
 * fixup required, or -1 if out of memory or unresolved.
 */
static int ConvertToken(ILLinker *linker, ILMethod *method,
						ILMethod *newMethod, ILToken *token)
{
	ILProgramItem *item;
	ILClass *classInfo;
	ILMember *member;
	ILType *type;
	ILStandAloneSig *sig;
	ILTypeSpec *spec;

	if(!(*token))
	{
		return 0;
	}
	item = ILProgramItem_FromToken(ILProgramItem_Image(method), *token);
	if(!item)
	{
		/* Invalid token, so just zero it */
		*token = 0;
		return 0;
	}
	switch(*token & IL_META_TOKEN_MASK)
	{
		case IL_META_TOKEN_TYPE_DEF:
		case IL_META_TOKEN_TYPE_REF:
		{
			/* Convert a class reference */
			classInfo = _ILLinkerConvertClassRef(linker, (ILClass *)item);
			if(!classInfo)
			{
				return -1;
			}
			*token = ILClass_Token(classInfo);
			return 1;
		}
		/* Not reached */

		case IL_META_TOKEN_FIELD_DEF:
		case IL_META_TOKEN_METHOD_DEF:
		case IL_META_TOKEN_MEMBER_REF:
		{
			/* Convert a member reference */
			member = _ILLinkerConvertMemberRef(linker, (ILMember *)item);
			if(!member)
			{
				return -1;
			}
			*token = ILMember_Token(member);
			return 1;
		}
		break;

		case IL_META_TOKEN_STAND_ALONE_SIG:
		{
			/* Convert a stand-alone signature token */
			type = ILStandAloneSigGetType((ILStandAloneSig *)item);
			type = _ILLinkerConvertType(linker, type);
			if(!type)
			{
				return -1;
			}
			sig = ILStandAloneSigCreate(linker->image, 0, type);
			if(!sig)
			{
				_ILLinkerOutOfMemory(linker);
				return -1;
			}
			*token = ILStandAloneSig_Token(sig);
			return 1;
		}
		break;

		case IL_META_TOKEN_TYPE_SPEC:
		{
			/* Convert a type specification */
			type = ILTypeSpecGetType((ILTypeSpec *)item);
			spec = _ILLinkerConvertTypeSpec(linker, type);
			if(!spec)
			{
				return -1;
			}
			*token = ILTypeSpec_Token(spec);
			return 1;
		}
		break;

		default:
		{
			/* This token type is illegal in method bodies, so just zero it */
			*token = 0;
		}
		break;
	}
	return 0;
}

/*
 * Convert the IL code that is associated with a method.
 */
static int ConvertCode(ILLinker *linker, ILMethod *method,
					   ILMethod *newMethod, ILMethodCode *code,
					   ILException *exceptions)
{
	ILWriter *writer = linker->writer;
	unsigned char buffer[BUFSIZ];
	int bufPosn;
	unsigned long bufRVA;
	unsigned char *pc;
	ILUInt32 len;
	const ILOpcodeInfo *insn;
	ILUInt32 size;
	int argOffset;
	ILToken token;
	int needsFixup;
	const char *str;
	unsigned long strLen;
	ILType *localVars;
	ILStandAloneSig *sig;
	ILUInt32 numExceptions;
	ILException *exception;
	int fatExceptions;

	/* Construct the method header */
	if(code->headerSize == 1)
	{
		/* Encode a tiny format header */
		buffer[0] = (unsigned char)(((code->codeLen) << 2) | 0x02);
		ILWriterTextWrite(writer, buffer, 1);
	}
	else
	{
		/* Encode a fat format header */
		buffer[0] = (unsigned char)(code->initLocals ? 0x13 : 0x03);
		if(exceptions)
		{
			/* There will be more sections following the method code */
			buffer[0] |= (unsigned char)0x08;
		}
		buffer[1] = (unsigned char)0x30;
		buffer[2] = (unsigned char)(code->maxStack);
		buffer[3] = (unsigned char)(code->maxStack >> 8);
		buffer[4] = (unsigned char)(code->codeLen);
		buffer[5] = (unsigned char)(code->codeLen >> 8);
		buffer[6] = (unsigned char)(code->codeLen >> 16);
		buffer[7] = (unsigned char)(code->codeLen >> 24);
		if(code->localVarSig)
		{
			localVars = _ILLinkerConvertType
					(linker, ILStandAloneSigGetType(code->localVarSig));
			if(!localVars)
			{
				return 0;
			}
			sig = ILStandAloneSigCreate(linker->image, 0, localVars);
			if(!sig)
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
			token = ILProgramItem_Token(sig);
			buffer[8]  = (unsigned char)token;
			buffer[9]  = (unsigned char)(token >> 8);
			buffer[10] = (unsigned char)(token >> 16);
			buffer[11] = (unsigned char)(token >> 24);
		}
		else
		{
			buffer[8]  = (unsigned char)0x00;
			buffer[9]  = (unsigned char)0x00;
			buffer[10] = (unsigned char)0x00;
			buffer[11] = (unsigned char)0x00;
		}
		ILWriterTextWrite(writer, buffer, 12);
	}

	/* Copy the code to the final image and convert any tokens that we find */
	bufRVA = ILWriterGetTextRVA(writer);
	bufPosn = 0;
	pc = code->code;
	len = code->codeLen;
	while(len > 0)
	{
		/* Fetch the opcode information for the next instruction */
		if(pc[0] != IL_OP_PREFIX)
		{
			/* Ordinary instruction */
			insn = &(ILMainOpcodeTable[pc[0]]);
			argOffset = 1;
		}
		else
		{
			/* Prefixed instruction */
			insn = &(ILPrefixOpcodeTable[pc[1]]);
			argOffset = 2;
		}
		size = (ILUInt32)(insn->size);

		/* Determine how to process the instruction */
		switch(insn->args)
		{
			case IL_OPCODE_ARGS_TOKEN:
			case IL_OPCODE_ARGS_CALL:
			case IL_OPCODE_ARGS_CALLI:
			case IL_OPCODE_ARGS_CALLVIRT:
			case IL_OPCODE_ARGS_NEW:
			{
				/* This instruction takes a token argument */
				ILMemCpy(buffer + bufPosn, pc, argOffset);
				bufPosn += argOffset;
				token = IL_READ_UINT32(pc + argOffset);
				needsFixup = ConvertToken(linker, method, newMethod, &token);
				buffer[bufPosn++] = (unsigned char)token;
				buffer[bufPosn++] = (unsigned char)(token >> 8);
				buffer[bufPosn++] = (unsigned char)(token >> 16);
				buffer[bufPosn++] = (unsigned char)(token >> 24);
				if(needsFixup < 0)
				{
					return 0;
				}
				else if(needsFixup)
				{
					/* Record a fixup on the token for later */
					ILWriterSetFixup(writer, bufRVA + bufPosn - 4,
						 ILProgramItem_FromToken(linker->image, token));
				}
			}
			break;

			case IL_OPCODE_ARGS_STRING:
			{
				/* This instruction takes a string argument */
				token = (IL_READ_UINT32(pc + 1) & ~IL_META_TOKEN_MASK);
				str = ILImageGetUserString(ILProgramItem_Image(method),
										   token, &strLen);
				if(str)
				{
					/* Copy the string to the output image */
					token = ILImageAddEncodedUserString
								(linker->image, str, strLen);
					if(!token)
					{
						_ILLinkerOutOfMemory(linker);
						return 0;
					}
					token |= IL_META_TOKEN_STRING;
				}
				else
				{
					/* Shouldn't happen, but do something sane anyway */
					token = 0;
				}
				buffer[bufPosn++] = IL_OP_LDSTR;
				buffer[bufPosn++] = (unsigned char)token;
				buffer[bufPosn++] = (unsigned char)(token >> 8);
				buffer[bufPosn++] = (unsigned char)(token >> 16);
				buffer[bufPosn++] = (unsigned char)(token >> 24);
			}
			break;

			case IL_OPCODE_ARGS_SWITCH:
			{
				/* Determine the total length of the switch */
				size = 5 + IL_READ_UINT32(pc + 1) * 4;

				/* Flush the current buffer contents */
			flushLong:
				if(bufPosn > 0)
				{
					ILWriterTextWrite(writer, buffer, bufPosn);
					bufRVA += bufPosn;
					bufPosn = 0;
				}

				/* Write the entire switch to the output image */
				ILWriterTextWrite(writer, pc, size);
				bufRVA += size;
			}
			break;

			case IL_OPCODE_ARGS_ANN_DATA:
			{
				/* Determine the length of the data annotation */
				if(pc[0] == IL_OP_ANN_DATA_S)
				{
					size = (((ILUInt32)(pc[1])) & 0xFF) + 2;
				}
				else
				{
					size = (IL_READ_UINT32(pc + 2) + 6);
				}
				goto flushLong;
			}
			/* Not reached */

			case IL_OPCODE_ARGS_ANN_PHI:
			{
				/* Determine the length of the phi annotation */
				size = ((ILUInt32)IL_READ_UINT16(pc + 1)) * 2 + 3;
				goto flushLong;
			}
			/* Not reached */

			default:
			{
				/* This instruction does not need any special handling */
				ILMemCpy(buffer + bufPosn, pc, size);
				bufPosn += size;
			}
			break;
		}

		/* Flush the buffer contents if it is over the high water mark */
		if(bufPosn >= (BUFSIZ - 64))
		{
			ILWriterTextWrite(writer, buffer, bufPosn);
			bufRVA += bufPosn;
			bufPosn = 0;
		}

		/* Advance to the next instruction */
		pc += size;
		len -= size;
	}

	/* Flush the remainder of the buffer contents */
	if(bufPosn > 0)
	{
		ILWriterTextWrite(writer, buffer, bufPosn);
		bufRVA += bufPosn;
	}

	/* Convert and output the exception blocks */
	if(exceptions)
	{
		/* Align the text section on a 4-byte boundary */
		ILWriterTextAlign(writer);

		/* Should we use the tiny or fat format for the exception blocks? */
		fatExceptions = 0;
		numExceptions = 0;
		exception = exceptions;
		while(exception != 0)
		{
			if(exception->tryOffset > (ILUInt32)0xFFFF ||
			   exception->tryLength > (ILUInt32)0xFF ||
			   exception->handlerOffset > (ILUInt32)0xFFFF ||
			   exception->handlerLength > (ILUInt32)0xFF)
			{
				fatExceptions = 1;
			}
			++numExceptions;
			exception = exception->next;
		}
		if(!fatExceptions && ((numExceptions * 12) + 4) > (ILUInt32)0xFF)
		{
			fatExceptions = 1;
		}

		/* What type of exception header should we use? */
		if(fatExceptions)
		{
			/* Use the fat format for the exception information */
			len = (numExceptions * 24) + 4;
			buffer[0] = (unsigned char)0x41;
			buffer[1] = (unsigned char)len;
			buffer[2] = (unsigned char)(len >> 8);
			buffer[3] = (unsigned char)(len >> 16);
			ILWriterTextWrite(writer, buffer, 4);
			bufRVA += 4;
			exception = exceptions;
			while(exception != 0)
			{
				buffer[0]  = (unsigned char)(exception->flags);
				buffer[1]  = (unsigned char)(exception->flags >> 8);
				buffer[2]  = (unsigned char)(exception->flags >> 16);
				buffer[3]  = (unsigned char)(exception->flags >> 24);
				buffer[4]  = (unsigned char)(exception->tryOffset);
				buffer[5]  = (unsigned char)(exception->tryOffset >> 8);
				buffer[6]  = (unsigned char)(exception->tryOffset >> 16);
				buffer[7]  = (unsigned char)(exception->tryOffset >> 24);
				buffer[8]  = (unsigned char)(exception->tryLength);
				buffer[9]  = (unsigned char)(exception->tryLength >> 8);
				buffer[10] = (unsigned char)(exception->tryLength >> 16);
				buffer[11] = (unsigned char)(exception->tryLength >> 24);
				buffer[12] = (unsigned char)(exception->handlerOffset);
				buffer[13] = (unsigned char)(exception->handlerOffset >> 8);
				buffer[14] = (unsigned char)(exception->handlerOffset >> 16);
				buffer[15] = (unsigned char)(exception->handlerOffset >> 24);
				buffer[16] = (unsigned char)(exception->handlerLength);
				buffer[17] = (unsigned char)(exception->handlerLength >> 8);
				buffer[18] = (unsigned char)(exception->handlerLength >> 16);
				buffer[19] = (unsigned char)(exception->handlerLength >> 24);
				if((exception->flags & IL_META_EXCEPTION_FILTER) != 0)
				{
					buffer[20] = (unsigned char)(exception->extraArg);
					buffer[21] = (unsigned char)(exception->extraArg >> 8);
					buffer[22] = (unsigned char)(exception->extraArg >> 16);
					buffer[23] = (unsigned char)(exception->extraArg >> 24);
				}
				else if((exception->flags & 0x07) == IL_META_EXCEPTION_CATCH)
				{
					token = exception->extraArg;
					needsFixup = ConvertToken(linker, method,
											  newMethod, &token);
					buffer[20] = (unsigned char)(token);
					buffer[21] = (unsigned char)(token >> 8);
					buffer[22] = (unsigned char)(token >> 16);
					buffer[23] = (unsigned char)(token >> 24);
					if(needsFixup < 0)
					{
						return 0;
					}
					else if(needsFixup)
					{
						/* Record a fixup for this class token */
						ILWriterSetFixup(writer, bufRVA + 20,
						 	ILProgramItem_FromToken(linker->image, token));
					}
				}
				else
				{
					buffer[20] = (unsigned char)0x00;
					buffer[21] = (unsigned char)0x00;
					buffer[22] = (unsigned char)0x00;
					buffer[23] = (unsigned char)0x00;
				}
				ILWriterTextWrite(writer, buffer, 24);
				bufRVA += 24;
				exception = exception->next;
			}
		}
		else
		{
			/* Use the tiny format for the exception information */
			len = (numExceptions * 12) + 4;
			buffer[0] = (unsigned char)0x01;
			buffer[1] = (unsigned char)len;
			buffer[2] = (unsigned char)0x00;
			buffer[3] = (unsigned char)0x00;
			ILWriterTextWrite(writer, buffer, 4);
			bufRVA += 4;
			exception = exceptions;
			while(exception != 0)
			{
				buffer[0] = (unsigned char)(exception->flags);
				buffer[1] = (unsigned char)(exception->flags >> 8);
				buffer[2] = (unsigned char)(exception->tryOffset);
				buffer[3] = (unsigned char)(exception->tryOffset >> 8);
				buffer[4] = (unsigned char)(exception->tryLength);
				buffer[5] = (unsigned char)(exception->handlerOffset);
				buffer[6] = (unsigned char)(exception->handlerOffset >> 8);
				buffer[7] = (unsigned char)(exception->handlerLength);
				if((exception->flags & IL_META_EXCEPTION_FILTER) != 0)
				{
					buffer[8]  = (unsigned char)(exception->extraArg);
					buffer[9]  = (unsigned char)(exception->extraArg >> 8);
					buffer[10] = (unsigned char)(exception->extraArg >> 16);
					buffer[11] = (unsigned char)(exception->extraArg >> 24);
				}
				else if((exception->flags & 0x07) == IL_META_EXCEPTION_CATCH)
				{
					token = exception->extraArg;
					needsFixup = ConvertToken(linker, method,
											  newMethod, &token);
					buffer[8]  = (unsigned char)(token);
					buffer[9]  = (unsigned char)(token >> 8);
					buffer[10] = (unsigned char)(token >> 16);
					buffer[11] = (unsigned char)(token >> 24);
					if(needsFixup < 0)
					{
						return 0;
					}
					else if(needsFixup)
					{
						/* Record a fixup for this class token */
						ILWriterSetFixup(writer, bufRVA + 8,
						 	ILProgramItem_FromToken(linker->image, token));
					}
				}
				else
				{
					buffer[8]  = (unsigned char)0x00;
					buffer[9]  = (unsigned char)0x00;
					buffer[10] = (unsigned char)0x00;
					buffer[11] = (unsigned char)0x00;
				}
				ILWriterTextWrite(writer, buffer, 12);
				bufRVA += 12;
				exception = exception->next;
			}
		}
	}

	/* Done */
	return 1;
}

int _ILLinkerConvertMethod(ILLinker *linker, ILMethod *method,
						   ILClass *newClass)
{
	ILMethod *newMethod;
	const char *name = ILMethod_Name(method);
	ILType *signature = ILMethod_Signature(method);
	ILType *newSignature;
	ILParameter *param;
	ILParameter *newParam;
	ILModule *module;
	ILPInvoke *pinvoke;
	ILOverride *over;
	ILMethod *decl;
	ILMethodCode code;
	ILException *exceptions;

	/* See if we already have a definition of this method in the class */
	newMethod = 0;
	while((newMethod = (ILMethod *)ILClassNextMemberByKind
				(newClass, (ILMember *)newMethod, IL_META_MEMBERKIND_METHOD))
						!= 0)
	{
		if(!strcmp(ILMethod_Name(newMethod), name) &&
		   ILTypeIdentical(ILMethod_Signature(newMethod), signature))
		{
			/* Bail out if the method is already defined.  This shouldn't
			   happen very often because duplicate classes are trapped long
			   before control gets to here.  Global methods may result in
			   this code being used, however */
			if((ILMethod_Token(newMethod) & IL_META_TOKEN_MASK)
					!= IL_META_TOKEN_MEMBER_REF)
			{
				return 1;
			}

			/* Allocate a new token for the method */
			if(!ILMethodNewToken(newMethod))
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
			break;
		}
	}
	if(!newMethod)
	{
		/* Create a new method block underneath "newClass" */
		newMethod = ILMethodCreate(newClass, 0, name, ILMethod_Attrs(method));
		if(!newMethod)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}

		/* Convert the signature and apply it */
		newSignature = _ILLinkerConvertType(linker, signature);
		if(!newSignature)
		{
			return 0;
		}
		ILMemberSetSignature((ILMember *)newMethod, newSignature);
	}
	else
	{
		/* Extract the signature of the existing method */
		newSignature = ILMethod_Signature(newMethod);
	}

	/* Copy the calling conventions */
	newSignature->kind = (newSignature->kind & 0xFF) |
						 (signature->kind & 0xFF00);
	ILMethodSetCallConv(newMethod, (signature->kind >> 8));

	/* Copy the implementation attributes */
	ILMethodSetImplAttrs(newMethod, ~((ILUInt32)0),
						 ILMethod_ImplAttrs(method));

	/* Copy the parameter definition blocks */
	param = 0;
	while((param = ILMethodNextParam(method, param)) != 0)
	{
		newParam = ILParameterCreate(newMethod, 0, ILParameter_Name(param),
									 ILParameter_Attrs(param),
									 ILParameter_Num(param));
		if(!newParam)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
		if(!_ILLinkerConvertAttrs(linker, (ILProgramItem *)param,
							      (ILProgramItem *)newParam))
		{
			return 0;
		}
		if(!_ILLinkerConvertMarshal(linker, (ILProgramItem *)param,
									(ILProgramItem *)newParam, 1))
		{
			return 0;
		}
	}

	/* Convert the custom attributes that are attached to the method */
	if(!_ILLinkerConvertAttrs(linker, (ILProgramItem *)method,
						      (ILProgramItem *)newMethod))
	{
		return 0;
	}

	/* Convert the security declarations that are attached to the method */
	if(!_ILLinkerConvertSecurity(linker, (ILProgramItem *)method,
						         (ILProgramItem *)newMethod))
	{
		return 0;
	}

	/* Convert the PInvoke information for the method */
	pinvoke = ILPInvokeFind(method);
	if(pinvoke)
	{
		module = ILPInvoke_Module(pinvoke);
		if(module)
		{
			module = ILModuleRefCreateUnique(linker->image,
											 ILModule_Name(module));
			if(!module)
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
		}
		if(!ILPInvokeCreate(newMethod, 0, ILPInvoke_Attrs(pinvoke),
							module, ILPInvoke_Alias(pinvoke)))
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
	}

	/* Convert the override information for the method */
	over = ILOverrideFromMethod(method);
	if(over)
	{
		decl = ILOverrideGetDecl(over);
		decl = (ILMethod *)_ILLinkerConvertMemberRef(linker, (ILMember *)decl);
		if(!decl)
		{
			return 0;
		}
		if(!ILOverrideCreate(newClass, 0, decl, newMethod))
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
	}

	/* Set up the entry point information */
	if(ILImageGetEntryPoint(ILProgramItem_Image(method))
			== ILMethod_Token(method))
	{
		/* This method was marked as an entry point by the compiler */
	setEntryPoint:
		if(linker->entryPoint)
		{
			fputs("program has multiple entry points\n", stderr);
		}
		ILWriterSetEntryPoint(linker->writer, newMethod);
		linker->entryPoint = newMethod;
	}
	else if(!strcmp(name, "Main") && ILMethod_IsStatic(method))
	{
		/* This may be an entry point method: check the signature */
		if(signature->un.method.retType == ILType_Int32 ||
		   signature->un.method.retType == ILType_UInt32 ||
		   signature->un.method.retType == ILType_Void)
		{
			if(signature->num == 0 ||
			   (signature->num == 1 &&
			    IsStringArray(signature->un.method.param[0])))
			{
				goto setEntryPoint;
			}
		}
	}

	/* Get the method's code.  If there is no code, then we are done */
	if(!ILMethodGetCode(method, &code))
	{
		return 1;
	}
	if(!ILMethodGetExceptions(method, &code, &exceptions))
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}

	/* Align the text section and record the RVA within the new method */
	ILWriterTextAlign(linker->writer);
	ILMethodSetRVA(newMethod, ILWriterGetTextRVA(linker->writer));

	/* Convert the method code */
	if(!ConvertCode(linker, method, newMethod, &code, exceptions))
	{
		ILMethodFreeExceptions(exceptions);
		return 0;
	}

	/* Free the exception list, which we no longer require */
	ILMethodFreeExceptions(exceptions);

	/* Finished */
	return 1;
}

ILMember *_ILLinkerConvertMemberRef(ILLinker *linker, ILMember *member)
{
	ILClass *owner;
	ILType *synType;
	ILTypeSpec *spec;
	const char *name;
	ILType *signature;
	ILType *newSignature;
	ILMethod *method;
	ILField *field;

	/* Convert the member's owner reference */
	owner = ILMember_Owner(member);
	synType = ILClassGetSynType(owner);
	if(synType)
	{
		/* Map the synthetic type reference into the new image */
		spec = _ILLinkerConvertTypeSpec(linker, synType);
		if(!spec)
		{
			return 0;
		}

		/* Import the synthetic class and assign a TypeRef token
		   to it.  We give this TypeRef the same token as the
		   TypeSpec, which will cause it to be removed from the
		   final table during metadata compaction */
		owner = ILTypeSpecGetClassRef(spec);
		if(!owner)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
	}
	else
	{
		owner = _ILLinkerConvertClassRef(linker, owner);
		if(!owner)
		{
			return 0;
		}
	}

	/* Search for an existing member with the requested signature */
	name = ILMember_Name(member);
	signature = ILMember_Signature(member);
	if(ILMember_IsMethod(member))
	{
		/* Searching for a method */
		method = 0;
		while((method = (ILMethod *)ILClassNextMemberByKind
				(owner, (ILMember *)method, IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if(!strcmp(ILMethod_Name(method), name) &&
			   ILTypeIdentical(ILMethod_Signature(method), signature))
			{
				return (ILMember *)method;
			}
		}

		/* Create a MemberRef for the method */
		method = ILMethodCreate(owner, (ILToken)IL_MAX_UINT32, name, 0);
		if(!method)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
		newSignature = _ILLinkerConvertType(linker, signature);
		if(!newSignature)
		{
			return 0;
		}
		ILMemberSetSignature((ILMember *)method, newSignature);
		ILMethodSetCallConv(method, (newSignature->kind >> 8));
		return (ILMember *)method;
	}
	else
	{
		/* Searching for a field */
		field = 0;
		while((field = (ILField *)ILClassNextMemberByKind
				(owner, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
		{
			if(!strcmp(ILField_Name(field), name) &&
			   ILTypeIdentical(ILField_Type(field), signature))
			{
				return (ILMember *)field;
			}
		}

		/* Create a MemberRef for the field */
		field = ILFieldCreate(owner, (ILToken)IL_MAX_UINT32, name, 0);
		if(!field)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
		newSignature = _ILLinkerConvertType(linker, signature);
		if(!newSignature)
		{
			return 0;
		}
		ILMemberSetSignature((ILMember *)field, newSignature);
		return (ILMember *)field;
	}
}

int ILLinkerSetEntryPoint(ILLinker *linker, const char *name)
{
	int len, dot;
	const char *methodName;
	const char *className;
	int classNameLen;
	const char *namespace;
	int namespaceLen;
	ILClass *classInfo;
	ILMethod *method;
	int found;

	/* Split the name into class and method */
	len = strlen(name);
	while(len > 0 && (name[len - 1] != ':' || name[len] != ':'))
	{
		--len;
	}
	if(len > 0)
	{
		methodName = name + len + 1;
		--len;
	}
	else
	{
		methodName = "Main";
		len = strlen(name);
	}

	/* Split the class name into its name and namespace components */
	dot = len;
	while(dot > 0 && name[dot - 1] != '.')
	{
		--dot;
	}
	if(dot > 0)
	{
		className = name + dot;
		classNameLen = len - dot;
		namespace = name;
		namespaceLen = dot - 1;
	}
	else
	{
		className = name;
		classNameLen = len;
		namespace = 0;
		namespaceLen = 0;
	}

	/* Look for the class within the image's global scope */
	classInfo = ILClassLookup(ILClassGlobalScope(linker->image),
							  name, namespace);
	if(!classInfo)
	{
		return 0;
	}

	/* Look for the method within the class */
	method = 0;
	found = 0;
	while((method = (ILMethod *)ILClassNextMemberByKind
				(classInfo, (ILMember *)method,
				 IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if(strcmp(ILMethod_Name(method), methodName) &&
		   ILMethod_IsStatic(method))
		{
			/* If the method is different from the inferred entry point,
			   then report an error */
			if(linker->entryPoint != 0 && linker->entryPoint != method)
			{
				fputs("program has multiple entry points\n", stderr);
			}
			linker->entryPoint = method;
			ILWriterSetEntryPoint(linker->writer, method);
			found = 1;
		}
	}

	/* Done */
	return found;
}

int ILLinkerHasEntryPoint(ILLinker *linker)
{
	return (linker->entryPoint != 0);
}

#ifdef	__cplusplus
};
#endif