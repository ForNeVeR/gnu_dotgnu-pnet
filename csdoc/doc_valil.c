/*
 * doc_valil.c - Validate that an IL program implements a csdoc specification.
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

#include <stdio.h>
#include "il_system.h"
#include "il_utils.h"
#include "il_program.h"
#include "il_dumpasm.h"
#include "il_serialize.h"
#include "doc_tree.h"
#include "doc_backend.h"

#ifdef	__cplusplus
extern	"C" {
#endif

char const ILDocProgramHeader[] =
	"CSDOCVALIL " VERSION " - Validate IL binaries against C# documentation";

char const ILDocProgramName[] = "CSDOCVALIL";

ILCmdLineOption const ILDocProgramOptions[] = {
	{"-fimage", 'f', 1,
		"-fimage=PATH",
		"Specify the IL image path to validate."},
	{"-fextra-types-ok", 'f', 1,
		"-fextra-types-ok",
		"It is OK for extra types to appear in the image."},
	{"-fextra-members-ok", 'f', 1,
		"-fextra-members-ok",
		"It is OK for extra members to appear in the image."},
	{"-fassembly-map", 'f', 1,
		"-fassembly-map=NAME1,NAME2",
		"Map the assembly `NAME1' in the image to `NAME2' in the XML file."},
	{0, 0, 0, 0, 0}
};

char *ILDocDefaultOutput(int numInputs, char **inputs, const char *progname)
{
	/* The default output is always stdout */
	return "-";
}

int ILDocValidateOutput(char *outputPath, const char *progname)
{
	/* Nothing to do here: any pathname is considered valid */
	return 1;
}

/*
 * Get the full name of an image class.
 */
static char *GetFullClassName(ILClass *classInfo)
{
	const char *name = ILClass_Name(classInfo);
	const char *namespace = ILClass_Namespace(classInfo);
	char *fullName;
	if(namespace)
	{
		fullName = (char *)ILMalloc(strlen(namespace) +
									strlen(name) + 2);
		if(!fullName)
		{
			ILDocOutOfMemory(0);
		}
		strcpy(fullName, namespace);
		strcat(fullName, ".");
		strcat(fullName, name);
	}
	else
	{
		fullName = (char *)ILMalloc(strlen(name) + 1);
		if(!fullName)
		{
			ILDocOutOfMemory(0);
		}
		strcpy(fullName, name);
	}
	return fullName;
}

/*
 * Determine if an image class is a delegate type.
 */
static int IsDelegateType(ILClass *classInfo)
{
	ILClass *parent = ILClass_ParentRef(classInfo);
	const char *name;
	while(parent != 0)
	{
		name = ILClass_Name(parent);
		if(!strcmp(name, "MulticastDelegate"))
		{
			name = ILClass_Namespace(parent);
			if(name && !strcmp(name, "System"))
			{
				return 1;
			}
		}
		parent = ILClass_ParentRef(parent);
	}
	return 0;
}

/*
 * Type attribute flags that are relevant during comparisons.
 */
#define	VALID_TYPE_FLAGS	(IL_META_TYPEDEF_VALID_BITS & \
						     ~IL_META_TYPEDEF_BEFORE_FIELD_INIT & \
							 ~IL_META_TYPEDEF_SERIALIZABLE)

/*
 * Field attribute flags that are relevant during comparisons.
 */
#define	VALID_FIELD_FLAGS	(0x7FFF & \
							 ~IL_META_FIELDDEF_INIT_ONLY)

/*
 * Method attribute flags that are relevant during comparisons.
 */
#define	VALID_METHOD_FLAGS	(0x7FFF & \
							 ~IL_META_METHODDEF_NEW_SLOT)

/*
 * Constructor attribute flags that are relevant during comparisons.
 */
#define	VALID_CTOR_FLAGS	(0x7FFF & \
							 ~IL_META_METHODDEF_HIDE_BY_SIG & \
							 ~IL_META_METHODDEF_RT_SPECIAL_NAME)

/*
 * Event attribute flags that are relevant during comparisons.
 */
#define	VALID_EVENT_FLAGS	(0x7FFF & \
							 ~IL_META_METHODDEF_HIDE_BY_SIG & \
							 ~IL_META_METHODDEF_SPECIAL_NAME)

/*
 * Convert an image type into a type name.
 */
static char *TypeToName(ILType *type, int shortForm)
{
	char *name;
	ILClass *classInfo;
	const char *className;
	const char *classNamespace;
	const char *suffix;
	char buffer[128];
	int posn;

	if(ILType_IsPrimitive(type))
	{
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:		name = "System.Void"; break;
			case IL_META_ELEMTYPE_BOOLEAN:	name = "System.Boolean"; break;
			case IL_META_ELEMTYPE_I1:		name = "System.SByte"; break;
			case IL_META_ELEMTYPE_U1:		name = "System.Byte"; break;
			case IL_META_ELEMTYPE_I2:		name = "System.Int16"; break;
			case IL_META_ELEMTYPE_U2:		name = "System.UInt16"; break;
			case IL_META_ELEMTYPE_CHAR:		name = "System.Char"; break;
			case IL_META_ELEMTYPE_I4:		name = "System.Int32"; break;
			case IL_META_ELEMTYPE_U4:		name = "System.UInt32"; break;
			case IL_META_ELEMTYPE_I8:		name = "System.Int64"; break;
			case IL_META_ELEMTYPE_U8:		name = "System.UInt64"; break;
			case IL_META_ELEMTYPE_I:		name = "System.IntPtr"; break;
			case IL_META_ELEMTYPE_U:		name = "System.UIntPtr"; break;
			case IL_META_ELEMTYPE_R4:		name = "System.Single"; break;
			case IL_META_ELEMTYPE_R8:		name = "System.Double"; break;
			case IL_META_ELEMTYPE_TYPEDBYREF:
							name = "System.TypedReference"; break;
			default:						name = "*Unknown*"; break;
		}
		if(shortForm && name[0] == 'S')
		{
			name += 7;
		}
		name = ILDupString(name);
	}
	else if(ILType_IsClass(type) || ILType_IsValueType(type))
	{
		classInfo = ILType_ToClass(type);
		className = ILClass_Name(classInfo);
		classNamespace = ILClass_Namespace(classInfo);
		if(!classNamespace || shortForm)
		{
			name = ILDupString(className);
		}
		else
		{
			name = (char *)ILMalloc(strlen(className) +
									strlen(classNamespace) + 2);
			if(!name)
			{
				ILDocOutOfMemory(0);
			}
			strcpy(name, classNamespace);
			strcat(name, ".");
			strcat(name, className);
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		if(type->kind == IL_TYPE_COMPLEX_BYREF)
		{
			name = TypeToName(type->un.refType, shortForm);
			suffix = "&";
		}
		else if(type->kind == IL_TYPE_COMPLEX_PTR)
		{
			name = TypeToName(type->un.refType, shortForm);
			suffix = "*";
		}
		else if(type->kind == IL_TYPE_COMPLEX_ARRAY)
		{
			name = TypeToName(type->un.refType, shortForm);
			suffix = "[]";
		}
		else if(type->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE)
		{
			buffer[0] = '[';
			posn = 1;
			while(type != 0 && ILType_IsComplex(type) &&
			      type->kind == IL_TYPE_COMPLEX_ARRAY_CONTINUE &&
				  posn < (sizeof(buffer) - 8))
			{
				buffer[posn++] = ',';
				type = type->un.array.elemType;
			}
			buffer[posn++] = ']';
			buffer[posn] = '\0';
			suffix = buffer;
			if(type != 0 && ILType_IsComplex(type) &&
			   type->kind == IL_TYPE_COMPLEX_ARRAY)
			{
				name = TypeToName(type->un.array.elemType, shortForm);
			}
			else
			{
				name = ILDupString("*Unknown*");
			}
		}
		else
		{
			name = ILDupString("*Unknown*");
			suffix = "";
		}
		if(*suffix != '\0')
		{
			name = (char *)ILRealloc(name, strlen(name) + strlen(suffix) + 1);
			if(!name)
			{
				ILDocOutOfMemory(0);
			}
			strcat(name, suffix);
		}
	}
	else
	{
		name = ILDupString("*Unknown*");
	}
	if(!name)
	{
		ILDocOutOfMemory(0);
	}
	return name;
}

