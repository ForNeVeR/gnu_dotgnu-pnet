/*
 * marshal.c - Determine how to marshal PInvoke method parameters.
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

#include "program.h"
#include "il_serialize.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Determine the character set to use for strings.
 */
static ILUInt32 StringCharSet(ILPInvoke *pinvoke)
{
	/* Check the PInvoke record for character set information */
	switch(pinvoke->member.attributes & IL_META_PINVOKE_CHAR_SET_MASK)
	{
		case IL_META_PINVOKE_CHAR_SET_ANSI:
		case IL_META_PINVOKE_CHAR_SET_AUTO:
		{
			return IL_META_MARSHAL_CHAR_SET_ANSI;
		}
		/* Not reached */

		case IL_META_PINVOKE_CHAR_SET_UNICODE:
		{
			return IL_META_MARSHAL_CHAR_SET_UTF8;
		}
		/* Not reached */
	}

	/* Check the class for character set information */
	if((pinvoke->member.owner->attributes &
				IL_META_TYPEDEF_STRING_FORMAT_MASK) ==
			IL_META_TYPEDEF_UNICODE_CLASS)
	{
		return IL_META_MARSHAL_CHAR_SET_UTF8;
	}
	else
	{
		return IL_META_MARSHAL_CHAR_SET_ANSI;
	}
}

/*
 * Determine if a program item has a "NativeTypeSizes" attribute.
 * Returns -1 if not present, 0 if set to false, and 1 if set to true.
 */
static int NativeTypeSizesAttr(ILProgramItem *item)
{
	ILAttribute *attr;
	ILMethod *method;
	const void *blob;
	unsigned long blobLen;
	ILSerializeReader *reader;
	int value;

	attr = 0;
	while((attr = ILProgramItemNextAttribute(item, attr)) != 0)
	{
		method = ILProgramItemToMethod(ILAttributeTypeAsItem(attr));
		if(method != 0 && !strcmp(method->member.owner->name,
								  "NativeTypeSizesAttribute"))
		{
			blob = ILAttributeGetValue(attr, &blobLen);
			if(blob &&
			   (reader = ILSerializeReaderInit(method, blob, blobLen)) != 0)
			{
				if(ILSerializeReaderGetParamType(reader) ==
						IL_META_SERIALTYPE_BOOLEAN)
				{
					/* Get the boolean value from the attribute */
					value = (ILSerializeReaderGetInt32
								(reader, IL_META_SERIALTYPE_BOOLEAN) != 0);
				}
				else
				{
					/* No parameters means the the attribute is enabled */
					value = 1;
				}
				ILSerializeReaderDestroy(reader);
				return value;
			}
			return 1;
		}
	}

	/* The attribute is not present */
	return -1;
}

/*
 * Determine if a PInvoke method has a "NativeTypeSizes" declaration.
 */
static int NativeTypeSizes(ILPInvoke *pinvoke)
{
	int value;

	/* Look for the attribute on the method */
	value = NativeTypeSizesAttr(&(pinvoke->method->member.programItem));
	if(value != -1)
	{
		return value;
	}

	/* Look for the attribute on the class */
	value = NativeTypeSizesAttr(&(pinvoke->method->member.owner->programItem));
	if(value != -1)
	{
		return value;
	}

	/* The attribute is not present */
	return 0;
}

ILUInt32 ILPInvokeGetMarshalType(ILPInvoke *pinvoke, unsigned long param)
{
	ILType *type = ILTypeGetParam(pinvoke->method->member.signature, param);
	ILParameter *parameter;
	ILFieldMarshal *marshal;
	const unsigned char *nativeType;
	unsigned long nativeTypeLen;
	int nativeTypeCode;

	/* Find the parameter information block */
	parameter = pinvoke->method->parameters;
	while(parameter != 0 && parameter->paramNum != param)
	{
		parameter = parameter->next;
	}

	/* See if we have native type information for the parameter */
	nativeType = 0;
	nativeTypeLen = 0;
	nativeTypeCode = IL_META_NATIVETYPE_END;
	if(parameter != 0)
	{
		marshal = ILFieldMarshalGetFromOwner(&(parameter->programItem));
		if(marshal != 0)
		{
			nativeType = (const unsigned char *)
				ILFieldMarshalGetType(marshal, &nativeTypeLen);
			if(nativeTypeLen > 0)
			{
				nativeTypeCode = (int)(nativeType[0]);
			}
		}
	}

	/* If the native type is "interface", then always marshal directly.
	   This is normally used to force strings to be passed as objects */
	if(nativeTypeCode == IL_META_NATIVETYPE_INTF)
	{
		return IL_META_MARSHAL_DIRECT;
	}

	/* Is this a string type? */
	if(ILTypeIsStringClass(type))
	{
		/* Value string type */
		return (IL_META_MARSHAL_STRING | StringCharSet(pinvoke));
	}
	else if(type != ILType_Invalid && ILType_IsComplex(type) &&
			ILType_Kind(type) == IL_TYPE_COMPLEX_BYREF &&
			ILTypeIsStringClass(ILType_Ref(type)))
	{
		/* Reference string type */
		if(parameter != 0 &&
		   (parameter->attributes & IL_META_PARAMDEF_OUT) != 0)
		{
			/* This is an "out" string parameter */
			return (IL_META_MARSHAL_STRING_OUT | StringCharSet(pinvoke));
		}
		else
		{
			/* This is a "ref" string parameter */
			return (IL_META_MARSHAL_STRING_REF | StringCharSet(pinvoke));
		}
	}

	/* Check for numeric conversions */
	if(ILType_IsPrimitive(type))
	{
	#ifdef IL_NATIVE_INT64
		if(type == ILType_Int32)
		{
			if(nativeTypeCode == IL_META_NATIVETYPE_INT)
			{
				return IL_META_MARSHAL_I4_TO_I8;
			}
			else if(NativeTypeSizes(pinvoke))
			{
				if(sizeof(int) == 8)
				{
					return IL_META_MARSHAL_I4_TO_I8;
				}
			}
		}
		else if(type == ILType_UInt32)
		{
			if(nativeTypeCode == IL_META_NATIVETYPE_UINT)
			{
				return IL_META_MARSHAL_U4_TO_U8;
			}
			else if(NativeTypeSizes(pinvoke))
			{
				if(sizeof(unsigned int) == 8)
				{
					return IL_META_MARSHAL_U4_TO_U8;
				}
			}
		}
		else
	#endif
		if(type == ILType_Int64)
		{
			if(nativeTypeCode == IL_META_NATIVETYPE_INT)
			{
			#ifdef IL_NATIVE_INT32
				return IL_META_MARSHAL_I8_TO_I4;
			#endif
			}
			else if(NativeTypeSizes(pinvoke))
			{
				if(sizeof(long) == 4)
				{
					return IL_META_MARSHAL_I8_TO_I4;
				}
			}
		}
		else if(type == ILType_UInt64)
		{
			if(nativeTypeCode == IL_META_NATIVETYPE_UINT)
			{
			#ifdef IL_NATIVE_INT32
				return IL_META_MARSHAL_I8_TO_I4;
			#endif
			}
			else if(NativeTypeSizes(pinvoke))
			{
				if(sizeof(unsigned long) == 4)
				{
					return IL_META_MARSHAL_I8_TO_I4;
				}
			}
		}
	}

	/* Marshal the parameter directly */
	return IL_META_MARSHAL_DIRECT;
}

#ifdef	__cplusplus
};
#endif
