/*
 * c_types.c - Type representation for the C programming language.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#include <cscc/c/c_internal.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Forward declarations.
 */
static ILUInt32 TypeSizeAndAlign(ILType *type, ILUInt32 *align, int force);
static char *AppendThree(ILGenInfo *info, const char *prefix,
						 char *str, const char *suffix);

/*
 * Create a "struct" or "union" type given a name and prefix.
 */
static ILType *CreateStructOrUnion(ILGenInfo *info, const char *name,
								   const char *prefix)
{
	int prefixLen = strlen(prefix);
	ILClass *classInfo;
	char *newName;

	/* Create a new name by prepending the prefix to the name */
	newName = (char *)ILMalloc(strlen(name) + prefixLen + 1);
	if(!newName)
	{
		ILGenOutOfMemory(info);
	}
	strcpy(newName, prefix);
	strcpy(newName + prefixLen, name);

	/* Search for a class information block with the name */
	classInfo = ILClassLookup(ILClassGlobalScope(info->image), newName, 0);
	if(classInfo)
	{
		ILFree(newName);
		return ILType_FromValueType(classInfo);
	}

	/* Create a new class reference with the specified name */
	classInfo = ILClassCreateRef(ILClassGlobalScope(info->image), 0,
							     newName, 0);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Clean up and exit */
	ILFree(newName);
	return ILType_FromValueType(classInfo);
}

ILType *CTypeCreateStruct(ILGenInfo *info, const char *name)
{
	return CreateStructOrUnion(info, name, "struct ");
}

ILType *CTypeCreateUnion(ILGenInfo *info, const char *name)
{
	return CreateStructOrUnion(info, name, "union ");
}

ILType *CTypeCreateEnum(ILGenInfo *info, const char *name)
{
	char *newName;
	ILClass *classInfo;

	/* Create a new name by prepending "enum " to the name */
	newName = (char *)ILMalloc(strlen(name) + 6);
	if(!newName)
	{
		ILGenOutOfMemory(info);
	}
	strcpy(newName, "enum ");
	strcpy(newName + 5, name);

	/* Search for a class information block with the name */
	classInfo = ILClassLookup(ILClassGlobalScope(info->image), newName, 0);
	if(classInfo)
	{
		ILFree(newName);
		return ILType_FromValueType(classInfo);
	}

	/* Create a new class with the specified name */
	classInfo = ILClassCreateRef(ILClassGlobalScope(info->image), 0,
							     newName, 0);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Clean up and exit */
	ILFree(newName);
	return ILType_FromValueType(classInfo);
}

ILType *CTypeCreateArray(ILGenInfo *info, ILType *elemType, ILUInt32 size)
{
	char sizeName[64];
	char *name;
	ILUInt32 elemSize, align;
	ILUInt32 attrs, index;
	ILClass *classInfo;
	ILField *field;

	/* Create the name of the array type */
	sprintf(sizeName, "[%lu]", (unsigned long)size);
	name = AppendThree(info, "array ", CTypeToName(info, elemType), sizeName);

	/* See if we already have a type with this name */
	classInfo = ILClassLookup(ILClassGlobalScope(info->image), name, 0);
	if(classInfo)
	{
		ILFree(name);
		return ILType_FromValueType(classInfo);
	}

	/* Get the size and alignment of the element type */
	elemSize = TypeSizeAndAlign(elemType, &align, 0);

	/* Determine the attributes for the array type */
	if(ILType_IsValueType(elemType) &&
	   !ILClass_IsPublic(ILType_ToValueType(elemType)))
	{
		/* The element type is not exported, so neither should the array.
		   This can happen when creating arrays of anonymous structs */
		attrs = IL_META_TYPEDEF_NOT_PUBLIC;
	}
	else
	{
		/* Export the array type to match the element type */
		attrs = IL_META_TYPEDEF_PUBLIC;
	}
	attrs |= IL_META_TYPEDEF_SEALED |
			 IL_META_TYPEDEF_SERIALIZABLE |
			 IL_META_TYPEDEF_VALUE_TYPE;
	if(elemSize != CTYPE_DYNAMIC)
	{
		attrs |= IL_META_TYPEDEF_EXPLICIT_LAYOUT;
	}

	/* Create the class that corresponds to the array type */
	classInfo = ILType_ToClass(ILFindSystemType(info, "ValueType"));
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  name, 0, classInfo);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}
	ILClassSetAttrs(classInfo, ~((ILUInt32)0), attrs);

	/* Set the explicit size and alignment if not native */
	if(elemSize != CTYPE_DYNAMIC)
	{
		if(!ILClassLayoutCreate(info->image, 0, classInfo,
							    align, elemSize * size))
		{
			ILGenOutOfMemory(info);
		}
	}

	/* Create the "elem__" field which defines the type */
	if(size == 0)
	{
		/* Zero-sized arrays store the type as a "static" field */
		field = ILFieldCreate(classInfo, 0, "elem__",
							  IL_META_FIELDDEF_PUBLIC |
							  IL_META_FIELDDEF_STATIC |
							  IL_META_FIELDDEF_SPECIAL_NAME);
	}
	else
	{
		/* Other arrays store the type in a regular field */
		field = ILFieldCreate(classInfo, 0, "elem__",
							  IL_META_FIELDDEF_PUBLIC |
							  IL_META_FIELDDEF_SPECIAL_NAME);
		if(elemSize != CTYPE_DYNAMIC)
		{
			if(!ILFieldLayoutCreate(info->image, 0, field, 0))
			{
				ILGenOutOfMemory(info);
			}
		}
	}
	if(!field)
	{
		ILGenOutOfMemory(info);
	}
	ILMemberSetSignature((ILMember *)field, elemType);

	/* If this is a native array, then also create fields for
	   the other elements, to force the runtime engine to make
	   the structure big enough to hold all of the elements */
	if(elemSize == CTYPE_DYNAMIC)
	{
		for(index = 1; index < size; ++index)
		{
			sprintf(sizeName, "elem__%lu", (unsigned long)index);
			field = ILFieldCreate(classInfo, 0, sizeName,
								  IL_META_FIELDDEF_PUBLIC |
								  IL_META_FIELDDEF_SPECIAL_NAME);
			if(!field)
			{
				ILGenOutOfMemory(info);
			}
			ILMemberSetSignature((ILMember *)field, elemType);
		}
	}

	/* Return the array type to the caller */
	return ILType_FromValueType(classInfo);
}

