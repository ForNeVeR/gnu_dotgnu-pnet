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

#ifdef	__cplusplus
extern	"C" {
#endif

int _ILLinkerConvertMethod(ILLinker *linker, ILMethod *method,
						   ILClass *newClass)
{
	ILMethod *newMethod;
	const char *name = ILMethod_Name(method);
	ILType *signature = ILMethod_Signature(method);
	ILType *newSignature;
	ILParameter *param;
	ILParameter *newParam;
	ILMethodCode code;

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
	}

	/* Convert the custom attributes that are attached to the method */
	if(!_ILLinkerConvertAttrs(linker, (ILProgramItem *)method,
						      (ILProgramItem *)newMethod))
	{
		return 0;
	}

	/* Get the method's code.  If there is no code, then we are done */
	if(!ILMethodGetCode(method, &code))
	{
		return 1;
	}

	/* Finished */
	return 1;
}

#ifdef	__cplusplus
};
#endif