/*
 * Match an image type against a name from an XML file.
 * Returns zero if the types do not match.
 */
static int MatchType(ILType *type, const char *typeName)
{
	char *name = TypeToName(type, 1);
	if(!strcmp(name, typeName))
	{
		ILFree(name);
		return 1;
	}
	else
	{
		/* Try again with the long form of the name */
		ILFree(name);
		name = TypeToName(type, 0);
		if(!strcmp(name, typeName))
		{
			ILFree(name);
			return 1;
		}
		ILFree(name);
		return 0;
	}
}

/*
 * Match a method parameter signature.
 */
static int MatchSignature(ILType *signature, ILDocMember *member)
{
	ILDocParameter *param = member->parameters;
	unsigned numParams = signature->num;
	unsigned paramNum = 1;
	while(param != 0 && paramNum <= numParams)
	{
		if(!MatchType(ILTypeGetParam(signature, paramNum), param->type))
		{
			return 0;
		}
		param = param->next;
		++paramNum;
	}
	if(param != 0 || paramNum <= numParams)
	{
		return 0;
	}
	if(!strcmp(member->name, "op_Explicit") ||
	   !strcmp(member->name, "op_Implicit"))
	{
		/* The return type is part of the signature of a conversion */
		if(!MatchType(signature->un.method.retType, member->returnType))
		{
			return 0;
		}
	}
	return 1;
}

/*
 * Match a property parameter signature.
 */
static int MatchPropertySignature(ILType *signature,
								  ILDocMember *member, int isSet)
{
	ILDocParameter *param = member->parameters;
	unsigned numParams = signature->num;
	unsigned paramNum = 1;
	while(param != 0 && paramNum <= numParams)
	{
		if(!MatchType(ILTypeGetParam(signature, paramNum), param->type))
		{
			return 0;
		}
		param = param->next;
		++paramNum;
	}
	if(isSet)
	{
		/* Matching against a "set" accessor */
		if(param != 0 || paramNum != numParams)
		{
			return 0;
		}
	}
	else
	{
		/* Matching against a "get" accessor */
		if(param != 0 || paramNum <= numParams)
		{
			return 0;
		}
	}
	return 1;
}

/*
 * Print the name of an image type.
 */
static void PrintType(FILE *stream, ILType *type)
{
	char *name = TypeToName(type, 0);
	fputs(name, stream);
	ILFree(name);
}

/*
 * Print the name of a method, including its signature.
 */
static void PrintMethodName(FILE *stream, ILDocMember *member)
{
	ILDocParameter *param;

	/* Print the type and member name */
	if((member->memberAttrs & IL_META_METHODDEF_STATIC) != 0)
	{
		fputs("static ", stream);
	}
	fputs(member->type->fullName, stream);
	if(member->memberType != ILDocMemberType_Constructor)
	{
		fputs("::", stream);
		fputs(member->name, stream);
	}

	/* Print the parameters */
	putc('(', stream);
	param = member->parameters;
	while(param != 0)
	{
		fputs(param->type, stream);
		param = param->next;
		if(param != 0)
		{
			fputs(", ", stream);
		}
	}
	putc(')', stream);
	if(!strcmp(member->name, "op_Implicit") ||
	   !strcmp(member->name, "op_Explicit"))
	{
		/* Report the return type too for conversion operators */
		fputs(" : ", stream);
		fputs(member->returnType, stream);
	}
}

/*
 * Print the name and category of a program item.
 */
static void PrintName(FILE *stream, ILDocType *type, ILDocMember *member)
{
	if(member)
	{
		switch(member->memberType)
		{
			case ILDocMemberType_Constructor:
			{
				PrintMethodName(stream, member);
				fputs(" constructor ", stream);
			}
			break;

			case ILDocMemberType_Method:
			{
				PrintMethodName(stream, member);
				fputs(" method ", stream);
			}
			break;

			case ILDocMemberType_Field:
			{
				fputs(type->fullName, stream);
				fputs("::", stream);
				fputs(member->name, stream);
				fputs(" field ", stream);
			}
			break;

			case ILDocMemberType_Property:
			{
				fputs(type->fullName, stream);
				fputs("::", stream);
				fputs(member->name, stream);
				fputs(" property ", stream);
			}
			break;

			case ILDocMemberType_Event:
			{
				fputs(type->fullName, stream);
				fputs("::", stream);
				fputs(member->name, stream);
				fputs(" event ", stream);
			}
			break;

			case ILDocMemberType_Unknown:
			{
				fputs(type->fullName, stream);
				fputs("::", stream);
				fputs(member->name, stream);
				fputs(" member ", stream);
			}
			break;
		}
	}
	else
	{
		fputs(type->fullName, stream);
		putc(' ', stream);
	}
}

/*
 * Print the parameter signature for a method from an IL image.
 */
static void PrintILSignature(FILE *stream, ILType *signature,
							 const char *memberName)
{
	unsigned numParams = signature->num;
	unsigned paramNum;
	ILType *paramType;
	putc('(', stream);
	for(paramNum = 1; paramNum <= numParams; ++paramNum)
	{
		paramType = ILTypeGetParam(signature, paramNum);
		PrintType(stream, paramType);
		if(paramNum != numParams)
		{
			fputs(", ", stream);
		}
	}
	putc(')', stream);
	if(!strcmp(memberName, "op_Implicit") ||
	   !strcmp(memberName, "op_Explicit"))
	{
		/* Report the return type too for conversion operators */
		fputs(" : ", stream);
		PrintType(stream, signature->un.method.retType);
	}
}

