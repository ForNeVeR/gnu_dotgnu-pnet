/*
 * lib_attrs.c - Builtin library attributes with special meanings.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
 * Match an attribute parameter name and type.
 */
#define	IsParam(name,ptype)		\
			(paramNameLen == strlen((name)) && \
			 !strncmp(paramName, (name), paramNameLen) && \
			 type == (ptype))

/*
 * Process a DLL importing attribute, to define PInvoke information.
 */
static int DllImportAttribute(ILProgramItem *item, ILSerializeReader *reader)
{
	ILMethod *method;
	const char *name;
	int nameLen;
	ILUInt32 attrs;
	const char *entryPoint;
	int entryPointLen;
	int numExtra;
	const char *paramName;
	int paramNameLen;
	int type;
	ILMember *member;
	char *dllName;
	char *aliasName;
	ILModule *module;
	int result;

	/* We must use this on a method */
	method = ILProgramItemToMethod(item);
	if(!method)
	{
		return 0;
	}

	/* Get the name of the DLL, which is the only attribute parameter */
	if(ILSerializeReaderGetParamType(reader) != IL_META_SERIALTYPE_STRING)
	{
		return 0;
	}
	nameLen = ILSerializeReaderGetString(reader, &name);
	if(nameLen < 0)
	{
		return 0;
	}
	if(ILSerializeReaderGetParamType(reader) != 0)
	{
		return 0;
	}

	/* Collect up extra information from the attribute blob */
	attrs = IL_META_PINVOKE_CALL_CONV_CDECL;
	entryPoint = 0;
	entryPointLen = -1;
	numExtra = ILSerializeReaderGetNumExtra(reader);
	if(numExtra < 0)
	{
		return 0;
	}
	while(numExtra > 0)
	{
		type = ILSerializeReaderGetExtra(reader, &member, &paramName,
										 &paramNameLen);
		if(type == -1)
		{
			return 0;
		}
		if(IsParam("CallingConvention", IL_META_SERIALTYPE_I4))
		{
			attrs &= ~IL_META_PINVOKE_CALL_CONV_MASK;
			attrs |= (ILUInt32)(ILSerializeReaderGetInt32(reader, type) << 8);
		}
		else if(IsParam("CharSet", IL_META_SERIALTYPE_I4))
		{
			attrs &= ~IL_META_PINVOKE_CHAR_SET_MASK;
			attrs |=
				(ILUInt32)((ILSerializeReaderGetInt32(reader, type) - 1) << 1);
		}
		else if(IsParam("EntryPoint", IL_META_SERIALTYPE_STRING))
		{
			entryPointLen = ILSerializeReaderGetString(reader, &entryPoint);
			if(entryPointLen < 0)
			{
				return 0;
			}
		}
		else if(IsParam("ExactSpelling", IL_META_SERIALTYPE_BOOLEAN))
		{
			if(ILSerializeReaderGetInt32(reader, type))
			{
				attrs |= IL_META_PINVOKE_NO_MANGLE;
			}
			else
			{
				attrs &= ~IL_META_PINVOKE_NO_MANGLE;
			}
		}
		else if(IsParam("PreserveSig", IL_META_SERIALTYPE_BOOLEAN))
		{
			if(ILSerializeReaderGetInt32(reader, type))
			{
				attrs |= IL_META_PINVOKE_OLE;
			}
			else
			{
				attrs &= ~IL_META_PINVOKE_OLE;
			}
		}
		else if(IsParam("SetLastError", IL_META_SERIALTYPE_BOOLEAN))
		{
			if(ILSerializeReaderGetInt32(reader, type))
			{
				attrs |= IL_META_PINVOKE_SUPPORTS_LAST_ERROR;
			}
			else
			{
				attrs &= ~IL_META_PINVOKE_SUPPORTS_LAST_ERROR;
			}
		}
		else
		{
			return 0;
		}
		--numExtra;
	}

	/* Create the PInvoke record for the method */
	dllName = ILDupNString(name, nameLen);
	if(!dllName)
	{
		return -1;
	}
	if(entryPoint)
	{
		aliasName = ILDupNString(entryPoint, entryPointLen);
		if(!aliasName)
		{
			ILFree(dllName);
			return -1;
		}
	}
	else
	{
		aliasName = 0;
	}
	module = ILModuleRefCreateUnique(ILProgramItem_Image(item), dllName);
	if(!module)
	{
		ILFree(dllName);
		if(aliasName)
		{
			ILFree(aliasName);
		}
		return -1;
	}
	result = (ILPInvokeCreate(method, 0, attrs, module, aliasName) != 0);
	if(result)
	{
		/* Mark the method with the "pinvokeimpl" flag */
		ILMemberSetAttrs((ILMember *)method,
						 IL_META_METHODDEF_PINVOKE_IMPL,
						 IL_META_METHODDEF_PINVOKE_IMPL);
	}
	ILFree(dllName);
	if(aliasName)
	{
		ILFree(aliasName);
	}
	return (result ? 1 : -1);
}

