/*
 * il_types.h - Type representation for IL images.
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

#ifndef	_IL_TYPES_H
#define	_IL_TYPES_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Forward declarations.
 */
typedef struct _tagILType      ILType;
typedef struct _tagILClass     ILClass;

/*
 * Invalid type code.
 */
#define	ILType_Invalid		((ILType *)0)

/*
 * Convert a primitive element type into a type representation.
 */
#define	ILType_FromElement(elemType)	\
				((ILType *)((((unsigned long)(elemType)) << 2) | 0x01))

/*
 * Convert a type representation into a primitive element type.
 */
#define	ILType_ToElement(type)	\
				(((unsigned long)(type)) >> 2)

/*
 * Convert a class pointer into a type representation.
 */
#define	ILType_FromClass(info)	\
				((ILType *)(((unsigned long)(info)) | 0x02))

/*
 * Convert a type representation back into a class pointer.
 */
#define	ILType_ToClass(type)	\
				((ILClass *)(((unsigned long)(type)) & ~0x03))

/*
 * Convert a value type pointer into a type representation.
 */
#define	ILType_FromValueType(info)	\
				((ILType *)(((unsigned long)(info)) | 0x03))

/*
 * Convert a type representation back into a value type pointer.
 */
#define	ILType_ToValueType(type)	\
				((ILClass *)(((unsigned long)(type)) & ~0x03))

/*
 * Primitive types.
 */
#define	ILType_Void			ILType_FromElement(IL_META_ELEMTYPE_VOID)
#define	ILType_Boolean		ILType_FromElement(IL_META_ELEMTYPE_BOOLEAN)
#define	ILType_Char			ILType_FromElement(IL_META_ELEMTYPE_CHAR)
#define	ILType_Int8			ILType_FromElement(IL_META_ELEMTYPE_I1)
#define	ILType_UInt8		ILType_FromElement(IL_META_ELEMTYPE_U1)
#define	ILType_Int16		ILType_FromElement(IL_META_ELEMTYPE_I2)
#define	ILType_UInt16		ILType_FromElement(IL_META_ELEMTYPE_U2)
#define	ILType_Int32		ILType_FromElement(IL_META_ELEMTYPE_I4)
#define	ILType_UInt32		ILType_FromElement(IL_META_ELEMTYPE_U4)
#define	ILType_Int64		ILType_FromElement(IL_META_ELEMTYPE_I8)
#define	ILType_UInt64		ILType_FromElement(IL_META_ELEMTYPE_U8)
#define	ILType_Float32		ILType_FromElement(IL_META_ELEMTYPE_R4)
#define	ILType_Float64		ILType_FromElement(IL_META_ELEMTYPE_R8)
#define	ILType_Int			ILType_FromElement(IL_META_ELEMTYPE_I)
#define	ILType_UInt			ILType_FromElement(IL_META_ELEMTYPE_U)
#define	ILType_Float		ILType_FromElement(IL_META_ELEMTYPE_R)
#define	ILType_TypedRef		ILType_FromElement(IL_META_ELEMTYPE_TYPEDBYREF)
#define	ILType_Sentinel		ILType_FromElement(IL_META_ELEMTYPE_SENTINEL)

/*
 * Determine if a type is primitive.
 */
#define	ILType_IsPrimitive(type)	\
				((((unsigned long)(type)) & 0x03) == 0x01)

/*
 * Determine if a type is a class reference.
 */
#define	ILType_IsClass(type)	\
				((((unsigned long)(type)) & 0x03) == 0x02)

/*
 * Determine if a type is a value type reference.
 */
#define	ILType_IsValueType(type)	\
				((((unsigned long)(type)) & 0x03) == 0x03)

/*
 * Determine if a type is complex.
 */
#define	ILType_IsComplex(type)	\
				((((unsigned long)(type)) & 0x03) == 0x00)

/*
 * Kinds of complex types.
 */
#define	IL_TYPE_COMPLEX_BYREF					1
#define	IL_TYPE_COMPLEX_PTR						2
#define	IL_TYPE_COMPLEX_ARRAY					3
#define	IL_TYPE_COMPLEX_ARRAY_CONTINUE			4
#define	IL_TYPE_COMPLEX_CMOD_REQD				6
#define	IL_TYPE_COMPLEX_CMOD_OPT				7
#define	IL_TYPE_COMPLEX_PROPERTY				8
#define	IL_TYPE_COMPLEX_SENTINEL				9
#define	IL_TYPE_COMPLEX_PINNED					10
#define	IL_TYPE_COMPLEX_LOCALS					11
#define	IL_TYPE_COMPLEX_METHOD					16
#define	IL_TYPE_COMPLEX_METHOD_SENTINEL			1

/*
 * Complex types.
 */
struct _tagILType
{
	short			kind;		/* Kind of complex type */
	unsigned short	num;		/* Number of parameters for a method */
	union {
		ILType	   *refType;	/* Referenced type */
		struct {
			ILType *elemType;	/* Element type */
			long	size;		/* Size of the dimension */
			long	lowBound;	/* Low bound for the dimension */
		} array;
		struct {
			ILType *retType;	/* Return type */
			ILType *param[3];	/* Parameters */
			ILType *next;		/* Overflow for the rest of the parameters */
		} method;
		struct {
			ILType *param[4];	/* Overflow parameters */
			ILType *next;		/* Overflow for the rest of the parameters */
		} params;
		struct {
			ILClass *info;		/* Information on the modifier's class */
			ILType  *type;		/* The type that is being modified */
		} modifier;
		struct {
			ILType *local[4];	/* Types for up to 4 locals */
			ILType *next;		/* Overflow for the rest of the locals */
		} locals;
	} un;
};