/*
 * Print the name and category of a type member in an IL image.
 * Returns zero if the member can be ignored (PInvoke, override, etc).
 */
static int PrintILName(FILE *stream, ILDocType *type, ILMember *member)
{
	switch(ILMemberGetKind(member))
	{
		case IL_META_MEMBERKIND_METHOD:
		{
			if(ILMethod_IsConstructor((ILMethod *)member))
			{
				fputs(type->fullName, stream);
				PrintILSignature(stream, ILMember_Signature(member),
								 ILMember_Name(member));
				fputs(" constructor ", stream);
			}
			else
			{
				fputs(type->fullName, stream);
				fputs("::", stream);
				fputs(ILMember_Name(member), stream);
				PrintILSignature(stream, ILMember_Signature(member),
								 ILMember_Name(member));
				fputs(" method ", stream);
			}
		}
		break;

		case IL_META_MEMBERKIND_FIELD:
		{
			fputs(type->fullName, stream);
			fputs("::", stream);
			fputs(ILMember_Name(member), stream);
			fputs(" field ", stream);
		}
		break;

		case IL_META_MEMBERKIND_PROPERTY:
		{
			fputs(type->fullName, stream);
			fputs("::", stream);
			fputs(ILMember_Name(member), stream);
			fputs(" property ", stream);
		}
		break;

		case IL_META_MEMBERKIND_EVENT:
		{
			fputs(type->fullName, stream);
			fputs("::", stream);
			fputs(ILMember_Name(member), stream);
			fputs(" event ", stream);
		}
		break;
		
		default: return 0;
	}
	return 1;
}

/*
 * Append two strings.
 */
static char *AppendString(char *str1, const char *str2)
{
	str1 = (char *)ILRealloc(str1, strlen(str1) + strlen(str2) + 1);
	if(!str1)
	{
		ILDocOutOfMemory(0);
	}
	strcat(str1, str2);
	return str1;
}

/*
 * Append an attribute value to a name.  Returns NULL
 * if the value is invalid.
 */
static char *AppendAttrValue(char *name, ILSerializeReader *reader, int type)
{
	ILInt32 intValue;
	ILUInt32 uintValue;
	ILInt64 longValue;
	ILUInt64 ulongValue;
	ILFloat floatValue;
	ILDouble doubleValue;
	const char *strValue;
	int strLen, len;
	char buffer[64];

	switch(type)
	{
		case IL_META_SERIALTYPE_BOOLEAN:
		{
			intValue = ILSerializeReaderGetInt32(reader, type);
			if(intValue)
			{
				strcpy(buffer, "true");
			}
			else
			{
				strcpy(buffer, "false");
			}
		}
		break;

		case IL_META_SERIALTYPE_I1:
		case IL_META_SERIALTYPE_U1:
		case IL_META_SERIALTYPE_I2:
		case IL_META_SERIALTYPE_U2:
		case IL_META_SERIALTYPE_CHAR:
		case IL_META_SERIALTYPE_I4:
		{
			intValue = ILSerializeReaderGetInt32(reader, type);
			sprintf(buffer, "%ld", (long)intValue);
		}
		break;

		case IL_META_SERIALTYPE_U4:
		{
			uintValue = ILSerializeReaderGetUInt32(reader, type);
			sprintf(buffer, "%lu", (unsigned long)uintValue);
		}
		break;

		case IL_META_SERIALTYPE_I8:
		{
			longValue = ILSerializeReaderGetInt64(reader);
			sprintf(buffer, "0x%08lX%08lX",
					(unsigned long)((longValue >> 32) & IL_MAX_UINT32),
					(unsigned long)(longValue & IL_MAX_UINT32));
		}
		break;

		case IL_META_SERIALTYPE_U8:
		{
			ulongValue = ILSerializeReaderGetUInt64(reader);
			sprintf(buffer, "0x%08lX%08lX",
					(unsigned long)((ulongValue >> 32) & IL_MAX_UINT32),
					(unsigned long)(ulongValue & IL_MAX_UINT32));
		}
		break;

		case IL_META_SERIALTYPE_R4:
		{
			floatValue = ILSerializeReaderGetFloat32(reader);
			sprintf(buffer, "%.30e", (double)floatValue);
		}
		break;

		case IL_META_SERIALTYPE_R8:
		{
			doubleValue = ILSerializeReaderGetFloat64(reader);
			sprintf(buffer, "%.30e", (double)doubleValue);
		}
		break;

		case IL_META_SERIALTYPE_STRING:
		{
			strLen = ILSerializeReaderGetString(reader, &strValue);
			if(strLen == -1)
			{
				ILFree(name);
				return 0;
			}
			len = strlen(name);
			name = (char *)ILRealloc(name, len + strLen + 3);
			if(!name)
			{
				ILDocOutOfMemory(0);
			}
			name[len++] = '"';
			ILMemCpy(name + len, strValue, strLen);
			name[len + strLen] = '"';
			name[len + strLen + 1] = '\0';
			return name;
		}
		/* Not reached */

		case IL_META_SERIALTYPE_TYPE:
		{
			strLen = ILSerializeReaderGetString(reader, &strValue);
			if(strLen == -1)
			{
				ILFree(name);
				return 0;
			}
			len = strlen(name);
			name = (char *)ILRealloc(name, len + strLen + 9);
			if(!name)
			{
				ILDocOutOfMemory(0);
			}
			strcpy(name + len, "typeof(");
			len += 7;
			ILMemCpy(name + len, strValue, strLen);
			name[len + strLen] = ')';
			name[len + strLen + 1] = '\0';
			return name;
		}
		/* Not reached */

		default:
		{
			if((type & IL_META_SERIALTYPE_ARRAYOF) != 0)
			{
				intValue = ILSerializeReaderGetArrayLen(reader);
				name = AppendString(name, "{");
				while(intValue > 0)
				{
					name = AppendAttrValue(name, reader,
									       type & ~IL_META_SERIALTYPE_ARRAYOF);
					if(!name)
					{
						return 0;
					}
					--intValue;
					if(intValue > 0)
					{
						name = AppendString(name, ", ");
					}
				}
				return AppendString(name, "}");
			}
			else
			{
				ILFree(name);
				return 0;
			}
		}
		/* Not reached */
	}

	/* Append the buffer to the name and return */
	return AppendString(name, buffer);
}

/*
 * Convert a program item attribute into a string-form name.
 * Returns NULL if the attribute is private or invalid.
 */
