/*
 * ildb_display.c - Processing for "display" and "print" commands.
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

#include "ildb_context.h"
#include "ildb_utils.h"
#include "ildb_cmd.h"
#include "il_system.h"
#include "../engine/engine_private.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Print an integer, in either decimal or hexadecimal.
 */
static void PrintInt(ILInt32 value, int hexFlag)
{
	if(hexFlag)
	{
		printf("0x%lx", (long)value);
	}
	else
	{
		printf("%ld", (long)value);
	}
}

/*
 * Print a character value.
 */
static void PrintChar(ILInt32 value, int hexFlag)
{
	switch(value)
	{
		case 0x00:
			fputs((hexFlag ? "0x0000 '\\0'" : "0 '\\0'"), stdout); break;
		case 0x07:
			fputs((hexFlag ? "0x0007 '\\a'" : "7 '\\a'"), stdout); break;
		case 0x08:
			fputs((hexFlag ? "0x0008 '\\b'" : "8 '\\b'"), stdout); break;
		case 0x09:
			fputs((hexFlag ? "0x0009 '\\t'" : "9 '\\t'"), stdout); break;
		case 0x0A:
			fputs((hexFlag ? "0x000a '\\n'" : "10 '\\n'"), stdout); break;
		case 0x0B:
			fputs((hexFlag ? "0x000b '\\v'" : "11 '\\v'"), stdout); break;
		case 0x0C:
			fputs((hexFlag ? "0x000c '\\f'" : "12 '\\f'"), stdout); break;
		case 0x0D:
			fputs((hexFlag ? "0x000d '\\r'" : "13 '\\r'"), stdout); break;
		case 0x27:
			fputs((hexFlag ? "0x0027 '\\''" : "39 '\\''"), stdout); break;

		default:
		{
			if(value >= ' ' && value < 0x7F)
			{
				if(hexFlag)
				{
					printf("0x%04lx '%c'", (long)value, (int)value);
				}
				else
				{
					printf("%ld '%c'", (long)value, (int)value);
				}
			}
			else if(value < 0x100)
			{
				if(hexFlag)
				{
					printf("0x%04lx '\\x%02x'", (long)value, (int)value);
				}
				else
				{
					printf("%ld '\\x%02x'", (long)value, (int)value);
				}
			}
			else
			{
				if(hexFlag)
				{
					printf("0x%04lx '\\u%04lx'", (long)value, (long)value);
				}
				else
				{
					printf("%ld '\\u%04lx'", (long)value, (long)value);
				}
			}
		}
		break;
	}
}

/*
 * Print an unsigned int64 value in decimal.
 */
static void PrintUInt64(ILUInt64 value)
{
	char buffer[64];
	int posn = 64;
	do
	{
		buffer[--posn] = (char)('0' + (value % 10));
		value /= 10;
	}
	while(value != 0);
	fwrite(buffer + posn, 1, 64 - posn, stdout);
}

/*
 * Print a pointer value.
 */
static void PrintPtr(void *ptr)
{
#ifdef IL_NATIVE_INT32
	ILUInt32 value = (ILUInt32)ptr;
	if(value)
	{
		printf("0x%lx", (unsigned long)value);
	}
	else
	{
		fputs("null", stdout);
	}
#else
	ILUInt64 value = (ILUInt64)ptr;
	if(!value)
	{
		fputs("null", stdout);
	}
	else if(value < (((ILUInt64)1) << 32))
	{
		printf("0x%lx", (unsigned long)value);
	}
	else
	{
		printf("0x%lx%08lx",
		       (unsigned long)((value >> 32) & 0xFFFFFFFF),
			   (unsigned long)(value & 0xFFFFFFFF));
	}
#endif
}

/*
 * Print the contents of a UTF-8 string.
 */
static void PrintString(const char *str)
{
	int len = strlen(str);
	int posn = 0;
	unsigned long ch;
	while(posn < len)
	{
		ch = ILUTF8ReadChar(str, len, &posn);
		switch(ch)
		{
			case 0x00:	fputs("\\0", stdout); break;
			case 0x07:	fputs("\\a", stdout); break;
			case 0x08:	fputs("\\b", stdout); break;
			case 0x09:	fputs("\\t", stdout); break;
			case 0x0A:	fputs("\\n", stdout); break;
			case 0x0B:	fputs("\\v", stdout); break;
			case 0x0C:	fputs("\\f", stdout); break;
			case 0x0D:	fputs("\\r", stdout); break;
			case 0x22:	fputs("\\\"", stdout); break;

			default:
			{
				if(ch >= ' ' && ch < 0x7F)
				{
					putc((int)ch, stdout);
				}
				else if(ch < 0x100)
				{
					printf("\\x%02x", (int)ch);
				}
				else if(ch < 0x10000)
				{
					printf("\\u%04lx", ch);
				}
				else
				{
					printf("\\U%08lx", ch);
				}
			}
			break;
		}
	}
}

