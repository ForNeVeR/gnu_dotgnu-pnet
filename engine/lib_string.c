/*
 * string.c - Internalcall methods for "System.String".
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
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Allocate space for a new string object.
 */
static System_String *AllocString(ILExecThread *thread, ILInt32 length)
{
	System_String *str;
	ILClass *classInfo;
	ILInt32 roundLen = ((length + 7) & ~7);	/* Round to a multiple of 8 */
	classInfo = ILMethod_Owner(thread->method);
	str = (System_String *)_ILEngineAlloc(thread, classInfo,
						 				  sizeof(System_String) +
						 				  roundLen * sizeof(ILUInt16));
	if(str)
	{
		str->arrayLength = roundLen;
		str->length = length;
		return str;
	}
	else
	{
		return 0;
	}
}

/*
 * public String(char[] value, int startIndex, int length);
 */
static System_String *System_String_ctor_1(ILExecThread *thread,
								           System_Array *value,
								           ILInt32 startIndex,
								           ILInt32 length)
{
	System_String *_this;

	/* Validate the parameters */

	/* Allocate space for the string object */
	_this = AllocString(thread, length);
	if(!_this)
	{
		return 0;
	}

	/* Copy the contents of the character array into the string */
	if(length > 0)
	{
		ILMemCpy(StringToBuffer(_this), ArrayToBuffer(value),
				 sizeof(ILUInt16) * length);
	}

	/* The string has been initialized */
	return _this;
}

/*
 * public String(char[] value);
 */
static System_String *System_String_ctor_2(ILExecThread *thread,
								           System_Array *value)
{
	return System_String_ctor_1(thread, value, 0,
								(value ? value->length : 0));
}

/*
 * public String(char c, int count);
 */
static System_String *System_String_ctor_3(ILExecThread *thread,
								           ILUInt16 c, ILInt32 count)
{
	System_String *_this;
	ILUInt16 *buffer;

	/* Validate the parameters */

	/* Allocate space for the string object */
	_this = AllocString(thread, count);
	if(!_this)
	{
		return 0;
	}

	/* Copy the character into the string buffer */
	buffer = StringToBuffer(_this);
	while(count > 0)
	{
		*buffer++ = c;
		--count;
	}

	/* The string has been initialized */
	return _this;
}

/*
 * unsafe public String(char *value, int startIndex, int length);
 */
static System_String *System_String_ctor_4(ILExecThread *thread,
								           ILUInt16 *value,
								           ILInt32 startIndex,
								           ILInt32 length)
{
	System_String *_this;

	/* Validate the parameters */

	/* Allocate space for the string object */
	_this = AllocString(thread, length);
	if(!_this)
	{
		return 0;
	}

	/* Copy the contents of the character buffer into the string */
	if(length > 0)
	{
		ILMemCpy(StringToBuffer(_this), value + startIndex,
				 sizeof(ILUInt16) * length);
	}

	/* The string has been initialized */
	return _this;
}

/*
 * unsafe public String(char *value);
 */
static System_String *System_String_ctor_5(ILExecThread *thread,
								           ILUInt16 *value)
{
	System_String *_this;
	ILInt32 length;

	/* Validate the parameters */

	/* Determine the length of the input */
	length = 0;
	while(value[length] != 0)
	{
		++length;
	}

	/* Allocate space for the string object */
	_this = AllocString(thread, length);
	if(!_this)
	{
		return 0;
	}

	/* Copy the contents of the character buffer into the string */
	if(length > 0)
	{
		ILMemCpy(StringToBuffer(_this), value, sizeof(ILUInt16) * length);
	}

	/* The string has been initialized */
	return _this;
}

/*
 * unsafe public String(sbyte *value, int startIndex, int length,
 *                      Encoding enc);
 */
static System_String *System_String_ctor_6(ILExecThread *thread,
								           ILInt8 *value,
								           ILInt32 startIndex,
								           ILInt32 length,
										   void *encoding)
{
	System_String *_this;
	System_Array *array;
	ILInt32 posn;

	/* Validate the parameters */

	/* Find the default encoding object to use */
	if(!encoding)
	{
		if(ILExecThreadCallNamed(thread, "System.Text.Encoding",
								 "get_Default", "()oSystem.Text.Encoding;",
								 &encoding))
		{
			/* Return to the caller with the pending exception */
			return 0;
		}
		if(!encoding)
		{
			/* System.Text.Encoding.get_Default() returned "null",
			   so use a direct character-to-character copy */
			_this = AllocString(thread, length);
			if(!_this)
			{
				return 0;
			}
			for(posn = 0; posn < length; ++posn)
			{
				StringToBuffer(_this)[posn] =
				   (ILUInt16)(((ILUInt16)(value[startIndex + posn])) & 0xFFFF);
			}
			return _this;
		}
	}

	/* Construct an array that contains the bytes */
	array = (System_Array *)ILExecThreadNew(thread, "[B", "(Ti)V", length);
	if(!array)
	{
		return 0;
	}
	ILMemCpy(ArrayToBuffer(array), value + startIndex, length);

	/* Call "String System.Text.Encoding.GetChars(byte[])"
	   to construct the final string that we need */
	if(ILExecThreadCallNamedVirtual(thread, "System.Text.Encoding",
							        "GetChars", "(T[B)oSystem.String;",
							        &_this, encoding, array))
	{
		return 0;
	}
	else
	{
		return _this;
	}
}