static char *AttributeToName(ILAttribute *attr)
{
	ILMethod *method;
	char *name;
	const void *value;
	unsigned long len;
	ILSerializeReader *reader;
	ILUInt32 numParams;
	ILUInt32 numExtras;
	int needComma;
	int type;
	ILMember *member;
	const char *memberName;
	int memberNameLen;

	/* Get the attribute constructor and validate it */
	method = ILProgramItemToMethod(ILAttributeTypeAsItem(attr));
	if(!method)
	{
		return 0;
	}
	if(ILClass_IsPrivate(ILMethod_Owner(method)))
	{
		return 0;
	}

	/* Get the attribute name */
	name = ILDupString(ILClass_Name(ILMethod_Owner(method)));
	if(!name)
	{
		ILDocOutOfMemory(0);
	}

	/* Get the attribute value and prepare to parse it */
	value = ILAttributeGetValue(attr, &len);
	if(!value)
	{
		return 0;
	}
	reader = ILSerializeReaderInit(method, value, len);
	if(!reader)
	{
		ILDocOutOfMemory(0);
	}

	/* Get the attribute arguments */
	numParams = ILMethod_Signature(method)->num;
	needComma = 0;
	while(numParams > 0)
	{
		if(!needComma)
		{
			name = AppendString(name, "(");
			needComma = 1;
		}
		else
		{
			name = AppendString(name, ", ");
		}
		type = ILSerializeReaderGetParamType(reader);
		if(type == -1)
		{
			ILFree(name);
			return 0;
		}
		name = AppendAttrValue(name, reader, type);
		if(!name)
		{
			return 0;
		}
		--numParams;
	}

	/* Get the extra field and property values */
	numExtras = ILSerializeReaderGetNumExtra(reader);
	while(numExtras > 0)
	{
		if(!needComma)
		{
			name = AppendString(name, "(");
			needComma = 1;
		}
		else
		{
			name = AppendString(name, ", ");
		}
		type = ILSerializeReaderGetExtra(reader, &member, &memberName,
										 &memberNameLen);
		if(type == -1)
		{
			ILFree(name);
			return 0;
		}
		name = (char *)ILRealloc(name, strlen(name) + memberNameLen + 2);
		if(!name)
		{
			ILDocOutOfMemory(0);
		}
		ILMemCpy(name + strlen(name), memberName, memberNameLen);
		strcpy(name + strlen(name) + memberNameLen, "=");
		name = AppendAttrValue(name, reader, type);
		if(!name)
		{
			return 0;
		}
		--numExtras;
	}

	/* Add the closing parenthesis if necessary */
	if(needComma)
	{
		name = AppendString(name, ")");
	}

	/* Cleanup */
	ILSerializeReaderDestroy(reader);

	/* Return the name to the caller */
	return name;
}

/*
 * Validate the attributes on a program item.
 * Returns zero if a validation error occurred.
 */
static int ValidateAttributes(FILE *stream, ILDocType *type,
							  ILDocMember *member, ILDocAttribute *attrs,
							  ILProgramItem *item)
{
	int valid = 1;
	ILDocAttribute *docAttr;
	ILAttribute *itemAttr;
	char *name;

	/* Scan through the documentation's attributes */
	docAttr = attrs;
	while(docAttr != 0)
	{
		itemAttr = 0;
		while((itemAttr = ILProgramItemNextAttribute(item, itemAttr)) != 0)
		{
			name = AttributeToName(itemAttr);
			if(name)
			{
				if(!strcmp(name, docAttr->name))
				{
					ILFree(name);
					break;
				}
				ILFree(name);
			}
		}
		if(!itemAttr)
		{
			PrintName(stream, type, member);
			fprintf(stream, "should have custom attribute %s\n", docAttr->name);
			valid = 0;
		}
		docAttr = docAttr->next;
	}

	/* Scan through the actual attributes */
	itemAttr = 0;
	while((itemAttr = ILProgramItemNextAttribute(item, itemAttr)) != 0)
	{
		name = AttributeToName(itemAttr);
		if(!name)
		{
			continue;
		}
		docAttr = attrs;
		while(docAttr != 0)
		{
			if(!strcmp(name, docAttr->name))
			{
				break;
			}
			docAttr = docAttr->next;
		}
		if(!docAttr)
		{
			PrintName(stream, type, member);
			fprintf(stream, "has extra custom attribute %s\n", name);
			valid = 0;
		}
		ILFree(name);
	}

	/* Done */
	return valid;
}

/*
 * Validate a constructor member against an IL image.
 * Returns zero if a validation error occurred.
 */