ILType *CTypeCreatePointer(ILGenInfo *info, ILType *refType, int nativePtr)
{
	ILType *type = ILTypeCreateRef(info->context, IL_TYPE_COMPLEX_PTR, refType);
	if(nativePtr)
	{
		/* Add a modifier to the type to indicate its native status */
		ILClass *classInfo;
		ILType *modifiers;
		classInfo = ILType_ToClass(ILFindNonSystemType
				(info, "IsNativePointer", "OpenSystem.Languages.C"));
		modifiers = ILTypeCreateModifier(info->context, 0,
										 IL_TYPE_COMPLEX_CMOD_OPT,
										 classInfo);
		if(!modifiers)
		{
			ILGenOutOfMemory(info);
		}
		type = ILTypeAddModifiers(info->context, modifiers, type);
	}
	return type;
}

ILType *CTypeCreateVaList(ILGenInfo *info)
{
	/* The base class library's "System.ArgIterator" class is the
	   underlying representation for "__builtin_va_list" */
	return ILFindSystemType(info, "ArgIterator");
}

ILType *CTypeCreateVoidPtr(ILGenInfo *info)
{
	static ILType *voidPtr = 0;
	if(!voidPtr)
	{
		voidPtr = CTypeCreatePointer(info, ILType_Void, 0);
	}
	return voidPtr;
}

ILType *CTypeCreateCharPtr(ILGenInfo *info)
{
	static ILType *charPtr = 0;
	if(!charPtr)
	{
		charPtr = CTypeCreatePointer(info, ILType_Int8, 0);
	}
	return charPtr;
}

ILType *CTypeAddConst(ILGenInfo *info, ILType *type)
{
	ILClass *classInfo;
	ILType *modifiers;
	classInfo = ILType_ToClass(ILFindNonSystemType
			(info, "IsConst", "OpenSystem.Languages.C"));
	if(ILTypeHasModifier(type, classInfo))
	{
		/* The type already has the specified modifier, so don't add again */
		return type;
	}
	else
	{
		/* Add a modifier prefix to the type */
		modifiers = ILTypeCreateModifier(info->context, 0,
										 IL_TYPE_COMPLEX_CMOD_OPT,
										 classInfo);
		if(!modifiers)
		{
			ILGenOutOfMemory(info);
		}
		return ILTypeAddModifiers(info->context, modifiers, type);
	}
}

ILType *CTypeAddVolatile(ILGenInfo *info, ILType *type)
{
	ILClass *classInfo;
	ILType *modifiers;
	classInfo = ILType_ToClass(ILFindNonSystemType
			(info, "IsVolatile", "System.Runtime.CompilerServices"));
	if(ILTypeHasModifier(type, classInfo))
	{
		/* The type already has the specified modifier, so don't add again */
		return type;
	}
	else
	{
		/* Add a modifier prefix to the type */
		modifiers = ILTypeCreateModifier(info->context, 0,
										 IL_TYPE_COMPLEX_CMOD_REQD,
										 classInfo);
		if(!modifiers)
		{
			ILGenOutOfMemory(info);
		}
		return ILTypeAddModifiers(info->context, modifiers, type);
	}
}

int CTypeAlreadyDefined(ILType *type)
{
	if(ILType_IsValueType(type))
	{
		return !(ILClassIsRef(ILType_ToValueType(type)));
	}
	else
	{
		return 0;
	}
}

