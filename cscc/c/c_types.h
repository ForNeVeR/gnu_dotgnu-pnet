/*
 * c_types.h - Type representation for the C programming language.
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

#ifndef	_C_TYPES_H
#define	_C_TYPES_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Kinds of structs or unions that may be encountered by the C parser.
 */
#define	C_STKIND_STRUCT			0
#define	C_STKIND_STRUCT_NATIVE	1
#define	C_STKIND_UNION			2
#define	C_STKIND_UNION_NATIVE	3

/*
 * Create a struct or union type with a specific name.  If the
 * type already exists, then return it as-is.
 */
ILType *CTypeCreateStructOrUnion(ILGenInfo *info, const char *name,
								 int kind, const char *funcName);

/*
 * Create a C "enum" type with a specific name.  If the
 * "enum" type already exists, then return it as-is.
 */
ILType *CTypeCreateEnum(ILGenInfo *info, const char *name,
						const char *funcName);

/*
 * Create a C array type.  If the array type already exists,
 * then return it as-is.
 */
ILType *CTypeCreateArray(ILGenInfo *info, ILType *elemType, ILUInt32 size);

/*
 * Create a C pointer type.
 */
ILType *CTypeCreatePointer(ILGenInfo *info, ILType *refType, int nativePtr);

/*
 * Create a C type reference for "builtin_va_list".
 */
ILType *CTypeCreateVaList(ILGenInfo *info);

/*
 * Create a C type reference for "builtin_jmp_buf".
 */
ILType *CTypeCreateJmpBuf(ILGenInfo *info);

/*
 * Create a C type reference for "void *".
 */
ILType *CTypeCreateVoidPtr(ILGenInfo *info);

/*
 * Create a C type reference for "char *".
 */
ILType *CTypeCreateCharPtr(ILGenInfo *info);

/*
 * Mark a C type with a "const" qualifier.
 */
ILType *CTypeAddConst(ILGenInfo *info, ILType *type);

/*
 * Mark a C type with a "volatile" qualifier.
 */
ILType *CTypeAddVolatile(ILGenInfo *info, ILType *type);

/*
 * Determine if a type returned from "CTypeCreateStruct",
 * "CTypeCreateUnion", or "CTypeCreateEnum" has already
 * been defined by the compiler.
 */
int CTypeAlreadyDefined(ILType *type);

/*
 * Define a struct or union type with a specific name.  Returns NULL
 * if the structure is already defined.
 */
ILType *CTypeDefineStructOrUnion(ILGenInfo *info, const char *name,
								 int kind, const char *funcName);

/*
 * Define an anonymous "struct" or "union" type within a given parent
 * and function.
 */
ILType *CTypeDefineAnonStructOrUnion(ILGenInfo *info, ILType *parent,
							         const char *funcName, int kind);

/*
 * Define an "enum" type with a specific name.  Returns NULL
 * if the enum is already defined.
 */
ILType *CTypeDefineEnum(ILGenInfo *info, const char *name,
						const char *funcName);

/*
 * Define an anonymous "enum" type at the global level or within
 * a specified function.
 */
ILType *CTypeDefineAnonEnum(ILGenInfo *info, const char *funcName);

/*
 * Define a new field within a "struct" or "union".
 */
ILField *CTypeDefineField(ILGenInfo *info, ILType *structType,
					 	  const char *fieldName, ILType *fieldType);

/*
 * Define a new bit field within a "struct" or "union".
 */
ILField *CTypeDefineBitField(ILGenInfo *info, ILType *structType,
					 	     const char *fieldName, ILType *fieldType,
							 ILUInt32 numBits);

/*
 * End the definition of a "struct" or "union".  This will update
 * the type size to reflect the maximum alignment.
 */
void CTypeEndStruct(ILGenInfo *info, ILType *structType);

/*
 * Remove qualifiers from a C type.
 */
ILType *CTypeWithoutQuals(ILType *type);

/*
 * Determine if a C type has the "const" qualifier.
 */
int CTypeIsConst(ILType *type);

/*
 * Determine if a C type has the "volatile" qualifier.
 */
int CTypeIsVolatile(ILType *type);

/*
 * Determine if a C type is a structure.
 */
int CTypeIsStruct(ILType *type);

/*
 * Determine if a C type is a union.
 */
int CTypeIsUnion(ILType *type);

/*
 * Get the kind of a structure or union type.  -1 if not a struct or union.
 */
int CTypeGetStructKind(ILType *type);

/*
 * Determine if a C type is an enumerated type.
 */
int CTypeIsEnum(ILType *type);

/*
 * Determine if a C type is an array.
 */
int CTypeIsArray(ILType *type);

/*
 * Determine if a C type is a pointer type (native or regular).
 */
int CTypeIsPointer(ILType *type);

/*
 * Determine if a C type is a native pointer type.
 */
int CTypeIsNativePointer(ILType *type);

/*
 * Determine if a C type is not a C type at all, but is
 * actually a managed type imported from some other language
 * such as C#.
 */
int CTypeIsForeign(ILType *type);

/*
 * Get the number of elements within an array type.
 */
ILUInt32 CTypeGetNumElems(ILType *type);

/*
 * Get the element type of an array type.
 */
ILType *CTypeGetElemType(ILType *type);

/*
 * Get the type that is referenced by a pointer type.
 */
ILType *CTypeGetPtrRef(ILType *type);

/*
 * Special value that is used to indicate that the size of
 * a type must be computed at runtime.
 */
#define	CTYPE_DYNAMIC	IL_MAX_UINT32

/*
 * Get the size and alignment of a C type.  Returns CTYPE_DYNAMIC
 * if the size must be computed at runtime.
 */
ILUInt32 CTypeSizeAndAlign(ILType *type, ILUInt32 *align);

/*
 * Convert a C type into a name that can be displayed in an
 * error message.  Use "ILFree" to free the return value.
 */
char *CTypeToName(ILGenInfo *info, ILType *type);

/*
 * Mark a type as needing to be output at code generation time.
 */
void CTypeMarkForOutput(ILGenInfo *info, ILType *type);

/*
 * Output pending type definitions to an assembly output stream.
 * This must be called outside the context of a method.
 */
void CTypeOutputPending(ILGenInfo *info, FILE *stream);

#ifdef	__cplusplus
};
#endif

#endif	/* _C_TYPES_H */