/*
 * Process a field offset attribute.
 */
static int FieldOffsetAttribute(ILProgramItem *item, ILSerializeReader *reader)
{
	ILField *field;
	ILFieldLayout *layout;
	int type;
	ILUInt32 offset;

	/* We must use this on a field */
	field = ILProgramItemToField(item);
	if(!field)
	{
		return 0;
	}

	/* Get the first parameter, which should be int32 */
	type = ILSerializeReaderGetParamType(reader);
	if(type != IL_META_SERIALTYPE_I4)
	{
		return 0;
	}
	offset = (ILUInt32)(ILSerializeReaderGetInt32(reader, type));
	if(ILSerializeReaderGetParamType(reader) != 0 ||
	   ILSerializeReaderGetNumExtra(reader) != 0)
	{
		return 0;
	}

	/* Set the field offset */
	layout = ILFieldLayoutGetFromOwner(field);
	if(layout)
	{
		layout->offset = offset;
	}
	else if((layout = ILFieldLayoutCreate(ILProgramItem_Image(field), 0,
										  field, offset)) == 0)
	{
		return -1;
	}
	return 1;
}

/*
 * Process an "in" attribute on a parameter.
 */
static int InAttribute(ILProgramItem *item, ILSerializeReader *reader)
{
	ILParameter *param;

	/* We must use this on a parameter */
	param = ILProgramItemToParameter(item);
	if(!param)
	{
		return 0;
	}

	/* There should be no parameters or extra information */
	if(ILSerializeReaderGetParamType(reader) != 0 ||
	   ILSerializeReaderGetNumExtra(reader) != 0)
	{
		return 0;
	}

	/* Set the "in" flag on the parameter */
	ILParameterSetAttrs(param, IL_META_PARAMDEF_IN, IL_META_PARAMDEF_IN);
	return 1;
}

/*
 * Process an "out" attribute on a parameter.
 */
static int OutAttribute(ILProgramItem *item, ILSerializeReader *reader)
{
	ILParameter *param;

	/* We must use this on a parameter */
	param = ILProgramItemToParameter(item);
	if(!param)
	{
		return 0;
	}

	/* There should be no parameters or extra information */
	if(ILSerializeReaderGetParamType(reader) != 0 ||
	   ILSerializeReaderGetNumExtra(reader) != 0)
	{
		return 0;
	}

	/* Set the "out" flag on the parameter */
	ILParameterSetAttrs(param, IL_META_PARAMDEF_OUT, IL_META_PARAMDEF_OUT);
	return 1;
}

/*
 * Process a structure layout attribute.
 */
