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
	ILModule *module;
	ILPInvoke *pinvoke;
	ILOverride *over;
	ILMethod *decl;
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

	/* Get the method's code.  If there is no code, then we are done */
	if(!ILMethodGetCode(method, &code))
	{
		return 1;
	}

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

#ifdef	__cplusplus
};
#endif
