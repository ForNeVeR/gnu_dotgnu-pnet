/*
 * synthetic.c - Handle synthetic classes such as arrays, pointers, etc.
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

#include "program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Import a synthetic class from another image.
 */
static ILClass *ImportSynthetic(ILImage *image, ILClass *info)
{
	ILClass *newInfo;
	ILProgramItem *scope;

	/* Nothing to do if the class is already in the same image */
	if(info->programItem.image == image)
	{
		return info;
	}

	/* Create a new reference */
	scope = ILClassGlobalScope(image);
	if(!scope)
	{
		return 0;
	}
	newInfo = ILClassCreateRef(scope, 0, info->name, info->namespace);
	if(!newInfo)
	{
		return 0;
	}

	/* Link the reference to the imported class */
	if(!_ILProgramItemLink(&(newInfo->programItem), &(info->programItem)))
	{
		return 0;
	}

	/* Ready to go */
	return newInfo;
}

/*
 * Resolve a reference to a system type.
 */
ILClass *_ILCreateSystemType(ILImage *image, const char *name)
{
	ILClass *info;
	ILProgramItem *scope;

	/* Look in the current image */
	scope = ILClassGlobalScope(image);
	if(scope)
	{
		info = ILClassLookup(scope, name, "System");
		if(info)
		{
			return info;
		}
	}

	/* Look in all images */
	info = ILClassLookupGlobal(image->context, name, "System");
	if(info)
	{
		return ImportSynthetic(image, info);
	}

	/* Create a TypeRef within the current image */
	if(scope)
	{
		return ILClassCreateRef(scope, 0, name, "System");
	}
	else
	{
		return 0;
	}
}

/*
 * Add methods to a synthetic class that corresponds to an array.
 */
