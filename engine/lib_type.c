/*
 * lib_type.c - Internalcall methods for "Type" and related classes.
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
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Resolve an assembly name to an image.  Returns NULL if the
 * assembly could not be located.
 */
static ILImage *ResolveAssembly(ILExecThread *thread,
								ILUInt16 *name, ILInt32 len)
{
	return 0;
}

/*
 * Get the caller's image.
 */
static ILImage *CallerImage(ILExecThread *thread)
{
	ILImage *systemImage;
	ILImage *image;
	ILMethod *method;
	unsigned long num;

	/* We scan up the stack looking for a method that is
	   not within the system library image.  That method
	   is designated as the caller */
	systemImage = ILContextGetSystem(thread->process->context);
	if(!systemImage)
	{
		systemImage = ILProgramItem_Image(ILExecThreadStackMethod(thread, 0));
	}
	num = 0;
	while((method = ILExecThreadStackMethod(thread, num)) != 0)
	{
		if((image = ILProgramItem_Image(method)) != systemImage)
		{
			return image;
		}
		++num;
	}

	/* All methods are in the system image, so let that be the caller */
	return systemImage;
}

int _ILClrCheckAccess(ILExecThread *thread, ILClass *classInfo,
					  ILMember *member)
{
	ILImage *systemImage;
	ILMethod *method;
	ILClass *methodClass;
	unsigned long num;

	/* We scan up the stack looking for a method that is
	   not within the system library image.  That method
	   is used to perform the security check */
	systemImage = ILContextGetSystem(thread->process->context);
	if(!systemImage)
	{
		systemImage = ILProgramItem_Image(ILExecThreadStackMethod(thread, 0));
	}
	num = 0;
	while((method = ILExecThreadStackMethod(thread, num)) != 0)
	{
		if(ILProgramItem_Image(method) != systemImage)
		{
			break;
		}
		++num;
	}
	if(!method)
	{
		/* All methods are within the system image, so all accesses are OK.
		   We assume that the system library always has ReflectionPermission */
		return 1;
	}

	/* Check for direct accessibility */
	methodClass = ILMethod_Owner(method);
	if(classInfo != 0 && ILClassAccessible(classInfo, methodClass))
	{
		return 1;
	}
	else if(member != 0 && ILMemberAccessible(member, methodClass))
	{
		return 1;
	}

	/* Check that the caller has "ReflectionPermission" */
	/* TODO */
	return 1;
}

int _ILClrCheckItemAccess(ILExecThread *thread, ILProgramItem *item)
{
	ILClass *classInfo;
	ILMember *member;

	/* Is the item a class? */
	classInfo = ILProgramItemToClass(item);
	if(classInfo)
	{
		return _ILClrCheckAccess(thread, classInfo, (ILMember *)0);
	}

	/* Is the item a class member? */
	member = ILProgramItemToMember(item);
	if(member)
	{
		return _ILClrCheckAccess(thread, (ILClass *)0, member);
	}

	/* We assume that it is OK to access other types of items.
	   Usually these are assemblies and modules, which are public */
	return 1;
}

/*
 * Throw a "TypeLoadException" when a type lookup has failed.
 */
static void ThrowTypeLoad(ILExecThread *thread, ILString *name)
{
	ILExecThreadThrowSystem(thread, "System.TypeLoadException",
							(const char *)0);
}

ILObject *_ILGetClrType(ILExecThread *thread, ILClass *classInfo)
{
	ILObject *obj;

	/* Does the class already have a "ClrType" instance? */
	if(((ILClassPrivate *)(classInfo->userData))->clrType)
	{
		return ((ILClassPrivate *)(classInfo->userData))->clrType;
	}

	/* Create a new "ClrType" instance */
	if(!(thread->process->clrTypeClass))
	{
		ThrowTypeLoad(thread, 0);
		return 0;
	}
	obj = _ILEngineAllocObject(thread, thread->process->clrTypeClass);
	if(!obj)
	{
		return 0;
	}

	/* Fill in the object with the class information */
	((System_Reflection *)obj)->privateData = classInfo;

	/* Attach the object to the class so that it will be returned
	   for future calls to this function */
	((ILClassPrivate *)(classInfo->userData))->clrType = obj;

	/* Return the object to the caller */
	return obj;
}