ILType *CTypeDefineStruct(ILGenInfo *info, const char *name, int nativeLayout)
{
	ILType *type;
	ILClass *classInfo;
	ILClass *parent;

	/* Create the struct type reference, and bail out if already defined */
	type = CTypeCreateStruct(info, name);
	if(CTypeAlreadyDefined(type))
	{
		return 0;
	}

	/* Convert the reference into an actual class definition */
	classInfo = ILType_ToValueType(type);
	parent = ILType_ToClass(ILFindSystemType(info, "ValueType"));
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  ILClass_Name(classInfo), 0, parent);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Set the correct class attributes based on the native layout flag */
	if(nativeLayout)
	{
		/* Let the runtime engine determine how to lay out the fields */
		ILClassSetAttrs(classInfo, ~((ILUInt32)0),
						IL_META_TYPEDEF_PUBLIC |
						IL_META_TYPEDEF_SERIALIZABLE |
						IL_META_TYPEDEF_LAYOUT_SEQUENTIAL |
						IL_META_TYPEDEF_SEALED |
						IL_META_TYPEDEF_VALUE_TYPE);
	}
	else
	{
		/* We need to lay out the fields ourselves to ensure that
		   the structure looks the same on all platforms */
		ILClassSetAttrs(classInfo, ~((ILUInt32)0),
						IL_META_TYPEDEF_PUBLIC |
						IL_META_TYPEDEF_SERIALIZABLE |
						IL_META_TYPEDEF_EXPLICIT_LAYOUT |
						IL_META_TYPEDEF_SEALED |
						IL_META_TYPEDEF_VALUE_TYPE);

		/* The structure initially has a packing alignment of 1
		   and a total size of 0 */
		if(!ILClassLayoutCreate(info->image, 0, classInfo, 1, 0))
		{
			ILGenOutOfMemory(info);
		}
	}

	/* The struct definition is ready to go */
	return type;
}

ILType *CTypeDefineAnonStruct(ILGenInfo *info, ILType *parent,
							  const char *funcName, int nativeLayout)
{
	long number;
	ILNestedInfo *nested;
	ILClass *parentInfo;
	ILClass *classInfo;
	ILProgramItem *scope;
	char name[64];
	char *newName;
	ILUInt32 attrs;

	/* Get the number to assign to the anonymous structure */
	if(parent)
	{
		parentInfo = ILType_ToValueType(parent);
		number = 1;
		nested = 0;
		while((nested = ILClassNextNested(parentInfo, nested)) != 0)
		{
			++number;
		}
		scope = ILToProgramItem(parentInfo);
		attrs = IL_META_TYPEDEF_NESTED_PUBLIC;
	}
	else
	{
		number = (long)(ILImageNumTokens(info->image,
										 IL_META_TOKEN_TYPE_DEF) + 1);
		scope = ILClassGlobalScope(info->image);
		attrs = IL_META_TYPEDEF_NOT_PUBLIC;
	}

	/* Format the name of the structure type */
	if(funcName)
	{
		/* Format the name as "struct func(N)" */
		sprintf(name, "(%ld)", number);
		newName = ILDupString(funcName);
		if(!newName)
		{
			ILGenOutOfMemory(info);
		}
		newName = AppendThree(info, "struct ", newName, name);
	}
	else
	{
		/* Format the name as "struct (N)" */
		sprintf(name, "struct (%ld)", number);
		newName = ILDupString(name);
		if(!newName)
		{
			ILGenOutOfMemory(info);
		}
	}

	/* Create the anonymous structure */
	parentInfo = ILType_ToClass(ILFindSystemType(info, "ValueType"));
	classInfo = ILClassCreate(scope, 0, newName, 0, parentInfo);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Set the correct class attributes based on the native layout flag */
	if(nativeLayout)
	{
		/* Let the runtime engine determine how to lay out the fields */
		ILClassSetAttrs(classInfo, ~((ILUInt32)0),
						attrs |
						IL_META_TYPEDEF_SERIALIZABLE |
						IL_META_TYPEDEF_LAYOUT_SEQUENTIAL |
						IL_META_TYPEDEF_SEALED |
						IL_META_TYPEDEF_VALUE_TYPE);
	}
	else
	{
		/* We need to lay out the fields ourselves to ensure that
		   the structure looks the same on all platforms */
		ILClassSetAttrs(classInfo, ~((ILUInt32)0),
						attrs |
						IL_META_TYPEDEF_SERIALIZABLE |
						IL_META_TYPEDEF_EXPLICIT_LAYOUT |
						IL_META_TYPEDEF_SEALED |
						IL_META_TYPEDEF_VALUE_TYPE);

		/* The structure initially has a packing alignment of 1
		   and a total size of 0 */
		if(!ILClassLayoutCreate(info->image, 0, classInfo, 1, 0))
		{
			ILGenOutOfMemory(info);
		}
	}

	/* The struct definition is ready to go */
	ILFree(newName);
	return ILType_FromValueType(classInfo);
}