static int StructLayoutAttribute(ILProgramItem *item, ILSerializeReader *reader)
{
	ILClass *classInfo;
	int type;
	ILUInt32 attrs;
	ILInt32 sizeValue = 0;
	ILInt32 packValue = 0;
	ILClassLayout *layout;
	int numExtra;
	const char *paramName;
	int paramNameLen;
	ILMember *member;

	/* We must use this on a class */
	classInfo = ILProgramItemToClass(item);
	if(!classInfo)
	{
		return 0;
	}

	/* Get the layout kind, which must be either int32 or int16 */
	type = ILSerializeReaderGetParamType(reader);
	if(type != IL_META_SERIALTYPE_I4 && type != IL_META_SERIALTYPE_I2)
	{
		return 0;
	}
	type = (int)(ILSerializeReaderGetInt32(reader, type));
	if(ILSerializeReaderGetParamType(reader) != 0)
	{
		return 0;
	}

	/* Get the current state of the class's attributes */
	attrs = ILClass_Attrs(classInfo);

	/* Convert the kind value into a layout attribute value */
	switch(type)
	{
		case 0:
		{
			attrs = (attrs & ~IL_META_TYPEDEF_LAYOUT_MASK) |
					IL_META_TYPEDEF_LAYOUT_SEQUENTIAL;
		}
		break;

		case 2:
		{
			attrs = (attrs & ~IL_META_TYPEDEF_LAYOUT_MASK) |
					IL_META_TYPEDEF_EXPLICIT_LAYOUT;
		}
		break;

		case 3:
		{
			attrs = (attrs & ~IL_META_TYPEDEF_LAYOUT_MASK) |
					IL_META_TYPEDEF_AUTO_LAYOUT;
		}
		break;

		default:	break;
	}

	/* Collect up extra information from the attribute blob */
	numExtra = ILSerializeReaderGetNumExtra(reader);
	if(numExtra < 0)
	{
		return 0;
	}
	while(numExtra > 0)
	{
		type = ILSerializeReaderGetExtra(reader, &member, &paramName,
										 &paramNameLen);
		if(type == -1)
		{
			return 0;
		}
		if(IsParam("CharSet", IL_META_SERIALTYPE_I4))
		{
			attrs &= ~IL_META_TYPEDEF_STRING_FORMAT_MASK;
			type = (int)(ILSerializeReaderGetInt32(reader, type));
			if(type == 1 || type == 4)
			{
				attrs |= IL_META_TYPEDEF_AUTO_CLASS;
			}
			else if(type == 3)
			{
				attrs |= IL_META_TYPEDEF_UNICODE_CLASS;
			}
		}
		else if(IsParam("Size", IL_META_SERIALTYPE_I4))
		{
			sizeValue = ILSerializeReaderGetInt32(reader, type);
		}
		else if(IsParam("Pack", IL_META_SERIALTYPE_I4))
		{
			packValue = ILSerializeReaderGetInt32(reader, type);
		}
		else
		{
			return 0;
		}
		--numExtra;
	}

	/* Change the attributes for the class */
	ILClassSetAttrs(classInfo, ~((ILUInt32)0), attrs);

	/* Add size and packing information if necessary */
	layout = ILClassLayoutGetFromOwner(classInfo);
	if(layout)
	{
		/* Modify an existing layout information block */
		ILClassLayoutSetClassSize(layout, sizeValue);
		ILClassLayoutSetPackingSize(layout, packValue);
	}
	else
	{
		/* Create a new layout block if necessary */
		if(sizeValue != 0 || packValue != 0)
		{
			if(!ILClassLayoutCreate(ILProgramItem_Image(classInfo), 0,
									classInfo, packValue, sizeValue))
			{
				return -1;
			}
		}
	}

	/* Done */
	return 1;
}

/*
 * Process a field or parameter marshalling attribute.
 */
