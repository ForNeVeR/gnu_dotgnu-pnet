/*
 * layout.c - Type and object layout algorithms.
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

#include "engine_private.h"
#include "../libffi/include/ffi.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Information that results from laying out a type.
 */
typedef struct
{
	ILUInt32	size;
	ILUInt32	alignment;
	ILUInt32	vtableSize;
	ILMethod  **vtable;
	ILUInt32	staticSize;

} LayoutInfo;

/*
 * Perform a security check (TODO: replace this with something better).
 */
int _ILSecurityCheck(ILProgramItem *info, int type)
{
	return 0;
}

/*
 * Forward declaration.
 */
static int LayoutClass(ILClass *info, LayoutInfo *layout);

/*
 * Get the layout information for a type.  Returns zero
 * if there is something wrong with the type.
 */
static int LayoutType(ILType *type, LayoutInfo *layout)
{
	if(ILType_IsPrimitive(type))
	{
		/* Lay out a primitive type */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_BOOLEAN:
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			{
				layout->size = 1;
				layout->alignment = 1;
			}
			break;

			case IL_META_ELEMTYPE_CHAR:
			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			{
				layout->size = 2;
				layout->alignment = 2;
			}
			break;

			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
			case IL_META_ELEMTYPE_R4:
			{
				layout->size = 4;
				layout->alignment = 4;
			}
			break;

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
			{
				layout->size = ffi_type_uint64.size;
				layout->alignment = ffi_type_uint64.alignment;
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				layout->size = ffi_type_double.size;
				layout->alignment = ffi_type_double.alignment;
			}
			break;

			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
			{
			#ifdef IL_NATIVE_INT64
				layout->size = ffi_type_uint64.size;
				layout->alignment = ffi_type_uint64.alignment;
			#else
				layout->size = 4;
				layout->alignment = 4;
			#endif
			}
			break;

			case IL_META_ELEMTYPE_R:
			{
			#ifdef IL_NATIVE_FLOAT
				layout->size = ffi_type_longdouble.size;
				layout->alignment = ffi_type_longdouble.alignment;
			#else
				layout->size = ffi_type_double.size;
				layout->alignment = ffi_type_double.alignment;
			#endif
			}
			break;

			default: return 0;
		}
		layout->vtableSize = 0;
		layout->vtable = 0;
		layout->staticSize = 0;
		return 1;
	}
	else if(ILType_IsValueType(type))
	{
		/* Lay out a value type by getting the full size and alignment
		   of the class that underlies the value type */
		return LayoutClass(ILType_ToValueType(type), layout);
	}
	else
	{
		/* Everything else is laid out as a pointer */
		layout->size = ffi_type_pointer.size;
		layout->alignment = ffi_type_pointer.alignment;
		layout->vtableSize = 0;
		layout->vtable = 0;
		layout->staticSize = 0;
		return 1;
	}
}

/*
 * Find the virtual ancestor for a method.
 * Returns NULL if no ancestor found.
 */