ILType *CTypeDefineUnion(ILGenInfo *info, const char *name, int nativeLayout)
{
	ILType *type;
	ILClass *classInfo;
	ILClass *parent;

	/* Create the union type reference, and bail out if already defined */
	type = CTypeCreateUnion(info, name);
	if(CTypeAlreadyDefined(type))
	{
		return 0;
	}

	/* Convert the reference into an actual class definition */
	classInfo = ILType_ToValueType(type);
	parent = ILType_ToClass(ILFindSystemType(info, "ValueType"));
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  ILClass_Name(classInfo), 0, parent);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Set the correct class attributes for the union */
	ILClassSetAttrs(classInfo, ~((ILUInt32)0),
					IL_META_TYPEDEF_PUBLIC |
					IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_EXPLICIT_LAYOUT |
					IL_META_TYPEDEF_SEALED |
					IL_META_TYPEDEF_VALUE_TYPE);
	if(!nativeLayout)
	{
		/* The union initially has a packing alignment of 1
		   and a total size of 0 */
		if(!ILClassLayoutCreate(info->image, 0, classInfo, 1, 0))
		{
			ILGenOutOfMemory(info);
		}
	}

	/* The union definition is ready to go */
	return type;
}

ILType *CTypeDefineAnonUnion(ILGenInfo *info, ILType *parent,
							 const char *funcName, int nativeLayout)
{
	long number;
	ILNestedInfo *nested;
	ILClass *parentInfo;
	ILClass *classInfo;
	ILProgramItem *scope;
	char name[64];
	char *newName;
	ILUInt32 attrs;

	/* Get the number to assign to the anonymous union */
	if(parent)
	{
		parentInfo = ILType_ToValueType(parent);
		number = 1;
		nested = 0;
		while((nested = ILClassNextNested(parentInfo, nested)) != 0)
		{
			++number;
		}
		scope = ILToProgramItem(parentInfo);
		attrs = IL_META_TYPEDEF_NESTED_PUBLIC;
	}
	else
	{
		number = (long)(ILImageNumTokens(info->image,
										 IL_META_TOKEN_TYPE_DEF) + 1);
		scope = ILClassGlobalScope(info->image);
		attrs = IL_META_TYPEDEF_NOT_PUBLIC;
	}

	/* Format the name of the union type */
	if(funcName)
	{
		/* Format the name as "union func(N)" */
		sprintf(name, "(%ld)", number);
		newName = ILDupString(funcName);
		if(!newName)
		{
			ILGenOutOfMemory(info);
		}
		newName = AppendThree(info, "union ", newName, name);
	}
	else
	{
		/* Format the name as "union (N)" */
		sprintf(name, "union (%ld)", number);
		newName = ILDupString(name);
		if(!newName)
		{
			ILGenOutOfMemory(info);
		}
	}

	/* Create the anonymous union */
	parentInfo = ILType_ToClass(ILFindSystemType(info, "ValueType"));
	classInfo = ILClassCreate(scope, 0, name, 0, parentInfo);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Set the correct class attributes for the union */
	ILClassSetAttrs(classInfo, ~((ILUInt32)0),
					attrs |
					IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_EXPLICIT_LAYOUT |
					IL_META_TYPEDEF_SEALED |
					IL_META_TYPEDEF_VALUE_TYPE);
	if(!nativeLayout)
	{
		/* The union initially has a packing alignment of 1
		   and a total size of 0 */
		if(!ILClassLayoutCreate(info->image, 0, classInfo, 1, 0))
		{
			ILGenOutOfMemory(info);
		}
	}

	/* The union definition is ready to go */
	return ILType_FromValueType(classInfo);
}

ILType *CTypeDefineEnum(ILGenInfo *info, const char *name)
{
	ILType *type;
	ILClass *classInfo;
	ILClass *parent;
	ILField *field;

	/* Create the enum type reference, and bail out if already defined */
	type = CTypeCreateEnum(info, name);
	if(CTypeAlreadyDefined(type))
	{
		return 0;
	}

	/* Convert the reference into an actual class definition */
	classInfo = ILType_ToValueType(type);
	parent = ILType_ToClass(ILFindSystemType(info, "Enum"));
	classInfo = ILClassCreate(ILClassGlobalScope(info->image), 0,
							  ILClass_Name(classInfo), 0, parent);
	if(!classInfo)
	{
		ILGenOutOfMemory(info);
	}

	/* Set the attributes on the enumerated type correctly */
	ILClassSetAttrs(classInfo, ~((ILUInt32)0),
					IL_META_TYPEDEF_PUBLIC |
					IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_SEALED);

	/* Add the "value__" field to hold the enum's value */
	field = ILFieldCreate(classInfo, 0, "value__",
						  IL_META_FIELDDEF_PUBLIC |
						  IL_META_FIELDDEF_SPECIAL_NAME |
						  IL_META_FIELDDEF_RT_SPECIAL_NAME);
	if(!field)
	{
		ILGenOutOfMemory(info);
	}
	ILMemberSetSignature((ILMember *)field, ILType_Int32);

	/* The enum definition is ready to go */
	return type;
}

