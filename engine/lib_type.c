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

/*
 * Hash table entry information for a reflection mapping.
 */
#define	IL_REFLECTION_HASH_SIZE		512
typedef struct _tagILClrHash ILClrHash;
struct _tagILClrHash
{
	ILObject   *object;
	ILClrHash  *next;

};

ILObject *_ILClrToObject(ILExecThread *thread, void *item, const char *name)
{
	ILUInt32 hash;
	ILClrHash *entry;
	ILClrHash *newEntry;
	ILClass *classInfo;
	ILObject *object;

	/* Create the reflection hash table */
	if(!(thread->process->reflectionHash))
	{
		thread->process->reflectionHash =
			ILGCAllocPersistent(sizeof(ILClrHash) * IL_REFLECTION_HASH_SIZE);
		if(!(thread->process->reflectionHash))
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
	}

	/* Compute the hash value for the item */
	hash = ILProgramItem_Token((ILProgramItem *)item);
	hash = ((hash + (hash >> 20)) & (IL_REFLECTION_HASH_SIZE - 1));

	/* Look for an object that is already mapped to the item */
	entry = &(((ILClrHash *)(thread->process->reflectionHash))[hash]);
	while(entry != 0)
	{
		if(entry->object != 0 &&
		   ((System_Reflection *)(entry->object))->privateData == item)
		{
			return entry->object;
		}
		entry = entry->next;
	}

	/* Construct a new object of the class "name" to hold the item */
	classInfo = ILExecThreadLookupClass(thread, name);
	if(!classInfo)
	{
		return 0;
	}
	object = _ILEngineAllocObject(thread, classInfo);
	if(!object)
	{
		return 0;
	}
	((System_Reflection *)object)->privateData = item;

	/* Add the object to the reflection hash */
	entry = &(((ILClrHash *)(thread->process->reflectionHash))[hash]);
	if(entry->object == 0)
	{
		/* The main table slot is free, so put the object there */
		entry->object = object;
	}
	else
	{
		/* We need an overflow entry for the item */
		newEntry = (ILClrHash *)ILGCAlloc(sizeof(ILClrHash));
		if(!newEntry)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		newEntry->object = object;
		newEntry->next = entry->next;
		entry->next = newEntry;
	}

	/* Return the reflected object to the caller */
	return object;
}

void *_ILClrFromObject(ILExecThread *thread, ILObject *object)
{
	if(object)
	{
		return ((System_Reflection *)object)->privateData;
	}
	else
	{
		return 0;
	}
}