/*
 * unsafe public String(sbyte *value, int startIndex, int length)
 */
static System_String *System_String_ctor_7(ILExecThread *thread,
								           ILInt8 *value,
								           ILInt32 startIndex,
								           ILInt32 length)
{
	/* Construct the string using the default encoding */
	return System_String_ctor_6(thread, value, startIndex, length, 0);
}

/*
 * unsafe public String(sbyte *value)
 */
static System_String *System_String_ctor_8(ILExecThread *thread,
								           ILInt8 *value)
{
	/* Determine the length of the input buffer */
	ILInt32 length = 0;
	if(value)
	{
		while(value[length] != 0)
		{
			++length;
		}
	}

	/* Construct the string using the default encoding */
	return System_String_ctor_6(thread, value, 0, length, 0);
}

/*
 * private static String FastAllocateString(int length);
 */
static System_String *System_String_FastAllocateString
							(ILExecThread *thread, ILInt32 length)
{
	return AllocString(thread, length);
}

/*
 * private static void FillString(String dest, int destPos, String src);
 */
static void System_String_FillString(ILExecThread *thread,
									 System_String *dest,
									 ILInt32 destPos,
									 System_String *src)
{
	ILMemCpy(StringToBuffer(dest) + destPos,
			 StringToBuffer(src), src->length * sizeof(ILUInt16));
}

/*
 * private static void FillSubstring(String dest, int destPos,
 *									 String src, int srcPos,
 *                                   int length);
 */
static void System_String_FillSubstring(ILExecThread *thread,
									 	System_String *dest,
									 	ILInt32 destPos,
									 	System_String *src,
									 	ILInt32 srcPos,
									 	ILInt32 length)
{
	ILMemCpy(StringToBuffer(dest) + destPos,
			 StringToBuffer(src) + srcPos, length * sizeof(ILUInt16));
}

/*
 * private void InternalCopyTo(int sourceIndex, char[] destination,
 *							   int destinationIndex, int count);
 */
static void System_String_InternalCopyTo(ILExecThread *thread,
									 	 System_String *_this,
									 	 ILInt32 sourceIndex,
									 	 System_Array *destination,
									 	 ILInt32 destinationIndex,
									 	 ILInt32 count)
{
	ILMemCpy(((ILUInt16 *)(ArrayToBuffer(destination))) + destinationIndex,
			 StringToBuffer(_this) + sourceIndex, count * sizeof(ILUInt16));
}

/*
 * public override int GetHashCode();
 */
static ILInt32 System_String_GetHashCode(ILExecThread *thread,
									     System_String *_this)
{
	ILInt32 hash = 0;
	ILUInt16 *buf = StringToBuffer(_this);
	ILInt32 len = _this->length;
	ILInt32 posn;
	for(posn = 0; posn < len; ++posn)
	{
		hash = (hash << 5) + hash + (ILInt32)(*buf++);
	}
	return hash;
}

/*
 * public int IndexOf(char value, int startIndex, int count);
 */
static int System_String_IndexOf(ILExecThread *thread,
							 	 System_String *_this,
								 ILUInt16 value,
								 ILInt32 startIndex,
								 ILInt32 count)
{
	/* TODO */
	return -1;
}

/*
 * public int IndexOfAny(char[] anyOf, int startIndex, int count);
 */
static int System_String_IndexOfAny(ILExecThread *thread,
							 	    System_String *_this,
								    System_Array *anyOf,
								    ILInt32 startIndex,
								    ILInt32 count)
{
	/* TODO */
	return -1;
}

/*
 * public int LastIndexOf(char value, int startIndex, int count);
 */
static int System_String_LastIndexOf(ILExecThread *thread,
							 	 	 System_String *_this,
								 	 ILUInt16 value,
								 	 ILInt32 startIndex,
								 	 ILInt32 count)
{
	/* TODO */
	return -1;
}

/*
 * public int LastIndexOfAny(char[] anyOf, int startIndex, int count);
 */
static int System_String_LastIndexOfAny(ILExecThread *thread,
							 	    	 System_String *_this,
								    	 System_Array *anyOf,
								    	 ILInt32 startIndex,
								    	 ILInt32 count)
{
	/* TODO */
	return -1;
}

/*
 * public static String Intern(String str);
 */
static System_String *System_String_Intern(ILExecThread *thread,
							 	    	   System_String *str)
{
	/* TODO */
	return str;
}

/*
 * public static String IsInterned(String str);
 */
