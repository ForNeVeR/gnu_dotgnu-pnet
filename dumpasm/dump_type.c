/*
 * dump_type.c - Dump types in assembly format.
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

#include "il_dumpasm.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void ILDumpType(FILE *stream, ILImage *image, ILType *type, int flags)
{
	ILType *elem;

	if(ILType_IsPrimitive(type))
	{
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:
			{
				fputs("void", stream);
			}
			break;

			case IL_META_ELEMTYPE_BOOLEAN:
			{
				fputs("bool", stream);
			}
			break;

			case IL_META_ELEMTYPE_CHAR:
			{
				fputs("char", stream);
			}
			break;

			case IL_META_ELEMTYPE_I1:
			{
				fputs("int8", stream);
			}
			break;

			case IL_META_ELEMTYPE_U1:
			{
				fputs("unsigned int8", stream);
			}
			break;

			case IL_META_ELEMTYPE_I2:
			{
				fputs("int16", stream);
			}
			break;

			case IL_META_ELEMTYPE_U2:
			{
				fputs("unsigned int16", stream);
			}
			break;

			case IL_META_ELEMTYPE_I4:
			{
				fputs("int32", stream);
			}
			break;

			case IL_META_ELEMTYPE_U4:
			{
				fputs("unsigned int32", stream);
			}
			break;

			case IL_META_ELEMTYPE_I8:
			{
				fputs("int64", stream);
			}
			break;

			case IL_META_ELEMTYPE_U8:
			{
				fputs("unsigned int64", stream);
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
				fputs("float32", stream);
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				fputs("float64", stream);
			}
			break;

			case IL_META_ELEMTYPE_TYPEDBYREF:
			{
				fputs("typedref", stream);
			}
			break;

			case IL_META_ELEMTYPE_I:
			{
				fputs("native int", stream);
			}
			break;

			case IL_META_ELEMTYPE_U:
			{
				fputs("native unsigned int", stream);
			}
			break;

			case IL_META_ELEMTYPE_R:
			{
				fputs("native float", stream);
			}
			break;

			case IL_META_ELEMTYPE_SENTINEL:
			{
				fputs("SENTINEL", stream);
			}
			break;

			default:
			{
				fputs("UNKNOWN PRIMITIVE TYPE", stream);
			}
			break;
		}
	}
	else if(ILType_IsClass(type))
	{
		fputs("class ", stream);
		ILDumpClassName(stream, image, ILType_ToClass(type), flags);
	}
	else if(ILType_IsValueType(type))
	{
		fputs("valuetype ", stream);
		ILDumpClassName(stream, image, ILType_ToValueType(type), flags);
	}
	else if(type && ILType_IsComplex(type))
	{
		switch(type->kind & 0xFF)
		{
			case IL_TYPE_COMPLEX_BYREF:
			{
				ILDumpType(stream, image, type->un.refType, flags);
				fputs(" &", stream);
			}
			break;

			case IL_TYPE_COMPLEX_PTR:
			{
				ILDumpType(stream, image, type->un.refType, flags);
				fputs(" *", stream);
			}
			break;

			case IL_TYPE_COMPLEX_ARRAY:
			case IL_TYPE_COMPLEX_ARRAY_CONTINUE:
			{
				/* Find the element type and dump it */
				elem = type->un.array.elemType;
				while(elem != 0 && ILType_IsComplex(elem) &&
				      (elem->kind == IL_TYPE_COMPLEX_ARRAY ||
					   elem->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE))
				{
					elem = elem->un.array.elemType;
				}
				ILDumpType(stream, image, elem, flags);

				/* Dump the dimensions */
				putc('[', stream);
				elem = type;
				while(elem != 0 && ILType_IsComplex(elem) &&
				      (elem->kind == IL_TYPE_COMPLEX_ARRAY ||
					   elem->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE))
				{
					if(elem->un.array.size != 0)
					{
						if(elem->un.array.lowBound != 0)
						{
							/* We have both a low bound and a size */
							fprintf(stream, "%ld...%ld",
									elem->un.array.lowBound,
									elem->un.array.lowBound +
										elem->un.array.size - 1);
						}
						else
						{
							/* We only have a size */
							fprintf(stream, "%ld", elem->un.array.size);
						}
					}
					else if(elem->un.array.lowBound != 0)
					{
						/* We only have a low bound */
						fprintf(stream, "%ld...", elem->un.array.lowBound);
					}
					if(elem->kind == IL_TYPE_COMPLEX_ARRAY)
					{
						putc(']', stream);
						elem = elem->un.array.elemType;
						if(ILType_IsComplex(elem) && elem != 0 &&
						   (elem->kind == IL_TYPE_COMPLEX_ARRAY ||
						    elem->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE))
						{
							putc('[', stream);
						}
					}
					else
					{
						putc(',', stream);
						elem = elem->un.array.elemType;
					}
				}
			}
			break;

			case IL_TYPE_COMPLEX_CMOD_REQD:
			{
				ILDumpType(stream, image, type->un.modifier.type, flags);
				fputs(" modreq(", stream);
				ILDumpClassName(stream, image, type->un.modifier.info, flags);
				putc(')', stream);
			}
			break;

			case IL_TYPE_COMPLEX_CMOD_OPT:
			{
				ILDumpType(stream, image, type->un.modifier.type, flags);
				fputs(" modopt(", stream);
				ILDumpClassName(stream, image, type->un.modifier.info, flags);
				putc(')', stream);
			}
			break;

			case IL_TYPE_COMPLEX_PROPERTY:
			{
				fputs("property ", stream);
				ILDumpMethodType(stream, image, type, flags, 0, 0, 0);
			}
			break;

			case IL_TYPE_COMPLEX_SENTINEL:
			{
				fputs("...", stream);
			}
			break;

			case IL_TYPE_COMPLEX_PINNED:
			{
				ILDumpType(stream, image, type->un.refType, flags);
				fputs(" pinned", stream);
			}
			break;

			default:
			{
				if((type->kind & IL_TYPE_COMPLEX_METHOD) != 0)
				{
					fputs("method ", stream);
					ILDumpMethodType(stream, image, type, flags, 0, 0, 0);
				}
				else
				{
					fprintf(stream, "UNKNOWN COMPLEX TYPE %08X",
							(int)(type->kind));
				}
			}
			break;
		}
	}
	else
	{
		fputs("UNKNOWN TYPE", stream);
	}
}