ILObject *_ILGetTypeFromImage(ILExecThread *thread,
							  ILImage *assemblyImage,
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
		if(assemblyImage)
		{
			/* We are trying to look in a specific image,
			   so it is an error for the caller to supply
			   an explicit assembly name */
			if(throwOnError)
			{
				ThrowTypeLoad(thread, name);
			}
			return 0;
		}
		else
		{
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
 * public static Type GetType(String name, bool throwOnError, bool ignoreCase);
 */
static ILObject *System_Type_GetType(ILExecThread *thread,
									 ILString *name,
									 ILBool throwOnError,
									 ILBool ignoreCase)
{
	return _ILGetTypeFromImage(thread, 0, name, throwOnError, ignoreCase);
}

/*
 * public static RuntimeTypeHandle GetTypeHandle(Object o);
 */
static void System_Type_GetTypeHandle(ILExecThread *thread,
									  void *handle, ILObject *obj)
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
 * Method table for the "System.Type" class.
 */
IL_METHOD_BEGIN(_ILSystemTypeMethods)
	IL_METHOD("GetType",
					"(oSystem.String;ZZ)oSystem.Type;",
					System_Type_GetType)
	IL_METHOD("GetTypeHandle",
					"(oSystem.Object;)vSystem.RuntimeTypeHandle;",
					System_Type_GetTypeHandle)
	IL_METHOD("GetTypeFromHandle",
					"(vSystem.RuntimeTypeHandle;)oSystem.Type;",
					System_Type_GetTypeFromHandle)
IL_METHOD_END

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
		if(classInfo && otherClassInfo && classInfo != otherClassInfo)
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
 * Values for "System.Reflection.MemberTypes".
 */
typedef enum
{
    MT_Constructor         = 0x0001,
    MT_Event               = 0x0002,
    MT_Field               = 0x0004,
    MT_Method              = 0x0008,
    MT_Property            = 0x0010,
    MT_TypeInfo            = 0x0020,
    MT_Custom              = 0x0040,
    MT_NestedType          = 0x0080,
    MT_All                 = 0x00BF

} MemberTypes;

/*
 * Values for "System.Reflection.BindingFlags".
 */
typedef enum
{
    BF_Default              = 0x00000000,
    BF_IgnoreCase           = 0x00000001,
    BF_DeclaredOnly         = 0x00000002,
    BF_Instance             = 0x00000004,
    BF_Static               = 0x00000008,
    BF_Public               = 0x00000010,
    BF_NonPublic            = 0x00000020,
    BF_FlattenHierarchy     = 0x00000040,
    BF_InvokeMethod         = 0x00000100,
    BF_CreateInstance       = 0x00000200,
    BF_GetField             = 0x00000400,
    BF_SetField             = 0x00000800,
    BF_GetProperty          = 0x00001000,
    BF_SetProperty          = 0x00002000,
    BF_PutDispProperty      = 0x00004000,
    BF_PutRefDispProperty   = 0x00008000,
    BF_ExactBinding         = 0x00010000,
    BF_SuppressChangeType   = 0x00020000,
    BF_OptionalParamBinding = 0x00040000,
    BF_IgnoreReturn         = 0x01000000

} BindingFlags;

/*
 * Values for "System.Reflection.CallingConventions".
 */
typedef enum
{
	CC_Standard				= 0x01,
	CC_VarArgs				= 0x02,
	CC_Any					= 0x03,
	CC_Mask					= 0x1F,
	CC_HasThis				= 0x20,
	CC_ExplicitThis			= 0x40

} CallingConventions;

/*
 * Determine if a member kind matches a particular member type set.
 */
static int MemberTypeMatch(ILMember *member, ILInt32 memberTypes)
{
	switch(member->kind)
	{
		case IL_META_MEMBERKIND_METHOD:
		{
			if(ILMethod_IsConstructor((ILMethod *)member) ||
			   ILMethod_IsStaticConstructor((ILMethod *)member))
			{
				return ((memberTypes & (ILInt32)MT_Constructor) != 0);
			}
			else
			{
				return ((memberTypes & (ILInt32)MT_Method) != 0);
			}
		}
		/* Not reached */

		case IL_META_MEMBERKIND_FIELD:
			return ((memberTypes & (ILInt32)MT_Field) != 0);

		case IL_META_MEMBERKIND_PROPERTY:
			return ((memberTypes & (ILInt32)MT_Property) != 0);

		case IL_META_MEMBERKIND_EVENT:
			return ((memberTypes & (ILInt32)MT_Event) != 0);

		default: break;
	}
	return 0;
}

/*
 * Match binding attributes and calling conventions against a member.
 */
static int BindingAttrMatch(ILMember *member, ILInt32 bindingAttrs,
							ILInt32 callConv)
{
	ILMethod *method;

	/* Convert properties and events into their underlying semantic methods */
	if(member->kind == IL_META_MEMBERKIND_PROPERTY)
	{
		method = ILPropertyGetGetter((ILProperty *)member);
		if(!method)
		{
			method = ILPropertyGetSetter((ILProperty *)member);
			if(!method)
			{
				return 0;
			}
		}
		member = &(method->member);
	}
	else if(member->kind == IL_META_MEMBERKIND_EVENT)
	{
		method = ILEventGetAddOn((ILEvent *)member);
		if(!method)
		{
			method = ILEventGetRemoveOn((ILEvent *)member);
			if(!method)
			{
				method = ILEventGetFire((ILEvent *)member);
				if(!method)
				{
					return 0;
				}
			}
		}
		member = &(method->member);
	}

	/* Check the access level against the binding attributes */
	if((bindingAttrs & (ILInt32)BF_Public) != 0 &&
	   (bindingAttrs & (ILInt32)BF_NonPublic) == 0)
	{
		/* Only look for public members */
		if((member->attributes & IL_META_METHODDEF_MEMBER_ACCESS_MASK)
				!= IL_META_METHODDEF_PUBLIC)
		{
			return 0;
		}
	}
	else if((bindingAttrs & (ILInt32)BF_Public) == 0 &&
	        (bindingAttrs & (ILInt32)BF_NonPublic) != 0)
	{
		/* Only look for non-public members */
		if((member->attributes & IL_META_METHODDEF_MEMBER_ACCESS_MASK)
				== IL_META_METHODDEF_PUBLIC)
		{
			return 0;
		}
	}
	else if((bindingAttrs & (ILInt32)BF_Public) == 0 &&
	        (bindingAttrs & (ILInt32)BF_NonPublic) == 0)
	{
		/* Don't look for public or non-public members */
		return 0;
	}

	/* Check for instance and static properties */
	if((bindingAttrs & (ILInt32)BF_Static) != 0 &&
	   (bindingAttrs & (ILInt32)BF_Instance) == 0)
	{
		/* Look for static members only */
		if((member->attributes & IL_META_METHODDEF_STATIC) == 0)
		{
			return 0;
		}
	}
	else if((bindingAttrs & (ILInt32)BF_Static) == 0 &&
	        (bindingAttrs & (ILInt32)BF_Instance) != 0)
	{
		/* Look for instance members only */
		if((member->attributes & IL_META_METHODDEF_STATIC) != 0)
		{
			return 0;
		}
	}
	else if((bindingAttrs & (ILInt32)BF_Static) == 0 &&
	        (bindingAttrs & (ILInt32)BF_Instance) == 0)
	{
		/* Don't look for static or instance members */
		return 0;
	}

	/* If we have a method, then match the calling conventions also */
	if((method = ILProgramItemToMethod(&(member->programItem))) != 0)
	{
		int mconv = method->callingConventions;
		if((callConv & (ILInt32)CC_Mask) == CC_Standard)
		{
			if((mconv & IL_META_CALLCONV_MASK) != IL_META_CALLCONV_DEFAULT)
			{
				return 0;
			}
		}
		else if((callConv & (ILInt32)CC_Mask) == CC_VarArgs)
		{
			if((mconv & IL_META_CALLCONV_MASK) != IL_META_CALLCONV_VARARG)
			{
				return 0;
			}
		}
		else if((callConv & (ILInt32)CC_Mask) != CC_Any)
		{
			if((mconv & IL_META_CALLCONV_MASK) != IL_META_CALLCONV_DEFAULT &&
			   (mconv & IL_META_CALLCONV_MASK) != IL_META_CALLCONV_VARARG)
			{
				return 0;
			}
		}
		if((callConv & (ILInt32)CC_HasThis) != 0)
		{
			if((mconv & IL_META_CALLCONV_HASTHIS) == 0)
			{
				return 0;
			}
		}
		if((callConv & (ILInt32)CC_ExplicitThis) != 0)
		{
			if((mconv & IL_META_CALLCONV_EXPLICITTHIS) == 0)
			{
				return 0;
			}
		}
	}

	/* The member matches the binding attributes */
	return 1;
}

/*
 * Match binding attributes against a class.
 */
static int BindingAttrClassMatch(ILClass *classInfo, ILInt32 bindingAttrs)
{
	/* Check the access level against the binding attributes */
	if((bindingAttrs & (ILInt32)BF_Public) != 0 &&
	   (bindingAttrs & (ILInt32)BF_NonPublic) == 0)
	{
		/* Only look for public classes */
		if((classInfo->attributes & IL_META_TYPEDEF_VISIBILITY_MASK)
				!= IL_META_TYPEDEF_PUBLIC &&
		   (classInfo->attributes & IL_META_TYPEDEF_VISIBILITY_MASK)
				!= IL_META_TYPEDEF_NESTED_PUBLIC)
		{
			return 0;
		}
	}
	else if((bindingAttrs & (ILInt32)BF_Public) == 0 &&
	        (bindingAttrs & (ILInt32)BF_NonPublic) != 0)
	{
		/* Only look for non-public classes */
		if((classInfo->attributes & IL_META_TYPEDEF_VISIBILITY_MASK)
				== IL_META_TYPEDEF_PUBLIC ||
		   (classInfo->attributes & IL_META_TYPEDEF_VISIBILITY_MASK)
				== IL_META_TYPEDEF_NESTED_PUBLIC)
		{
			return 0;
		}
	}
	else if((bindingAttrs & (ILInt32)BF_Public) == 0 &&
	        (bindingAttrs & (ILInt32)BF_NonPublic) == 0)
	{
		/* Don't look for public or non-public classes */
		return 0;
	}

	/* The class matches the binding attributes */
	return 1;
}

/*
 * Convert a program item into a reflection object of the appropriate type.
 */
static ILObject *ItemToClrObject(ILExecThread *thread, ILProgramItem *item)
{
	ILMethod *method;
	ILField *field;
	ILProperty *property;
	ILEvent *event;
	ILClass *classInfo;

	/* Is this a method? */
	method = ILProgramItemToMethod(item);
	if(method)
	{
		if(ILMethod_IsConstructor(method) ||
		   ILMethod_IsStaticConstructor(method))
		{
			return _ILClrToObject
				(thread, method, "System.Reflection.ClrConstructor");
		}
		else
		{
			return _ILClrToObject
				(thread, method, "System.Reflection.ClrMethod");
		}
	}

	/* Is this a field? */
	field = ILProgramItemToField(item);
	if(field)
	{
		return _ILClrToObject(thread, field, "System.Reflection.ClrField");
	}

	/* Is this a property? */
	property = ILProgramItemToProperty(item);
	if(property)
	{
		return _ILClrToObject
			(thread, property, "System.Reflection.ClrProperty");
	}

	/* Is this an event? */
	event = ILProgramItemToEvent(item);
	if(event)
	{
		return _ILClrToObject(thread, event, "System.Reflection.ClrEvent");
	}

	/* Is this a nested type? */
	classInfo = ILProgramItemToClass(item);
	if(classInfo)
	{
		return _ILGetClrType(thread, classInfo);
	}

	/* Don't know what this is */
	return 0;
}

/*
 * Match the parameters for a method or property.
 */
static int ParameterTypeMatch(ILExecThread *thread, ILType *signature,
							  System_Array *types)
{
	ILObject **items;
	ILInt32 paramNum;
	ILClass *classInfo;
	ILType *typeInfo;

	/* Check the number of parameters */
	if(((ILInt32)(signature->num)) != types->length)
	{
		return 0;
	}

	/* Scan the parameters and check for matches */
	items = (ILObject **)ArrayToBuffer(types);
	for(paramNum = 0; paramNum < types->length; ++paramNum)
	{
		classInfo = _ILGetClrClass(thread, items[paramNum]);
		if(!classInfo)
		{
			return 0;
		}
		typeInfo = ILClassToType(classInfo);
		if(!ILTypeIdentical(typeInfo, ILTypeGetParam(signature, paramNum + 1)))
		{
			return 0;
		}
	}

	/* We have a parameter match */
	return 1;
}

/*
 * private MemberInfo GetMemberImpl(String name, MemberTypes memberTypes,
 *								    BindingFlags bindingAttrs,
 *								    Binder binder,
 *								    CallingConventions callConv,
 *								    Type[] types,
 *								    ParameterModifier[] modifiers);
 */
static ILObject *ClrType_GetMemberImpl(ILExecThread *thread,
									   ILObject *_this,
									   ILString *name,
									   ILInt32 memberTypes,
									   ILInt32 bindingAttrs,
									   ILObject *binder,
									   ILInt32 callConv,
									   ILObject *types,
									   ILObject *modifiers)
{
	ILClass *classInfo;
	char *nameUtf8;
	ILMember *member;
	ILProgramItem *foundItem;
	ILNestedInfo *nested;
	ILClass *child;

	/* Validate the parameters */
	if(!name)
	{
		ILExecThreadThrowArgNull(thread, "name");
		return 0;
	}
	if(bindingAttrs == 0)
	{
		bindingAttrs = (ILInt32)(BF_Public | BF_Instance | BF_Static);
	}

	/* Convert the name into a UTF-8 string */
	nameUtf8 = ILStringToUTF8(thread, name);
	if(!nameUtf8)
	{
		return 0;
	}

	/* If the name is greater than 128 characters in length,
	   then turn off the "ignore case" flag.  This is an ECMA
	   requirement, even though we can handle arbitrarily
	   sized member names just fine */
	if(strlen(nameUtf8) >= 128)
	{
		bindingAttrs &= ~(ILInt32)BF_IgnoreCase;
	}

	/* Convert the type into an ILClass structure */
	classInfo = _ILGetClrClass(thread, _this);
	if(!classInfo)
	{
		return 0;
	}

	/* Scan the class hierarchy looking for a member match */
	do
	{
		member = classInfo->firstMember;
		foundItem = 0;
		while(member != 0)
		{
			/* Check for a name match first */
			if((bindingAttrs & (ILInt32)BF_IgnoreCase) == 0)
			{
				if(strcmp(member->name, nameUtf8) != 0)
				{
					member = member->nextMember;
					continue;
				}
			}
			else
			{
				if(ILStrICmp(member->name, nameUtf8) != 0)
				{
					member = member->nextMember;
					continue;
				}
			}
	
			/* Filter based on the member type */
			if(!MemberTypeMatch(member, memberTypes))
			{
				member = member->nextMember;
				continue;
			}
	
			/* Filter based on the binding attributes and calling conventions */
			if(!BindingAttrMatch(member, bindingAttrs, callConv))
			{
				member = member->nextMember;
				continue;
			}
	
			/* Filter based on the parameter types */
			if(types != 0)
			{
				if(member->kind == IL_META_MEMBERKIND_METHOD ||
				   member->kind == IL_META_MEMBERKIND_PROPERTY)
				{
					if(!ParameterTypeMatch(thread, member->signature,
										   (System_Array *)types))
					{
						member = member->nextMember;
						continue;
					}
				}
			}
	
			/* Check that we have reflection access for this member */
			if(!_ILClrCheckAccess(thread, (ILClass *)0, member))
			{
				member = member->nextMember;
				continue;
			}
	
			/* This is a candidate member */
			if(foundItem)
			{
				/* The member match is ambiguous */
			ambiguous:
				ILExecThreadThrowSystem
						(thread, "System.Reflection.AmbiguousMatchException",
						 (const char *)0);
				return 0;
			}
			foundItem = &(member->programItem);
	
			/* Advance to the next member */
			member = member->nextMember;
		}

		/* Scan the nested types also */
		if((memberTypes & (ILInt32)MT_NestedType) != 0)
		{
			nested = 0;
			while((nested = ILClassNextNested(classInfo, nested)) != 0)
			{
				child = ILNestedInfoGetChild(nested);
				if(child)
				{
					/* Filter the child based on its name */
					if((bindingAttrs & (ILInt32)BF_IgnoreCase) == 0)
					{
						if(strcmp(child->name, nameUtf8) != 0)
						{
							continue;
						}
					}
					else
					{
						if(ILStrICmp(child->name, nameUtf8) != 0)
						{
							continue;
						}
					}

					/* Filter the child based on its access level */
					if(!BindingAttrClassMatch(child, bindingAttrs))
					{
						continue;
					}

					/* Check that we have reflection access for this child */
					if(!_ILClrCheckAccess(thread, child, (ILMember *)0))
					{
						continue;
					}
	
					/* We have a candidate nested type */
					if(foundItem)
					{
						/* The match is ambiguous */
						goto ambiguous;
					}
					foundItem = &(child->programItem);
				}
			}
		}

		/* Move up the class hierarchy */
		classInfo = ILClass_Parent(classInfo);
	}
	while(classInfo != 0 &&
	      (bindingAttrs & (ILInt32)BF_DeclaredOnly) == 0);

	/* Return the item that was found to the caller */
	if(foundItem)
	{
		return ItemToClrObject(thread, foundItem);
	}
	else
	{
		return 0;
	}
}

/*
 * private Object GetMembersImpl(MemberTypes memberTypes,
 *								 BindingFlags bindingAttr,
 *								 Type arrayType, String name);
 */
static ILObject *ClrType_GetMembersImpl(ILExecThread *thread,
									    ILObject *_this,
										ILInt32 memberTypes,
										ILInt32 bindingAttrs,
										ILObject *arrayType,
										ILString *name)
{
	ILClass *classInfo;
	char *nameUtf8;
	ILMember *member;
	ILObject *foundObject;
	ILNestedInfo *nested;
	ILClass *child;
	System_Array *array;
	System_Array *newArray;
	ILClass *arrayClass;
	ILInt32 numFound;

	/* Validate the parameters */
	if(bindingAttrs == 0)
	{
		bindingAttrs = (ILInt32)(BF_Public | BF_Instance | BF_Static);
	}

	/* Convert the name into a UTF-8 string */
	if(name)
	{
		nameUtf8 = ILStringToUTF8(thread, name);
		if(!nameUtf8)
		{
			return 0;
		}

		/* If the name is greater than 128 characters in length,
		   then turn off the "ignore case" flag.  This is an ECMA
		   requirement, even though we can handle arbitrarily
		   sized member names just fine */
		if(strlen(nameUtf8) >= 128)
		{
			bindingAttrs &= ~(ILInt32)BF_IgnoreCase;
		}
	}
	else
	{
		nameUtf8 = 0;
	}

	/* Allocate the initial array */
	arrayClass = _ILGetClrClass(thread, arrayType);
	if(!arrayClass)
	{
		return 0;
	}
	array = (System_Array *)_ILEngineAlloc(thread, arrayClass,
										   sizeof(System_Array) +
										   sizeof(void *) * 4);
	if(!array)
	{
		return 0;
	}
	array->length = 4;

	/* Convert the type into an ILClass structure */
	classInfo = _ILGetClrClass(thread, _this);
	if(!classInfo)
	{
		return 0;
	}

	/* Scan the class hierarchy looking for member matches */
	numFound = 0;
	do
	{
		member = classInfo->firstMember;
		while(member != 0)
		{
			/* Check for a name match first */
			if(nameUtf8)
			{
				if((bindingAttrs & (ILInt32)BF_IgnoreCase) == 0)
				{
					if(strcmp(member->name, nameUtf8) != 0)
					{
						member = member->nextMember;
						continue;
					}
				}
				else
				{
					if(ILStrICmp(member->name, nameUtf8) != 0)
					{
						member = member->nextMember;
						continue;
					}
				}
			}
	
			/* Filter based on the member type */
			if(!MemberTypeMatch(member, memberTypes))
			{
				member = member->nextMember;
				continue;
			}
	
			/* Filter based on the binding attributes */
			if(!BindingAttrMatch(member, bindingAttrs, CC_Any))
			{
				member = member->nextMember;
				continue;
			}
	
			/* Check that we have reflection access for this member */
			if(!_ILClrCheckAccess(thread, (ILClass *)0, member))
			{
				member = member->nextMember;
				continue;
			}
	
			/* Add the member to the array */
			foundObject = ItemToClrObject(thread, &(member->programItem));
			if(!foundObject)
			{
				return 0;
			}
			if(numFound >= array->length)
			{
				newArray = (System_Array *)_ILEngineAlloc
						(thread, arrayClass,
					     sizeof(System_Array) +
						 	sizeof(void *) * (array->length + 4));
				if(!newArray)
				{
					return 0;
				}
				ILMemCpy(ArrayToBuffer(newArray), ArrayToBuffer(array),
						 sizeof(void *) * array->length);
				newArray->length = array->length + 4;
				array = newArray;
			}
			((void **)ArrayToBuffer(array))[numFound++] = foundObject;
	
			/* Advance to the next member */
			member = member->nextMember;
		}

		/* Scan the nested types also */
		if((memberTypes & (ILInt32)MT_NestedType) != 0)
		{
			nested = 0;
			while((nested = ILClassNextNested(classInfo, nested)) != 0)
			{
				child = ILNestedInfoGetChild(nested);
				if(child)
				{
					/* Filter the child based on its name */
					if(nameUtf8)
					{
						if((bindingAttrs & (ILInt32)BF_IgnoreCase) == 0)
						{
							if(strcmp(child->name, nameUtf8) != 0)
							{
								continue;
							}
						}
						else
						{
							if(ILStrICmp(child->name, nameUtf8) != 0)
							{
								continue;
							}
						}
					}

					/* Filter the child based on its access level */
					if(!BindingAttrClassMatch(child, bindingAttrs))
					{
						continue;
					}

					/* Check that we have reflection access for this child */
					if(!_ILClrCheckAccess(thread, child, (ILMember *)0))
					{
						continue;
					}
	
					/* Add the nested type to the array */
					foundObject = ItemToClrObject
						(thread, &(child->programItem));
					if(!foundObject)
					{
						return 0;
					}
					if(numFound >= array->length)
					{
						newArray = (System_Array *)_ILEngineAlloc
								(thread, arrayClass,
							     sizeof(System_Array) +
								 	sizeof(void *) * (array->length + 4));
						if(!newArray)
						{
							return 0;
						}
						ILMemCpy(ArrayToBuffer(newArray), ArrayToBuffer(array),
								 sizeof(void *) * array->length);
						newArray->length = array->length + 4;
						array = newArray;
					}
					((void **)ArrayToBuffer(array))[numFound++] = foundObject;
				}
			}
		}

		/* Move up the class hierarchy */
		classInfo = ILClass_Parent(classInfo);
	}
	while(classInfo != 0 &&
	      (bindingAttrs & (ILInt32)BF_DeclaredOnly) == 0);

	/* Truncate the array to its actual length and return it */
	array->length = numFound;
	return (ILObject *)array;
}

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
	IL_METHOD("GetMemberImpl",
					"(ToSystem.String;"
						"vSystem.Reflection.MemberTypes;"
						"vSystem.Reflection.BindingFlags;"
						"oSystem.Reflection.Binder;"
						"vSystem.Reflection.CallingConventions;"
						"[oSystem.Type;"
						"[vSystem.Reflection.ParameterModifiers;)"
					")oSystem.Reflection.MethodInfo;",
					ClrType_GetMemberImpl)
	IL_METHOD("GetMembersImpl",
					"(TvSystem.Reflection.MemberTypes;"
						"vSystem.Reflection.BindingFlags;"
						"oSystem.Type;oSystem.String;)oSystem.Object;",
					ClrType_GetMembersImpl)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif