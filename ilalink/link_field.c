/*
 * link_field.c - Convert a field and copy it to the final image.
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

int _ILLinkerConvertField(ILLinker *linker, ILField *field, ILClass *newClass)
{
	ILField *newField;
	const char *name = ILField_Name(field);
	ILType *type = ILField_Type(field);

	/* See if we already have a definition of this field in the class */
	newField = 0;
	while((newField = (ILField *)ILClassNextMemberByKind
				(newClass, (ILMember *)newField, IL_META_MEMBERKIND_FIELD))
						!= 0)
	{
		if(!strcmp(ILField_Name(newField), name) &&
		   ILTypeIdentical(ILField_Type(newField), type))
		{
			/* Bail out if the field is already defined.  This shouldn't
			   happen very often because duplicate classes are trapped long
			   before control gets to here.  Global fields may result in
			   this code being used, however */
			if((ILField_Token(newField) & IL_META_TOKEN_MASK)
					!= IL_META_TOKEN_MEMBER_REF)
			{
				return 1;
			}

			/* Allocate a new token for the field */
			if(!ILFieldNewToken(newField))
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
			break;
		}
	}
	if(!newField)
	{
		/* Create the field within the new class */
		newField = ILFieldCreate(newClass, 0, name, ILField_Attrs(field));
		if(!newField)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}

		/* Convert the field's type */
		type = _ILLinkerConvertType(linker, type);
		if(!type)
		{
			return 0;
		}
		ILMemberSetSignature((ILMember *)newField, type);
	}
	else
	{
		/* Set the attribute flags to their correct values */
		ILMemberSetAttrs((ILMember *)newField, ~((ILUInt32)0),
						 ILField_Attrs(field));
	}

	/* Convert the attributes that are attached to the field */
	if(!_ILLinkerConvertAttrs(linker, (ILProgramItem *)field,
							  (ILProgramItem *)newField))
	{
		return 0;
	}

	/* Convert the field marshalling and layout information */
	if(!_ILLinkerConvertMarshal(linker, (ILProgramItem *)field,
								(ILProgramItem *)newField, 0))
	{
		return 0;
	}

	/* Done */
	return 1;
}

int _ILLinkerConvertMarshal(ILLinker *linker, ILProgramItem *oldItem,
						    ILProgramItem *newItem, int isParam)
{
	ILFieldMarshal *marshal;
	ILFieldMarshal *newMarshal;
	ILConstant *constant;
	ILConstant *newConstant;
	ILFieldLayout *layout;
	ILFieldRVA *rva;
	const void *blob;
	unsigned long blobLen;

	/* Convert the marshalling information */
	marshal = ILFieldMarshalGetFromOwner(oldItem);
	if(marshal)
	{
		newMarshal = ILFieldMarshalCreate(linker->image, 0, newItem);
		if(!newMarshal)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
		blob = ILFieldMarshalGetType(marshal, &blobLen);
		if(blob)
		{
			if(!ILFieldMarshalSetType(newMarshal, blob, blobLen))
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
		}
	}

	/* Convert the constant information */
	constant = ILConstantGetFromOwner(oldItem);
	if(constant)
	{
		newConstant = ILConstantCreate(linker->image, 0, newItem,
									   ILConstant_ElemType(constant));
		if(!newConstant)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
		blob = ILConstantGetValue(constant, &blobLen);
		if(blob)
		{
			if(!ILConstantSetValue(newConstant, blob, blobLen))
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
		}
	}

	/* Bail out now if we are processing a parameter */
	if(isParam)
	{
		return 1;
	}

	/* Convert the field layout information */
	layout = ILFieldLayoutGetFromOwner((ILField *)oldItem);
	if(layout)
	{
		layout = ILFieldLayoutCreate(linker->image, 0, (ILField *)newItem,
									 ILFieldLayout_Offset(layout));
		if(!layout)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
	}

	/* Convert the field RVA information */
	rva = ILFieldRVAGetFromOwner((ILField *)oldItem);
	if(rva)
	{
		rva = ILFieldRVACreate(linker->image, 0, (ILField *)newItem,
							   ILFieldRVA_RVA(rva));
		if(!rva)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
	}

	/* Done */
	return 1;
}

#ifdef	__cplusplus
};
#endif
