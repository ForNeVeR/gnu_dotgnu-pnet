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
#include "il_opcodes.h"
#if defined(HAVE_LIBFFI)
#include "ffi.h"
#else
#include "il_align.h"
#endif

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
	ILUInt32	nativeSize;
	ILUInt32	nativeAlignment;
	ILUInt32	vtableSize;
	ILMethod  **vtable;
	ILUInt32	staticSize;
	int			hasFinalizer;
	int			managedInstance;
	int			managedStatic;

} LayoutInfo;

/*
 * Forward declaration.
 */
static int LayoutClass(ILExecProcess *process, ILClass *info, LayoutInfo *layout);

/*
 * Get the layout information for a type.  Returns zero
 * if there is something wrong with the type.
 */
static int LayoutType(ILExecProcess *process, ILType *type, LayoutInfo *layout)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsPrimitive(type))
	{
		/* Lay out a primitive type */
		layout->managedInstance = 0;
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
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
		#endif
			{
				layout->size = 4;
				layout->alignment = 4;
			}
			break;

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
		#ifdef IL_NATIVE_INT64
			case IL_META_ELEMTYPE_I:
			case IL_META_ELEMTYPE_U:
		#endif
			{
			#if defined(HAVE_LIBFFI)
				layout->size = ffi_type_uint64.size;
				layout->alignment = ffi_type_uint64.alignment;
			#else
				layout->size = sizeof(ILInt64);
				layout->alignment = _IL_ALIGN_FOR_TYPE(long);
			#endif
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
			#if defined(HAVE_LIBFFI)
				layout->size = ffi_type_float.size;
				layout->alignment = ffi_type_float.alignment;
			#else
				layout->size = sizeof(ILFloat);
				layout->alignment = _IL_ALIGN_FOR_TYPE(float);
			#endif
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
			#if defined(HAVE_LIBFFI)
				layout->size = ffi_type_double.size;
				layout->alignment = ffi_type_double.alignment;
			#else
				layout->size = sizeof(ILDouble);
				layout->alignment = _IL_ALIGN_FOR_TYPE(double);
			#endif
			}
			break;

			case IL_META_ELEMTYPE_R:
			{
			#if defined(HAVE_LIBFFI)
				#ifdef IL_NATIVE_FLOAT
					layout->size = ffi_type_longdouble.size;
					layout->alignment = ffi_type_longdouble.alignment;
				#else
					layout->size = ffi_type_double.size;
					layout->alignment = ffi_type_double.alignment;
				#endif
			#else
				layout->size = sizeof(ILNativeFloat);
				layout->alignment = _IL_ALIGN_FOR_TYPE(long_double);
			#endif
			}
			break;

			case IL_META_ELEMTYPE_TYPEDBYREF:
			{
			#if defined(HAVE_LIBFFI)
				layout->size = sizeof(ILTypedRef);
				layout->alignment = ffi_type_pointer.alignment;
			#else
				layout->size = sizeof(ILTypedRef);
				layout->alignment = _IL_ALIGN_FOR_TYPE(void_p);
			#endif
				layout->managedInstance = 1;
			}
			break;

			default: return 0;
		}
		layout->nativeSize = layout->size;
		layout->nativeAlignment = layout->alignment;
		layout->vtableSize = 0;
		layout->vtable = 0;
		layout->staticSize = 0;
		layout->hasFinalizer = 0;
		layout->managedStatic = 0;
		return 1;
	}
	else if(ILType_IsValueType(type))
	{
		/* Lay out a value type by getting the full size and alignment
		   of the class that underlies the value type */
		ILClass *classInfo = ILClassResolve(ILType_ToValueType(type));
		ILType *synType = ILClassGetSynType(classInfo);
		if(synType == 0)
		{
			return LayoutClass(process, classInfo, layout);
		}
		else
		{
			return LayoutType(process, ILTypeStripPrefixes(synType), layout);
		}
	}
	else
	{
		/* Everything else is laid out as a pointer */
	#if defined(HAVE_LIBFFI)
		layout->size = ffi_type_pointer.size;
		layout->alignment = ffi_type_pointer.alignment;
	#else
		layout->size = sizeof(void *);
		layout->alignment = _IL_ALIGN_FOR_TYPE(void_p);
	#endif
		layout->nativeSize = layout->size;
		layout->nativeAlignment = layout->alignment;
		layout->vtableSize = 0;
		layout->vtable = 0;
		layout->staticSize = 0;
		layout->hasFinalizer = 0;
		layout->managedInstance = ILTypeIsReference(ILTypeStripPrefixes(type));
		layout->managedStatic = 0;
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
static int ComputeInterfaceTable(ILClass *info, ILClass *interface)
{
	ILClass *parent;
	ILImplPrivate *impl;
	ILImplPrivate *impl2;
	ILUInt32 size;
	ILUInt16 *table;
	ILUInt32 slot;
	ILMethod *method;
	ILMethod *method2;
	ILOverride *over;
	ILImplements *implBlock;

	/* Determine if we already have an implementation of this interface.
	   This may happen if we reach the same interface via two different
	   paths in the interface inheritance tree */
	impl = ((ILClassPrivate *)(info->userData))->implements;
	while(impl != 0)
	{
		if(impl->interface == interface)
		{
			return 1;
		}
		impl = impl->next;
	}

	/* Create a new interface method table for the class */
	size = ((ILClassPrivate *)(interface->userData))->vtableSize;
	impl = (ILImplPrivate *)ILMemStackAllocItem
					(&(info->programItem.image->memStack),
				     sizeof(ILImplPrivate) + size * sizeof(ILUInt16));
	if(!impl)
	{
		return 0;
	}
	table = ILImplPrivate_Table(impl);
	impl->interface = interface;
	impl->next = ((ILClassPrivate *)(info->userData))->implements;
	((ILClassPrivate *)(info->userData))->implements = impl;

	/* Determine if the parent class implements the interface */
	parent = info;
	impl2 = 0;
	while(impl2 == 0 && (parent = ILClassGetParent(parent)) != 0)
	{
		impl2 = ((ILClassPrivate *)(parent->userData))->implements;
		while(impl2 != 0)
		{
			if(impl2->interface == interface)
			{
				break;
			}
			impl2 = impl2->next;
		}
	}

	/* Copy the parent's interface method table, or mark all slots abstract */
	if(impl2)
	{
		ILMemCpy(table, ILImplPrivate_Table(impl2), size * sizeof(ILUInt16));
	}
	else
	{
		ILMemSet(table, 0xFF, size * sizeof(ILUInt16));
	}

	/* Fill the interface table slots */
	for(slot = 0; slot < size; ++slot)
	{
		/* Get the method for this slot */
		method = ((ILClassPrivate *)(interface->userData))->vtable[slot];
		if(!method)
		{
			continue;
		}

		/* Search for a method in the current class with a matching name */
		method2 = 0;
		while((method2 = (ILMethod *)ILClassNextMemberByKind
						(info, (ILMember *)method2,
					 	 IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if((method2->member.attributes &
					(IL_META_METHODDEF_MEMBER_ACCESS_MASK |
					 IL_META_METHODDEF_VIRTUAL |
					 IL_META_METHODDEF_NEW_SLOT)) ==
							(IL_META_METHODDEF_PUBLIC |
					 		 IL_META_METHODDEF_VIRTUAL |
							 IL_META_METHODDEF_NEW_SLOT))
			{
				if(strcmp(method2->member.name, method->member.name) != 0 ||
				   !ILTypeIdentical(method2->member.signature,
				   					method->member.signature))
				{
					continue;
				}
				table[slot] = (ILUInt16)(method2->index);
				break;
			}
		}

		/* If the slot is still empty, then search the class
		   hierarchy for a virtual method match */
		if(table[slot] == (ILUInt16)0xFFFF)
		{
			parent = info;
			while(parent != 0)
			{
				method2 = 0;
				while((method2 = (ILMethod *)ILClassNextMemberByKind
								(parent, (ILMember *)method2,
							 	 IL_META_MEMBERKIND_METHOD)) != 0)
				{
					if((method2->member.attributes &
							(IL_META_METHODDEF_MEMBER_ACCESS_MASK |
							 IL_META_METHODDEF_VIRTUAL)) ==
									(IL_META_METHODDEF_PUBLIC |
							 		 IL_META_METHODDEF_VIRTUAL))
					{
						if(strcmp(method2->member.name,
								  method->member.name) != 0 ||
						   !ILTypeIdentical(method2->member.signature,
						   					method->member.signature))
						{
							continue;
						}
						table[slot] = (ILUInt16)(method2->index);
						break;
					}
				}
				if(table[slot] != (ILUInt16)0xFFFF)
				{
					break;
				}
				parent = ILClassGetParent(parent);
			}
		}

		/* Look for an override for the interface method */
		over = 0;
		while((over = (ILOverride *)ILClassNextMemberByKind
					(info, (ILMember *)over, IL_META_MEMBERKIND_OVERRIDE)) != 0)
		{
			if(ILMemberResolve((ILMember *)(ILOverrideGetDecl(over)))
					== (ILMember *)method)
			{
				table[slot] = (ILOverrideGetBody(over))->index;
				break;
			}
		}
		if(table[slot] == (ILUInt16)0xFFFF)
		{
			/* There is no implementation of this method, which is an error */
			return 0;
		}
	}

	/* Recursively compute all interfaces that the interface inherits from */
	implBlock = interface->implements;
	while(implBlock != 0)
	{
		if(!ComputeInterfaceTable(info, ILClassResolve(implBlock->interface)))
		{
			return 0;
		}
		implBlock = implBlock->nextInterface;
	}

	/* Done */
	return 1;
}

#ifdef IL_USE_IMTS

/*
 * Build an "Interface Method Table" (IMT) for a particular class, from
 * the interface information in that class.  The mechanism is described
 * in the following paper by IBM:
 *
 * "Efficient Implementation of Java Interfaces: Invokeinterface Considered
 * Harmless", Bowen Alpern, Anthony Cocchi, Stephen Fink, David Grove,
 * and Derek Lieber.  ACM Conference on Object-Oriented Programming, Systems,
 * Languages, and Applications (OOPSLA), Tampa, FL, USA, Oct 14-18, 2001.
 *
 * http://www.research.ibm.com/people/d/dgrove/papers/oopsla01.pdf
 *
 * Each interface method in the system is assigned a unique identifier.
 * The identifier is used to hash into a fixed-sized table of method entry
 * points, similar to a vtable.  In most cases, the hashed position will
 * be the right position.
 *
 * We modify the IMT conflict resolution mechanism slightly.  IBM's RVM
 * system generates special conflict resolution stubs for interface methods
 * that hash to the same IMT position.  We store NULL at any position where
 * there is a conflict and bail out to the traditional lookup algorithm.
 * This is necessary because we convert methods one at a time, instead of
 * class at a time like RVM does.
 */
static void BuildIMT(ILExecProcess *process, ILClass *info,
					 ILClassPrivate *classPrivate)
{
	ILImplPrivate *impl;
	ILClassPrivate *implPrivate;
	ILUInt32 posn, size;
	ILUInt32 vtableIndex;
	ILUInt32 imtIndex;

	/* Is this class itself an interface? */
	if(ILClass_IsInterface(info))
	{
		/* Allocate the base identifier for the interface.  We must do
		   this here in case an interface is referenced by a "callvirt"
		   instruction before any of the classes that implement it are
		   laid out */
		if(!(classPrivate->imtBase))
		{
			size = (ILUInt32)(classPrivate->vtableSize);
			classPrivate->imtBase = process->imtBase;
			process->imtBase += size;
		}
		return;
	}

	/* Process the implementation records that are attached to this class */
	impl = classPrivate->implements;
	while(impl != 0)
	{
		implPrivate = (ILClassPrivate *)(impl->interface->userData);
		if(implPrivate)
		{
			/* Allocate a base identifer for the interface if necessary.
			   This will probably never be used because interfaces are
			   typically laid out before their implementing classes, and
			   the allocation above will catch us.  But let's be paranoid
			   and allocate the base identifier here anyway, just in case */
			size = (ILUInt32)(implPrivate->vtableSize);
			if(!(implPrivate->imtBase))
			{
				implPrivate->imtBase = process->imtBase;
				process->imtBase += size;
			}

			/* Process the members of this interface */
			for(posn = 0; posn < size; ++posn)
			{
				imtIndex = (implPrivate->imtBase + posn) % IL_IMT_SIZE;
				vtableIndex = (ILImplPrivate_Table(impl))[posn];
				if(vtableIndex != (ILUInt32)(ILUInt16)0xFFFF)
				{
					if(!(classPrivate->imt[imtIndex]))
					{
						/* No conflict at this table position */
						classPrivate->imt[imtIndex] =
							classPrivate->vtable[vtableIndex];
					}
					else
					{
						/* We have encountered a conflict in the table */
						classPrivate->imt[imtIndex] =
							(ILMethod *)(ILNativeInt)(-1);
					}
				}
			}
		}
		impl = impl->next;
	}

	/* Clear positions in the table that indicate conflicts */
	for(posn = 0; posn < IL_IMT_SIZE; ++posn)
	{
		if(classPrivate->imt[posn] == (ILMethod *)(ILNativeInt)(-1))
		{
			classPrivate->imt[posn] = 0;
		}
	}
}

#endif /* IL_USE_IMTS */

/*
 * Lay out a particular class.  Returns zero if there
 * is something wrong with the class definition.
 */
static int LayoutClass(ILExecProcess *process, ILClass *info, LayoutInfo *layout)
{
	ILClassLayout *classLayout;
	ILFieldLayout *fieldLayout;
	ILFieldRVA *fieldRVA;
	LayoutInfo typeLayout;
	ILUInt32 maxAlignment;
	ILUInt32 maxNativeAlignment;
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
	ILMethodCode code;

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
			layout->nativeSize = classPrivate->nativeSize;
			layout->nativeAlignment = classPrivate->nativeAlignment;
			layout->vtableSize = classPrivate->vtableSize;
			layout->vtable = classPrivate->vtable;
			layout->staticSize = classPrivate->staticSize;
			layout->hasFinalizer = classPrivate->hasFinalizer;
			layout->managedInstance = classPrivate->managedInstance;
			layout->managedStatic = classPrivate->managedStatic;
			return 1;
		}
	}
	else
	{
		/* Create a new layout record and attach it to the class.
		   We must allocate this in memory accessible by the
		   garbage collector so that the object pointers that
		   it contains will be visible to the collector */
		classPrivate = (ILClassPrivate *)
				(ILGCAlloc(sizeof(ILClassPrivate)));
		if(!classPrivate)
		{
			return 0;
		}
		classPrivate->classInfo = info;
		info->userData = (void *)classPrivate;
		classPrivate->inLayout = 1;
		classPrivate->gcTypeDescriptor = IL_MAX_NATIVE_UINT;
		classPrivate->process = process;
		classPrivate->nextClassPrivate = process->firstClassPrivate;
		process->firstClassPrivate = classPrivate;

	}

	/* Lay out the parent class first */
	if(info->parent)
	{
		/* Use "ILClassGetParent" to resolve cross-image links */
		parent = ILClassGetParent(info);
		if(!LayoutClass(process, parent, layout))
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
		layout->nativeSize = 0;
		layout->nativeAlignment = 1;
		layout->vtableSize = 0;
		layout->hasFinalizer = 0;
		layout->managedInstance = 0;
		layout->managedStatic = 0;
	}

	/* Zero the static size, which must be recomputed for each class */
	layout->staticSize = 0;

	/* Lay out the interfaces that this class implements */
	implements = info->implements;
	while(implements != 0)
	{
		if(!LayoutClass(process, ILClassResolve(implements->interface), &typeLayout))
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
	allowRVALayout = ILImageIsSecure(ILProgramItem_Image(info));
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
			}

			/* Field layout is permitted */
			allowFieldLayout = 1;
		}
	}

	/* Use straight-forward field allocation, which will usually
	   match the algorithm used by the platform C compiler */
	field = 0;
	maxAlignment = 1;
	maxNativeAlignment = 1;
	while((field = (ILField *)ILClassNextMemberByKind
			(info, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if((field->member.attributes & IL_META_FIELDDEF_STATIC) == 0)
		{
			/* Get the layout information for this field's type */
			if(!LayoutType(process, field->member.signature, &typeLayout))
			{
				info->userData = 0;
				return 0;
			}

			/* Decrease the alignment if we have an explicit packing size */
			if(packingSize != 0 && packingSize < typeLayout.alignment)
			{
				typeLayout.alignment = packingSize;
			}
			if(packingSize != 0 && packingSize < typeLayout.nativeAlignment)
			{
				typeLayout.nativeAlignment = packingSize;
			}

			/* Use an explicit field offset if necessary */
			if(allowFieldLayout &&
			   (fieldLayout = ILFieldLayoutGetFromOwner(field)) != 0)
			{
				/* Record the explicit field offset */
				field->offset = fieldLayout->offset;
				field->nativeOffset = fieldLayout->offset;

				/* Extend the default class size to include the field */
				if((field->offset + typeLayout.size) > layout->size)
				{
					layout->size = field->offset + typeLayout.size;
				}
				if((field->offset + typeLayout.nativeSize) > layout->nativeSize)
				{
					layout->nativeSize = field->offset + typeLayout.nativeSize;
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
				if((layout->nativeSize % typeLayout.nativeAlignment) != 0)
				{
					layout->nativeSize += typeLayout.nativeAlignment -
						(layout->nativeSize % typeLayout.nativeAlignment);
				}

				/* Record the field's offset and advance past it */
				field->offset = layout->size;
				field->nativeOffset = layout->nativeSize;
				layout->size += typeLayout.size;
				layout->nativeSize += typeLayout.nativeSize;
			}

			/* Update the maximum alignment */
			if(typeLayout.alignment > maxAlignment)
			{
				maxAlignment = typeLayout.alignment;
			}
			if(typeLayout.nativeAlignment > maxNativeAlignment)
			{
				maxNativeAlignment = typeLayout.nativeAlignment;
			}

			/* Set the "managedInstance" flag if the type is managed */
			if(typeLayout.managedInstance)
			{
				layout->managedInstance = 1;
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
	if(maxNativeAlignment > layout->nativeAlignment)
	{
		layout->nativeAlignment = maxNativeAlignment;
	}
	if(explicitSize > layout->nativeSize)
	{
		layout->nativeSize = explicitSize;
	}
	else if((layout->nativeSize % layout->nativeAlignment) != 0)
	{
		layout->nativeSize += layout->nativeAlignment -
				(layout->nativeSize % layout->nativeAlignment);
	}

	/* Record the object size information for this class */
	classPrivate->size = layout->size;
	classPrivate->alignment = layout->alignment;
	classPrivate->nativeSize = layout->nativeSize;
	classPrivate->nativeAlignment = layout->nativeAlignment;
	classPrivate->inLayout = 0;

	/* Allocate the static fields.  We must do this after the
	   regular fields because some of the statics may be instances
	   of the class that we are trying to lay out, especially
	   in value type definitions */
	field = 0;
	while((field = (ILField *)ILClassNextMemberByKind
			(info, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if((field->member.attributes & IL_META_FIELDDEF_STATIC) != 0 &&
		   (field->member.attributes & IL_META_FIELDDEF_LITERAL) == 0)
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
				if(!LayoutType(process, field->member.signature, &typeLayout))
				{
					info->userData = 0;
					return 0;
				}

				/* Thread-static variables are allocated slots from the
				   ILExecProcess record.  We assume that some higher level
				   function has acquired the metadata lock on the process */
				if(ILFieldIsThreadStatic(field))
				{
					/* Store the slot number in the "offset" field and
					   the field size in the "nativeOffset" field */
					field->offset = (process->numThreadStaticSlots)++;
					field->nativeOffset = typeLayout.size;
					continue;
				}

				/* Align the field on an appropriate boundary */
				if((layout->staticSize % typeLayout.alignment) != 0)
				{
					layout->staticSize += typeLayout.alignment -
						(layout->staticSize % typeLayout.alignment);
				}

				/* Record the field's offset and advance past it */
				field->offset = layout->staticSize;
				field->nativeOffset = layout->staticSize;
				layout->staticSize += typeLayout.size;

				/* Set the "managedStatic" flag if the type is managed */
				if(typeLayout.managedInstance)
				{
					layout->managedStatic = 1;
				}
			}
		}
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

		/* Is this the finalize method? */
		if(method->member.name[0] == 'F' &&
		   !strcmp(method->member.name + 1, "inalize") &&
		   method->member.signature->un.method__.retType__ == ILType_Void &&
		   method->member.signature->num__ == 0)
		{
			/* Determine if the finalizer is non-trivial */
			if(!ILMethodGetCode(method, &code) ||
			   code.codeLen != 1 ||
			   ((unsigned char *)(code.code))[0] != IL_OP_RET)
			{
				layout->hasFinalizer = 1;
			}
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
	if((info->attributes & IL_META_TYPEDEF_CLASS_SEMANTICS_MASK) !=
				IL_META_TYPEDEF_INTERFACE)
	{
		implements = info->implements;
		while(implements != 0)
		{
			parent = ILClassResolve(implements->interface);
			if(parent->userData)
			{
				if(!ComputeInterfaceTable(info, parent))
				{
					info->userData = 0;
					return 0;
				}
			}
			implements = implements->nextInterface;
		}
	}

	/* Record the rest of the layout information for this class */
	classPrivate->staticSize = layout->staticSize;
	classPrivate->vtableSize = layout->vtableSize;
	classPrivate->vtable = vtable;
	classPrivate->hasFinalizer = layout->hasFinalizer;
	classPrivate->managedInstance = layout->managedInstance;
	classPrivate->managedStatic = layout->managedStatic;
	layout->vtable = vtable;

#ifdef IL_USE_IMTS
	/* Build the interface method table for this class */
	BuildIMT(process, info, classPrivate);
#endif

	/* Done */
	return 1;
}

int _ILLayoutClass(ILExecProcess *process, ILClass *info)
{
	LayoutInfo layout;
	return LayoutClass(process, info, &layout);
}

ILUInt32 _ILLayoutClassReturn(ILExecProcess *process, ILClass *info, ILUInt32 *alignment)
{
	LayoutInfo layout;
	if(LayoutClass(process, info, &layout))
	{
		*alignment = layout.alignment;
		return layout.size;
	}
	else
	{
		*alignment = 0;
		return 0;
	}
}

int _ILLayoutAlreadyDone(ILClass *info)
{
	if(info->userData)
	{
		ILClassPrivate *classPrivate = (ILClassPrivate *)(info->userData);
		return !(classPrivate->inLayout);
	}
	else
	{
		return 0;
	}
}

ILUInt32 _ILSizeOfTypeLocked(ILExecProcess *process, ILType *type)
{
	LayoutInfo layout;
	if(!LayoutType(process, type, &layout))
	{
		return 0;
	}
	else
	{
		return layout.size;
	}
}

ILUInt32 ILSizeOfType(ILExecThread *thread, ILType *type)
{
	if(!ILType_IsValueType(type))
	{
		/* We can take a shortcut because the type is not a value type */
		return _ILSizeOfTypeLocked(_ILExecThreadProcess(thread), type);
	}
	else
	{
		/* We have to lock down the metadata first */
		ILUInt32 size;
		IL_METADATA_WRLOCK(_ILExecThreadProcess(thread));
		size = _ILSizeOfTypeLocked(_ILExecThreadProcess(thread), type);
		IL_METADATA_UNLOCK(_ILExecThreadProcess(thread));
		return size;
	}
}

#ifdef	__cplusplus
};
#endif