/*
 * Print the fields within a class and its ancestors.
 */
static int PrintClassFields(ILExecThread *thread, ILClass *classInfo,
							void *ptr, int expand, int hexFlag,
							int sawPrev, int wantStatic)
{
	ILField *field;
	if(!classInfo)
	{
		return sawPrev;
	}
	if(!wantStatic)
	{
		sawPrev = PrintClassFields(thread, ILClass_Parent(classInfo),
								   ptr, expand, hexFlag, sawPrev, wantStatic);
	}
	field = 0;
	while((field = (ILField *)ILClassNextMemberByKind
				(classInfo, (ILMember *)field,
				 IL_META_MEMBERKIND_FIELD)) != 0)
	{
		/* Skip static or non-static fields that are not required */
		if(wantStatic)
		{
			if(!ILField_IsStatic(field) || ILField_IsLiteral(field))
			{
				continue;
			}
		}
		else
		{
			if(ILField_IsStatic(field))
			{
				continue;
			}
		}

		/* Print the field separator */
		if(sawPrev)
		{
			fputs(", ", stdout);
		}
		else
		{
			sawPrev = 1;
		}

		/* Print the field name */
		fputs(ILField_Name(field), stdout);
		fputs(" = ", stdout);

		/* Print the field's value */
		ILDbPrintValue(thread, ((unsigned char *)ptr) + field->offset,
					   ILField_Type(field), expand, hexFlag);
	}
	return sawPrev;
}

/*
 * Print the contents of a class instance.
 */
static void PrintClassContents(ILExecThread *thread, ILClass *classInfo,
							   void *ptr, int hexFlag)
{
	/* Force the class to be laid out */
	ILSizeOfType(thread, ILType_FromValueType(classInfo));

	/* Print the contents of the class */
	putc('{', stdout);
	PrintClassFields(thread, ILClassResolve(classInfo), ptr, 0, hexFlag, 0, 0);
	putc('}', stdout);
}

/*
 * Print the contents of an array object.
 */
static void PrintArrayContents(ILExecThread *thread, void *array,
							   ILType *elemType, int hexFlag)
{
	ILInt32 length;
	ILUInt32 elemSize;

	/* Extract the length of the array */
	length = *((ILInt32 *)array);
	array = (void *)(((unsigned char *)array) + sizeof(ILInt32));

	/* Get the size of the element type */
	elemSize = ILSizeOfType(thread, elemType);

	/* Print the elements of the array */
	putc('{', stdout);
	while(length > 0)
	{
		ILDbPrintValue(thread, array, elemType, 0, hexFlag);
		array = (void *)(((unsigned char *)array) + elemSize);
		--length;
		if(length > 0)
		{
			fputs(", ", stdout);
		}
	}
	putc('}', stdout);
}