static int MarshalAsAttribute(ILProgramItem *item, ILSerializeReader *reader)
{
	int type;
	ILUInt32 unmanagedType;
	ILUInt32 arraySubType = IL_META_NATIVETYPE_END;
	ILUInt32 safeArraySubType = IL_META_VARIANTTYPE_EMPTY;
	int numExtra;
	const char *paramName;
	int paramNameLen;
	ILMember *member;
	const char *marshalCookie = 0;
	int marshalCookieLen = 0;
	const char *marshalType = 0;
	int marshalTypeLen = 0;
	ILInt32 sizeValue = 0;
	ILInt32 sizeParamIndex = 0;
	unsigned char buf[64];
	unsigned char *blob;
	int blobLen;
	ILFieldMarshal *marshal;

	/* We must use this on a field or parameter */
	if(!ILProgramItemToField(item) && !ILProgramItemToParameter(item))
	{
		return 0;
	}

	/* Get the unmanaged type, which must be either int32 or int16 */
	type = ILSerializeReaderGetParamType(reader);
	if(type != IL_META_SERIALTYPE_I4 && type != IL_META_SERIALTYPE_I2)
	{
		return 0;
	}
	unmanagedType = (ILUInt32)(ILSerializeReaderGetInt32(reader, type));
	if(ILSerializeReaderGetParamType(reader) != 0)
	{
		return 0;
	}

	/* Collect up extra information from the attribute blob */
	numExtra = ILSerializeReaderGetNumExtra(reader);
	if(numExtra < 0)
	{
		return 0;
	}
	while(numExtra > 0)
	{
		type = ILSerializeReaderGetExtra(reader, &member, &paramName,
										 &paramNameLen);
		if(type == -1)
		{
			return 0;
		}
		if(IsParam("ArraySubType", IL_META_SERIALTYPE_I4))
		{
			arraySubType = (ILUInt32)(ILSerializeReaderGetInt32(reader, type));
		}
		else if(IsParam("SafeArraySubType", IL_META_SERIALTYPE_I4))
		{
			safeArraySubType =
				(ILUInt32)(ILSerializeReaderGetInt32(reader, type));
		}
		else if(IsParam("MarshalCookie", IL_META_SERIALTYPE_STRING))
		{
			marshalCookieLen = ILSerializeReaderGetString
					(reader, &marshalCookie);
			if(marshalCookieLen < 0)
			{
				return 0;
			}
		}
		else if(IsParam("MarshalType", IL_META_SERIALTYPE_STRING))
		{
			marshalTypeLen = ILSerializeReaderGetString(reader, &marshalType);
			if(marshalTypeLen < 0)
			{
				return 0;
			}
		}
		else if(IsParam("MarshalTypeRef", IL_META_SERIALTYPE_TYPE))
		{
			/* Unused - ignore it */
		}
		else if(IsParam("SizeConst", IL_META_SERIALTYPE_I4))
		{
			sizeValue = ILSerializeReaderGetInt32(reader, type);
		}
		else if(IsParam("SizeParamIndex", IL_META_SERIALTYPE_I2))
		{
			sizeParamIndex = ILSerializeReaderGetInt32(reader, type);
		}
		else
		{
			return 0;
		}
		--numExtra;
	}

	/* Build the marshalling blob that corresponds to the supplied data */
	switch(unmanagedType)
	{
		case IL_META_NATIVETYPE_SAFEARRAY:
		{
			/* Safe variant array */
			buf[0] = IL_META_NATIVETYPE_SAFEARRAY;
			blobLen = 1 + ILMetaCompressData
				(buf + 1, (unsigned long)safeArraySubType);
			blob = buf;
		}
		break;

		case IL_META_NATIVETYPE_FIXEDARRAY:
		{
			/* Fixed length array */
			buf[0] = IL_META_NATIVETYPE_FIXEDARRAY;
			blobLen = 1 + ILMetaCompressData
				(buf + 1, (unsigned long)sizeValue);
			if(arraySubType == IL_META_NATIVETYPE_END)
			{
				buf[blobLen++] = IL_META_NATIVETYPE_MAX;
			}
			else
			{
				blobLen += ILMetaCompressData
					(buf + blobLen, (unsigned long)arraySubType);
			}
			blob = buf;
		}
		break;

		case IL_META_NATIVETYPE_ARRAY:
		{
			/* An array that has a size value in a separate parameter */
			buf[0] = IL_META_NATIVETYPE_ARRAY;
			blobLen = 1 + ILMetaCompressData
				(buf + 1, (unsigned long)arraySubType);
			blobLen += ILMetaCompressData
				(buf + blobLen, (unsigned long)sizeParamIndex);
			buf[blobLen++] = 1;		/* Multiplier */
			buf[blobLen++] = 0;		/* Number of elements */
			blob = buf;
		}
		break;

		case IL_META_NATIVETYPE_CUSTOMMARSHALER:
		{
			/* Custom marshalling directive */
			blob = (char *)ILMalloc(IL_META_COMPRESS_MAX_SIZE * 2 + 3 +
								    marshalTypeLen + marshalCookieLen);
			if(!blob)
			{
				return 0;
			}
			blob[0] = IL_META_NATIVETYPE_CUSTOMMARSHALER;
			blob[1] = 0;	/* Length of GUID string (unused) */
			blob[2] = 0;	/* Length of native type name string (unused) */
			blobLen = 3;
			blobLen += ILMetaCompressData
				(blob + blobLen, (unsigned long)marshalTypeLen);
			if(marshalTypeLen > 0)
			{
				ILMemCpy(blob + blobLen, marshalType, marshalTypeLen);
				blobLen += marshalTypeLen;
			}
			blobLen += ILMetaCompressData
				(blob + blobLen, (unsigned long)marshalCookieLen);
			if(marshalCookieLen > 0)
			{
				ILMemCpy(blob + blobLen, marshalCookie, marshalCookieLen);
				blobLen += marshalCookieLen;
			}
		}
		break;

		default:
		{
			/* This native type is represented as a simple value */
			blobLen = ILMetaCompressData(buf, (unsigned long)unmanagedType);
			blob = buf;
			blobLen = 1;
		}
		break;
	}

	/* Add the marshalling information to the program item */
	marshal = ILFieldMarshalGetFromOwner(item);
	if(!marshal)
	{
		marshal = ILFieldMarshalCreate(ILProgramItem_Image(item), 0, item);
		if(!marshal)
		{
			if(blob != buf)
			{
				ILFree(blob);
			}
			return -1;
		}
	}
	if(!ILFieldMarshalSetType(marshal, blob, blobLen))
	{
		if(blob != buf)
		{
			ILFree(blob);
		}
		return -1;
	}

	/* Done */
	if(blob != buf)
	{
		ILFree(blob);
	}
	return 1;
}

