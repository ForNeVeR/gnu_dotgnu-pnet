/*
 * strapi.c - External API for manipulating engine strings.
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

Note: we never access the internal representation of strings directly.
This is because we may be using a foreign C# library that doesn't use
the same definition for "String" as pnetlib does.  By avoiding the
specific internals, we can usually adapt to the foreign requirements.
This isn't foolproof, but should work in most cases.

*/

ILString *ILStringCreate(ILExecThread *thread, const char *str)
{
	if(str)
	{
		return ILStringCreateLen(thread, str, strlen(str));
	}
	else
	{
		return (ILString *)0;
	}
}

ILString *ILStringCreateLen(ILExecThread *thread, const char *str, int len)
{
	if(str && len >= 0)
	{
		/* Call the "String(sbyte *, int, int)" constructor */
		return (ILString *)ILExecThreadNew
					(thread, "System.String", "(T*bii)V",
					 (ILInt8 *)str, (ILVaInt)0, (ILVaInt)len);
	}
	else
	{
		return (ILString *)0;
	}
}

ILString *ILStringWCreate(ILExecThread *thread, const ILUInt16 *str)
{
	if(str)
	{
		int len = 0;
		while(str[len] != 0)
		{
			++len;
		}
		return ILStringWCreateLen(thread, str, len);
	}
	else
	{
		return (ILString *)0;
	}
}

ILString *ILStringWCreateLen(ILExecThread *thread,
							 const ILUInt16 *str, int len)
{
	if(str && len >= 0)
	{
		/* Call the "String(char *, int, int)" constructor */
		return (ILString *)ILExecThreadNew
					(thread, "System.String", "(T*cii)V",
					 (ILUInt16 *)str, (ILVaInt)0, (ILVaInt)len);
	}
	else
	{
		return (ILString *)0;
	}
}

int ILStringCompare(ILExecThread *thread, ILString *strA, ILString *strB)
{
	ILInt32 result = 0;
	ILExecThreadCallNamed(thread, "System.String", "Compare",
						  "(oSystem.String;oSystem.String;)i",
						  &result, strA, strB);
	return result;
}

int ILStringCompareIgnoreCase(ILExecThread *thread, ILString *strA,
							  ILString *strB)
{
	ILInt32 result = 0;
	ILExecThreadCallNamed(thread, "System.String", "Compare",
						  "(oSystem.String;oSystem.String;Z)i",
						  &result, strA, strB, (ILVaInt)0);
	return result;
}

int ILStringCompareOrdinal(ILExecThread *thread, ILString *strA,
						   ILString *strB)
{
	ILInt32 result = 0;
	ILExecThreadCallNamed(thread, "System.String", "CompareOrdinal",
						  "(oSystem.String;oSystem.String;)i",
						  &result, strA, strB);
	return result;
}

int ILStringEquals(ILExecThread *thread, ILString *strA, ILString *strB)
{
	return (ILStringCompare(thread, strA, strB) == 0);
}

ILString *ILStringConcat(ILExecThread *thread, ILString *strA, ILString *strB)
{
	ILString *result = 0;
	ILExecThreadCallNamed(thread, "System.String", "Concat",
						  "(oSystem.String;oSystem.String;)oSystem.String;",
						  &result, strA, strB);
	return result;
}

ILString *ILObjectToString(ILExecThread *thread, ILObject *object)
{
	ILString *result = 0;
	if(object)
	{
		/* Call the virtual "ToString" method on the object */
		ILExecThreadCallNamedVirtual(thread, "System.Object", "ToString",
									 "(T)oSystem.String;",
									 &result, object);
	}
	return result;
}

ILString *ILStringIntern(ILExecThread *thread, ILString *str)
{
	ILString *result = 0;
	ILExecThreadCallNamed(thread, "System.String", "Intern",
						  "(oSystem.String;)oSystem.String;",
						  &result, str);
	return result;
}

ILString *_ILStringInternFromImage(ILExecThread *thread, ILImage *image,
								   ILToken token)
{
	const char *str;
	unsigned long len;
	str = ILImageGetUserString(image, token & ~IL_META_TOKEN_MASK, &len);
	if(!str)
	{
		/* Shouldn't happen, but intern an empty string anyway */
		len = 0;
	}

	/* TODO: look up the intern'ed hash table and make library-portable */

#if defined(__i386) || defined(__i386__)
	/* We can take a short-cut on x86 platforms which can
	   access little-endian values on any boundary */
	return ILStringWCreateLen(thread, (const ILUInt16 *)str, len);
#else
	/* TODO */
	return 0;
#endif
}

ILInt32 _ILStringToBuffer(ILExecThread *thread, ILString *str, ILUInt16 **buf)
{
	ILClass *classInfo;
	ILField *field;
	ILType *type;
	System_Array *array;

	/* Bail out if "str" is NULL */
	if(!str)
	{
		*buf = 0;
		return 0;
	}

	/* Try to discover which C# library we are using by looking
	   for either a "char[]" or a "char" field within the string */
	classInfo = GetObjectClass(str);
	field = 0;
	while((field = (ILField *)ILClassNextMemberByKind
				(classInfo, (ILMember *)field, IL_META_MEMBERKIND_FIELD)) != 0)
	{
		type = ILField_Type(field);
		if(type == ILType_Char)
		{
			/* This looks like "pnetlib", which stores the characters
			   within the main object itself */
			*buf = StringToBuffer(str);
			return ((System_String *)str)->length;
		}
		else if(ILType_IsComplex(type) &&
				type->kind == IL_TYPE_COMPLEX_ARRAY &&
				type->un.array.elemType == ILType_Char)
		{
			/* This looks like "Mono" or "OCL", which stores the
			   characters in a separate array */
			array = *(System_Array **)(((unsigned char *)str) + field->offset);
			if(array)
			{
				*buf = (ILUInt16 *)(ArrayToBuffer(array));
				return ((System_Array *)array)->length;
			}
			else
			{
				*buf = 0;
				return 0;
			}
		}
	}

	/* If we get here, then we don't know what library we are using */
	*buf = 0;
	return 0;
}

#ifdef	__cplusplus
};
#endif