/*
 * Dump parameter type, attribute, marshalling, and name information.
 */
static void DumpParamType(FILE *stream, ILImage *image,
						  ILMethod *method, ILType *paramType,
						  ILUInt32 num, int flags)
{
	ILParameter *param;
	ILFieldMarshal *marshal;
	const void *type;
	unsigned long typeLen;
	const char *name;

	/* Get the parameter information block, if one is present */
	param = 0;
	if(method)
	{
		while((param = ILMethodNextParam(method, param)) != 0)
		{
			if(ILParameterGetNum(param) == num)
			{
				break;
			}
		}
	}

	/* Dump the parameter attributes */
	if(param)
	{
		if(ILParameter_IsIn(param))
		{
			fputs("[in] ", stream);
		}
		if(ILParameter_IsOut(param))
		{
			fputs("[out] ", stream);
		}
		if(ILParameter_IsRetVal(param))
		{
			fputs("[retval] ", stream);
		}
		if(ILParameter_IsOptional(param))
		{
			fputs("[opt] ", stream);
		}
	}

	/* Dump the parameter's type */
	ILDumpType(stream, image, paramType, flags);

	/* Dump the field marshalling information, if present */
    if(param &&
	   (ILParameter_Attrs(param) & IL_META_PARAMDEF_HAS_FIELD_MARSHAL) != 0)
	{
		marshal = ILFieldMarshalGetFromOwner((ILProgramItem *)param);
		if(marshal)
		{
			type = ILFieldMarshalGetType(marshal, &typeLen);
			if(type)
			{
				fputs(" marshal(", stream);
				ILDumpNativeType(stream, type, typeLen);
				putc(')', stream);
			}
		}
	}

	/* Dump the parameter's name */
	if(param && num != 0)
	{
		name = ILParameter_Name(param);
		if(name)
		{
			putc(' ', stream);
			ILDumpIdentifier(stream, name, 0, flags);
		}
	}
}

