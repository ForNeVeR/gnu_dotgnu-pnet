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

/*
 * Add a default constructor to a class.
 */
static int AddDefaultConstructor(ILClass *classInfo)
{
	ILMethod *method;
	ILType *signature;
	method = ILMethodCreate(classInfo, 0, ".ctor",
					  	    IL_META_METHODDEF_PUBLIC |
					  	    IL_META_METHODDEF_HIDE_BY_SIG |
							IL_META_METHODDEF_SPECIAL_NAME |
							IL_META_METHODDEF_RT_SPECIAL_NAME);
	if(!method)
	{
		return 0;
	}
	signature = ILTypeCreateMethod(ILClassToContext(classInfo), ILType_Void);
	if(!signature)
	{
		return 0;
	}
	signature->kind |= (IL_META_CALLCONV_HASTHIS << 8);
	ILMemberSetSignature((ILMember *)method, signature);
	return 1;
}

/*
 * Add the constructor to the "DecimalConstantAttribute" class.
 *
 * public DecimalConstantAttribute(byte scale, byte sign,
 *                                 uint hi, uint mid, uint low);
 */
static int AddDecimalConstructor(ILClass *classInfo)
{
	ILMethod *method;
	ILContext *context;
	ILType *signature;
	method = ILMethodCreate(classInfo, 0, ".ctor",
					  	    IL_META_METHODDEF_PUBLIC |
					  	    IL_META_METHODDEF_HIDE_BY_SIG |
							IL_META_METHODDEF_SPECIAL_NAME |
							IL_META_METHODDEF_RT_SPECIAL_NAME);
	if(!method)
	{
		return 0;
	}
	context = ILClassToContext(classInfo);
	signature = ILTypeCreateMethod(context, ILType_Void);
	if(!signature)
	{
		return 0;
	}
	signature->kind |= (IL_META_CALLCONV_HASTHIS << 8);
	if(!ILTypeAddParam(context, signature, ILType_UInt8))
	{
		return 0;
	}
	if(!ILTypeAddParam(context, signature, ILType_UInt8))
	{
		return 0;
	}
	if(!ILTypeAddParam(context, signature, ILType_UInt32))
	{
		return 0;
	}
	if(!ILTypeAddParam(context, signature, ILType_UInt32))
	{
		return 0;
	}
	if(!ILTypeAddParam(context, signature, ILType_UInt32))
	{
		return 0;
	}
	ILMemberSetSignature((ILMember *)method, signature);
	return 1;
}

/*
 * Add an abstract virtual method constructor to a class.
 */
static ILMethod *AddAbstractMethod(ILClass *classInfo, const char *name,
							       ILType *returnType, ILType *argType)
{
	ILMethod *method;
	ILType *signature;
	method = ILMethodCreate(classInfo, 0, name,
					  	    IL_META_METHODDEF_PUBLIC |
					  	    IL_META_METHODDEF_HIDE_BY_SIG |
							IL_META_METHODDEF_ABSTRACT |
							IL_META_METHODDEF_VIRTUAL |
							IL_META_METHODDEF_NEW_SLOT);
	if(!method)
	{
		return 0;
	}
	signature = ILTypeCreateMethod(ILClassToContext(classInfo), returnType);
	if(!signature)
	{
		return 0;
	}
	signature->kind |= (IL_META_CALLCONV_HASTHIS << 8);
	if(argType != ILType_Void)
	{
		if(!ILTypeAddParam(ILClassToContext(classInfo), signature, argType))
		{
			return 0;
		}
	}
	ILMemberSetSignature((ILMember *)method, signature);
	return method;
}

