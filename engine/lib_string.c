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
		str->capacity = roundLen;
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
 * Compare two Unicode strings.
 *
 * TODO: replace this with something better and move to "support".
 */
static int ILStrCmpUnicode(const ILUInt16 *str1, ILInt32 length1,
						   const ILUInt16 *str2, ILInt32 length2)
{
	while(length1 > 0 && length2 > 0)
	{
		if(*str1 < *str2)
		{
			return -1;
		}
		else if(*str1 > *str2)
		{
			return 1;
		}
		++str1;
		++str2;
		--length1;
		--length2;
	}
	if(length1 > 0)
	{
		return 1;
	}
	else if(length2 > 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/*
 * Compare two Unicode strings, while ignoring case.
 *
 * TODO: replace this with something better and move to "support".
 */
static int ILStrICmpUnicode(const ILUInt16 *str1, ILInt32 length1,
							const ILUInt16 *str2, ILInt32 length2)
{
	ILUInt16 ch1;
	ILUInt16 ch2;
	while(length1 > 0 && length2 > 0)
	{
		ch1 = *str1++;
		ch2 = *str2++;
		if(ch1 >= 'a' && ch1 <= 'z')
		{
			ch1 = (ILUInt16)(ch1 - 'a' + 'A');
		}
		if(ch2 >= 'a' && ch2 <= 'z')
		{
			ch2 = (ILUInt16)(ch2 - 'a' + 'A');
		}
		if(ch1 < ch2)
		{
			return -1;
		}
		else if(ch1 > ch2)
		{
			return 1;
		}
		--length1;
		--length2;
	}
	if(length1 > 0)
	{
		return 1;
	}
	else if(length2 > 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/*
 * public static int Compare(String strA, String strB);
 */
static ILInt32 System_String_Compare(ILExecThread *thread,
									 System_String *strA,
									 System_String *strB)
{
	/* Handle the easy cases first */
	if(!strA)
	{
		if(!strB)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else if(!strB)
	{
		return 1;
	}
	else if(strA == strB)
	{
		return 0;
	}

	/* Compare the two strings */
	return ILStrCmpUnicode(StringToBuffer(strA), strA->length,
						   StringToBuffer(strB), strB->length);
}

/*
 * public static int InternalCompare(String strA, int indexA, int lengthA,
 *									 String strB, int indexB, int lengthB,
 *							 		 bool ignoreCase, CultureInfo culture);
 */
static ILInt32 System_String_InternalCompare(ILExecThread *thread,
									   		 System_String *strA,
											 ILInt32 indexA, ILInt32 lengthA,
									   		 System_String *strB,
											 ILInt32 indexB, ILInt32 lengthB,
									   		 ILBool ignoreCase,
									   		 ILObject *culture)
{
	/* Handle the easy cases first */
	if(!strA)
	{
		if(!strB)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else if(!strB)
	{
		return 1;
	}

	/* TODO: handle culture information */

	/* Compare the two strings */
	if(ignoreCase)
	{
		return ILStrICmpUnicode(StringToBuffer(strA) + indexA, lengthA,
							    StringToBuffer(strB) + indexB, lengthB);
	}
	else
	{
		return ILStrCmpUnicode(StringToBuffer(strA) + indexA, lengthA,
							   StringToBuffer(strB) + indexB, lengthB);
	}
}

/*
 * public static int InternalOrdinal(String strA, int indexA, int lengthA,
 *									 String strB, int indexB, int lengthB);
 */
static ILInt32 System_String_InternalOrdinal(ILExecThread *thread,
									   		 System_String *strA,
											 ILInt32 indexA, ILInt32 lengthA,
									   		 System_String *strB,
											 ILInt32 indexB, ILInt32 lengthB)
{
	ILUInt16 *bufA;
	ILUInt16 *bufB;

	/* Handle the easy cases first */
	if(!strA)
	{
		if(!strB)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else if(!strB)
	{
		return 1;
	}

	/* Compare the two strings */
	bufA = StringToBuffer(strA) + indexA;
	bufB = StringToBuffer(strB) + indexB;
	while(lengthA > 0 && lengthB > 0)
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
		--lengthA;
		--lengthB;
	}

	/* Determine the ordering based on the tail sections */
	if(lengthA > 0)
	{
		return 1;
	}
	else if(lengthB > 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/*
 * public static bool Equals(String strA, String strB);
 */
static ILBool System_String_Equals(ILExecThread *thread,
								   System_String *strA,
								   System_String *strB)
{
	if(!strA)
	{
		if(!strB)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if(!strB)
	{
		return 0;
	}
	else if(strA == strB)
	{
		return 1;
	}
	else if(strA->length != strB->length)
	{
		return 0;
	}
	else if(!(strA->length))
	{
		return 1;
	}
	else
	{
		return !ILMemCmp(StringToBuffer(strA),
						 StringToBuffer(strB), strA->length);
	}
}

/*
 * internal static String FastAllocateString(int length);
 */
static System_String *System_String_FastAllocateString
							(ILExecThread *thread, ILInt32 length)
{
	return AllocString(thread, length);
}

/*
 * internal static String FastAllocateBuilder(String value, int length);
 */
static System_String *System_String_FastAllocateBuilder
							(ILExecThread *thread,
							 System_String *value,
							 ILInt32 length)
{
	System_String *str;
	ILInt32 roundLen;
	if(length == -1)
	{
		roundLen = value->length;
	}
	else
	{
		roundLen = length;
	}
	roundLen = ((length + 7) & ~7);	/* Round to a multiple of 8 */
	str = (System_String *)_ILEngineAllocAtomic(thread,
												thread->process->stringClass,
						 				  		sizeof(System_String) +
						 				  		roundLen * sizeof(ILUInt16));
	if(str)
	{
		str->capacity = roundLen;
		if(value != 0)
		{
			if(value->length <= roundLen)
			{
				ILMemCpy(StringToBuffer(str), StringToBuffer(value),
						 value->length);
				str->length = value->length;
			}
			else
			{
				ILMemCpy(StringToBuffer(str), StringToBuffer(value), roundLen);
				str->length = roundLen;
			}
		}
		else
		{
			str->length = 0;
		}
		return str;
	}
	else
	{
		return 0;
	}
}

/*
 * internal static void FillString(String dest, int destPos, String src);
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
 * internal static void FillSubstring(String dest, int destPos,
 *									  String src, int srcPos,
 *                                    int length);
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
 * internal void InsertSpace(String dest, int srcPos, int destPos)
 */
static void System_String_InsertSpace(ILExecThread *thread,
									  System_String *dest,
									  ILInt32 srcPos, ILInt32 destPos)
{
	ILMemMove(StringToBuffer(dest) + destPos,
			  StringToBuffer(dest) + srcPos, dest->length - srcPos);
	dest->length += (destPos - srcPos);
}

/*
 * internal void RemoveSpace(String dest, int index, int length)
 */
static void System_String_RemoveSpace(ILExecThread *thread,
									  System_String *dest,
									  ILInt32 index, ILInt32 length)
{
	ILMemMove(StringToBuffer(dest) + index,
			  StringToBuffer(dest) + index + length,
			  dest->length - (index + length));
	dest->length -= length;
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
 * Size of the intern'ed string hash table.
 */
#define	IL_INTERN_HASH_SIZE		512

/*
 * Structure of a intern'ed string hash table entry.
 */
typedef struct _tagILStrHash ILStrHash;
struct _tagILStrHash
{
	System_String	*value;
	ILStrHash		*next;

};

/*
 * Look up the intern'ed string hash table for a value.
 */
static System_String *InternString(ILExecThread *thread,
								   System_String *str, int add)
{
	ILStrHash *table;
	ILStrHash *entry;
	ILUInt32 hash;

	/* Allocate a new hash table, if required */
	table = (ILStrHash *)(thread->process->internHash);
	if(!table)
	{
		table = (ILStrHash *)ILGCAllocPersistent(sizeof(ILStrHash) *
												 IL_INTERN_HASH_SIZE);
		if(!table)
		{
			if(add)
			{
				ILExecThreadThrowOutOfMemory(thread);
			}
			return 0;
		}
		thread->process->internHash = (void *)table;
	}

	/* Compute the hash of the string */
	hash = ((ILUInt32)(System_String_GetHashCode(thread, str)))
				% IL_INTERN_HASH_SIZE;

	/* Look for an existing string with the same value */
	entry = &(table[hash]);
	while(entry != 0 && entry->value != 0)
	{
		if(entry->value->length == str->length &&
		   (entry->value->length == 0 ||
		    !ILMemCmp(StringToBuffer(entry->value), StringToBuffer(str),
					  entry->value->length)))
		{
			if(add || entry->value == str)
			{
				return entry->value;
			}
			else
			{
				return 0;
			}
		}
		entry = entry->next;
	}
	if(!add)
	{
		return 0;
	}

	/* Add a new entry to the intern'ed string hash table */
	entry = &(table[hash]);
	if(entry->value == 0)
	{
		entry->value = str;
	}
	else
	{
		entry = (ILStrHash *)ILGCAlloc(sizeof(ILStrHash));
		if(!entry)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		entry->value = str;
		entry->next = table[hash].next;
		table[hash].next = entry;
	}
	return str;
}

/*
 * public static String Intern(String str);
 */
static System_String *System_String_Intern(ILExecThread *thread,
							 	    	   System_String *str)
{
	if(str)
	{
		return InternString(thread, str, 1);
	}
	else
	{
		return str;
	}
}

/*
 * public static String IsInterned(String str);
 */
static System_String *System_String_IsInterned(ILExecThread *thread,
							 	    	       System_String *str)
{
	if(str)
	{
		return InternString(thread, str, 0);
	}
	else
	{
		return str;
	}
}

/*
 * internal static void FillChar(String str, int start, int count, char ch);
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
 * internal static void FillWithChars(String str, int start,
 *									  char[] chars, int index,
 									  int count);
 */
static void System_String_FillWithChars(ILExecThread *thread,
							 	   	 	System_String *str,
								   		ILInt32 start,
							 	   	 	System_Array *chars,
								   		ILInt32 index,
								   		ILInt32 count)
{
	ILUInt16 *src = ArrayToBuffer(chars) + index;
	ILUInt16 *dest = StringToBuffer(str) + start;
	while(count > 0)
	{
		*dest++ = *src++;
		--count;
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
 * internal char InternalGetChar(int posn);
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
 * internal void InternalSetChar(int posn, char value);
 */
static void System_String_InternalSetChar(ILExecThread *thread,
							     	      System_String *_this,
							    	 	  ILInt32 posn, ILUInt16 value)
{
	if(posn >= 0 && posn < _this->length)
	{
		StringToBuffer(_this)[posn] = value;
	}
	else
	{
		ILExecThreadThrowSystem(thread, "System.IndexOutOfRangeException",
								"ArgRange_StringIndex");
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
	IL_METHOD("Compare", "(oSystem.String;oSystem.String;)i",
					System_String_Compare)
	IL_METHOD("InternalCompare",
					"(oSystem.String;iioSystem.String;iiZ"
						"oSystem.Globalization.CultureInfo;)i",
					System_String_InternalCompare)
	IL_METHOD("InternalOrdinal",
					"(oSystem.String;iioSystem.String;ii)i",
					System_String_InternalOrdinal)
	IL_METHOD("Equals", "(oSystem.String;oSystem.String;)Z",
					System_String_Equals)
	IL_METHOD("FastAllocateString", "(i)oSystem.String;",
					System_String_FastAllocateString)
	IL_METHOD("FastAllocateBuilder", "(oSystem.String;i)oSystem.String;",
					System_String_FastAllocateBuilder)
	IL_METHOD("FillString",	"(oSystem.String;ioSystem.String;)V",
					System_String_FillString)
	IL_METHOD("FillSubstring", "(oSystem.String;ioSystem.String;ii)V",
				   	System_String_FillSubstring)
	IL_METHOD("InsertSpace", "(oSystem.String;ii)V",
					System_String_InsertSpace)
	IL_METHOD("RemoveSpace", "(oSystem.String;ii)V",
					System_String_RemoveSpace)
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
	IL_METHOD("FillWithChars", "(oSystem.String;i[cii)V",
					System_String_FillWithChars)
	IL_METHOD("Replace",	 "(Tcc)oSystem.String;", System_String_Replace_1)
	IL_METHOD("Replace",	 "(ToSystem.String;oSystem.String;)oSystem.String;",
					System_String_Replace_2)
	IL_METHOD("TrimHelper",	 "(T[ci)oSystem.String;", System_String_TrimHelper)
	IL_METHOD("InternalGetChar", "(Ti)c", System_String_InternalGetChar)
	IL_METHOD("InternalSetChar", "(Tic)V", System_String_InternalSetChar)
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
	return (int)(System_String_Compare(thread,
									   (System_String *)strA,
									   (System_String *)strB));
}

int ILStringCompareIgnoreCase(ILExecThread *thread, ILString *strA,
							  ILString *strB)
{
	return (int)(System_String_InternalCompare
						(thread,
						 (System_String *)strA, 0,
						 ((strA != 0) ? ((System_String *)strA)->length : 0),
						 (System_String *)strB, 0,
						 ((strB != 0) ? ((System_String *)strB)->length : 0),
						 (ILBool)1, (void *)0));
}

int ILStringCompareOrdinal(ILExecThread *thread, ILString *strA,
						   ILString *strB)
{
	return (int)(System_String_InternalOrdinal
						(thread,
						 (System_String *)strA, 0,
						 ((strA != 0) ? ((System_String *)strA)->length : 0),
						 (System_String *)strB, 0,
						 ((strB != 0) ? ((System_String *)strB)->length : 0)));
}

int ILStringEquals(ILExecThread *thread, ILString *strA, ILString *strB)
{
	return (int)(System_String_Equals(thread,
									  (System_String *)strA,
									  (System_String *)strB));
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
	if(str)
	{
		return (ILString *)InternString(thread, (System_String *)str, 1);
	}
	else
	{
		return 0;
	}
}

/*
 * Determine if the contents of a string buffer is the
 * same as a literal string value from an image.
 */
static int SameAsImage(ILUInt16 *buf, const char *str, ILInt32 len)
{
#if defined(__i386) || defined(__i386__)
	/* We can take a short-cut on x86 platforms which already
	   have the string in the correct format */
	if(len > 0)
	{
		return !ILMemCmp(buf, str, len * sizeof(ILUInt16));
	}
	else
	{
		return 1;
	}
#else
	while(len > 0)
	{
		if(*buf++ != IL_READ_UINT16(str))
		{
			return 0;
		}
		str += 2;
		--len;
	}
	return 1;
#endif
}

ILString *_ILStringInternFromImage(ILExecThread *thread, ILImage *image,
								   ILToken token)
{
	const char *str;
	unsigned long len;
	unsigned long posn;
	System_String *newStr;
	ILStrHash *table;
	ILStrHash *entry;
	ILInt32 hashTemp;
	ILUInt32 hash;

	/* Get the string from the image's "#US" blob */
	str = ILImageGetUserString(image, token & ~IL_META_TOKEN_MASK, &len);
	if(!str)
	{
		/* Shouldn't happen, but intern an empty string anyway */
		len = 0;
	}

	/* Allocate a new hash table, if required */
	table = (ILStrHash *)(thread->process->internHash);
	if(!table)
	{
		table = (ILStrHash *)ILGCAllocPersistent(sizeof(ILStrHash) *
												 IL_INTERN_HASH_SIZE);
		if(!table)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		thread->process->internHash = (void *)table;
	}

	/* Compute the hash of the string */
	hashTemp = 0;
	for(posn = 0; posn < len; ++posn)
	{
		hashTemp = (hashTemp << 5) + hashTemp +
				   (ILInt32)(IL_READ_UINT16(str + posn * 2));
	}
	hash = ((ILUInt32)hashTemp) % IL_INTERN_HASH_SIZE;

	/* Look for an existing string with the same value */
	entry = &(table[hash]);
	while(entry != 0 && entry->value != 0)
	{
		if(entry->value->length == (ILInt32)len &&
		   (entry->value->length == 0 ||
		    SameAsImage(StringToBuffer(entry->value), str,
					    entry->value->length)))
		{
			return (ILString *)(entry->value);
		}
		entry = entry->next;
	}

	/* Allocate space for the string */
	newStr = AllocString(thread, (ILInt32)len);
	if(!newStr)
	{
		return 0;
	}

	/* Copy the image data into the string */
#if defined(__i386) || defined(__i386__)
	/* We can take a short-cut on x86 platforms which already
	   have the string in the correct format */
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

	/* Add a new entry to the intern'ed string hash table */
	entry = &(table[hash]);
	if(entry->value == 0)
	{
		entry->value = newStr;
	}
	else
	{
		entry = (ILStrHash *)ILGCAlloc(sizeof(ILStrHash));
		if(!entry)
		{
			ILExecThreadThrowOutOfMemory(thread);
			return 0;
		}
		entry->value = newStr;
		entry->next = table[hash].next;
		table[hash].next = entry;
	}

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

char *ILStringToUTF8(ILExecThread *thread, ILString *str)
{
	ILUInt16 *buffer;
	ILInt32 length;
	ILInt32 utf8Len;
	ILUInt16 ch;
	ILUInt32 fullChar;
	char *newStr;
	char *temp;

	/* Bail out immediately if the string is NULL */
	if(!str)
	{
		return 0;
	}

	/* Determine the length of the string in UTF-8 characters */
	buffer = StringToBuffer(str);
	length = ((System_String *)str)->length;
	utf8Len = 0;
	while(length > 0)
	{
		ch = *buffer++;
		--length;
		if(!ch)
		{
			/* Embedded NUL's are encoded as two non-NUL characters */
			utf8Len += 2;
		}
		else if(ch < (ILUInt16)0x0080)
		{
			/* Ordinary ASCII character */
			++utf8Len;
		}
		else if(ch < (((ILUInt16)1) << 11))
		{
			/* Encode as two characters */
			utf8Len += 2;
		}
		else if(ch >= (ILUInt16)0xD800 && ch < (ILUInt16)0xDC00 &&
		        length > 0 &&
				buffer[0] >= (ILUInt16)0xDC00 && buffer[0] < (ILUInt16)0xE000)
		{
			/* This is a surrogate for a 20-bit Unicode character */
			fullChar = (((ILUInt32)(ch - 0xD800)) << 10);
			ch = *buffer++;
			--length;
			fullChar |= ((ILUInt32)(ch - 0xDC00));
			if(!fullChar)
			{
				utf8Len += 2;
			}
			else if(fullChar < (ILUInt32)0x0080)
			{
				++utf8Len;
			}
			else if(fullChar < (((ILUInt32)1) << 11))
			{
				utf8Len += 2;
			}
			else if(fullChar < (((ILUInt32)1) << 16))
			{
				utf8Len += 3;
			}
			else
			{
				utf8Len += 4;
			}
		}
		else
		{
			/* Encode as three characters */
			utf8Len += 3;
		}
	}

	/* Allocate space within the garbage-collected heap */
	newStr = (char *)ILGCAllocAtomic(utf8Len + 1);
	if(!newStr)
	{
		ILExecThreadThrowOutOfMemory(thread);
		return 0;
	}

	/* Copy the characters into the allocated buffer */
	temp = newStr;
	buffer = StringToBuffer(str);
	length = ((System_String *)str)->length;
	while(length > 0)
	{
		ch = *buffer++;
		--length;
		if(!ch)
		{
			/* Embedded NUL's are encoded as two non-NUL characters */
			*temp++ = (char)0xC0;
			*temp++ = (char)0x80;
		}
		else if(ch < (ILUInt16)0x0080)
		{
			/* Ordinary ASCII character */
			*temp++ = (char)ch;
		}
		else if(ch < (((ILUInt16)1) << 11))
		{
			/* Encode as two characters */
			*temp++ = (char)(0xC0 | (ch >> 6));
			*temp++ = (char)(0x80 | (ch & 0x3F));
		}
		else if(ch >= (ILUInt16)0xD800 && ch < (ILUInt16)0xDC00 &&
		        length > 0 &&
				buffer[0] >= (ILUInt16)0xDC00 && buffer[0] < (ILUInt16)0xE000)
		{
			/* This is a surrogate for a 20-bit Unicode character */
			fullChar = (((ILUInt32)(ch - 0xD800)) << 10);
			ch = *buffer++;
			--length;
			fullChar |= ((ILUInt32)(ch - 0xDC00));
			if(!fullChar)
			{
				*temp++ = (char)0xC0;
				*temp++ = (char)0x80;
			}
			else if(fullChar < (ILUInt32)0x0080)
			{
				*temp++ = (char)fullChar;
			}
			else if(fullChar < (((ILUInt32)1) << 11))
			{
				*temp++ = (char)(0xC0 | (fullChar >> 6));
				*temp++ = (char)(0x80 | (fullChar & 0x3F));
			}
			else if(fullChar < (((ILUInt32)1) << 16))
			{
				*temp++ = (char)(0xE0 | (fullChar >> 12));
				*temp++ = (char)(0x80 | ((fullChar >> 6) & 0x3F));
				*temp++ = (char)(0x80 | (fullChar & 0x3F));
			}
			else
			{
				*temp++ = (char)(0xF0 | (fullChar >> 18));
				*temp++ = (char)(0x80 | ((fullChar >> 12) & 0x3F));
				*temp++ = (char)(0x80 | ((fullChar >> 6) & 0x3F));
				*temp++ = (char)(0x80 | (fullChar & 0x3F));
			}
		}
		else
		{
			/* Encode as three characters */
			*temp++ = (char)(0xE0 | (ch >> 12));
			*temp++ = (char)(0x80 | ((ch >> 6) & 0x3F));
			*temp++ = (char)(0x80 | (ch & 0x3F));
		}
	}
	*temp = '\0';

	/* Done */
	return newStr;
}

char *ILStringToAnsi(ILExecThread *thread, ILString *str)
{
	/* TODO */
	return ILStringToUTF8(thread, str);
}

#ifdef	__cplusplus
};
#endif