static void DumpParams(FILE *stream, ILImage *image, ILType *type,
					   ILMethod *methodInfo, int flags)
{
	ILType *temp;
	unsigned long num;
	ILUInt32 pnum;
	if(!(type->num))
	{
		return;
	}
	DumpParamType(stream, image, methodInfo,
				  type->un.method.param[0], 1, flags);
	if(type->num == 1)
	{
		return;
	}
	fputs(", ", stream);
	DumpParamType(stream, image, methodInfo,
				  type->un.method.param[1], 2, flags);
	if(type->num == 2)
	{
		return;
	}
	fputs(", ", stream);
	DumpParamType(stream, image, methodInfo,
				  type->un.method.param[2], 3, flags);
	temp = type->un.method.next;
	num = type->num - 3;
	pnum = 4;
	while(num > 4)
	{
		fputs(", ", stream);
		DumpParamType(stream, image, methodInfo,
					  temp->un.params.param[0], pnum++, flags);
		fputs(", ", stream);
		DumpParamType(stream, image, methodInfo,
					  temp->un.params.param[1], pnum++, flags);
		fputs(", ", stream);
		DumpParamType(stream, image, methodInfo,
					  temp->un.params.param[2], pnum++, flags);
		fputs(", ", stream);
		DumpParamType(stream, image, methodInfo,
					  temp->un.params.param[3], pnum++, flags);
		num -= 4;
		temp = temp->un.params.next;
	}
	if(num > 0)
	{
		fputs(", ", stream);
		DumpParamType(stream, image, methodInfo,
					  temp->un.params.param[0], pnum++, flags);
	}
	if(num > 1)
	{
		fputs(", ", stream);
		DumpParamType(stream, image, methodInfo,
					  temp->un.params.param[1], pnum++, flags);
	}
	if(num > 2)
	{
		fputs(", ", stream);
		DumpParamType(stream, image, methodInfo,
					  temp->un.params.param[2], pnum++, flags);
	}
	if(num > 3)
	{
		fputs(", ", stream);
		DumpParamType(stream, image, methodInfo,
					  temp->un.params.param[3], pnum++, flags);
	}
}

void ILDumpMethodType(FILE *stream, ILImage *image, ILType *type, int flags,
					  ILClass *info, const char *methodName,
					  ILMethod *methodInfo)
{
	ILUInt32 callingConventions;
	ILType *synType;

	/* Dump the calling conventions for the method */
	callingConventions = (ILUInt32)(type->kind >> 8);
	ILDumpFlags(stream, callingConventions, ILMethodCallConvFlags, 0);

	/* Dump the return type */
	DumpParamType(stream, image, methodInfo,
				  type->un.method.retType, 0, flags);
	putc(' ', stream);

	/* Dump the class name and method name */
	if(info)
	{
		synType = ILClass_SynType(info);
		if(synType)
		{
			ILDumpType(stream, image, synType, flags);
		}
		else
		{
			ILDumpClassName(stream, image, info, flags);
		}
		fputs("::", stream);
	}
	if(methodName)
	{
		ILDumpIdentifier(stream, methodName, 0, flags);
	}
	else
	{
		putc('*', stream);
	}

	/* Dump the parameters */
	putc('(', stream);
	DumpParams(stream, image, type, methodInfo, flags);
	putc(')', stream);
}

/*
 * Dump a variant type.
 */