/*
 * Process a method implementation attribute.
 */
static int MethodImplAttribute(ILProgramItem *item, ILSerializeReader *reader)
{
	ILMethod *method;
	int type;
	ILUInt32 attrs;

	/* We must use this on a method */
	method = ILProgramItemToMethod(item);
	if(!method)
	{
		return 0;
	}

	/* Get the first parameter, which should be int32 or int16 */
	type = ILSerializeReaderGetParamType(reader);
	if(type != IL_META_SERIALTYPE_I4 && type != IL_META_SERIALTYPE_I2)
	{
		return 0;
	}
	attrs = (ILUInt32)(ILSerializeReaderGetInt32(reader, type) & 0xFFFF);
	if(ILSerializeReaderGetParamType(reader) != 0 ||
	   ILSerializeReaderGetNumExtra(reader) != 0)
	{
		return 0;
	}

	/* Merge the attributes into the method's definition */
	ILMethodSetImplAttrs(method, ~IL_META_METHODIMPL_CODE_TYPE_MASK,
						 (attrs & ~IL_META_METHODIMPL_CODE_TYPE_MASK));
	return 1;
}

/*
 * Attribute lookup tables.
 */
typedef struct
{
	const char *name;
	int (*func)(ILProgramItem *item, ILSerializeReader *reader);

} AttrConvertInfo;
static AttrConvertInfo const interopAttrs[] = {
	{"DllImportAttribute",	DllImportAttribute},
	{"FieldOffsetAttribute", FieldOffsetAttribute},
	{"InAttribute", InAttribute},
	{"OutAttribute", OutAttribute},
	{"StructLayoutAttribute", StructLayoutAttribute},
	{"MarshalAsAttribute", MarshalAsAttribute},
	{0, 0}
};
static AttrConvertInfo const compilerAttrs[] = {
	{"MethodImplAttribute",	MethodImplAttribute},
	{0, 0}
};

/*
 * Convert an attribute into metadata information.
 * Returns zero if not a builtin attribute, -1 if
 * out of memory, and 1 if OK.
 */
static int ConvertAttribute(ILProgramItem *item, ILAttribute *attr)
{
	ILMethod *ctor;
	const char *name;
	const char *namespace;
	const AttrConvertInfo *info;
	const void *blob;
	unsigned long blobLen;
	ILSerializeReader *reader;
	int result;

	/* Get the name of the class and the constructor signature */
	ctor = ILProgramItemToMethod(ILAttributeTypeAsItem(attr));
	if(!ctor)
	{
		return 0;
	}
	name = ILClass_Name(ILMethod_Owner(ctor));
	namespace = ILClass_Namespace(ILMethod_Owner(ctor));
	if(!namespace)
	{
		return 0;
	}

	/* Determine which attribute we are dealing with */
	if(!strcmp(namespace, "System.Runtime.InteropServices"))
	{
		info = interopAttrs;
	}
	else if(!strcmp(namespace, "System.Runtime.CompilerServices"))
	{
		info = compilerAttrs;
	}
	else
	{
		return 0;
	}
	while(info->name != 0 && strcmp(info->name, name) != 0)
	{
		++info;
	}
	if(!(info->name))
	{
		return 0;
	}

	/* Get the data blob from the attribute and create a reader for it */
	blob = ILAttributeGetValue(attr, &blobLen);
	if(!blob)
	{
		return 0;
	}
	reader = ILSerializeReaderInit(ctor, blob, blobLen);
	if(!reader)
	{
		return 0;
	}

	/* Convert the attribute into metadata structures */
	result = (*(info->func))(item, reader);
	ILSerializeReaderDestroy(reader);
	return result;
}

int ILProgramItemConvertAttrs(ILProgramItem *item)
{
	ILAttribute *attr = ILProgramItemNextAttribute(item, 0);
	int result;
	while(attr != 0)
	{
		if((result = ConvertAttribute(item, attr)) < 0)
		{
			return 0;
		}
		else if(result > 0)
		{
			attr = ILProgramItemRemoveAttribute(item, attr);
		}
		else
		{
			attr = ILProgramItemNextAttribute(item, attr);
		}
	}
	return 1;
}

#ifdef	__cplusplus
};
#endif