void ILDbPrintValue(ILExecThread *thread, void *ptr, ILType *type,
				    int expand, int hexFlag)
{
	if(ILType_IsPrimitive(type))
	{
		/* Print a primitive value */
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:		fputs("void", stdout); break;

			case IL_META_ELEMTYPE_BOOLEAN:
			{
				ILInt32 value = (ILInt32)(*((ILInt8 *)ptr));
				if(value == 0)
				{
					fputs("false", stdout);
				}
				else if(value == 1)
				{
					fputs("true", stdout);
				}
				else
				{
					fputs("invalid_bool(", stdout);
					PrintInt(value, hexFlag);
					putc(')', stdout);
				}
			}
			break;

			case IL_META_ELEMTYPE_I1:
			{
				PrintInt((ILInt32)(*((ILInt8 *)ptr)), hexFlag);
			}
			break;

			case IL_META_ELEMTYPE_U1:
			{
				PrintInt((ILInt32)(*((ILUInt8 *)ptr)), hexFlag);
			}
			break;

			case IL_META_ELEMTYPE_I2:
			{
				PrintInt((ILInt32)(*((ILInt16 *)ptr)), hexFlag);
			}
			break;

			case IL_META_ELEMTYPE_U2:
			{
				PrintInt((ILInt32)(*((ILUInt16 *)ptr)), hexFlag);
			}
			break;

			case IL_META_ELEMTYPE_CHAR:
			{
				PrintChar((ILInt32)(*((ILUInt16 *)ptr)), hexFlag);
			}
			break;

			case IL_META_ELEMTYPE_I4:
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_I:
		#endif
			{
				PrintInt((ILInt32)(*((ILInt32 *)ptr)), hexFlag);
			}
			break;

			case IL_META_ELEMTYPE_U4:
		#ifdef IL_NATIVE_INT32
			case IL_META_ELEMTYPE_U:
		#endif
			{
				ILUInt32 value = *((ILUInt32 *)ptr);
				if(hexFlag)
				{
					printf("0x%lx", (unsigned long)value);
				}
				else
				{
					printf("%lu", (unsigned long)value);
				}
			}
			break;

			case IL_META_ELEMTYPE_I8:
		#ifdef IL_NATIVE_INT64
			case IL_META_ELEMTYPE_I:
		#endif
			{
				ILInt64 value;
				ILMemCpy(&value, ptr, sizeof(ILInt64));
				if(hexFlag)
				{
					if(((ILUInt64)value) < (((ILUInt64)1) << 32))
					{
						printf("0x%lx", (unsigned long)value);
					}
					else
					{
						printf("0x%lx%08lx",
						       (unsigned long)((value >> 32) & 0xFFFFFFFF),
							   (unsigned long)(value & 0xFFFFFFFF));
					}
				}
				else if(value < 0)
				{
					putc('-', stdout);
					PrintUInt64((ILUInt64)(-value));
				}
				else
				{
					PrintUInt64((ILUInt64)value);
				}
			}
			break;

			case IL_META_ELEMTYPE_U8:
		#ifdef IL_NATIVE_INT64
			case IL_META_ELEMTYPE_U:
		#endif
			{
				ILUInt64 value;
				ILMemCpy(&value, ptr, sizeof(ILUInt64));
				if(hexFlag)
				{
					if(value < (((ILUInt64)1) << 32))
					{
						printf("0x%lx", (unsigned long)value);
					}
					else
					{
						printf("0x%lx%08lx",
						       (unsigned long)((value >> 32) & 0xFFFFFFFF),
							   (unsigned long)(value & 0xFFFFFFFF));
					}
				}
				else
				{
					PrintUInt64(value);
				}
			}
			break;

			case IL_META_ELEMTYPE_R4:
			{
				if(hexFlag)
				{
					PrintInt(*((ILInt32 *)ptr), 1);
				}
				else
				{
					ILFloat value;
					ILMemCpy(&value, ptr, sizeof(ILFloat));
					printf("%g", (double)value);
				}
			}
			break;

			case IL_META_ELEMTYPE_R8:
			{
				if(hexFlag)
				{
					ILUInt64 value;
					ILMemCpy(&value, ptr, sizeof(ILUInt64));
					if(value < (((ILUInt64)1) << 32))
					{
						printf("0x%lx", (unsigned long)value);
					}
					else
					{
						printf("0x%lx%08lx",
						       (unsigned long)((value >> 32) & 0xFFFFFFFF),
							   (unsigned long)(value & 0xFFFFFFFF));
					}
				}
				else
				{
					ILDouble value;
					ILMemCpy(&value, ptr, sizeof(ILDouble));
					printf("%g", (double)value);
				}
			}
			break;

			case IL_META_ELEMTYPE_R:
			{
				if(hexFlag && sizeof(ILDouble) == sizeof(ILNativeFloat))
				{
					ILUInt64 value;
					ILMemCpy(&value, ptr, sizeof(ILUInt64));
					if(value < (((ILUInt64)1) << 32))
					{
						printf("0x%lx", (unsigned long)value);
					}
					else
					{
						printf("0x%lx%08lx",
						       (unsigned long)((value >> 32) & 0xFFFFFFFF),
							   (unsigned long)(value & 0xFFFFFFFF));
					}
				}
				else
				{
					ILNativeFloat value;
					ILMemCpy(&value, ptr, sizeof(ILNativeFloat));
					printf("%g", (double)value);
				}
			}
			break;

			case IL_META_ELEMTYPE_TYPEDBYREF:
			{
				ILTypedRef *ref = (ILTypedRef *)ptr;
				fputs("{type = ", stdout);
				PrintPtr(ref->type);
				fputs(", value = ", stdout);
				PrintPtr(ref->value);
				putc('}', stdout);
			}
			break;
		}
	}
	else if(ILType_IsValueType(type))
	{
		/* Print a value type instance */
		if(ILTypeIsEnum(type))
		{
			ILDbPrintValue(thread, ptr, ILTypeGetEnumType(type),
						   expand, hexFlag);
		}
		else
		{
			PrintClassContents(thread, ILType_ToValueType(type), ptr, hexFlag);
		}
	}
	else if(ILType_IsClass(type))
	{
		/* Print a class instance */
		ptr = *((void **)ptr);
		if(!ptr)
		{
			fputs("null", stdout);
		}
		else if(ILTypeIsStringClass(type))
		{
			char *str = ILStringToUTF8(thread, (ILString *)ptr);
			if(!str)
			{
				ILDbOutOfMemory(0);
			}
			PrintPtr(ptr);
			fputs(" \"", stdout);
			PrintString(str);
			putc('"', stdout);
		}
		else if(expand)
		{
			PrintClassContents(thread, ILType_ToClass(type), ptr, hexFlag);
		}
		else
		{
			PrintPtr(ptr);
		}
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		/* Print a complex type value */
		switch(ILType_Kind(type))
		{
			case IL_TYPE_COMPLEX_BYREF:
			case IL_TYPE_COMPLEX_PTR:
			{
				ptr = *((void **)ptr);
				if(!ptr)
				{
					fputs("null", stdout);
				}
				else if(expand)
				{
					ILDbPrintValue(thread, ptr, ILType_Ref(type), 0, hexFlag);
				}
				else
				{
					PrintPtr(ptr);
				}
			}
			break;

			case IL_TYPE_COMPLEX_CMOD_REQD:
			case IL_TYPE_COMPLEX_CMOD_OPT:
			case IL_TYPE_COMPLEX_PINNED:
			{
				ILDbPrintValue(thread, ptr, ILTypeStripPrefixes(type),
							   expand, hexFlag);
			}
			break;

			case IL_TYPE_COMPLEX_ARRAY:
			{
				void *ptr = *((void **)ptr);
				if(ptr && ILType_IsSimpleArray(type) && expand)
				{
					PrintArrayContents(ptr, ILTypeGetElemType(type),
									   0, hexFlag);
				}
				else
				{
					PrintPtr(ptr);
				}
			}
			break;

			default:
			{
				PrintPtr(*((void **)ptr));
			}
			break;
		}
	}
	fflush(stdout);
}