static void DumpVariantType(FILE *stream, ILUInt32 type)
{
	switch(type & IL_META_VARIANTTYPE_BASE_TYPE_MASK)
	{
		case IL_META_VARIANTTYPE_EMPTY:
		{
			/* Nothing to print here */
		}
		break;

		case IL_META_VARIANTTYPE_NULL:
		{
			fputs("null", stream);
		}
		break;

		case IL_META_VARIANTTYPE_I2:
		{
			fputs("int16", stream);
		}
		break;

		case IL_META_VARIANTTYPE_I4:
		{
			fputs("int32", stream);
		}
		break;

		case IL_META_VARIANTTYPE_R4:
		{
			fputs("float32", stream);
		}
		break;

		case IL_META_VARIANTTYPE_R8:
		{
			fputs("float64", stream);
		}
		break;

		case IL_META_VARIANTTYPE_CY:
		{
			fputs("currency", stream);
		}
		break;

		case IL_META_VARIANTTYPE_DATE:
		{
			fputs("date", stream);
		}
		break;

		case IL_META_VARIANTTYPE_BSTR:
		{
			fputs("bstr", stream);
		}
		break;

		case IL_META_VARIANTTYPE_DISPATCH:
		{
			fputs("idispatch", stream);
		}
		break;

		case IL_META_VARIANTTYPE_ERROR:
		{
			fputs("error", stream);
		}
		break;

		case IL_META_VARIANTTYPE_BOOL:
		{
			fputs("bool", stream);
		}
		break;

		case IL_META_VARIANTTYPE_VARIANT:
		{
			fputs("variant", stream);
		}
		break;

		case IL_META_VARIANTTYPE_UNKNOWN:
		{
			fputs("iunknown", stream);
		}
		break;

		case IL_META_VARIANTTYPE_DECIMAL:
		{
			fputs("decimal", stream);
		}
		break;

		case IL_META_VARIANTTYPE_I1:
		{
			fputs("int8", stream);
		}
		break;

		case IL_META_VARIANTTYPE_UI1:
		{
			fputs("unsigned int8", stream);
		}
		break;

		case IL_META_VARIANTTYPE_UI2:
		{
			fputs("unsigned int16", stream);
		}
		break;

		case IL_META_VARIANTTYPE_UI4:
		{
			fputs("unsigned int32", stream);
		}
		break;

		case IL_META_VARIANTTYPE_I8:
		{
			fputs("int64", stream);
		}
		break;

		case IL_META_VARIANTTYPE_UI8:
		{
			fputs("unsigned int64", stream);
		}
		break;

		case IL_META_VARIANTTYPE_INT:
		{
			fputs("int", stream);
		}
		break;

		case IL_META_VARIANTTYPE_UINT:
		{
			fputs("unsigned int", stream);
		}
		break;

		case IL_META_VARIANTTYPE_VOID:
		{
			fputs("void", stream);
		}
		break;

		case IL_META_VARIANTTYPE_HRESULT:
		{
			fputs("hresult", stream);
		}
		break;

		case IL_META_VARIANTTYPE_PTR:
		{
			fputs("ptr", stream);
		}
		break;

		case IL_META_VARIANTTYPE_SAFEARRAY:
		{
			fputs("safearray", stream);
		}
		break;

		case IL_META_VARIANTTYPE_CARRAY:
		{
			fputs("carray", stream);
		}
		break;

		case IL_META_VARIANTTYPE_USERDEFINED:
		{
			fputs("userdefined", stream);
		}
		break;

		case IL_META_VARIANTTYPE_LPSTR:
		{
			fputs("lpstr", stream);
		}
		break;

		case IL_META_VARIANTTYPE_LPWSTR:
		{
			fputs("lpwstr", stream);
		}
		break;

		case IL_META_VARIANTTYPE_RECORD:
		{
			fputs("record", stream);
		}
		break;

		case IL_META_VARIANTTYPE_FILETIME:
		{
			fputs("filetime", stream);
		}
		break;

		case IL_META_VARIANTTYPE_BLOB:
		{
			fputs("blob", stream);
		}
		break;

		case IL_META_VARIANTTYPE_STREAM:
		{
			fputs("stream", stream);
		}
		break;

		case IL_META_VARIANTTYPE_STORAGE:
		{
			fputs("storage", stream);
		}
		break;

		case IL_META_VARIANTTYPE_STREAMED_OBJECT:
		{
			fputs("streamed_object", stream);
		}
		break;

		case IL_META_VARIANTTYPE_STORED_OBJECT:
		{
			fputs("stored_object", stream);
		}
		break;

		case IL_META_VARIANTTYPE_BLOB_OBJECT:
		{
			fputs("blob_object", stream);
		}
		break;

		case IL_META_VARIANTTYPE_CF:
		{
			fputs("cf", stream);
		}
		break;

		case IL_META_VARIANTTYPE_CLSID:
		{
			fputs("clsid", stream);
		}
		break;

		default:
		{
			fputs("UNKNOWN", stream);
		}
		break;
	}
	if((type & IL_META_VARIANTTYPE_ARRAY) != 0)
	{
		fputs("[]", stream);
	}
	if((type & IL_META_VARIANTTYPE_VECTOR) != 0)
	{
		fputs(" vector", stream);
	}
	if((type & IL_META_VARIANTTYPE_BYREF) != 0)
	{
		fputs(" &", stream);
	}
}

/*
 * Inner version of "ILDumpNativeType".
 */