ILObject *_ILGetClrTypeForILType(ILExecThread *thread, ILType *type)
{
	ILClass *classInfo;

	/* Strip custom modifier prefixes from the type */
	type = ILTypeStripPrefixes(type);

	/* Convert the type into an "ILClass" structure */
	classInfo = ILClassFromType(ILProgramItem_Image(thread->method),
								0, type, 0);

	/* Get the "ClrType" object for the "ILClass" structure */
	if(classInfo)
	{
		return _ILGetClrType(thread, classInfo);
	}
	else
	{
		return 0;
	}
}

ILClass *_ILGetClrClass(ILExecThread *thread, ILObject *type)
{
	if(type)
	{
		/* Make sure that "type" is an instance of "ClrType" */
		if(ILClassInheritsFrom(GetObjectClass(type),
							   thread->process->clrTypeClass))
		{
			return (ILClass *)(((System_Reflection *)type)->privateData);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

ILObject *_ILClrToObject(ILExecThread *thread, void *item, const char *name)
{
	/* TODO */
	return 0;
}

void *_ILClrFromObject(ILExecThread *thread, ILObject *object)
{
	/* TODO */
	return 0;
}

/*
 * public static Type GetType(String name, bool throwOnError, bool ignoreCase);
 */
static ILObject *System_Type_GetType(ILExecThread *thread,
									 ILString *name,
									 ILBool throwOnError,
									 ILBool ignoreCase)
{
	ILInt32 length;
	ILUInt16 *buf;
	ILInt32 posn;
	ILInt32 dot;
	ILInt32 nameStart;
	ILInt32 nameEnd;
	ILInt32 nameSpecial;
	int brackets;
	ILImage *assemblyImage = 0;
	ILProgramItem *scope;
	ILClass *classInfo;

	/* Validate the parameters */
	if(!name)
	{
		ILExecThreadThrowArgNull(thread, "name");
		return 0;
	}

	/* Extract the buffer portion of the name string */
	length = _ILStringToBuffer(thread, name, &buf);

	/* Turn off "ignoreCase" if the length is greater than 128.
	   Provided for consistency with ECMA, even though we can
	   check for case-insensitive type names of any length */
	if(length > 128)
	{
		ignoreCase = 0;
	}

	/* Check for an assembly suffix, to determine where to
	   start looking for the type */
	posn = 0;
	nameStart = 0;
	nameSpecial = -1;
	brackets = 0;
	while(posn < length && (buf[posn] != (ILUInt16)',' || brackets))
	{
		if(buf[posn] == '\\')
		{
			++posn;
			if(posn >= length)
			{
				/* Invalid escape, so this cannot be a valid type name */
				if(throwOnError)
				{
					ThrowTypeLoad(thread, name);
				}
				return 0;
			}
		}
		else if(buf[posn] == '[')
		{
			if(nameSpecial == -1)
			{
				nameSpecial = posn;
			}
			++brackets;
		}
		else if(buf[posn] == ']')
		{
			if(brackets > 0)
			{
				--brackets;
			}
		}
		else if(buf[posn] == '*' || buf[posn] == '&')
		{
			if(nameSpecial == -1)
			{
				nameSpecial = posn;
			}
		}
		++posn;
	}
	if(nameSpecial == -1)
	{
		nameSpecial = posn;
	}
	nameEnd = posn;
	if(posn < length)
	{
		++posn;
		assemblyImage = ResolveAssembly(thread, buf + posn, length - posn);
		if(!assemblyImage)
		{
			if(throwOnError)
			{
				ThrowTypeLoad(thread, name);
			}
			return 0;
		}
	}
	if(nameStart >= nameSpecial)
	{
		/* Empty type name means no type */
		if(throwOnError)
		{
			ThrowTypeLoad(thread, name);
		}
		return 0;
	}

	/* Find the initial lookup scope */
	if(assemblyImage)
	{
		scope = ILClassGlobalScope(assemblyImage);
	}
	else
	{
		scope = 0;
	}

	/* Locate the type */
	classInfo = 0;
	posn = nameStart;
	while(posn < nameSpecial)
	{
		/* Find the next '+', which is used to separate nested types.
		   Also find the last dot that indicates the namespace */
		dot = -1;
		while(posn < nameSpecial && buf[posn] != '+')
		{
			if(buf[posn] == '\\')
			{
				++posn;
			}
			else if(buf[posn] == '.')
			{
				dot = posn;
			}
			++posn;
		}
		if(classInfo != 0)
		{
			/* Dots in nested type names are not namespace delimiters */
			dot = -1;
		}

		/* Look for the class within the current scope */
		if(scope != 0)
		{
			if(dot != -1)
			{
				classInfo = ILClassLookupUnicode
					(scope, buf + dot + 1, posn - (dot + 1),
					 buf + nameStart, dot - nameStart, ignoreCase);
			}
			else
			{
				classInfo = ILClassLookupUnicode
					(scope, buf + nameStart, posn - nameStart,
					 0, 0, ignoreCase);
			}
		}
		else
		{
			/* Look in the same image as the caller first */
			scope = ILClassGlobalScope(CallerImage(thread));
			if(dot != -1)
			{
				classInfo = ILClassLookupUnicode
					(scope, buf + dot + 1, posn - (dot + 1),
					 buf + nameStart, dot - nameStart, ignoreCase);
			}
			else
			{
				classInfo = ILClassLookupUnicode
					(scope, buf + nameStart, posn - nameStart,
					 0, 0, ignoreCase);
			}

			/* Look in the global scope if not in the caller's image */
			if(!classInfo)
			{
				if(dot != -1)
				{
					classInfo = ILClassLookupGlobalUnicode
						(thread->process->context,
						 buf + dot + 1, posn - (dot + 1),
						 buf + nameStart, dot - nameStart, ignoreCase);
				}
				else
				{
					classInfo = ILClassLookupGlobalUnicode
						(thread->process->context,
						 buf + nameStart, posn - nameStart,
						 0, 0, ignoreCase);
				}
			}
		}
		if(!classInfo)
		{
			/* Could not find the class */
			if(throwOnError)
			{
				ThrowTypeLoad(thread, name);
			}
			return 0;
		}
		scope = (ILProgramItem *)classInfo;

		/* Advance to the next nested type name */
		if(posn < nameSpecial)
		{
			++posn;
			nameStart = posn;
			if(posn >= nameSpecial)
			{
				/* Empty nested type name */
				if(throwOnError)
				{
					ThrowTypeLoad(thread, name);
				}
				return 0;
			}
		}
	}

	/* Check that we have permission to reflect the type */
	if(!_ILClrCheckAccess(thread, classInfo, 0))
	{
		return 0;
	}

	/* Resolve special suffixes for array and pointer designations */
	if(nameSpecial < nameEnd)
	{
		/* TODO */
		if(throwOnError)
		{
			ThrowTypeLoad(thread, name);
		}
		return 0;
	}

	/* Convert the class information block into a "ClrType" instance */
	return _ILGetClrType(thread, classInfo);
}

/*
 * public static RuntimeTypeHandle GetTypeHandleFromObject(Object o);
 */
static void System_Type_GetTypeHandleFromObject
				(ILExecThread *thread, void *handle, ILObject *obj)
{
	if(obj)
	{
		*((ILClass **)handle) = GetObjectClass(obj);
	}
	else
	{
		ILExecThreadThrowArgNull(thread, "obj");
	}
}

/*
 * public static Type GetTypeFromHandle(RuntimeTypeHandle handle);
 */
static ILObject *System_Type_GetTypeFromHandle
						(ILExecThread *thread, void *handle)
{
	ILClass *classInfo = *((ILClass **)handle);
	if(classInfo)
	{
		return _ILGetClrType(thread, classInfo);
	}
	else
	{
		return 0;
	}
}

/*
 * private int GetClrArrayRank();
 */
static ILInt32 ClrType_GetClrArrayRank(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	ILType *synType = (classInfo ? ILClassGetSynType(classInfo) : 0);
	if(synType != 0)
	{
		ILInt32 rank = 1;
		while(synType != 0 && ILType_IsComplex(synType) &&
		      synType->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
		{
			++rank;
			synType = synType->un.array.elemType;
		}
		return rank;
	}
	else
	{
		return 0;
	}
}

/*
 * protected override TypeAttributes GetAttributeFlagsImpl();
 */
static ILInt32 ClrType_GetAttributeFlagsImpl(ILExecThread *thread,
											 ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	if(classInfo)
	{
		return (ILInt32)(ILClassGetAttrs(classInfo));
	}
	else
	{
		return 0;
	}
}

/*
 * public override Type GetElementType();
 */
static ILObject *ClrType_GetElementType(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	ILType *synType;
	if(classInfo)
	{
		/* Only interesting for array, pointer, and byref types */
		synType = ILClassGetSynType(classInfo);
		if(synType != 0 && ILType_IsComplex(synType))
		{
		   	while(synType->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
			{
				synType = synType->un.array.elemType;
			}
			if(synType->kind == IL_TYPE_COMPLEX_ARRAY)
			{
				return _ILGetClrTypeForILType
							(thread, synType->un.array.elemType);
			}
			else if(synType->kind == IL_TYPE_COMPLEX_BYREF ||
			        synType->kind == IL_TYPE_COMPLEX_PTR)
			{
				return _ILGetClrTypeForILType(thread, synType->un.refType);
			}
		}
	}
	return 0;
}

/*
 * Type categories: this must be kept in sync with the
 * definition of the "System.Reflection.ClrTypeCategory"
 * enumeration in "pnetlib".
 */
#define	ClrTypeCategory_Primitive		0
#define	ClrTypeCategory_Class			1
#define	ClrTypeCategory_ValueType		2
#define	ClrTypeCategory_Enum			3
#define	ClrTypeCategory_Array			4
#define	ClrTypeCategory_ByRef			5
#define	ClrTypeCategory_Pointer			6
#define	ClrTypeCategory_Method			7
#define	ClrTypeCategory_COMObject		8
#define	ClrTypeCategory_Other			9

/*
 * private ClrTypeCategory GetClrTypeCategory();
 */
static ILInt32 ClrType_GetClrTypeCategory(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	ILType *synType;
	if(classInfo)
	{
		synType = ILClassGetSynType(classInfo);
		if(synType != 0 && ILType_IsComplex(synType))
		{
			switch(synType->kind & 0xFF)
			{
				case IL_TYPE_COMPLEX_ARRAY:
				case IL_TYPE_COMPLEX_ARRAY_CONTINUE:
						 return ClrTypeCategory_Array;

				case IL_TYPE_COMPLEX_BYREF:
						 return ClrTypeCategory_ByRef;

				case IL_TYPE_COMPLEX_PTR:
						 return ClrTypeCategory_Pointer;

				case IL_TYPE_COMPLEX_METHOD:
				case IL_TYPE_COMPLEX_METHOD | IL_TYPE_COMPLEX_METHOD_SENTINEL:
						 return ClrTypeCategory_Method;

				default: return ClrTypeCategory_Other;
			}
		}
		if(ILClassIsValueType(classInfo))
		{
			ILType *type = ILClassToType(classInfo);
			if(ILType_IsPrimitive(type))
			{
				switch(ILType_ToElement(type))
				{
					case IL_META_ELEMTYPE_BOOLEAN:
					case IL_META_ELEMTYPE_I1:
					case IL_META_ELEMTYPE_U1:
					case IL_META_ELEMTYPE_I2:
					case IL_META_ELEMTYPE_U2:
					case IL_META_ELEMTYPE_CHAR:
					case IL_META_ELEMTYPE_I4:
					case IL_META_ELEMTYPE_U4:
					case IL_META_ELEMTYPE_I8:
					case IL_META_ELEMTYPE_U8:
					case IL_META_ELEMTYPE_R4:
					case IL_META_ELEMTYPE_R8:
						return ClrTypeCategory_Primitive;
				}
			}
			else if(ILTypeGetEnumType(type) != type)
			{
				return ClrTypeCategory_Enum;
			}
			return ClrTypeCategory_ValueType;
		}
	}
	return ClrTypeCategory_Class;
}

/*
 * public override bool IsSubclassOf(Type c);
 */
static ILBool ClrType_IsSubclassOf(ILExecThread *thread,
								   ILObject *_this,
								   ILObject *otherType)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	ILClass *otherClassInfo;
	if(otherType)
	{
		otherClassInfo = _ILGetClrClass(thread, otherType);
		if(classInfo && otherClassInfo)
		{
			return ILClassInheritsFrom(classInfo, otherClassInfo);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		ILExecThreadThrowArgNull(thread, "c");
		return 0;
	}
}

/*
 * private bool IsClrNestedType();
 */
static ILBool ClrType_IsClrNestedType(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	if(classInfo && ILClass_NestedParent(classInfo) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * private Type GetClrBaseType();
 */
static ILObject *ClrType_GetClrBaseType(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	if(classInfo)
	{
		ILClass *parent = ILClass_Parent(classInfo);
		if(parent)
		{
			return _ILGetClrType(thread, parent);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

/*
 * private Assembly GetClrAssembly();
 */
static ILObject *ClrType_GetClrAssembly(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	ILImage *image = (classInfo ? ILClassToImage(classInfo) : 0);
	void *item;
	if(image)
	{
		item = ILImageTokenInfo(image, (IL_META_TOKEN_ASSEMBLY | 1));
		if(item)
		{
			return _ILClrToObject(thread, item, "System.Reflection.Assembly");
		}
		/* TODO: if the image does not have an assembly manifest,
		   then look for the parent assembly */
	}
	return 0;
}

/*
 * private Module GetClrModule();
 */
static ILObject *ClrType_GetClrModule(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	ILImage *image = (classInfo ? ILClassToImage(classInfo) : 0);
	void *item;
	if(image)
	{
		item = ILImageTokenInfo(image, (IL_META_TOKEN_MODULE | 1));
		if(item)
		{
			return _ILClrToObject(thread, item, "System.Reflection.Module");
		}
	}
	return 0;
}

/*
 * Output a string to a Unicode name buffer, or compute the length
 * of the buffer.  Returns the length.
 */
static ILInt32 NameOutputString(ILUInt16 *buf, const char *str, int quoteDot)
{
	int slen, sposn;
	ILInt32 len = 0;
	unsigned long ch;
	unsigned long tempch;
	slen = strlen(str);
	sposn = 0;
	while(sposn < slen)
	{
		ch = ILUTF8ReadChar(str, slen, &sposn);
		if(ch == '\\' || (ch == '.' && quoteDot) || ch == '+' ||
		   ch == ',' || ch == '[' || ch == ']' || ch == '&' || ch == '*')
		{
			/* We need to quote this character */
			if(buf)
			{
				buf[0] = (ILUInt16)'\\';
				buf[1] = (ILUInt16)ch;
				buf += 2;
			}
			len += 2;
		}
		else if(ch < (unsigned long)0x10000)
		{
			/* Ordinary character */
			if(buf)
			{
				*buf++ = (ILUInt16)ch;
			}
			++len;
		}
		else if(ch < ((((unsigned long)1)) << 20))
		{
			/* Surrogate-based character */
			if(buf)
			{
				tempch = ((ch >> 10) + 0xD800);
				buf[0] = (ILUInt16)tempch;
				tempch = ((ch & ((((ILUInt32)1) << 10) - 1)) + 0xDC00);
				buf[1] = (ILUInt16)tempch;
				buf += 2;
			}
			len += 2;
		}
	}
	return len;
}

/*
 * Output a class name to a Unicode name buffer, or compute the length.
 * Returns the computed length.
 */
static ILInt32 NameOutputClassName(ILUInt16 *buf, ILClass *classInfo,
								   int fullyQualified)
{
	ILClass *nestedParent;
	ILInt32 len;
	const char *namespace;
	if(fullyQualified)
	{
		nestedParent = ILClass_NestedParent(classInfo);
		if(nestedParent != 0)
		{
			len = NameOutputClassName(buf, ILClassResolve(nestedParent),
									  fullyQualified);
			if(buf != 0)
			{
				buf[len++] = (ILUInt16)'+';
			}
			else
			{
				++len;
			}
		}
		else
		{
			namespace = ILClass_Namespace(classInfo);
			if(namespace)
			{
				len = NameOutputString(buf, namespace, 0);
				if(buf != 0)
				{
					buf[len++] = (ILUInt16)'.';
				}
				else
				{
					++len;
				}
			}
			else
			{
				len = 0;
			}
		}
	}
	else
	{
		len = 0;
	}
	if(buf != 0)
	{
		return len + NameOutputString(buf + len, ILClass_Name(classInfo), 1);
	}
	else
	{
		return len + NameOutputString(0, ILClass_Name(classInfo), 1);
	}
}

/*
 * Output the suffixes for a type, or compute the buffer length.
 * Returns the computed length.
 */
static ILInt32 NameOutputTypeSuffixes(ILUInt16 *buf, ILType *type)
{
	ILInt32 len = 0;
	ILInt32 rank;
	if(type != 0 && ILType_IsComplex(type))
	{
		if(type->kind == IL_TYPE_COMPLEX_ARRAY)
		{
			len += NameOutputTypeSuffixes(buf, type->un.array.elemType);
			if(buf != 0)
			{
				buf[len++] = (ILUInt16)'[';
				buf[len++] = (ILUInt16)']';
			}
			else
			{
				len += 2;
			}
		}
		else if(type->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
		{
			rank = 1;
			type = type->un.array.elemType;
			while(type != 0 && ILType_IsComplex(type) &&
			      type->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
			{
				++rank;
				type = type->un.array.elemType;
			}
			len += NameOutputTypeSuffixes(buf, type);
			if(buf != 0)
			{
				buf[len++] = (ILUInt16)'[';
			}
			else
			{
				++len;
			}
			while(rank > 0)
			{
				if(buf != 0)
				{
					buf[len++] = (ILUInt16)',';
				}
				else
				{
					++len;
				}
				--rank;
			}
			if(buf != 0)
			{
				buf[len++] = (ILUInt16)']';
			}
			else
			{
				++len;
			}
		}
		else if(type->kind == IL_TYPE_COMPLEX_BYREF)
		{
			len += NameOutputTypeSuffixes(buf, type->un.refType);
			if(buf != 0)
			{
				buf[len++] = (ILUInt16)'&';
			}
			else
			{
				++len;
			}
		}
		else if(type->kind == IL_TYPE_COMPLEX_PTR)
		{
			len += NameOutputTypeSuffixes(buf, type->un.refType);
			if(buf != 0)
			{
				buf[len++] = (ILUInt16)'*';
			}
			else
			{
				++len;
			}
		}
	}
	return len;
}

/*
 * Get the name of a type in either regular or fully-qualified form.
 */
static ILString *GetTypeName(ILExecThread *thread, ILObject *_this,
							 int fullyQualified)
{
	ILClass *classInfo;
	ILClass *elemInfo;
	ILInt32 len;
	ILString *str;
	ILUInt16 *buf;
	ILType *synType;

	/* Get the ILClass structure for the runtime type */
	classInfo = _ILGetClrClass(thread, _this);
	if(!classInfo)
	{
		/* Shouldn't happen, but do something sane anyway */
		return 0;
	}

	/* Find the innermost element type if this is a complex type */
	elemInfo = classInfo;
	synType = ILClassGetSynType(classInfo);
	while(synType != 0 && ILType_IsComplex(synType))
	{
		if(synType->kind == IL_TYPE_COMPLEX_ARRAY ||
		   synType->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
		{
			synType = synType->un.array.elemType;
		}
		else if(synType->kind == IL_TYPE_COMPLEX_BYREF ||
		        synType->kind == IL_TYPE_COMPLEX_PTR)
		{
			synType = synType->un.refType;
		}
		else
		{
			break;
		}
	}
	if(synType != 0)
	{
		elemInfo = ILClassFromType(ILProgramItem_Image(thread->method),
								   0, synType, 0);
		if(!elemInfo)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
	}

	/* Compute the size of the full name */
	len = NameOutputClassName(0, elemInfo, fullyQualified);
	if(classInfo != elemInfo)
	{
		len += NameOutputTypeSuffixes(0, synType);
	}

	/* Allocate a string to hold the full name */
	str = (ILString *)ILExecThreadNew(thread, "System.String", "(Tci)V",
						  			  (ILVaInt)' ', (ILVaInt)len);
	if(!str)
	{
		return 0;
	}

	/* Write the name into the string */
	if(_ILStringToBuffer(thread, str, &buf))
	{
		len = NameOutputClassName(buf, elemInfo, fullyQualified);
		if(classInfo != elemInfo)
		{
			NameOutputTypeSuffixes(buf + len, synType);
		}
	}
	return str;
}

/*
 * private String GetClrFullName();
 */
static ILString *ClrType_GetClrFullName(ILExecThread *thread, ILObject *_this)
{
	return GetTypeName(thread, _this, 1);
}

/*
 * private Guid GetClrGUID();
 */
static void ClrType_GetClrGUID(ILExecThread *thread,
							   void *result, ILObject *_this)
{
	/* We don't use GUID's in this system, as they are intended for
	   use with COM, which we don't have.  Besides, they are a stupid
	   way to specify globally-unique names.  URI's are much better */
	ILMemZero(result, 16);
}

/*
 * private Type GetClrNestedDeclaringType();
 */
static ILObject *ClrType_GetClrNestedDeclaringType
					(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	ILClass *nestedParent = (classInfo ? ILClass_NestedParent(classInfo) : 0);
	if(nestedParent)
	{
		return _ILGetClrType(thread, nestedParent);
	}
	else
	{
		return 0;
	}
}

/*
 * private String GetClrName();
 */
static ILString *ClrType_GetClrName(ILExecThread *thread, ILObject *_this)
{
	return GetTypeName(thread, _this, 0);
}

/*
 * private String GetClrNamespace();
 */
static ILString *ClrType_GetClrNamespace(ILExecThread *thread, ILObject *_this)
{
	ILClass *classInfo = _ILGetClrClass(thread, _this);
	ILClass *nestedParent;
	ILType *synType;
	const char *namespace;
	if(classInfo)
	{
		/* Find the innermost element type if the class is synthesised */
		synType = ILClassGetSynType(classInfo);
		while(synType != 0 && ILType_IsComplex(synType))
		{
			if(synType->kind == IL_TYPE_COMPLEX_ARRAY ||
			   synType->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
			{
				synType = synType->un.array.elemType;
			}
			else if(synType->kind == IL_TYPE_COMPLEX_BYREF ||
			        synType->kind == IL_TYPE_COMPLEX_PTR)
			{
				synType = synType->un.refType;
			}
			else
			{
				break;
			}
		}
		if(synType != 0)
		{
			classInfo = ILClassFromType(ILProgramItem_Image(thread->method),
									    0, synType, 0);
			if(!classInfo)
			{
				ILExecThreadThrowOutOfMemory(thread);
				return 0;
			}
		}

		/* Find the outermost type in the nesting levels, to
		   determine the namespace in which nested types reside */
		while((nestedParent = ILClass_NestedParent(classInfo)) != 0)
		{
			classInfo = ILClassResolve(nestedParent);
		}
		namespace = ILClass_Namespace(classInfo);
		if(namespace)
		{
			return ILStringCreate(thread, namespace);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

/*
 * Method table for the "System.Type" class.
 */
IL_METHOD_BEGIN(_ILSystemTypeMethods)
	IL_METHOD("GetType",
					"(oSystem.String;ZZ)oSystem.Type;",
					System_Type_GetType)
	IL_METHOD("GetTypeHandleFromObject",
					"(oSystem.Object;)vSystem.RuntimeTypeHandle;",
					System_Type_GetTypeHandleFromObject)
	IL_METHOD("GetTypeFromHandle",
					"(vSystem.RuntimeTypeHandle;)oSystem.Type;",
					System_Type_GetTypeFromHandle)
IL_METHOD_END

/*
 * Method table for the "System.Reflection.ClrType" class.
 */
IL_METHOD_BEGIN(_ILReflectionClrTypeMethods)
	IL_METHOD("GetClrArrayRank",
					"(T)i",
					ClrType_GetClrArrayRank)
	IL_METHOD("GetAttributeFlagsImpl",
					"(T)i",
					ClrType_GetAttributeFlagsImpl)
	IL_METHOD("GetElementType",
					"(T)oSystem.Type;",
					ClrType_GetElementType)
	IL_METHOD("GetClrTypeCategory",
					"(T)vSystem.Reflection.ClrTypeCategory;",
					ClrType_GetClrTypeCategory)
	IL_METHOD("IsSubclassOf",
					"(ToSystem.Type;)Z",
					ClrType_IsSubclassOf)
	IL_METHOD("IsClrNestedType",
					"(T)Z",
					ClrType_IsClrNestedType)
	IL_METHOD("GetClrBaseType",
					"(T)oSystem.Type;",
					ClrType_GetClrBaseType)
	IL_METHOD("GetClrAssembly",
					"(T)oSystem.Reflection.Assembly;",
					ClrType_GetClrAssembly)
	IL_METHOD("GetClrModule",
					"(T)oSystem.Reflection.Module;",
					ClrType_GetClrModule)
	IL_METHOD("GetClrFullName",
					"(T)oSystem.String;",
					ClrType_GetClrFullName)
	IL_METHOD("GetClrGUID",
					"(T)vSystem.Guid;",
					ClrType_GetClrGUID)
	IL_METHOD("GetClrNestedDeclaringType",
					"(T)oSystem.Type;",
					ClrType_GetClrNestedDeclaringType)
	IL_METHOD("GetClrName",
					"(T)vSystem.String;",
					ClrType_GetClrName)
	IL_METHOD("GetClrNamespace",
					"(T)vSystem.String;",
					ClrType_GetClrNamespace)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