static ILMethod *FindVirtualAncestor(ILClass *scope, ILClass *info,
									 ILMethod *testMethod)
{
	ILMethod *method;
	while(info != 0)
	{
		method = 0;
		while((method = (ILMethod *)ILClassNextMemberByKind
					(info, (ILMember *)method, IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if((method->member.attributes & IL_META_METHODDEF_VIRTUAL) != 0 &&
			   !strcmp(method->member.name, testMethod->member.name) &&
			   ILTypeIdentical(method->member.signature,
			   				   testMethod->member.signature))
			{
				/* If the ancestor is not accessible from the original class,
				   then allocate a new vtable slot for the method */
				if(ILMemberAccessible(&(method->member), scope))
				{
					return method;
				}
				else
				{
					return 0;
				}
			}
		}
		info = ILClassGetParent(info);
	}
	return 0;
}

/*
 * Compute the method table for a class's interface.
 */
static int ComputeInterfaceTable(ILClass *info, ILImplements *implements,
								 ILClass *parent)
{
	return 1;
}

/*
 * Lay out a particular class.  Returns zero if there
 * is something wrong with the class definition.
 */
static int LayoutClass(ILClass *info, LayoutInfo *layout)
{
	ILClassLayout *classLayout;
	ILFieldLayout *fieldLayout;
	ILFieldRVA *fieldRVA;
	LayoutInfo typeLayout;
	ILUInt32 maxAlignment;
	ILUInt32 packingSize;
	ILUInt32 explicitSize;
	int allowFieldLayout;
	int allowRVALayout;
	ILClassPrivate *classPrivate;
	ILField *field;
	ILMethod *method;
	ILMethod *ancestor;
	ILMethod **vtable;
	ILClass *parent;
	ILImplements *implements;

	/* Determine if we have already tried to lay out this class */
	if(info->userData)
	{
		/* We have layout data, so return it */
		classPrivate = (ILClassPrivate *)(info->userData);
		if(classPrivate->inLayout)
		{
			/* We have detected a layout loop.  This can occur if a
			   class attempts to include itself in a value type field */
			return 0;
		}
		else
		{
			layout->size = classPrivate->size;
			layout->alignment = classPrivate->alignment;
			layout->vtableSize = classPrivate->vtableSize;
			layout->vtable = classPrivate->vtable;
			layout->staticSize = classPrivate->staticSize;
			return 1;
		}
	}
	else
	{
		/* Create a new layout record and attach it to the class */
		classPrivate = (ILClassPrivate *)
			(ILMemStackAlloc(&(info->programItem.image->memStack),
							 ILClassPrivate));
		if(!classPrivate)
		{
			return 0;
		}
		info->userData = (void *)classPrivate;
		classPrivate->inLayout = 1;
	}

	/* Lay out the parent class first */
	if(info->parent)
	{
		/* Use "ILClassGetParent" to resolve cross-image links */
		parent = ILClassGetParent(info);
		if(!LayoutClass(parent, layout))
		{
			info->userData = 0;
			return 0;
		}
	}
	else
	{
		/* This is a top-level class (normally "System.Object") */
		parent = 0;
		layout->size = 0;
		layout->alignment = 1;
		layout->vtableSize = 0;
	}

	/* Zero the static size, which must be recomputed for each class */
	layout->staticSize = 0;

	/* Lay out the interfaces that this class implements */
	implements = info->implements;
	while(implements != 0)
	{
		if(!LayoutClass(ILClassResolve(implements->interface), &typeLayout))
		{
			info->userData = 0;
			return 0;
		}
		implements = implements->nextInterface;
	}

	/* Should we use the explicit layout algorithm? */
	packingSize = 0;
	explicitSize = 0;
	allowFieldLayout = 0;
	allowRVALayout = _ILSecurityCheck((ILProgramItem *)info,
									  IL_SECURITY_LAYOUT);
	if((info->attributes & IL_META_TYPEDEF_LAYOUT_MASK) ==
			IL_META_TYPEDEF_EXPLICIT_LAYOUT)
	{
		/* Check that security permissions allow explicit layout */
		if(allowRVALayout)
		{
			/* Look for class layout information to specify the size */
			classLayout = ILClassLayoutGetFromOwner(info);
			if(classLayout)
			{
				/* Validate the class packing size */
				if(classLayout->packingSize != 0 &&
				   classLayout->packingSize != 1 &&
				   classLayout->packingSize != 2 &&
				   classLayout->packingSize != 4 &&
				   classLayout->packingSize != 8)
				{
					info->userData = 0;
					return 0;
				}
				packingSize = classLayout->packingSize;

				/* Record the explicit size to be used later */
				explicitSize = classLayout->classSize;

				/* Field layout is permitted */
				allowFieldLayout = 1;
			}
		}
	}

	/* Use straight-forward field allocation, which will usually
	   match the algorithm used by the platform C compiler */
	field = 0;
	maxAlignment = 1;
	while((field = (ILField *)ILClassNextMemberByKind
			(info, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if((field->member.attributes & IL_META_FIELDDEF_STATIC) == 0)
		{
			/* Get the layout information for this field's type */
			if(!LayoutType(field->member.signature, &typeLayout))
			{
				info->userData = 0;
				return 0;
			}

			/* Decrease the alignment if we have an explicit packing size */
			if(packingSize != 0 && packingSize < typeLayout.alignment)
			{
				typeLayout.alignment = packingSize;
			}

			/* Use an explicit field offset if necessary */
			if(allowFieldLayout &&
			   (fieldLayout = ILFieldLayoutGetFromOwner(field)) != 0)
			{
				/* Record the explicit field offset */
				field->offset = fieldLayout->offset;

				/* Extend the default class size to include the field */
				if((field->offset + typeLayout.size) > layout->size)
				{
					layout->size = field->offset + typeLayout.size;
				}
			}
			else
			{
				/* Align the field on an appropriate boundary */
				if((layout->size % typeLayout.alignment) != 0)
				{
					layout->size += typeLayout.alignment -
						(layout->size % typeLayout.alignment);
				}

				/* Record the field's offset and advance past it */
				field->offset = layout->size;
				layout->size += typeLayout.size;
			}

			/* Update the maximum alignment */
			if(typeLayout.alignment > maxAlignment)
			{
				maxAlignment = typeLayout.alignment;
			}
		}
		else if((field->member.attributes & IL_META_FIELDDEF_LITERAL) == 0)
		{
			/* Lay out a static field */
			fieldRVA = ILFieldRVAGetFromOwner(field);
			if(fieldRVA && !allowRVALayout)
			{
				/* RVA fields are not permitted, so remove the attribute */
				field->member.attributes &= ~IL_META_FIELDDEF_HAS_FIELD_RVA;
			}
			if(!fieldRVA || !allowRVALayout)
			{
				/* Get the layout information for this field's type */
				if(!LayoutType(field->member.signature, &typeLayout))
				{
					info->userData = 0;
					return 0;
				}

				/* Align the field on an appropriate boundary */
				if((layout->staticSize % typeLayout.alignment) != 0)
				{
					layout->staticSize += typeLayout.alignment -
						(layout->staticSize % typeLayout.alignment);
				}

				/* Record the field's offset and advance past it */
				field->offset = layout->staticSize;
				layout->staticSize += typeLayout.size;
			}
		}
	}

	/* Compute the final class size based on explicit sizes and alignment */
	if(maxAlignment > layout->alignment)
	{
		layout->alignment = maxAlignment;
	}
	if(explicitSize > layout->size)
	{
		layout->size = explicitSize;
	}
	else if((layout->size % layout->alignment) != 0)
	{
		layout->size += layout->alignment -
				(layout->size % layout->alignment);
	}

	/* Allocate vtable slots to the virtual methods in this class */
	method = 0;
	explicitSize = layout->vtableSize;
	while((method = (ILMethod *)ILClassNextMemberByKind
				(info, (ILMember *)method, IL_META_MEMBERKIND_METHOD)) != 0)
	{
		/* Skip this method if it isn't virtual */
		if((method->member.attributes & IL_META_METHODDEF_VIRTUAL) == 0)
		{
			continue;
		}

		/* Do we need a new slot for this method? */
		if((method->member.attributes & IL_META_METHODDEF_NEW_SLOT) != 0)
		{
			/* Allocate a vtable slot */
			method->index = layout->vtableSize;
			++(layout->vtableSize);
		}
		else
		{
			/* Find the method in an ancestor class that this one overrides */
			ancestor = FindVirtualAncestor(info, parent, method);
			if(ancestor)
			{
				/* Use the same index as the ancestor */
				method->index = ancestor->index;
			}
			else
			{
				/* No ancestor, so allocate a new slot.  This case is
				   quite rare and will typically only happen with code
				   that has been loaded from a Java .class file, or
				   where the ancestor method is not accessible due to
				   permission issues */
				method->member.attributes |= IL_META_METHODDEF_NEW_SLOT;
				method->index = layout->vtableSize;
				++(layout->vtableSize);
			}
		}
	}

	/* If the vtable has grown too big, then bail out */
	if(layout->vtableSize > (ILUInt32)65535)
	{
		info->userData = 0;
		return 0;
	}

	/* Allocate the vtable and copy the parent's vtable into it */
	if((vtable = (ILMethod **)
			ILMemStackAllocItem(&(info->programItem.image->memStack),
						        layout->vtableSize * sizeof(ILMethod *))) == 0)
	{
		info->userData = 0;
		return 0;
	}
	if(explicitSize > 0)
	{
		ILMemCpy(vtable, layout->vtable, explicitSize * sizeof(ILMethod *));
	}

	/* Override the vtable slots with this class's method implementations */
	method = 0;
	while((method = (ILMethod *)ILClassNextMemberByKind
				(info, (ILMember *)method, IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if((method->member.attributes & IL_META_METHODDEF_VIRTUAL) != 0)
		{
			vtable[method->index] = method;
		}
	}

	/* Compute the interface tables for this class */
	implements = info->implements;
	while(implements != 0)
	{
		parent = ILClassResolve(implements->interface);
		if(parent->userData &&
		   ((ILClassPrivate *)(parent->userData))->vtableSize)
		{
			if(!ComputeInterfaceTable(info, implements, parent))
			{
				info->userData = 0;
				return 0;
			}
		}
		implements = implements->nextInterface;
	}

	/* Record the layout information for this class */
	classPrivate->size = layout->size;
	classPrivate->staticSize = layout->staticSize;
	classPrivate->alignment = layout->alignment;
	classPrivate->vtableSize = layout->vtableSize;
	classPrivate->vtable = vtable;
	classPrivate->inLayout = 0;
	layout->vtable = vtable;

	/* Done */
	return 1;
}

int _ILLayoutClass(ILClass *info)
{
	LayoutInfo layout;
	return LayoutClass(info, &layout);
}

ILUInt32 ILSizeOfType(ILType *type)
{
	LayoutInfo layout;
	if(!LayoutType(type, &layout))
	{
		return 0;
	}
	else
	{
		return layout.size;
	}
}

#ifdef	__cplusplus
};
#endif