static System_String *System_String_IsInterned(ILExecThread *thread,
							 	    	       System_String *str)
{
	/* TODO */
	return str;
}

/*
 * private static void FillChar(String str, int start, int count, char ch);
 */
static void System_String_FillChar(ILExecThread *thread,
							 	   System_String *str,
								   ILInt32 start,
								   ILInt32 count,
								   ILUInt16 ch)
{
	while(count > 0)
	{
		StringToBuffer(str)[start] = ch;
		--count;
		++start;
	}
}

/*
 * public String Replace(char oldChar, char newChar)
 */
static System_String *System_String_Replace_1(ILExecThread *thread,
							 	    		  System_String *_this,
								    		  ILUInt16 oldChar,
								    		  ILUInt16 newChar)
{
	/* TODO */
	return _this;
}

/*
 * public String Replace(String oldValue, String newValue)
 */
static System_String *System_String_Replace_2(ILExecThread *thread,
							 	    		  System_String *_this,
								    		  System_String *oldValue,
								    		  System_String *newValue)
{
	/* TODO */
	return _this;
}

/*
 * public String[] Split(char[] separator, int count);
 */
static System_Array *System_String_Split(ILExecThread *thread,
							 	    	 System_String *_this,
								    	 System_Array *separator,
								    	 ILInt32 count)
{
	/* TODO */
	return 0;
}

#define	TRIM_HEAD		0
#define	TRIM_TAIL		1
#define	TRIM_BOTH		2

/*
 * private String TrimHelper(char[] trimChars, int trimType);
 */
static System_String *System_String_TrimHelper(ILExecThread *thread,
							 	    	       System_String *_this,
								    	 	   System_Array *trimChars,
								    	 	   ILInt32 trimType)
{
	/* TODO */
	return _this;
}

/*
 * private char InternalGetChar(int posn);
 */
static ILUInt16 System_String_InternalGetChar(ILExecThread *thread,
							 	    	      System_String *_this,
								    	 	  ILInt32 posn)
{
	if(posn >= 0 && posn < _this->length)
	{
		return StringToBuffer(_this)[posn];
	}
	else
	{
		/* Throw an "IndexOutOfRangeException" */
		return 0;
	}
}

/*
 * Method table for the "System.String" class.
 */
IL_METHOD_BEGIN(_ILSystemStringMethods)
	IL_CONSTRUCTOR(".ctor",	"(T[cii)V",		0, System_String_ctor_1)
	IL_CONSTRUCTOR(".ctor",	"(T[c)V",		0, System_String_ctor_2)
	IL_CONSTRUCTOR(".ctor",	"(Tci)V",		0, System_String_ctor_3)
	IL_CONSTRUCTOR(".ctor",	"(T*cii)V",		0, System_String_ctor_4)
	IL_CONSTRUCTOR(".ctor",	"(T*c)V",		0, System_String_ctor_5)
	IL_CONSTRUCTOR(".ctor",	"(T*biioSystem.Text.Encoding;)V",
					0, System_String_ctor_6)
	IL_CONSTRUCTOR(".ctor",	"(T*bii)V",		0, System_String_ctor_7)
	IL_CONSTRUCTOR(".ctor",	"(T*b)V",		0, System_String_ctor_8)
	IL_METHOD("FastAllocateString", "(i)oSystem.String;",
					System_String_FastAllocateString)
	IL_METHOD("FillString",	"(oSystem.String;ioSystem.String;)V",
					System_String_FillString)
	IL_METHOD("FillSubstring", "(oSystem.String;ioSystem.String;ii)V",
				   	System_String_FillSubstring)
	IL_METHOD("InternalCopyTo", "(Ti[cii)V",
					System_String_InternalCopyTo)
	IL_METHOD("GetHashCode", "(T)i",	System_String_GetHashCode)
	IL_METHOD("IndexOf",	 "(Tcii)i",	System_String_IndexOf)
	IL_METHOD("IndexOfAny",	 "(T[cii)i", System_String_IndexOfAny)
	IL_METHOD("Intern",		 "(T)oSystem.String;", System_String_Intern)
	IL_METHOD("IsInterned",	 "(T)oSystem.String;", System_String_IsInterned)
	IL_METHOD("LastIndexOf", "(Tcii)i", System_String_LastIndexOf)
	IL_METHOD("LastIndexOfAny", "(T[cii)i", System_String_LastIndexOfAny)
	IL_METHOD("FillChar",	 "(oSystem.String;iic)V", System_String_FillChar)
	IL_METHOD("Replace",	 "(Tcc)oSystem.String;", System_String_Replace_1)
	IL_METHOD("Replace",	 "(ToSystem.String;oSystem.String;)oSystem.String;",
					System_String_Replace_2)
	IL_METHOD("Split",		 "(T[ci)[oSystem.String;", System_String_Split)
	IL_METHOD("TrimHelper",	 "(T[ci)oSystem.String;", System_String_TrimHelper)
	IL_METHOD("InternalGetChar", "(Ti)c", System_String_InternalGetChar)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