static int AddArrayMethods(ILClass *info, ILType *type)
{
	ILContext *context = ILClassToContext(info);
	ILType *temp;
	ILUInt32 numDims;
	ILUInt32 dim;
	ILMethod *method;
	ILType *signature;

	/* Count the number of dimensions and get the element type */
	temp = type;
	numDims = 0;
	while(temp != 0 && ILType_IsComplex(temp) &&
		  temp->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
	{
		++numDims;
		temp = temp->un.array.elemType;
	}
	if(temp != 0 && ILType_IsComplex(temp) &&
	   temp->kind == IL_TYPE_COMPLEX_ARRAY)
	{
		++numDims;
		temp = temp->un.array.elemType;
	}
	if(!temp)
	{
		return 0;
	}

	/* Build the first constructor, which only specifies dimensions */
	method = ILMethodCreate(info, 0, ".ctor",
							IL_META_METHODDEF_PUBLIC |
							IL_META_METHODDEF_HIDE_BY_SIG |
							IL_META_METHODDEF_SPECIAL_NAME |
							IL_META_METHODDEF_RT_SPECIAL_NAME);
	if(!method)
	{
		return 0;
	}
	signature = ILTypeCreateMethod(context, ILType_Void);
	if(!signature)
	{
		return 0;
	}
	signature->kind |= (IL_META_CALLCONV_HASTHIS << 8);
	for(dim = 0; dim < numDims; ++dim)
	{
		if(!ILTypeAddParam(context, signature, ILType_Int32))
		{
			return 0;
		}
	}
	ILMemberSetSignature((ILMember *)method, signature);

	/* Build the second constructor, which specifies lower bounds and lengths */
	method = ILMethodCreate(info, 0, ".ctor",
							IL_META_METHODDEF_PUBLIC |
							IL_META_METHODDEF_HIDE_BY_SIG |
							IL_META_METHODDEF_SPECIAL_NAME |
							IL_META_METHODDEF_RT_SPECIAL_NAME);
	if(!method)
	{
		return 0;
	}
	signature = ILTypeCreateMethod(context, ILType_Void);
	if(!signature)
	{
		return 0;
	}
	signature->kind |= (IL_META_CALLCONV_HASTHIS << 8);
	for(dim = 0; dim < numDims; ++dim)
	{
		if(!ILTypeAddParam(context, signature, ILType_Int32))
		{
			return 0;
		}
		if(!ILTypeAddParam(context, signature, ILType_Int32))
		{
			return 0;
		}
	}
	ILMemberSetSignature((ILMember *)method, signature);

	/* Build the "Get" method */
	method = ILMethodCreate(info, 0, "Get",
							IL_META_METHODDEF_PUBLIC |
							IL_META_METHODDEF_HIDE_BY_SIG);
	if(!method)
	{
		return 0;
	}
	signature = ILTypeCreateMethod(context, temp);
	if(!signature)
	{
		return 0;
	}
	signature->kind |= (IL_META_CALLCONV_HASTHIS << 8);
	for(dim = 0; dim < numDims; ++dim)
	{
		if(!ILTypeAddParam(context, signature, ILType_Int32))
		{
			return 0;
		}
	}
	ILMemberSetSignature((ILMember *)method, signature);

	/* Build the "Set" method */
	method = ILMethodCreate(info, 0, "Set",
							IL_META_METHODDEF_PUBLIC |
							IL_META_METHODDEF_HIDE_BY_SIG);
	if(!method)
	{
		return 0;
	}
	signature = ILTypeCreateMethod(context, ILType_Void);
	if(!signature)
	{
		return 0;
	}
	signature->kind |= (IL_META_CALLCONV_HASTHIS << 8);
	for(dim = 0; dim < numDims; ++dim)
	{
		if(!ILTypeAddParam(context, signature, ILType_Int32))
		{
			return 0;
		}
	}
	if(!ILTypeAddParam(context, signature, temp))
	{
		return 0;
	}
	ILMemberSetSignature((ILMember *)method, signature);

	/* Build the "Address" method */
	method = ILMethodCreate(info, 0, "Address",
							IL_META_METHODDEF_PUBLIC |
							IL_META_METHODDEF_HIDE_BY_SIG);
	if(!method)
	{
		return 0;
	}
	temp = ILTypeCreateRef(context, IL_TYPE_COMPLEX_BYREF, temp);
	if(!temp)
	{
		return 0;
	}
	signature = ILTypeCreateMethod(context, temp);
	if(!signature)
	{
		return 0;
	}
	signature->kind |= (IL_META_CALLCONV_HASTHIS << 8);
	for(dim = 0; dim < numDims; ++dim)
	{
		if(!ILTypeAddParam(context, signature, ILType_Int32))
		{
			return 0;
		}
	}
	ILMemberSetSignature((ILMember *)method, signature);

	/* Done */
	return 1;
}

ILClass *_ILTypeToSyntheticClass(ILImage *image, ILType *type)
{
	ILClass *info;
	char *name;
	const char *persistName;
	ILProgramItem *scope;

	/* Remove unnecessary type prefixes */
	type = ILTypeStripPrefixes(type);

	/* Handle the simple cases first */
	if(!type)
	{
		return 0;
	}
	else if(ILType_IsClass(type))
	{
		return ILType_ToClass(type);
	}
	else if(ILType_IsValueType(type))
	{
		return ILType_ToValueType(type);
	}
	else if(ILType_IsPrimitive(type))
	{
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
					return _ILCreateSystemType(image, "Void");

			case IL_META_ELEMTYPE_BOOLEAN:
					return _ILCreateSystemType(image, "Boolean");

			case IL_META_ELEMTYPE_CHAR:
					return _ILCreateSystemType(image, "Char");

			case IL_META_ELEMTYPE_I1:
					return _ILCreateSystemType(image, "SByte");

			case IL_META_ELEMTYPE_U1:
					return _ILCreateSystemType(image, "Byte");

			case IL_META_ELEMTYPE_I2:
					return _ILCreateSystemType(image, "Int16");

			case IL_META_ELEMTYPE_U2:
					return _ILCreateSystemType(image, "UInt16");

			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_I:
					return _ILCreateSystemType(image, "Int32");

			case IL_META_ELEMTYPE_U4:
			case IL_META_ELEMTYPE_U:
					return _ILCreateSystemType(image, "UInt32");

			case IL_META_ELEMTYPE_I8:
					return _ILCreateSystemType(image, "Int64");

			case IL_META_ELEMTYPE_U8:
					return _ILCreateSystemType(image, "UInt64");

			case IL_META_ELEMTYPE_R4:
					return _ILCreateSystemType(image, "Single");

			case IL_META_ELEMTYPE_R8:
			case IL_META_ELEMTYPE_R:
					return _ILCreateSystemType(image, "Double");

			break;
		}
		return 0;
	}

	/* Create a synthesized class name for the type */
	name = ILTypeToName(type);
	if(!name)
	{
		return 0;
	}

	/* Do we already have a synthesized class for the type? */
	info = ILClassLookupGlobal(image->context, name, "$Synthetic");
	if(info)
	{
		return ImportSynthetic(image, info);
	}

	/* Create a new class to hold the synthetic infomation */
	persistName = _ILContextPersistMalloc(image, name);
	if(!persistName)
	{
		return 0;
	}
	scope = ILClassGlobalScope(image);
	if(!scope)
	{
		return 0;
	}
	info = ILClassCreateRef(scope, 0, persistName, "$Synthetic");
	if(!info)
	{
		return 0;
	}

	/* Register the type with the synthetic class */
	info->synthetic = type;

	/* Add the methods that are relevant to this kind of synthetic class */
	switch(type->kind)
	{
		case IL_TYPE_COMPLEX_PTR:
		{
		}
		break;

		case IL_TYPE_COMPLEX_ARRAY:
		case IL_TYPE_COMPLEX_ARRAY_CONTINUE:
		{
			if(!AddArrayMethods(info, type))
			{
				return 0;
			}
		}
		break;

		case IL_TYPE_COMPLEX_METHOD:
		{
		}
		break;

		default: break;
	}

	/* Done */
	return info;
}

#ifdef	__cplusplus
};
#endif