/*
 * Create a reference type.  Returns NULL if out of memory.
 */
ILType *ILTypeCreateRef(ILContext *context, int kind, ILType *refType);

/*
 * Create an array type.  Returns NULL if out of memory.
 */
ILType *ILTypeCreateArray(ILContext *context, unsigned long rank,
						  ILType *elem);

/*
 * Set the size for a particular array dimension.
 */
void ILTypeSetSize(ILType *array, unsigned long dimension, long value);

/*
 * Set the low bound for a particular array dimension.
 */
void ILTypeSetLowBound(ILType *array, unsigned long dimension, long value);

/*
 * Create a modifier and add it to a list.  Returns the new list,
 * or NULL if out of memory.
 */
ILType *ILTypeCreateModifier(ILContext *context, ILType *list,
							 int kind, ILClass *info);

/*
 * Add a list of modifiers to a type and return the modified type.
 */
ILType *ILTypeAddModifiers(ILContext *context, ILType *modifiers,
						   ILType *type);

/*
 * Create a local variable list.  Returns NULL if out of memory.
 */
ILType *ILTypeCreateLocalList(ILContext *context);

/*
 * Add a type to a local variable list.  Returns zero if out of memory.
 */
int ILTypeAddLocal(ILContext *context, ILType *locals, ILType *type);

/*
 * Get the number of locals within a local variable list.
 */
unsigned long ILTypeNumLocals(ILType *locals);

/*
 * Get the type for a specific local within a local variable list.
 */
ILType *ILTypeGetLocal(ILType *locals, unsigned long index);

/*
 * Create a method type with a specific return type.
 * Returns NULL if out of memory.
 */
ILType *ILTypeCreateMethod(ILContext *context, ILType *returnType);

/*
 * Create a property type with a specific type.
 * Returns NULL if out of memory.
 */
ILType *ILTypeCreateProperty(ILContext *context, ILType *propType);

/*
 * Add a parameter to a method or property type.  Returns
 * zero if out of memory.
 */
int ILTypeAddParam(ILContext *context, ILType *method, ILType *paramType);

/*
 * Add a varargs sentinel to a method type.  Returns zero
 * if out of memory.
 */
int ILTypeAddSentinel(ILContext *context, ILType *method);

/*
 * Get a specific parameter from a method or property type.
 * The index 0 indicates the return type.  Returns NULL if
 * the parameter index is invalid.
 */
ILType *ILTypeGetParam(ILType *method, unsigned long index);

/*
 * Parse a type from a MethodDefSig within the signature blob.
 * Returns NULL if the signature is invalid, or out of memory.
 */
ILType *ILTypeFromMethodDefSig(ILContext *context, ILImage *image,
							   unsigned long blobStart, unsigned long blobLen);

/*
 * Parse a type from a MethodRefSig within the signature blob.
 * Returns NULL if the signature is invalid, or out of memory.
 */
ILType *ILTypeFromMethodRefSig(ILContext *context, ILImage *image,
							   unsigned long blobStart, unsigned long blobLen);

/*
 * Parse a type from a StandAloneMethodSig within the signature blob.
 * Returns NULL if the signature is invalid, or out of memory.
 */
ILType *ILTypeFromStandAloneMethodSig(ILContext *context, ILImage *image,
							   		  unsigned long blobStart,
									  unsigned long blobLen);

/*
 * Parse a type from a FieldSig within the signature blob.
 * Returns NULL if the signature is invalid, or out of memory.
 */
ILType *ILTypeFromFieldSig(ILContext *context, ILImage *image,
						   unsigned long blobStart, unsigned long blobLen);

/*
 * Parse a type from a PropertySig within the signature blob.
 * Returns NULL if the signature is invalid, or out of memory.
 */
ILType *ILTypeFromPropertySig(ILContext *context, ILImage *image,
							  unsigned long blobStart, unsigned long blobLen);

/*
 * Parse a type from a TypeSpec within the signature blob.
 * Returns NULL if the signature is invalid, or out of memory.
 */
ILType *ILTypeFromTypeSpec(ILContext *context, ILImage *image,
						   unsigned long blobStart, unsigned long blobLen);

/*
 * Parse a list of locals from within the signature block.
 * Returns NULL if the list is invalid, or out of memory.
 */
ILType *ILTypeFromLocalVarSig(ILImage *image, unsigned long blobOffset);

/*
 * Strip prefixes from a type to get to the actual type.
 */
ILType *ILTypeStripPrefixes(ILType *type);

/*
 * Determine if two types are identical after stripping prefixes.
 */
int ILTypeIdentical(ILType *type1, ILType *type2);

/*
 * Convert a type into the primitive runtime type that represents it.
 */
ILValueType ILTypeToValueType(ILType *type);

/*
 * Get the name form of a type.  The return value should
 * be free'd with "ILFree".
 */
char *ILTypeToName(ILType *type);

/*
 * Get the underlying type for an enumerated type.
 * If the type is not enumerated, then return as-is.
 */
ILType *ILTypeGetEnumType(ILType *type);

/*
 * Convert a type into a blob offset for the encoded form
 * of a method signature.  Returns zero if out of memory.
 */
unsigned long ILTypeToMethodSig(ILImage *image, ILType *type);

/*
 * Convert a type into a blob offset for the encoded form
 * of a field signature.  Returns zero if out of memory.
 */
unsigned long ILTypeToFieldSig(ILImage *image, ILType *type);

/*
 * Convert a type into a blob offset for the encoded form of
 * some other kind of signature.  Returns zero if out of memory.
 */
unsigned long ILTypeToOtherSig(ILImage *image, ILType *type);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_TYPES_H */