ILField *CTypeDefineField(ILGenInfo *info, ILType *structType,
					 	  const char *fieldName, ILType *fieldType)
{
	ILClass *classInfo = ILType_ToValueType(structType);
	ILClassLayout *layout = ILClassLayoutGetFromOwner(classInfo);
	ILUInt32 size, align;
	ILUInt32 classSize, offset;
	int isUnion = (strncmp(ILClass_Name(classInfo), "union ", 6) == 0);
	ILField *field;

	/* Determine the size and alignment of the new field */
	size = TypeSizeAndAlign(fieldType, &align, 1);

	/* Create the new field */
	field = ILFieldCreate(classInfo, 0, fieldName, IL_META_FIELDDEF_PUBLIC);
	if(!field)
	{
		ILGenOutOfMemory(info);
	}

	/* Perform explicit layout on the field */
	if(layout)
	{
		classSize = ILClassLayoutGetClassSize(layout);
		if(isUnion)
		{
			offset = 0;
		}
		else
		{
			offset = classSize;
		}
		if((offset % align) != 0)
		{
			offset += align - (offset % align);
		}
		if(!ILFieldLayoutCreate(info->image, 0, field, offset))
		{
			ILGenOutOfMemory(info);
		}
		offset += size;
		if(offset > classSize)
		{
			ILClassLayoutSetClassSize(layout, classSize);
		}
		if(align > ILClassLayoutGetPackingSize(layout))
		{
			ILClassLayoutSetPackingSize(layout, align);
		}
	}
	else if(isUnion)
	{
		if(!ILFieldLayoutCreate(info->image, 0, field, 0))
		{
			ILGenOutOfMemory(info);
		}
	}

	/* Return the final field to the caller */
	return field;
}

ILField *CTypeDefineBitField(ILGenInfo *info, ILType *structType,
					 	     const char *fieldName, ILType *fieldType,
							 ILUInt32 numBits)
{
	/* TODO */
	return 0;
}

void CTypeEndStruct(ILGenInfo *info, ILType *structType)
{
	ILClass *classInfo = ILType_ToValueType(structType);
	ILClassLayout *layout = ILClassLayoutGetFromOwner(classInfo);
	ILUInt32 size, align;
	if(layout != 0)
	{
		size = ILClassLayoutGetClassSize(layout);
		align = ILClassLayoutGetPackingSize(layout);
		if((size % align) != 0)
		{
			size += align - (size % align);
			ILClassLayoutSetClassSize(layout, size);
		}
	}
}

ILType *CTypeWithoutQuals(ILType *type)
{
	/* Qualifiers are stored in the IL type as custom modifiers */
	return ILTypeStripPrefixes(type);
}

static int CheckForModifier(ILType *type, const char *name,
						    const char *namespace)
{
	ILClass *classInfo;
	while(type != 0 && ILType_IsComplex(type))
	{
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_CMOD_OPT ||
		   ILType_Kind(type) == IL_TYPE_COMPLEX_CMOD_REQD)
		{
			classInfo = type->un.modifier__.info__;
			if(!strcmp(ILClass_Name(classInfo), name) &&
			   ILClass_Namespace(classInfo) != 0 &&
			   !strcmp(ILClass_Namespace(classInfo), namespace))
			{
				return 1;
			}
			type = type->un.modifier__.type__;
		}
		else
		{
			break;
		}
	}
	return 0;
}

int CTypeIsConst(ILType *type)
{
	return CheckForModifier(type, "IsConst", "OpenSystem.Languages.C");
}

int CTypeIsVolatile(ILType *type)
{
	return CheckForModifier(type, "IsVolatile",
							"System.Runtime.CompilerServices");
}

int CTypeIsStruct(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "struct ", 7))
		{
			return 1;
		}
	}
	return 0;
}

int CTypeIsUnion(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "union ", 6))
		{
			return 1;
		}
	}
	return 0;
}

int CTypeIsEnum(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "enum ", 5))
		{
			return 1;
		}
	}
	return 0;
}

int CTypeIsArray(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		if(!strncmp(ILClass_Name(ILType_ToValueType(type)), "array ", 6))
		{
			return 1;
		}
	}
	return 0;
}

int CTypeIsPointer(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(type != 0 && ILType_IsComplex(type) &&
	   ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int CTypeIsNativePointer(ILType *type)
{
	return CheckForModifier(type, "IsNativePointer", "OpenSystem.Languages.C");
}

int CTypeIsForeign(ILType *type)
{
	const char *name;
	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		name = ILClass_Name(ILType_ToValueType(type));
		if(!strncmp(name, "struct ", 7) ||
		   !strncmp(name, "union ", 6) ||
		   !strncmp(name, "enum ", 5) ||
		   !strncmp(name, "array ", 6))
		{
			/* This is a specially-constructed value type of our own */
			return 0;
		}
		else if(!strcmp(name, "ArgIterator"))
		{
			/* The "System.ArgIterator" class corresponds to C's
			   "__builtin_va_list" type */
			name = ILClass_Namespace(ILType_ToValueType(type));
			if(name && !strcmp(name, "System"))
			{
				return 0;
			}
		}
	}
	else if(ILType_IsPrimitive(type))
	{
		/* Most primitive types are known to C */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
			case IL_META_ELEMTYPE_R4:
			case IL_META_ELEMTYPE_R8:
			case IL_META_ELEMTYPE_R:
			{
				return 0;
			}
			/* Not reached */
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_PTR ||
		   ILType_IsMethod(type))
		{
			/* Pointer and method types are known to C */
			return 0;
		}
	}
	return 1;
}