static void DumpNativeType(FILE *stream, ILMetaDataRead *reader)
{
	unsigned long value;
	unsigned long value2;
	if(reader->len == 0)
	{
		fputs("UNKNOWN", stream);
		return;
	}
	--(reader->len);
	switch(*(reader->data++))
	{
		case IL_META_NATIVETYPE_VOID:
		{
			fputs("void", stream);
		}
		break;

		case IL_META_NATIVETYPE_BOOLEAN:
		{
			fputs("bool", stream);
		}
		break;

		case IL_META_NATIVETYPE_I1:
		{
			fputs("int8", stream);
		}
		break;

		case IL_META_NATIVETYPE_U1:
		{
			fputs("unsigned int8", stream);
		}
		break;

		case IL_META_NATIVETYPE_I2:
		{
			fputs("int16", stream);
		}
		break;

		case IL_META_NATIVETYPE_U2:
		{
			fputs("unsigned int16", stream);
		}
		break;

		case IL_META_NATIVETYPE_I4:
		{
			fputs("int32", stream);
		}
		break;

		case IL_META_NATIVETYPE_U4:
		{
			fputs("unsigned int32", stream);
		}
		break;

		case IL_META_NATIVETYPE_I8:
		{
			fputs("int64", stream);
		}
		break;

		case IL_META_NATIVETYPE_U8:
		{
			fputs("unsigned int64", stream);
		}
		break;

		case IL_META_NATIVETYPE_R4:
		{
			fputs("float32", stream);
		}
		break;

		case IL_META_NATIVETYPE_R8:
		{
			fputs("float64", stream);
		}
		break;

		case IL_META_NATIVETYPE_SYSCHAR:
		{
			fputs("syschar", stream);
		}
		break;

		case IL_META_NATIVETYPE_VARIANT:
		{
			fputs("variant", stream);
		}
		break;

		case IL_META_NATIVETYPE_CURRENCY:
		{
			fputs("currency", stream);
		}
		break;

		case IL_META_NATIVETYPE_PTR:
		{
			DumpNativeType(stream, reader);
			fputs(" *", stream);
		}
		break;

		case IL_META_NATIVETYPE_DECIMAL:
		{
			fputs("decimal", stream);
		}
		break;

		case IL_META_NATIVETYPE_DATE:
		{
			fputs("date", stream);
		}
		break;

		case IL_META_NATIVETYPE_BSTR:
		{
			fputs("bstr", stream);
		}
		break;

		case IL_META_NATIVETYPE_LPSTR:
		{
			fputs("lpstr", stream);
		}
		break;

		case IL_META_NATIVETYPE_LPWSTR:
		{
			fputs("lpwstr", stream);
		}
		break;

		case IL_META_NATIVETYPE_LPTSTR:
		{
			fputs("lptstr", stream);
		}
		break;

		case IL_META_NATIVETYPE_FIXEDSYSSTRING:
		{
			fputs("fixed sysstring [", stream);
			value = ILMetaUncompressData(reader);
			fprintf(stream, "%lu]", value);
		}
		break;

		case IL_META_NATIVETYPE_OBJECTREF:
		{
			fputs("objectref", stream);
		}
		break;

		case IL_META_NATIVETYPE_IUNKNOWN:
		{
			fputs("iunknown", stream);
		}
		break;

		case IL_META_NATIVETYPE_IDISPATCH:
		{
			fputs("idispatch", stream);
		}
		break;

		case IL_META_NATIVETYPE_STRUCT:
		{
			fputs("struct", stream);
		}
		break;

		case IL_META_NATIVETYPE_INTF:
		{
			fputs("interface", stream);
		}
		break;

		case IL_META_NATIVETYPE_SAFEARRAY:
		{
			fputs("safearray ", stream);
			value = ILMetaUncompressData(reader);
			DumpVariantType(stream, (ILUInt32)value);
		}
		break;

		case IL_META_NATIVETYPE_FIXEDARRAY:
		{
			/* There seems to be some confusion in Microsoft's
			   specification as to whether a fixed array type
			   has an element type or not.  The assembly syntax
			   seems to indicate no, but the marshalling blob
			   description seems to indicate yes.  We therefore
			   handle both cases by checking for an optional native
			   type following the number of array elements */
			value = ILMetaUncompressData(reader);
			if(reader->len)
			{
				fputs("fixed array ", stream);
				DumpNativeType(stream, reader);
				fprintf(stream, "[%lu]", value);
			}
			else
			{
				fprintf(stream, "fixed array [%lu]", value);
			}
		}
		break;

		case IL_META_NATIVETYPE_INT:
		{
			fputs("int", stream);
		}
		break;

		case IL_META_NATIVETYPE_UINT:
		{
			fputs("unsigned int", stream);
		}
		break;

		case IL_META_NATIVETYPE_NESTEDSTRUCT:
		{
			fputs("nested struct", stream);
		}
		break;

		case IL_META_NATIVETYPE_BYVALSTR:
		{
			fputs("byvalstr", stream);
		}
		break;

		case IL_META_NATIVETYPE_ANSIBSTR:
		{
			fputs("ansi bstr", stream);
		}
		break;

		case IL_META_NATIVETYPE_TBSTR:
		{
			fputs("tbstr", stream);
		}
		break;

		case IL_META_NATIVETYPE_VARIANTBOOL:
		{
			fputs("variant bool", stream);
		}
		break;

		case IL_META_NATIVETYPE_FUNC:
		{
			fputs("method", stream);
		}
		break;

		case IL_META_NATIVETYPE_LPVOID:
		{
			fputs("lpvoid", stream);
		}
		break;

		case IL_META_NATIVETYPE_ASANY:
		{
			fputs("as any", stream);
		}
		break;

		case IL_META_NATIVETYPE_R:
		{
			fputs("float", stream);
		}
		break;

		case IL_META_NATIVETYPE_ARRAY:
		{
			DumpNativeType(stream, reader);
			putc('[', stream);
			value = ILMetaUncompressData(reader);
			if(value)
			{
				fprintf(stream, ".size .param = %lu", value);
				value = ILMetaUncompressData(reader);
				value2 = ILMetaUncompressData(reader);
				if(value != 1)
				{
					fprintf(stream, " * %lu]", value);
				}
				else
				{
					putc(']', stream);
				}
			}
			else
			{
				value = ILMetaUncompressData(reader);
				value2 = ILMetaUncompressData(reader);
				if(value2 != 0)
				{
					fprintf(stream, "%lu]", value2);
				}
				else
				{
					putc(']', stream);
				}
			}
		}
		break;

		case IL_META_NATIVETYPE_LPSTRUCT:
		{
			fputs("lpstruct", stream);
		}
		break;

		case IL_META_NATIVETYPE_CUSTOMMARSHALER:
		{
			fputs("custom(", stream);
			value = ILMetaUncompressData(reader);
			if(!(reader->error) && value <= reader->len)
			{
				ILDumpStringLen(stream, (const char *)(reader->data),
								(int)value);
				reader->data += value;
				reader->len -= value;
			}
			else
			{
				fputs("UNKNOWN", stream);
				reader->len = 0;
			}
			fputs(", ", stream);
			value = ILMetaUncompressData(reader);
			if(!(reader->error) && value <= reader->len)
			{
				ILDumpStringLen(stream, (const char *)(reader->data),
								(int)value);
				reader->data += value;
				reader->len -= value;
			}
			else
			{
				fputs("UNKNOWN", stream);
				reader->len = 0;
			}
			fputs(", ", stream);
			value = ILMetaUncompressData(reader);
			if(!(reader->error) && value <= reader->len)
			{
				ILDumpStringLen(stream, (const char *)(reader->data),
								(int)value);
				reader->data += value;
				reader->len -= value;
			}
			else
			{
				fputs("UNKNOWN", stream);
				reader->len = 0;
			}
			fputs(", ", stream);
			value = ILMetaUncompressData(reader);
			if(!(reader->error) && value <= reader->len)
			{
				ILDumpStringLen(stream, (const char *)(reader->data),
								(int)value);
				reader->data += value;
				reader->len -= value;
			}
			else
			{
				fputs("UNKNOWN", stream);
				reader->len = 0;
			}
			putc(')', stream);
		}
		break;

		case IL_META_NATIVETYPE_ERROR:
		{
			fputs("error", stream);
		}
		break;

		case IL_META_NATIVETYPE_MAX:
		{
			/* Nothing to print here */
		}
		break;

		default:
		{
			fputs("UNKNOWN", stream);
		}
		break;
	}
}

void ILDumpNativeType(FILE *stream, const void *type, unsigned long len)
{
	ILMetaDataRead reader;
	reader.data = (const unsigned char *)type;
	reader.len = len;
	reader.error = 0;
	DumpNativeType(stream, &reader);
}

#ifdef	__cplusplus
};
#endif