/*
 * Get the static data area for a class.
 */
static void *GetStaticArea(ILExecThread *thread, ILClass *classInfo)
{
	ILSizeOfType(thread, ILType_FromValueType(classInfo));
	if(((ILClassPrivate *)(classInfo->userData))->staticData == 0)
	{
		if((((ILClassPrivate *)(classInfo->userData))->staticData =
			_ILEngineAlloc(thread, 0,
			   ((ILClassPrivate *)(classInfo->userData))->staticSize)) == 0)
		{
			ILDbOutOfMemory(0);
		}
	}
	return ((ILClassPrivate *)(classInfo->userData))->staticData;
}

void ILDbPrintStatic(ILExecThread *thread, ILClass *classInfo,
					 int expand, int hexFlag)
{
	void *ptr;

	/* Get the static data area for the class */
	ptr = GetStaticArea(thread, classInfo);

	/* Print the contents of the class */
	putc('{', stdout);
	PrintClassFields(thread, ILClassResolve(classInfo), ptr,
					 expand, hexFlag, 0, 1);
	putc('}', stdout);
}

/*
 * Display the value of an expression every time we stop.
 */
static void Display(ILDb *db, char *args)
{
}

/*
 * Table of "display" and "print" commands.
 */
ILDbCmdInfo ILDbDisplayCommands[] = {
	{"display", 4, 0, 0, 0, Display,
		"print an expression value each time the program stops",
		0},
};
int ILDbNumDisplayCommands = (sizeof(ILDbDisplayCommands) /
							  sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