ILUInt32 CTypeGetNumElems(ILType *type)
{
	ILClass *classInfo;
	ILField *field;
	ILType *elemType;
	ILUInt32 highest;
	ILUInt32 current;
	ILUInt32 elemSize;
	ILClassLayout *layout;
	const char *num;

	/* Strip the prefixes and check that this is actually an array type */
	type = ILTypeStripPrefixes(type);
	if(!CTypeIsArray(type))
	{
		return 0;
	}

	/* Search for the "elem__" field within the array type and find
	   the highest-numbered field that begins with "elem__" */
	classInfo = ILType_ToValueType(type);
	field = 0;
	highest = 0;
	elemType = ILType_Invalid;
	while((field = (ILField *)ILClassNextMemberByKind
				(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if(!strcmp(ILField_Name(field), "elem__"))
		{
			/* If the field is static, then the array size is zero */
			if(ILField_IsStatic(field))
			{
				return 0;
			}
			elemType = ILField_Type(field);
		}
		else if(!strncmp(ILField_Name(field), "elem__", 6))
		{
			num = ILField_Name(field) + 6;
			current = 0;
			while(*num >= '0' && *num <= '9')
			{
				current = current * 10 + (ILUInt32)(*num++ - '0');
			}
			if(current > highest)
			{
				highest = current;
			}
		}
	}
	if(elemType == ILType_Invalid)
	{
		return 0;
	}

	/* Determine the size */
	layout = ILClassLayoutGetFromOwner(classInfo);
	if(layout != 0)
	{
		/* The size is determined from the class and element sizes */
		elemSize = TypeSizeAndAlign(elemType, 0, 1);
		if(elemSize == 0)
		{
			/* Avoid divide by zero errors when the element type is empty */
			return 0;
		}
		else
		{
			return (ILClassLayoutGetClassSize(layout) / elemSize);
		}
	}
	else
	{
		/* The size is the highest index plus one */
		return highest + 1;
	}
}

ILType *CTypeGetElemType(ILType *type)
{
	ILClass *classInfo;
	ILField *field;

	/* Strip the prefixes and check that this is actually an array type */
	type = ILTypeStripPrefixes(type);
	if(!CTypeIsArray(type))
	{
		return 0;
	}

	/* Search for the "elem__" field within the array type */
	classInfo = ILType_ToValueType(type);
	field = 0;
	while((field = (ILField *)ILClassNextMemberByKind
				(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if(!strcmp(ILField_Name(field), "elem__"))
		{
			return ILField_Type(field);
		}
	}
	return 0;
}

ILType *CTypeGetPtrRef(ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(type != 0 && ILType_IsComplex(type) &&
	   ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
	{
		return ILType_Ref(type);
	}
	else
	{
		return 0;
	}
}

/*
 * Size and alignment values for the primitive types.
 */
#define	SIZE_INT8				1
#define	ALIGN_INT8				1
#define	SIZE_INT16				2
#define	ALIGN_INT16				2
#define	SIZE_INT32				4
#define	ALIGN_INT32				4
#define	SIZE_INT64				8
#define	ALIGN_INT64				8
#define	SIZE_NATIVE_INT32		4
#define	ALIGN_NATIVE_INT32		4
#define	SIZE_NATIVE_INT64		8
#define	ALIGN_NATIVE_INT64		8
#define	SIZE_FLOAT32			4
#define	ALIGN_FLOAT32			4
#define	SIZE_FLOAT64			8
#define	ALIGN_FLOAT64			8
#define	SIZE_NATIVE_FLOAT		16
#define	ALIGN_NATIVE_FLOAT		16
#define	SIZE_PTR32				4
#define	ALIGN_PTR32				4
#define	SIZE_PTR64				8
#define	ALIGN_PTR64				8

/*
 * Compute the size and alignment of a type.  If "force" is
 * zero, then this function will return CTYPE_DYNAMIC if the type's
 * size must be determined at runtime.  If "force" is non-zero,
 * then the type's compile time size is returned regardless.
 */
static ILUInt32 TypeSizeAndAlign(ILType *_type, ILUInt32 *align, int force)
{
	ILType *type = ILTypeStripPrefixes(_type);
	ILUInt32 alignTemp;
	ILType *subType;
	ILClass *classInfo;
	ILClassLayout *layout;

	if(!align)
	{
		align = &alignTemp;
	}
	if(ILType_IsPrimitive(type))
	{
		/* Decode the primitive type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			{
				*align = ALIGN_INT8;
				return SIZE_INT8;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			{
				*align = ALIGN_INT16;
				return SIZE_INT16;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
			{
				*align = ALIGN_INT32;
				return SIZE_INT32;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
			{
				*align = ALIGN_INT64;
				return SIZE_INT64;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
			{
				if(gen_32bit_only)
				{
					*align = ALIGN_NATIVE_INT32;
					return SIZE_NATIVE_INT32;
				}
				else if(force)
				{
					*align = ALIGN_NATIVE_INT64;
					return SIZE_NATIVE_INT64;
				}
				else
				{
					*align = ALIGN_NATIVE_INT64;
					return CTYPE_DYNAMIC;
				}
			}
			/* Not reached */

			case IL_META_ELEMTYPE_R4:
			{
				*align = ALIGN_FLOAT32;
				return SIZE_FLOAT32;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_R8:
			{
				*align = ALIGN_FLOAT64;
				return SIZE_FLOAT64;
			}
			/* Not reached */

			case IL_META_ELEMTYPE_R:
			{
				*align = ALIGN_NATIVE_FLOAT;
				if(force)
				{
					return SIZE_NATIVE_FLOAT;
				}
				else
				{
					return CTYPE_DYNAMIC;
				}
			}
			/* Not reached */
		}
		*align = ALIGN_INT32;
		return SIZE_INT32;
	}
	else if(ILType_IsValueType(type))
	{
		/* Check for enumerated types */
		if((subType = ILTypeGetEnumType(type)) != 0)
		{
			/* Use the size of the type underlying the enumeration */
			return TypeSizeAndAlign(subType, align, force);
		}

		/* Bail out if this is a struct or union with unknown size */
		classInfo = ILClassResolve(ILType_ToClass(type));
		if(ILClassIsRef(classInfo))
		{
			*align = 1;
			return CTYPE_DYNAMIC;
		}

		/* Look for explicit size and alignment values on the class */
		layout = ILClassLayoutGetFromOwner(classInfo);
		if(layout != 0)
		{
			*align = ILClassLayoutGetPackingSize(layout);
			return ILClassLayoutGetClassSize(layout);
		}

		/* This is a native structure or union: bail out if not forced */
		if(!force)
		{
			*align = 1;
			return CTYPE_DYNAMIC;
		}

		/* Add up the sizes of all non-static fields in the class,
		   and guess its most likely alignment */
		/* TODO */
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		/* Check for pointer and method pointer types */
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_PTR ||
		   (ILType_Kind(type) & IL_TYPE_COMPLEX_METHOD) != 0)
		{
			if(gen_32bit_only)
			{
				*align = ALIGN_PTR32;
				return SIZE_PTR32;
			}
			else if(force || !CheckForModifier(_type, "IsNativePointer",
										       "OpenSystem.Languages.C"))
			{
				*align = ALIGN_PTR64;
				return SIZE_PTR64;
			}
			else
			{
				*align = ALIGN_PTR64;
				return CTYPE_DYNAMIC;
			}
		}
	}

	/* Assume that everything else is the same size as a native pointer */
	if(gen_32bit_only)
	{
		*align = ALIGN_PTR32;
		return SIZE_PTR32;
	}
	else if(force)
	{
		*align = ALIGN_PTR64;
		return SIZE_PTR64;
	}
	else
	{
		*align = ALIGN_PTR64;
		return CTYPE_DYNAMIC;
	}
}

ILUInt32 CTypeSizeAndAlign(ILType *type, ILUInt32 *align)
{
	return TypeSizeAndAlign(type, align, 0);
}

/*
 * Append three strings, where the middle one is realloc'able.
 */
static char *AppendThree(ILGenInfo *info, const char *prefix,
						 char *str, const char *suffix)
{
	int prefixLen = (prefix ? strlen(prefix) : 0);
	int suffixLen = (suffix ? strlen(suffix) : 0);
	char *result;
	if(prefixLen)
	{
		result = (char *)ILMalloc(strlen(str) + prefixLen + suffixLen);
		if(!result)
		{
			ILGenOutOfMemory(info);
		}
		strcpy(result, prefix);
		strcat(result, str);
		if(suffixLen)
		{
			strcat(result, suffix);
		}
		ILFree(str);
	}
	else
	{
		result = (char *)ILRealloc(str, strlen(str) + prefixLen + suffixLen);
		if(!result)
		{
			ILGenOutOfMemory(info);
		}
		if(suffixLen)
		{
			strcat(result, suffix);
		}
	}
	return result;
}

char *CTypeToName(ILGenInfo *info, ILType *type)
{
	const char *cname;
	char *name;
	ILType *stripped;
	int modFlags;
	static const char * const beforeModifiers[] =
		{0, "const ", "volatile ", "const volatile "};
	static const char * const afterModifiers[] =
		{0, " const", " volatile", " const volatile"};

	/* Determine what kind of C type we have */
	if(ILType_IsPrimitive(type))
	{
		/* Recognise the primitive C types */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:		cname = "void"; break;
			case IL_META_ELEMTYPE_BOOLEAN:	cname = "__bool__"; break;
			case IL_META_ELEMTYPE_I1:		cname = "char"; break;
			case IL_META_ELEMTYPE_U1:		cname = "unsigned char"; break;
			case IL_META_ELEMTYPE_I2:		cname = "short"; break;
			case IL_META_ELEMTYPE_U2:		cname = "unsigned short"; break;
			case IL_META_ELEMTYPE_CHAR:		cname = "__wchar__"; break;
			case IL_META_ELEMTYPE_I4:		cname = "int"; break;
			case IL_META_ELEMTYPE_U4:		cname = "unsigned int"; break;
			case IL_META_ELEMTYPE_I:		cname = "__nint__"; break;
			case IL_META_ELEMTYPE_U:		cname = "unsigned __nint__"; break;

			case IL_META_ELEMTYPE_I8:
			{
				if(gen_32bit_only)
					cname = "long long";
				else
					cname = "long";
			}
			break;

			case IL_META_ELEMTYPE_U8:
			{
				if(gen_32bit_only)
					cname = "unsigned long long";
				else
					cname = "unsigned long";
			}
			break;

			case IL_META_ELEMTYPE_R4:		cname = "float"; break;
			case IL_META_ELEMTYPE_R8:		cname = "double"; break;
			case IL_META_ELEMTYPE_R:		cname = "long double"; break;
			default:						cname = 0; break;
		}
		if(cname)
		{
			name = ILDupString(cname);
			if(!name)
			{
				ILGenOutOfMemory(info);
			}
			return name;
		}
	}
	else if(ILType_IsValueType(type))
	{
		/* Recognise C value types */
		cname = ILClass_Name(ILType_ToValueType(type));
		if(!strncmp(cname, "struct ", 7) ||
		   !strncmp(cname, "union ", 6) ||
		   !strncmp(cname, "enum ", 5))
		{
			name = ILDupString(cname);
			if(!name)
			{
				ILGenOutOfMemory(info);
			}
			return name;
		}
		else if(!strncmp(cname, "array ", 6))
		{
			/* TODO: C array types */
		}
		else if(!strcmp(cname, "ArgIterator"))
		{
			cname = ILClass_Namespace(ILType_ToValueType(type));
			if(cname != 0 && !strcmp(cname, "System"))
			{
				/* "System.ArgIterator" is known to C as "__builtin_va_list" */
				name = ILDupString("__builtin_va_list");
				if(!name)
				{
					ILGenOutOfMemory(info);
				}
				return name;
			}
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		/* Recognise complex C types */
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
		{
			/* Convert a pointer type */
			return AppendThree
				(info, 0, CTypeToName(info, ILType_Ref(type)), " *");
		}
		else if(ILType_Kind(type) == IL_TYPE_COMPLEX_CMOD_OPT ||
		        ILType_Kind(type) == IL_TYPE_COMPLEX_CMOD_REQD)
		{
			/* Look for "const", "volatile", and "__native__" qualifiers */
			stripped = ILTypeStripPrefixes(type);
			modFlags = 0;
			if(CheckForModifier(type, "IsConst",
							    "OpenSystem.Languages.C"))
			{
				modFlags |= 1;
			}
			if(CheckForModifier(type, "IsVolatile",
						        "System.Runtime.CompilerServices"))
			{
				modFlags |= 2;
			}
			if(CheckForModifier(type, "IsNativePointer",
							    "OpenSystem.Languages.C") &&
			   CTypeIsPointer(stripped))
			{
				name = AppendThree(info, 0,
								   CTypeToName(info, ILType_Ref(stripped)),
								   " __native__ *");
			}
			else
			{
				name = CTypeToName(info, stripped);
			}
			if(modFlags != 0)
			{
				if(CTypeIsPointer(stripped) || CTypeIsArray(stripped))
				{
					/* Put the qualifiers after the type, not before */
					name = AppendThree(info, 0, name,
									   afterModifiers[modFlags]);
				}
				else
				{
					/* Put the qualifiers before the type */
					return AppendThree(info, beforeModifiers[modFlags],
									   name, 0);
				}
			}
			return name;
		}
		else if(ILType_IsMethod(type))
		{
			/* Convert a method pointer type */
			/* TODO */
		}
	}

	/* If we get here, then the type is foreign to C */
	name = ILTypeToName(type);
	if(!name)
	{
		ILGenOutOfMemory(info);
	}
	return name;
}

void CTypeMarkForOutput(ILGenInfo *info, ILType *type)
{
	ILClass *classInfo;
	const char *name;
	unsigned long param;
	unsigned long numParams;

	type = ILTypeStripPrefixes(type);
	if(ILType_IsValueType(type))
	{
		classInfo = ILType_ToValueType(type);
		name = ILClass_Name(classInfo);
		if(!strncmp(name, "struct ", 7) ||
		   !strncmp(name, "union ", 6) ||
		   !strncmp(name, "enum ", 5) ||
		   !strncmp(name, "array ", 6))
		{
			if(!ILClassIsComplete(classInfo))
			{
				/* TODO: register the class to be expanded later */
			}
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		if(ILType_Kind(type) == IL_TYPE_COMPLEX_PTR)
		{
			/* Mark the referenced type */
			CTypeMarkForOutput(info, ILType_Ref(type));
		}
		else if((ILType_Kind(type) & IL_TYPE_COMPLEX_METHOD) != 0)
		{
			/* Mark the return and parameter types */
			CTypeMarkForOutput(info, ILTypeGetReturn(type));
			numParams = ILTypeNumParams(type);
			for(param = 1; param <= numParams; ++param)
			{
				CTypeMarkForOutput(info, ILTypeGetParam(type, param));
			}
		}
	}
}

void CTypeOutputPending(ILGenInfo *info, FILE *stream)
{
	/* TODO */
}

#ifdef	__cplusplus
};
#endif
