/*
 * lib_string.c - Internalcall methods for "System.String".
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
	ILInt32 roundLen = ((length + 7) & ~7);	/* Round to a multiple of 8 */
	str = (System_String *)_ILEngineAllocAtomic(thread,
												thread->process->stringClass,
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
	if(!value)
	{
		ILExecThreadThrowArgNull(thread, "value");
		return 0;
	}
	if(startIndex < 0)
	{
		ILExecThreadThrowArgRange(thread, "startIndex",
								  "ArgRange_Array");
		return 0;
	}
	if(length < 0 || (value->length - startIndex) < length)
	{
		ILExecThreadThrowArgRange(thread, "length",
								  "ArgRange_Array");
		return 0;
	}

	/* Allocate space for the string object */
	_this = AllocString(thread, length);
	if(!_this)
	{
		return 0;
	}

	/* Copy the contents of the character array into the string */
	if(length > 0)
	{
		ILMemCpy(StringToBuffer(_this), ArrayToBuffer(value) + startIndex,
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
	System_String *_this;
	ILInt32 length;

	/* Determine the length of the final string.  A null argument
	   indicates that the empty string should be constructed */
	if(value)
	{
		length = value->length;
	}
	else
	{
		length = 0;
	}

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
 * public String(char c, int count);
 */
static System_String *System_String_ctor_3(ILExecThread *thread,
								           ILUInt16 c, ILInt32 count)
{
	System_String *_this;
	ILUInt16 *buffer;

	/* Validate the parameters */
	if(count < 0)
	{
		ILExecThreadThrowArgRange(thread, "count",
								  "ArgRange_NonNegative");
		return 0;
	}

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
	if(startIndex < 0)
	{
		ILExecThreadThrowArgRange(thread, "startIndex",
								  "ArgRange_Array");
		return 0;
	}
	if(length < 0 || (!value && length != 0))
	{
		ILExecThreadThrowArgRange(thread, "length",
								  "ArgRange_Array");
		return 0;
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

	/* Determine the length of the input.  A null pointer is valid
	   and indicates that an empty string should be created */
	length = 0;
	if(value != 0)
	{
		while(value[length] != 0)
		{
			++length;
		}
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
	if(startIndex < 0)
	{
		ILExecThreadThrowArgRange(thread, "startIndex",
								  "ArgRange_Array");
		return 0;
	}
	if(length < 0 || (!value && length != 0))
	{
		ILExecThreadThrowArgRange(thread, "length",
								  "ArgRange_Array");
		return 0;
	}

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
	array = (System_Array *)ILExecThreadNew(thread, "[B", "(Ti)V",
											(ILVaInt)length);
	if(!array)
	{
		return 0;
	}
	ILMemCpy(ArrayToBuffer(array), value + startIndex, length);

	/* Call "String System.Text.Encoding.GetChars(byte[])"
	   to construct the final string that we need */
	_this = 0;
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
	ILUInt16 *buf;

	/* Validate the parameters */
	if(startIndex < 0)
	{
		ILExecThreadThrowArgRange(thread, "startIndex",
								  "ArgRange_StringIndex");
		return -1;
	}
	if(count < 0 || (_this->length - startIndex) < count)
	{
		ILExecThreadThrowArgRange(thread, "count",
								  "ArgRange_StringRange");
		return -1;
	}

	/* Search for the value */
	buf = StringToBuffer(_this) + startIndex;
	while(count > 0)
	{
		if(*buf++ == value)
		{
			return startIndex;
		}
		++startIndex;
		--count;
	}
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
	ILUInt16 *buf;
	ILUInt16 *anyBuf;
	ILInt32 anyLength;
	ILInt32 anyPosn;

	/* Validate the parameters */
	if(!anyOf)
	{
		ILExecThreadThrowArgNull(thread, "anyOf");
		return -1;
	}
	if(startIndex < 0)
	{
		ILExecThreadThrowArgRange(thread, "startIndex",
								  "ArgRange_StringIndex");
		return -1;
	}
	if(count < 0 || (_this->length - startIndex) < count)
	{
		ILExecThreadThrowArgRange(thread, "count",
								  "ArgRange_StringRange");
		return -1;
	}

	/* Get the start and extent of the "anyOf" array */
	anyBuf = ArrayToBuffer(anyOf);
	anyLength = anyOf->length;
	if(!anyLength)
	{
		/* Bail out because there is nothing to find */
		return -1;
	}

	/* Search for the value */
	buf = StringToBuffer(_this) + startIndex;
	while(count > 0)
	{
		for(anyPosn = 0; anyPosn < anyLength; ++anyPosn)
		{
			if(*buf == anyBuf[anyPosn])
			{
				return startIndex;
			}
		}
		++buf;
		++startIndex;
		--count;
	}
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
	ILUInt16 *buf;

	/* Validate the parameters */
	if(startIndex < 0 || startIndex >= _this->length)
	{
		ILExecThreadThrowArgRange(thread, "startIndex",
								  "ArgRange_StringIndex");
		return -1;
	}
	if(count < 0 || (startIndex - count) < -1)
	{
		ILExecThreadThrowArgRange(thread, "count",
								  "ArgRange_StringRange");
		return -1;
	}

	/* Search for the value */
	buf = StringToBuffer(_this) + startIndex;
	while(count > 0)
	{
		if(*buf-- == value)
		{
			return startIndex;
		}
		--startIndex;
		--count;
	}
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
	ILUInt16 *buf;
	ILUInt16 *anyBuf;
	ILInt32 anyLength;
	ILInt32 anyPosn;

	/* Validate the parameters */
	if(!anyOf)
	{
		ILExecThreadThrowArgNull(thread, "anyOf");
		return -1;
	}
	if(startIndex < 0 || startIndex >= _this->length)
	{
		ILExecThreadThrowArgRange(thread, "startIndex",
								  "ArgRange_StringIndex");
		return -1;
	}
	if(count < 0 || (startIndex - count) < -1)
	{
		ILExecThreadThrowArgRange(thread, "count",
								  "ArgRange_StringRange");
		return -1;
	}

	/* Get the start and extent of the "anyOf" array */
	anyBuf = ArrayToBuffer(anyOf);
	anyLength = anyOf->length;
	if(!anyLength)
	{
		/* Bail out because there is nothing to find */
		return -1;
	}

	/* Search for the value */
	buf = StringToBuffer(_this) + startIndex;
	while(count > 0)
	{
		for(anyPosn = 0; anyPosn < anyLength; ++anyPosn)
		{
			if(*buf == anyBuf[anyPosn])
			{
				return startIndex;
			}
		}
		--buf;
		--startIndex;
		--count;
	}
	return -1;
}

/*
 * private bool EqualRange(int srcIndex, int count,
 *						   String dest, int destIndex);
 */
static ILBool System_String_EqualRange(ILExecThread *thread,
									   System_String *_this,
									   ILInt32 srcIndex,
									   ILInt32 count,
									   System_String *dest,
									   ILInt32 destIndex)
{
	ILUInt16 *buf1 = StringToBuffer(_this) + srcIndex;
	ILUInt16 *buf2 = StringToBuffer(dest) + destIndex;
	return (!ILMemCmp(buf1, buf2, count * sizeof(ILUInt16)));
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
	System_String *str;
	ILUInt16 *buf1;
	ILUInt16 *buf2;
	ILInt32 len;

	/* If nothing will happen, then return the current string as-is */
	len = _this->length;
	if(oldChar == newChar || len == 0)
	{
		return _this;
	}

	/* Allocate a new string */
	str = AllocString(thread, len);
	if(!str)
	{
		return 0;
	}

	/* Scan the two strings, copying and replacing as we go */
	buf1 = StringToBuffer(_this);
	buf2 = StringToBuffer(str);
	while(len > 0)
	{
		if(*buf1 != oldChar)
		{
			*buf2++ = *buf1++;
		}
		else
		{
			*buf2++ = newChar;
		}
		--len;
	}
	return str;
}

/*
 * public String Replace(String oldValue, String newValue)
 */
static System_String *System_String_Replace_2(ILExecThread *thread,
							 	    		  System_String *_this,
								    		  System_String *oldValue,
								    		  System_String *newValue)
{
	ILInt32 oldLen;
	ILInt32 newLen;
	ILInt32 finalLen;
	ILInt32 posn;
	System_String *str;
	ILUInt16 *buf;

	/* If "oldValue" is null or an empty string, then the
	   string will not be changed */
	if(!oldValue || !(oldValue->length))
	{
		return _this;
	}

	/* Get the length of the old and new values */
	oldLen = oldValue->length;
	if(newValue)
	{
		newLen = newValue->length;
	}
	else
	{
		newLen = 0;
	}

	/* Determine the length of the final string */
	finalLen = 0;
	posn = 0;
	while((posn + oldLen) <= _this->length)
	{
		if(System_String_EqualRange(thread, _this, posn, oldLen,
									oldValue, 0))
		{
			finalLen += newLen;
			posn += oldLen;
		}
		else
		{
			++finalLen;
			++posn;
		}
		if(((ILUInt32)finalLen) > (ILUInt32)((IL_MAX_INT32 / 4) - 16))
		{
			/* The resulting string will be way too big */
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
	}
	finalLen += _this->length - posn;

	/* Allocate a new string */
	str = AllocString(thread, finalLen);
	if(!str)
	{
		return 0;
	}

	/* Scan the input string again and perform the replacement */
	buf = StringToBuffer(str);
	posn = 0;
	while((posn + oldLen) <= _this->length)
	{
		if(System_String_EqualRange(thread, _this, posn, oldLen,
									oldValue, 0))
		{
			if(newLen > 0)
			{
				ILMemCpy(buf + finalLen, StringToBuffer(newValue),
						 newLen * sizeof(ILUInt16));
			}
			finalLen += newLen;
			posn += oldLen;
		}
		else
		{
			buf[finalLen++] = StringToBuffer(_this)[posn++];
		}
	}

	/* Return the final replaced string to the caller */
	return str;
}

#define	TRIM_HEAD		0
#define	TRIM_TAIL		1
#define	TRIM_BOTH		2

/*
 * Match a character against an array of characters.
 */
static IL_INLINE int IsCharMatch(System_Array *trimChars, ILUInt16 ch)
{
	if(trimChars)
	{
		ILInt32 len = trimChars->length;
		ILUInt16 *buf = StringToBuffer(trimChars);
		while(len > 0)
		{
			if(*buf++ == ch)
			{
				return 1;
			}
			--len;
		}
	}
	return 0;
}

/*
 * private String TrimHelper(char[] trimChars, int trimType);
 */
static System_String *System_String_TrimHelper(ILExecThread *thread,
							 	    	       System_String *_this,
								    	 	   System_Array *trimChars,
								    	 	   ILInt32 trimType)
{
	ILInt32 start, end;
	ILUInt16 *buf = StringToBuffer(_this);
	System_String *str;
	start = 0;
	end = _this->length;
	if(trimType == TRIM_HEAD || trimType == TRIM_BOTH)
	{
		while(start < end && IsCharMatch(trimChars, buf[start]))
		{
			++start;
		}
	}
	if(trimType == TRIM_TAIL || trimType == TRIM_BOTH)
	{
		while(start < end && IsCharMatch(trimChars, buf[end - 1]))
		{
			--end;
		}
	}
	str = AllocString(thread, end - start);
	if(str)
	{
		if(start < end)
		{
			ILMemCpy(StringToBuffer(str), buf + start,
					 (end - start) * sizeof(ILUInt16));
		}
		return str;
	}
	else
	{
		return 0;
	}
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
		ILExecThreadThrowSystem(thread, "System.IndexOutOfRangeException",
								"ArgRange_StringIndex");
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
	IL_METHOD("EqualRange",	 "(TiioSystem.String;i)Z",
					System_String_EqualRange)
	IL_METHOD("Intern",		 "(T)oSystem.String;", System_String_Intern)
	IL_METHOD("IsInterned",	 "(T)oSystem.String;", System_String_IsInterned)
	IL_METHOD("LastIndexOf", "(Tcii)i", System_String_LastIndexOf)
	IL_METHOD("LastIndexOfAny", "(T[cii)i", System_String_LastIndexOfAny)
	IL_METHOD("FillChar",	 "(oSystem.String;iic)V", System_String_FillChar)
	IL_METHOD("Replace",	 "(Tcc)oSystem.String;", System_String_Replace_1)
	IL_METHOD("Replace",	 "(ToSystem.String;oSystem.String;)oSystem.String;",
					System_String_Replace_2)
	IL_METHOD("TrimHelper",	 "(T[ci)oSystem.String;", System_String_TrimHelper)
	IL_METHOD("InternalGetChar", "(Ti)c", System_String_InternalGetChar)
IL_METHOD_END

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
		/* Call the "String(sbyte *, int, int, Encoding)" constructor */
		return (ILString *)System_String_ctor_6
					(thread, (ILInt8 *)str, 0, (ILInt32)len, (void *)0);
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
		return (ILString *)System_String_ctor_4
				(thread, (ILUInt16 *)str, 0, (ILInt32)len);
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
	ILUInt16 *bufA;
	ILUInt16 *bufB;
	ILInt32 lenA;
	ILInt32 lenB;

	/* Handle the easy cases first */
	if(!strA)
	{
		if(strB)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
	else if(!strB)
	{
		return 1;
	}

	/* Scan the buffers looking for differences */
	bufA = StringToBuffer(strA);
	lenA = ((System_String *)strA)->length;
	bufB = StringToBuffer(strB);
	lenB = ((System_String *)strB)->length;
	while(lenA > 0 && lenB > 0)
	{
		if(*bufA < *bufB)
		{
			return -1;
		}
		else if(*bufA > *bufB)
		{
			return 1;
		}
		++bufA;
		++bufB;
		--lenA;
		--lenB;
	}

	/* Determine the ordering based on the tails */
	if(lenA > 0)
	{
		return 1;
	}
	else if(lenB > 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int ILStringEquals(ILExecThread *thread, ILString *strA, ILString *strB)
{
	ILUInt16 *bufA;
	ILUInt16 *bufB;
	ILInt32 lenA;
	ILInt32 lenB;

	/* Handle the easy cases first */
	if(!strA)
	{
		return (strB == 0);
	}
	else if(!strB)
	{
		return 0;
	}

	/* Scan the buffers looking for differences */
	lenA = ((System_String *)strA)->length;
	lenB = ((System_String *)strB)->length;
	if(lenA != lenB)
	{
		return 0;
	}
	else if(!lenA)
	{
		return 1;
	}
	else
	{
		bufA = StringToBuffer(strA);
		bufB = StringToBuffer(strB);
		return (ILMemCmp(bufA, bufB, lenA * sizeof(ILUInt16)) == 0);
	}
}

ILString *ILStringConcat(ILExecThread *thread, ILString *strA, ILString *strB)
{
	if(!strA || ((System_String *)strA)->length == 0)
	{
		if(!strB || ((System_String *)strB)->length == 0)
		{
			if(strB)
			{
				return strB;
			}
			else if(strA)
			{
				return strA;
			}
			else
			{
				return (ILString *)AllocString(thread, 0);
			}
		}
		else
		{
			return strB;
		}
	}
	else if(!strB || ((System_String *)strB)->length == 0)
	{
		return strA;
	}
	else
	{
		ILInt32 lenA = ((System_String *)strA)->length;
		ILInt32 lenB = ((System_String *)strB)->length;
		System_String *result;
		if(((ILUInt32)(lenA + lenB)) >= ((ILUInt32)(IL_MAX_INT32 / 2)))
		{
			/* The resulting string is too big */
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		result = AllocString(thread, lenA + lenB);
		if(result)
		{
			ILMemCpy(StringToBuffer(result), StringToBuffer(strA), lenA);
			ILMemCpy(StringToBuffer(result) + lenA, StringToBuffer(strB), lenB);
		}
		return (ILString *)result;
	}
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
	return (ILString *)System_String_Intern(thread, (System_String *)str);
}

ILString *_ILStringInternFromImage(ILExecThread *thread, ILImage *image,
								   ILToken token)
{
	const char *str;
	unsigned long len;
	System_String *newStr;

	/* Get the string from the image's "#US" blob */
	str = ILImageGetUserString(image, token & ~IL_META_TOKEN_MASK, &len);
	if(!str)
	{
		/* Shouldn't happen, but intern an empty string anyway */
		len = 0;
	}

	/* TODO: look up the intern'ed string hash table */

	/* Allocate space for the string */
	newStr = AllocString(thread, (ILInt32)len);
	if(!newStr)
	{
		return 0;
	}

	/* Copy the image data into the string */
#if defined(__i386) || defined(__i386__)
	/* We can take a short-cut on x86 platforms which already
	   has the string in the correct format */
	if(len > 0)
	{
		ILMemCpy(StringToBuffer(newStr), str, len * sizeof(ILUInt16));
	}
#else
	{
		ILUInt16 *dest = StringToBuffer(newStr);
		while(len > 0)
		{
			*dest++ = IL_READ_UINT16(str);
			str += 2;
			--len;
		}
	}
#endif

	/* Return the final string to the caller */
	return (ILString *)newStr;
}

ILInt32 _ILStringToBuffer(ILExecThread *thread, ILString *str, ILUInt16 **buf)
{
	if(str)
	{
		*buf = StringToBuffer(str);
		return ((System_String *)str)->length;
	}
	else
	{
		*buf = 0;
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif
