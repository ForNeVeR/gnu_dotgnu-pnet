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

#include "engine.h"

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
					 (ILInt8 *)str, 0, len);
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
					 (ILUInt16 *)str, 0, len);
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

#ifdef	__cplusplus
};
#endif