void ILGenMakeLibrary(ILGenInfo *info)
{
	ILImage *image = info->libImage;
	ILProgramItem *scope = ILClassGlobalScope(image);
	ILClass *objectClass;
	ILClass *stringClass;
	ILClass *typeClass;
	ILClass *valueTypeClass;
	ILClass *enumClass;
	ILClass *voidClass;
	ILClass *intPtrClass;
	ILClass *uintPtrClass;
	ILClass *typedRefClass;
	ILClass *attributeClass;
	ILClass *paramAttributeClass;
	ILClass *decimalConstantClass;
	ILClass *exceptionClass;
	ILClass *disposableInterface;
	ILClass *collectionInterface;
	ILClass *enumeratorInterface;
	int constructorOK;
	ILMethod *method;
	ILProperty *property;
	ILType *signature;

	/* Create the "System.Object" class */
	ABORT_IF(objectClass, ILClassCreate(scope, 0, "Object", "System", 0));
	ILClassSetAttrs(objectClass, ~0,
				    IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_BEFORE_FIELD_INIT |
					IL_META_TYPEDEF_SERIALIZABLE);
	ABORT_IF(constructorOK, AddDefaultConstructor(objectClass));

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
	ABORT_IF(constructorOK, AddDefaultConstructor(typeClass));

	/* Create the "System.ValueType" class */
	ABORT_IF(valueTypeClass,
			 ILClassCreate(scope, 0, "ValueType", "System", objectClass));
	ILClassSetAttrs(valueTypeClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SERIALIZABLE);
	ABORT_IF(constructorOK, AddDefaultConstructor(valueTypeClass));

	/* Create the "System.Enum" class */
	ABORT_IF(enumClass,
			 ILClassCreate(scope, 0, "Enum", "System", valueTypeClass));
	ILClassSetAttrs(enumClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
					IL_META_TYPEDEF_ABSTRACT |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SERIALIZABLE);
	ABORT_IF(constructorOK, AddDefaultConstructor(enumClass));

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

	/* Create the "System.Attribute" class */
	ABORT_IF(attributeClass,
			 ILClassCreate(scope, 0, "Attribute", "System",
			 			   objectClass));
	ILClassSetAttrs(attributeClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_ABSTRACT);

	/* Create the "System.ParamArrayAttribute" class */
	ABORT_IF(paramAttributeClass,
			 ILClassCreate(scope, 0, "ParamArrayAttribute", "System",
			 			   attributeClass));
	ILClassSetAttrs(paramAttributeClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SEALED);
	ABORT_IF(constructorOK, AddDefaultConstructor(paramAttributeClass));

	/* Create "System.Runtime.CompilerServices.DecimalConstantAttribute" */
	ABORT_IF(decimalConstantClass,
			 ILClassCreate(scope, 0, "DecimalConstantAttribute",
			 			   "System.Runtime.CompilerServices",
			 			   attributeClass));
	ILClassSetAttrs(decimalConstantClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT |
				    IL_META_TYPEDEF_SEALED);
	ABORT_IF(constructorOK, AddDecimalConstructor(decimalConstantClass));

	/* Create the "System.Exception" class */
	ABORT_IF(exceptionClass,
			 ILClassCreate(scope, 0, "Exception", "System",
			 			   objectClass));
	ILClassSetAttrs(exceptionClass, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_SERIALIZABLE |
					IL_META_TYPEDEF_BEFORE_FIELD_INIT);
	ABORT_IF(constructorOK, AddDefaultConstructor(exceptionClass));

	/* Create the "System.IDisposable" interface */
	ABORT_IF(disposableInterface,
			 ILClassCreate(scope, 0, "IDisposable", "System", 0));
	ILClassSetAttrs(disposableInterface, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_INTERFACE |
					IL_META_TYPEDEF_ABSTRACT);

	/* Create the "System.Collections.ICollection" interface */
	ABORT_IF(collectionInterface,
			 ILClassCreate(scope, 0, "ICollection", "System.Collections", 0));
	ILClassSetAttrs(collectionInterface, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_INTERFACE |
					IL_META_TYPEDEF_ABSTRACT);

	/* Create the "System.Collections.IEnumerator" interface */
	ABORT_IF(enumeratorInterface,
			 ILClassCreate(scope, 0, "IEnumerator", "System.Collections", 0));
	ILClassSetAttrs(enumeratorInterface, ~0,
					IL_META_TYPEDEF_PUBLIC |
				    IL_META_TYPEDEF_INTERFACE |
					IL_META_TYPEDEF_ABSTRACT);

	/* Add the "GetEnumerator" method to the "ICollection" interface */
	ABORT_IF(method, AddAbstractMethod(collectionInterface, "GetEnumerator",
									   ILType_FromClass(enumeratorInterface),
									   ILType_Void));

	/* Add the "MoveNext" and "Reset" methods to "IEnumerator" */
	ABORT_IF(method, AddAbstractMethod(enumeratorInterface, "MoveNext",
									   ILType_Boolean, ILType_Void));
	ABORT_IF(method, AddAbstractMethod(enumeratorInterface, "Reset",
									   ILType_Void, ILType_Void));

	/* Add the "Current" property to "IEnumerator" */
	ABORT_IF(method, AddAbstractMethod(enumeratorInterface, "get_Current",
									   ILType_FromClass(objectClass),
									   ILType_Void));
	ABORT_IF(signature, ILTypeCreateProperty(info->context,
									 		 ILType_FromClass(objectClass)));
	signature->kind |= (short)(IL_META_CALLCONV_HASTHIS << 8);
	ABORT_IF(property, ILPropertyCreate(enumeratorInterface, 0, "Current",
										0, signature));
	if(!ILMethodSemCreate((ILProgramItem *)property, 0,
				  		  IL_META_METHODSEM_GETTER, method))
	{
		ILGenOutOfMemory(info);
	}
}

#ifdef	__cplusplus
};
#endif
