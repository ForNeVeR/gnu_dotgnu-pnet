/*
 * cg_library.c - Routines for manipulating the builtin C# library.
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

#include "cg_nodes.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Helper macro that aborts if we run out of memory.
 */
#define	ABORT_IF(var, call)	\
			do { \
				(var) = (call); \
				if(!(var)) \
				{ \
					ILGenOutOfMemory(info); \
				} \
			} while (0)

/*
 * Make a value type.
 */
static void MakeValueType(ILGenInfo *info, ILImage *image,
						  const char *name, ILClass *parent)
{
	ILClass *newClass;
	ABORT_IF(newClass, ILClassCreate(ILClassGlobalScope(image),
									 0, name, "System", parent));
	ILClassSetAttrs(newClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_VALUE_TYPE |
				    IL_META_TYPEDEF_LAYOUT_SEQUENTIAL |
				    IL_META_TYPEDEF_SERIALIZABLE |
				    IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SEALED);
}

void ILGenMakeLibrary(ILGenInfo *info)
{
	ILImage *image = info->libImage;
	ILProgramItem *scope = ILClassGlobalScope(image);
	ILClass *objectClass;
	ILClass *stringClass;
	ILClass *typeClass;
	ILClass *valueTypeClass;
	ILClass *voidClass;
	ILClass *intPtrClass;
	ILClass *uintPtrClass;
	ILClass *typedRefClass;

	/* Create the "System.Object" class */
	ABORT_IF(objectClass, ILClassCreate(scope, 0, "Object", "System", 0));
	ILClassSetAttrs(objectClass, ~0,
				    IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_BEFORE_FIELD_INIT |
					IL_META_TYPEDEF_SERIALIZABLE);

	/* Create the "System.String" class */
	ABORT_IF(stringClass,
			 ILClassCreate(scope, 0, "String", "System", objectClass));
	ILClassSetAttrs(stringClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SEALED);

	/* Create the "System.Type" class */
	ABORT_IF(typeClass,
			 ILClassCreate(scope, 0, "Type", "System", objectClass));
	ILClassSetAttrs(stringClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_ABSTRACT);

	/* Create the "System.ValueType" class */
	ABORT_IF(valueTypeClass,
			 ILClassCreate(scope, 0, "ValueType", "System", objectClass));
	ILClassSetAttrs(valueTypeClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SERIALIZABLE);

	/* Create the "System.Void" class */
	ABORT_IF(voidClass,
			 ILClassCreate(scope, 0, "Void", "System", valueTypeClass));
	ILClassSetAttrs(voidClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_VALUE_TYPE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
					IL_META_TYPEDEF_SEALED);

	/* Create the numeric value types */
	MakeValueType(info, image, "SByte", valueTypeClass);
	MakeValueType(info, image, "Byte", valueTypeClass);
	MakeValueType(info, image, "Int16", valueTypeClass);
	MakeValueType(info, image, "UInt16", valueTypeClass);
	MakeValueType(info, image, "Int32", valueTypeClass);
	MakeValueType(info, image, "UInt32", valueTypeClass);
	MakeValueType(info, image, "Int64", valueTypeClass);
	MakeValueType(info, image, "UInt64", valueTypeClass);
	MakeValueType(info, image, "Single", valueTypeClass);
	MakeValueType(info, image, "Double", valueTypeClass);
	MakeValueType(info, image, "Decimal", valueTypeClass);

	/* Create the "System.IntPtr" class */
	ABORT_IF(intPtrClass,
			 ILClassCreate(scope, 0, "IntPtr", "System", valueTypeClass));
	ILClassSetAttrs(intPtrClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_VALUE_TYPE |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SEALED);

	/* Create the "System.UIntPtr" class */
	ABORT_IF(uintPtrClass,
			 ILClassCreate(scope, 0, "UIntPtr", "System", valueTypeClass));
	ILClassSetAttrs(uintPtrClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_VALUE_TYPE |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SEALED);

	/* Create the "System.TypedReference" class */
	ABORT_IF(typedRefClass,
			 ILClassCreate(scope, 0, "TypedReference", "System",
			 			   valueTypeClass));
	ILClassSetAttrs(typedRefClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_VALUE_TYPE |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SEALED);
}

#ifdef	__cplusplus
};
#endif