static int ValidateConstructor(FILE *stream, ILImage *image,
							   ILClass *classInfo, ILDocType *type,
							   ILDocMember *member)
{
	int valid = 1;
	ILMethod *method;

	/* Locate the constructor method in the class */
	method = 0;
	while((method = (ILMethod *)ILClassNextMemberByKind
				(classInfo, (ILMember *)method,
				 IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if(ILMethod_IsConstructor(method) &&
		   MatchSignature(ILMethod_Signature(method), member))
		{
			break;
		}
	}
	if(!method)
	{
		PrintName(stream, type, member);
		fputs("is missing\n", stream);
		return 0;
	}

	/* Mark the constructor as seen so we don't pick
	   it up during the "extras" phase */
	ILMemberSetAttrs((ILMember *)method, 0x8000, 0x8000);

	/* Validate the constructor attributes */
	if((ILMethod_Attrs(method) & VALID_CTOR_FLAGS) !=
			(member->memberAttrs & VALID_CTOR_FLAGS))
	{
		PrintName(stream, type, member);
		fputs("should have attributes `", stream);
		ILDumpFlags(stream, (member->memberAttrs & VALID_CTOR_FLAGS),
				    ILMethodDefinitionFlags, 0);
		fputs("', but has `", stream);
		ILDumpFlags(stream, (ILMethod_Attrs(method) & VALID_CTOR_FLAGS),
				    ILMethodDefinitionFlags, 0);
		fputs("' instead\n", stream);
		valid = 0;
	}

	/* Validate the constructor attributes */
	if(!ValidateAttributes(stream, type, member, member->attributes,
						   ILToProgramItem(method)))
	{
		valid = 0;
	}

	/* Return the validation result to the caller */
	return valid;
}

/*
 * Validate a method member against an IL image.
 * Returns zero if a validation error occurred.
 */
static int ValidateMethod(FILE *stream, ILImage *image,
						  ILClass *classInfo, ILDocType *type,
					      ILDocMember *member)
{
	int valid = 1;
	ILMethod *method;

	/* Locate the method in the class */
	method = 0;
	while((method = (ILMethod *)ILClassNextMemberByKind
				(classInfo, (ILMember *)method,
				 IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if(!strcmp(ILMethod_Name(method), member->name) &&
		   MatchSignature(ILMethod_Signature(method), member))
		{
			/* Check the static vs instance state */
			if(ILMethod_IsStatic(method))
			{
				if((member->memberAttrs & IL_META_METHODDEF_STATIC) != 0)
				{
					break;
				}
			}
			else
			{
				if((member->memberAttrs & IL_META_METHODDEF_STATIC) == 0)
				{
					break;
				}
			}
		}
	}
	if(!method)
	{
		PrintName(stream, type, member);
		fputs("is missing\n", stream);
		return 0;
	}

	/* Mark the method as seen so we don't pick
	   it up during the "extras" phase */
	ILMemberSetAttrs((ILMember *)method, 0x8000, 0x8000);

	/* Validate the method attributes */
	if((ILMethod_Attrs(method) & VALID_METHOD_FLAGS) !=
			(member->memberAttrs & VALID_METHOD_FLAGS))
	{
		PrintName(stream, type, member);
		fputs("should have attributes `", stream);
		ILDumpFlags(stream, (member->memberAttrs & VALID_METHOD_FLAGS),
				    ILMethodDefinitionFlags, 0);
		fputs("', but has `", stream);
		ILDumpFlags(stream, (ILMethod_Attrs(method) & VALID_METHOD_FLAGS),
				    ILMethodDefinitionFlags, 0);
		fputs("' instead\n", stream);
		valid = 0;
	}

	/* Match the return type */
	if(!MatchType(ILMethod_Signature(method)->un.method.retType,
				  member->returnType))
	{
		PrintName(stream, type, member);
		fprintf(stream, "should have return type `%s', but has `",
				member->returnType);
		PrintType(stream, ILMethod_Signature(method)->un.method.retType);
		fputs("' instead\n", stream);
		valid = 0;
	}

	/* Validate the method attributes */
	if(!ValidateAttributes(stream, type, member, member->attributes,
						   ILToProgramItem(method)))
	{
		valid = 0;
	}

	/* Return the validation result to the caller */
	return valid;
}

/*
 * Validate a field member against an IL image.
 * Returns zero if a validation error occurred.
 */
static int ValidateField(FILE *stream, ILImage *image,
						 ILClass *classInfo, ILDocType *type,
					     ILDocMember *member)
{
	int valid = 1;
	ILField *field;

	/* Locate the field in the class */
	field = 0;
	while((field = (ILField *)ILClassNextMemberByKind
				(classInfo, (ILMember *)field,
				 IL_META_MEMBERKIND_FIELD)) != 0)
	{
		if(!strcmp(ILField_Name(field), member->name))
		{
			break;
		}
	}
	if(!field)
	{
		PrintName(stream, type, member);
		fputs("is missing\n", stream);
		return 0;
	}

	/* Mark the field as seen so we don't pick
	   it up during the "extras" phase */
	ILMemberSetAttrs((ILMember *)field, 0x8000, 0x8000);

	/* Validate the field attributes */
	if((ILField_Attrs(field) & VALID_FIELD_FLAGS) !=
			(member->memberAttrs & VALID_FIELD_FLAGS))
	{
		PrintName(stream, type, member);
		fputs("should have attributes `", stream);
		ILDumpFlags(stream, (member->memberAttrs & VALID_FIELD_FLAGS),
				    ILFieldDefinitionFlags, 0);
		fputs("', but has `", stream);
		ILDumpFlags(stream, (ILField_Attrs(field) & VALID_FIELD_FLAGS),
				    ILFieldDefinitionFlags, 0);
		fputs("' instead\n", stream);
		valid = 0;
	}

	/* Match the field type */
	if(!MatchType(ILField_Type(field), member->returnType))
	{
		fprintf(stream, "%s::%s field should have type `%s', but has `",
				type->fullName, member->name, member->returnType);
		PrintType(stream, ILField_Type(field));
		fputs("' instead\n", stream);
		valid = 0;
	}

	/* Validate the field attributes */
	if(!ValidateAttributes(stream, type, member, member->attributes,
						   ILToProgramItem(field)))
	{
		valid = 0;
	}

	/* Return the validation result to the caller */
	return valid;
}

/*
 * Validate a property member against an IL image.
 * Returns zero if a validation error occurred.
 */
static int ValidateProperty(FILE *stream, ILImage *image,
						    ILClass *classInfo, ILDocType *type,
					        ILDocMember *member)
{
	int valid = 1;
	ILProperty *property;
	ILMethod *accessor;
	int isSet;
	ILType *propertyType;

	/* Locate the property in the class */
	property = 0;
	accessor = 0;
	propertyType = 0;
	while((property = (ILProperty *)ILClassNextMemberByKind
				(classInfo, (ILMember *)property,
				 IL_META_MEMBERKIND_PROPERTY)) != 0)
	{
		/* Check the name */
		if(strcmp(ILProperty_Name(property), member->name) != 0)
		{
			continue;
		}

		/* Find the get or set accessor for the property */
		accessor = ILPropertyGetGetter(property);
		if(accessor)
		{
			isSet = 0;
			propertyType = ILMethod_Signature(accessor)->un.method.retType;
		}
		else
		{
			accessor = ILPropertyGetSetter(property);
			if(!accessor)
			{
				PrintName(stream, type, member);
				fputs(" does not have a get or set accessor\n", stream);
				return 0;
			}
			isSet = 1;
			propertyType = ILMethod_Signature(accessor);
			propertyType = ILTypeGetParam(propertyType, propertyType->num);
		}

		/* Match the property signature */
		if(MatchPropertySignature(ILMethod_Signature(accessor), member, isSet))
		{
			break;
		}
	}
	if(!property)
	{
		PrintName(stream, type, member);
		fputs("is missing\n", stream);
		return 0;
	}

	/* Mark the property as seen so we don't pick
	   it up during the "extras" phase */
	ILMemberSetAttrs((ILMember *)property, 0x8000, 0x8000);

	/* Validate the property attributes */
	if((ILMethod_Attrs(accessor) & VALID_METHOD_FLAGS) !=
			(member->memberAttrs & VALID_METHOD_FLAGS))
	{
		PrintName(stream, type, member);
		fputs("should have attributes `", stream);
		ILDumpFlags(stream, (member->memberAttrs & VALID_METHOD_FLAGS),
				    ILMethodDefinitionFlags, 0);
		fputs("', but has `", stream);
		ILDumpFlags(stream, (ILMethod_Attrs(accessor) & VALID_METHOD_FLAGS),
				    ILMethodDefinitionFlags, 0);
		fputs("' instead\n", stream);
		valid = 0;
	}

	/* Match the property type */
	if(!MatchType(propertyType, member->returnType))
	{
		PrintName(stream, type, member);
		fprintf(stream, "should have type `%s', but has `",
				member->returnType);
		PrintType(stream, propertyType);
		fputs("' instead\n", stream);
		valid = 0;
	}

	/* Validate the property attributes */
	if(!ValidateAttributes(stream, type, member, member->attributes,
						   ILToProgramItem(property)))
	{
		valid = 0;
	}

	/* Return the validation result to the caller */
	return valid;
}

/*
 * Validate an event member against an IL image.
 * Returns zero if a validation error occurred.
 */
static int ValidateEvent(FILE *stream, ILImage *image,
					     ILClass *classInfo, ILDocType *type,
				         ILDocMember *member)
{
	int valid = 1;
	ILEvent *event;
	ILMethod *accessor;

	/* Locate the event in the class */
	event = 0;
	while((event = (ILEvent *)ILClassNextMemberByKind
				(classInfo, (ILMember *)event,
				 IL_META_MEMBERKIND_EVENT)) != 0)
	{
		if(!strcmp(ILEvent_Name(event), member->name))
		{
			break;
		}
	}
	if(!event)
	{
		PrintName(stream, type, member);
		fputs("is missing\n", stream);
		return 0;
	}

	/* Mark the event as seen so we don't pick
	   it up during the "extras" phase */
	ILMemberSetAttrs((ILMember *)event, 0x8000, 0x8000);

	/* Find an accessor to use to validate the attributes */
	accessor = ILEventGetAddOn(event);
	if(!accessor)
	{
		accessor = ILEventGetRemoveOn(event);
		if(!accessor)
		{
			accessor = ILEventGetFire(event);
		}
	}

	/* Validate the event attributes */
	if(accessor &&
	   (ILMethod_Attrs(accessor) & VALID_EVENT_FLAGS) !=
			(member->memberAttrs & VALID_EVENT_FLAGS))
	{
		PrintName(stream, type, member);
		fputs("should have attributes `", stream);
		ILDumpFlags(stream, (member->memberAttrs & VALID_EVENT_FLAGS),
				    ILMethodDefinitionFlags, 0);
		fputs("', but has `", stream);
		ILDumpFlags(stream, (ILMethod_Attrs(accessor) & VALID_EVENT_FLAGS),
				    ILMethodDefinitionFlags, 0);
		fputs("' instead\n", stream);
		valid = 0;
	}

#if 0
	/* Can't do this yet because ECMA doesn't specify the type in a tag */
	/* Match the event type */
	if(!MatchType(ILEvent_Type(event), member->returnType))
	{
		fprintf(stream, "%s::%s event should have type `%s', but has `",
				type->fullName, member->name, member->returnType);
		PrintType(stream, ILEvent_Type(event));
		fputs("' instead\n", stream);
		valid = 0;
	}
#endif

	/* Validate the event attributes */
	if(!ValidateAttributes(stream, type, member, member->attributes,
						   ILToProgramItem(event)))
	{
		valid = 0;
	}

	/* Return the validation result to the caller */
	return valid;
}

/*
 * Validate a documentation member against an IL image.
 * Returns zero if a validation error occurred.
 */
static int ValidateMember(FILE *stream, ILImage *image, ILClass *classInfo,
						  ILDocType *type, ILDocMember *member)
{
	int valid = 1;

	/* Determine how to validate the member */
	switch(member->memberType)
	{
		case ILDocMemberType_Constructor:
		{
			valid = ValidateConstructor(stream, image, classInfo,
										type, member);
		}
		break;

		case ILDocMemberType_Method:
		{
			valid = ValidateMethod(stream, image, classInfo,
								   type, member);
		}
		break;

		case ILDocMemberType_Field:
		{
			valid = ValidateField(stream, image, classInfo,
								  type, member);
		}
		break;

		case ILDocMemberType_Property:
		{
			valid = ValidateProperty(stream, image, classInfo,
								     type, member);
		}
		break;

		case ILDocMemberType_Event:
		{
			valid = ValidateEvent(stream, image, classInfo,
								  type, member);
		}
		break;

		case ILDocMemberType_Unknown:
		{
			fprintf(stream, "%s has an unknown type in the XML file\n",
					type->fullName);
			valid = 0;
		}
		break;
	}

	/* Done */
	return valid;
}

/*
 * Determine if an image member is visible outside the assembly.
 */
static int MemberIsVisible(ILMember *member)
{
	ILUInt32 attrs;
	ILMethod *method;

	if(ILMember_IsField(member))
	{
		attrs = ILMember_Attrs(member);
	}
	else if(ILMember_IsMethod(member))
	{
		if(!strncmp(ILMember_Name(member), "get_", 4) ||
		   !strncmp(ILMember_Name(member), "set_", 4) ||
		   !strncmp(ILMember_Name(member), "add_", 4) ||
		   !strncmp(ILMember_Name(member), "remove_", 7))
		{
			if(ILMethod_HasSpecialName((ILMethod *)member))
			{
				/* Ignore property and event accessors */
				return 0;
			}
		}
		attrs = ILMember_Attrs(member);
	}
	else if(ILMember_IsProperty(member))
	{
		method = ILPropertyGetGetter((ILProperty *)member);
		if(!method)
		{
			method = ILPropertyGetSetter((ILProperty *)member);
		}
		if(method)
		{
			attrs = ILMethod_Attrs(method);
		}
		else
		{
			attrs = 0;
		}
	}
	else if(ILMember_IsEvent(member))
	{
		method = ILEventGetAddOn((ILEvent *)member);
		if(!method)
		{
			method = ILEventGetRemoveOn((ILEvent *)member);
			if(!method)
			{
				method = ILEventGetFire((ILEvent *)member);
			}
		}
		if(method)
		{
			attrs = ILMethod_Attrs(method);
		}
		else
		{
			attrs = 0;
		}
	}
	else
	{
		return 0;
	}

	switch(attrs & IL_META_METHODDEF_MEMBER_ACCESS_MASK)
	{
		case IL_META_METHODDEF_PUBLIC:
		case IL_META_METHODDEF_FAMILY:
		case IL_META_METHODDEF_FAM_OR_ASSEM:
		{
			return 1;
		}
		/* Not reached */
	}
	return 0;
}

/*
 * Validate a documentation type against an IL image.
 * Returns zero if a validation error occurred.
 */
static int ValidateType(FILE *stream, ILImage *image, ILDocType *type)
{
	ILClass *classInfo;
	ILClass *tempClass;
	ILClass *parent;
	int valid = 1;
	char *fullName;
	ILDocInterface *interface;
	ILImplements *impl;
	int extrasOK = ILDocFlagSet("extra-members-ok");
	int implemented;
	ILDocMember *member;

	/* See if the type exists within the image */
	classInfo = ILClassLookup(ILClassGlobalScope(image),
							  type->name, type->namespace->name);
	if(!classInfo)
	{
		PrintName(stream, type, 0);
		fputs("is missing\n", stream);
		return 0;
	}

	/* Validate the type kind */
	switch(type->kind)
	{
		case ILDocTypeKind_Class:
		{
			if(ILClass_IsInterface(classInfo))
			{
				fprintf(stream, "%s is an interface, but should be a class\n",
						type->fullName);
				valid = 0;
			}
			else if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) !=
						ILType_FromValueType(classInfo))
				{
					fprintf(stream, "%s is an enum, but should be a class\n",
							type->fullName);
					valid = 0;
				}
				else
				{
					fprintf(stream, "%s is a struct, but should be a class\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(IsDelegateType(classInfo))
			{
				fprintf(stream, "%s is a delegate, but should be a class\n",
						type->fullName);
				valid = 0;
			}
		}
		break;

		case ILDocTypeKind_Interface:
		{
			if(ILClass_IsInterface(classInfo))
			{
				break;
			}
			if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) !=
						ILType_FromValueType(classInfo))
				{
					fprintf(stream, "%s is an enum, but should "
									"be an interface\n",
							type->fullName);
					valid = 0;
				}
				else
				{
					fprintf(stream, "%s is a struct, but should "
									"be an interface\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(IsDelegateType(classInfo))
			{
				fprintf(stream, "%s is a delegate, but should "
								"be an interface\n",
						type->fullName);
				valid = 0;
			}
			else
			{
				fprintf(stream, "%s is a class, but should "
								"be an interface\n",
						type->fullName);
				valid = 0;
			}
		}
		break;

		case ILDocTypeKind_Struct:
		{
			if(ILClass_IsInterface(classInfo))
			{
				fprintf(stream, "%s is an interface, but should be a struct\n",
						type->fullName);
				valid = 0;
			}
			else if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) !=
						ILType_FromValueType(classInfo))
				{
					fprintf(stream, "%s is an enum, but should be a struct\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(IsDelegateType(classInfo))
			{
				fprintf(stream, "%s is a delegate, but should be a struct\n",
						type->fullName);
				valid = 0;
			}
			else
			{
				fprintf(stream, "%s is a class, but should be a struct\n",
						type->fullName);
				valid = 0;
			}
		}
		break;

		case ILDocTypeKind_Enum:
		{
			if(ILClass_IsInterface(classInfo))
			{
				fprintf(stream, "%s is an interface, but should be an enum\n",
						type->fullName);
				valid = 0;
			}
			else if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) ==
						ILType_FromValueType(classInfo))
				{
					fprintf(stream, "%s is a struct, but should be an enum\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(IsDelegateType(classInfo))
			{
				fprintf(stream, "%s is a delegate, but should be an enum\n",
						type->fullName);
				valid = 0;
			}
			else
			{
				fprintf(stream, "%s is a class, but should be an enum\n",
						type->fullName);
				valid = 0;
			}
		}
		break;

		case ILDocTypeKind_Delegate:
		{
			if(ILClass_IsInterface(classInfo))
			{
				fprintf(stream, "%s is an interface, but should "
								"be a delegate\n",
						type->fullName);
				valid = 0;
			}
			else if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) !=
						ILType_FromValueType(classInfo))
				{
					fprintf(stream, "%s is an enum, but should be a delegate\n",
							type->fullName);
					valid = 0;
				}
				else
				{
					fprintf(stream, "%s is a struct, but should "
									"be a delegate\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(!IsDelegateType(classInfo))
			{
				fprintf(stream, "%s is a class, but should be a delegate\n",
						type->fullName);
				valid = 0;
			}
		}
		break;
	}

	/* Validate the type attributes */
	if(!ValidateAttributes(stream, type, 0, type->attributes,
						   ILToProgramItem(classInfo)))
	{
		valid = 0;
	}

	/* Validate the type flags */
	if(type->typeAttrs != ILDocInvalidAttrs &&
	   (ILClass_Attrs(classInfo) & VALID_TYPE_FLAGS) !=
	   		(type->typeAttrs & VALID_TYPE_FLAGS))
	{
		PrintName(stream, type, 0);
		fputs("should have attributes `", stream);
		ILDumpFlags(stream, (type->typeAttrs & VALID_TYPE_FLAGS),
				    ILTypeDefinitionFlags, 0);
		fputs("', but has `", stream);
		ILDumpFlags(stream, (ILClass_Attrs(classInfo) & VALID_TYPE_FLAGS),
				    ILTypeDefinitionFlags, 0);
		fputs("' instead\n", stream);
		valid = 0;
	}

	/* Validate the base type */
	parent = ILClass_ParentRef(classInfo);
	if(!parent)
	{
		if(type->baseType)
		{
			fprintf(stream, "%s should have base type %s, but has no base\n",
					type->fullName, type->baseType);
			valid = 0;
		}
	}
	else
	{
		fullName = GetFullClassName(parent);
		if(!(type->baseType))
		{
			fprintf(stream, "%s should have no base type, but has base %s\n",
					type->fullName, fullName);
			valid = 0;
		}
		else if(strcmp(type->baseType, fullName) != 0)
		{
			/* There is a bug in the ECMA "All.xml" file that says
			   that delegates inherit from "Delegate", when other
			   ECMA specs say "MulticastDelegate".  The other specs
			   are the correct ones */
			if(strcmp(type->baseType, "System.Delegate") != 0 ||
			   strcmp(fullName, "System.MulticastDelegate") != 0)
			{
				/* Don't report an error if the base type is excluded */
				if(!(type->excludedBaseType) ||
				   strcmp(type->excludedBaseType, fullName) != 0)
				{
					fprintf(stream, "%s should have base type %s, "
									"but has base %s instead\n",
							type->fullName, type->baseType, fullName);
					valid = 0;
				}
			}
		}
		ILFree(fullName);
	}

	/* Validate the interfaces */
	interface = type->interfaces;
	while(interface != 0)
	{
		tempClass = classInfo;
		implemented = 0;
		while(tempClass != 0 && !implemented)
		{
			impl = 0;
			while((impl = ILClassNextImplements(tempClass, impl)) != 0)
			{
				parent = ILImplementsGetInterface(impl);
				fullName = GetFullClassName(parent);
				if(!strcmp(fullName, interface->name))
				{
					implemented = 1;
					ILFree(fullName);
					break;
				}
				ILFree(fullName);
			}
			tempClass = ILClass_ParentRef(tempClass);
		}
		if(!implemented)
		{
			fprintf(stream, "%s should implement %s, but does not\n",
					type->fullName, interface->name);
			valid = 0;
		}
		interface = interface->next;
	}
	if(!extrasOK)
	{
		impl = 0;
		fullName = 0;
		while((impl = ILClassNextImplements(classInfo, impl)) != 0)
		{
			parent = ILImplementsGetInterface(impl);
			if(ILClass_IsPrivate(parent))
			{
				/* It is OK to implement a private interface */
				continue;
			}
			fullName = GetFullClassName(parent);
			interface = type->interfaces;
			while(interface != 0)
			{
				if(!strcmp(fullName, interface->name))
				{
					break;
				}
				interface = interface->next;
			}
			if(!interface)
			{
				break;
			}
			ILFree(fullName);
			fullName = 0;
		}
		if(fullName)
		{
			fprintf(stream, "%s implements %s, but should not\n",
					type->fullName, fullName);
			ILFree(fullName);
			valid = 0;
		}
	}

	/* Mark all members as not yet seen if "extra-members-ok" is not set */
	if(!extrasOK)
	{
		ILMember *classMember = 0;
		while((classMember = ILClassNextMember(classInfo, classMember)) != 0)
		{
			ILMemberSetAttrs(classMember, 0x8000, 0);
		}
	}

	/* Validate the type members */
	member = type->members;
	while(member != 0)
	{
		if(!ValidateMember(stream, image, classInfo, type, member))
		{
			valid = 0;
		}
		member = member->next;
	}

	/* Report on all members not yet seen if "extra-members-ok" is not set */
	if(!extrasOK)
	{
		ILMember *classMember = 0;
		while((classMember = ILClassNextMember(classInfo, classMember)) != 0)
		{
			if((ILMember_Attrs(classMember) & 0x8000) == 0 &&
			   MemberIsVisible(classMember))
			{
				if(PrintILName(stream, type, classMember))
				{
					fputs("is not documented\n", stream);
					valid = 0;
				}
			}
		}
	}

	/* Done */
	return valid;
}

/*
 * Determine if we have an assembly match between a type
 * and an image.
 */
#define	AssemblyMatch(name1,name2)	\
			(!(name1) || !(name2) || !ILStrICmp((name1), (name2)))

/*
 * Scan an image to check for global public types that are defined
 * in the image, but not within the documentation.  Returns the number
 * of extra types that were found.
 */
static int CheckForExtraTypes(FILE *stream, ILImage *image,
							  ILDocTree *tree, const char *assemName,
							  const char *progname)
{
	ILClass *classInfo;
	int numExtras = 0;
	const char *name;
	const char *namespace;
	char *fullName;
	ILDocType *type;

	/* Scan the TypeDef table for global public types */
	classInfo = 0;
	while((classInfo = (ILClass *)ILImageNextToken
				(image, IL_META_TOKEN_TYPE_DEF, classInfo)) != 0)
	{
		if(ILClass_IsPublic(classInfo) && !ILClass_NestedParent(classInfo))
		{
			/* Build the full class name */
			name = ILClass_Name(classInfo);
			namespace = ILClass_Namespace(classInfo);
			if(namespace)
			{
				fullName = (char *)ILMalloc(strlen(namespace) +
											strlen(name) + 2);
				if(!fullName)
				{
					ILDocOutOfMemory(progname);
				}
				strcpy(fullName, namespace);
				strcat(fullName, ".");
				strcat(fullName, name);
			}
			else
			{
				fullName = (char *)ILMalloc(strlen(name) + 1);
				if(!fullName)
				{
					ILDocOutOfMemory(progname);
				}
				strcpy(fullName, name);
			}

			/* Look for a type in the documentation */
			type = ILDocTypeFind(tree, fullName);
			ILFree(fullName);
			if(!type)
			{
				/* This is an extra type */
				++numExtras;
				if(namespace)
				{
					fprintf(stream, "%s.%s is not documented\n",
							namespace, name);
				}
				else
				{
					fprintf(stream, "%s is not documented\n", name);
				}
			}
			else if(!AssemblyMatch(type->assembly, assemName))
			{
				fprintf(stream, "%s should be in the assembly %s, "
								"but was instead found in the assembly %s\n",
						type->fullName, type->assembly, assemName);
				++numExtras;
			}
		}
	}

	/* Return the number of extra types to the caller */
	return numExtras;
}

int ILDocConvert(ILDocTree *tree, int numInputs, char **inputs,
				 char *outputPath, const char *progname)
{
	const char *imageFilename;
	ILContext *context;
	ILImage *image;
	ILDocNamespace *namespace;
	ILDocType *type;
	FILE *stream;
	int closeStream;
	int numTypes;
	int numValidated;
	int numExtraTypes;
	const char *assemName;
	int assemNameLen;
	const char *mapName;

	/* Load the IL image to be validated */
	imageFilename = ILDocFlagValue("image");
	if(!imageFilename)
	{
		fprintf(stderr, "%s: `-fimage=PATH' must be specified\n", progname);
		return 0;
	}
	context = ILContextCreate();
	if(!context)
	{
		ILDocOutOfMemory(progname);
	}
	if(ILImageLoadFromFile(imageFilename, context, &image,
						   IL_LOADFLAG_FORCE_32BIT |
						   IL_LOADFLAG_NO_RESOLVE, 1) != 0)
	{
		return 0;
	}

	/* Open the output stream */
	if(!strcmp(outputPath, "-"))
	{
		stream = stdout;
		closeStream = 0;
	}
	else if((stream = fopen(outputPath, "w")) == NULL)
	{
		perror(outputPath);
		ILContextDestroy(context);
		return 0;
	}
	else
	{
		closeStream = 1;
	}

	/* Get the name of the image's assembly */
	assemName = ILImageGetAssemblyName(image);
	mapName = ILDocFlagValue("assembly-map");
	if(assemName && mapName)
	{
		assemNameLen = strlen(assemName);
		if(!strncmp(assemName, mapName, assemNameLen) &&
		   mapName[assemNameLen] == ',')
		{
			assemName = mapName + assemNameLen + 1;
		}
	}

	/* Process every type in the documentation tree */
	numTypes = 0;
	numValidated = 0;
	namespace = tree->namespaces;
	while(namespace != 0)
	{
		type = namespace->types;
		while(type != 0)
		{
			if(AssemblyMatch(type->assembly, assemName))
			{
				++numTypes;
				if(ValidateType(stream, image, type))
				{
					++numValidated;
				}
			}
			type = type->nextNamespace;
		}
		namespace = namespace->next;
	}

	/* Process public types that are in the image, but not the documentation */
	if(!ILDocFlagSet("extra-types-ok"))
	{
		numExtraTypes = CheckForExtraTypes(stream, image, tree,
										   assemName, progname);
	}
	else
	{
		numExtraTypes = 0;
	}

	/* Print a summary of how many types were validated */
	if(numExtraTypes != 0)
	{
		fprintf(stream, "\n%d types in document, %d were validated, "
						"%d undocumented types in image\n",
				numTypes, numValidated, numExtraTypes);
	}
	else
	{
		fprintf(stream, "\n%d types in document, %d were validated\n",
				numTypes, numValidated);
	}

	/* Clean up and exit */
	if(closeStream)
	{
		fclose(stream);
	}
	ILContextDestroy(context);
	return 1;
}

#ifdef	__cplusplus
};
#endif