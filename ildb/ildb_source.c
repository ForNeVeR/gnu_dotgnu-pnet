/*
 * ildb_source.c - Manipulate source files in memory.
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

#include "ildb_context.h"
#include "ildb_utils.h"
#include "ildb_search.h"
#include "ildb_source.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Print the name of a class in C# form.
 */
static void PrintClassName(ILDbSourceFile *file, ILClass *classInfo)
{
	ILClass *nestedParent;
	const char *namespace;
	classInfo = ILClassResolve(classInfo);
	nestedParent = ILClass_NestedParent(classInfo);
	if(nestedParent)
	{
		PrintClassName(file, nestedParent);
		ILDbSourcePrint(file, ".");
		ILDbSourcePrint(file, ILClass_Name(classInfo));
	}
	else if(ILTypeIsStringClass(ILType_FromClass(classInfo)))
	{
		ILDbSourcePrint(file, "string");
	}
	else if(ILTypeIsObjectClass(ILType_FromClass(classInfo)))
	{
		ILDbSourcePrint(file, "object");
	}
	else
	{
		namespace = ILClass_Namespace(classInfo);
		if(namespace && strcmp(namespace, "System") != 0)
		{
			ILDbSourcePrint(file, namespace);
			ILDbSourcePrint(file, ".");
		}
		ILDbSourcePrint(file, ILClass_Name(classInfo));
	}
}

/*
 * Print common field/method member attributes.
 */
static void PrintMemberAttrs(ILDbSourceFile *file, ILUInt32 attrs)
{
	const char *access;
	switch(attrs & IL_META_METHODDEF_MEMBER_ACCESS_MASK)
	{
		case IL_META_METHODDEF_COMPILER_CONTROLLED:
			access = "compiler_controlled "; break;
		case IL_META_METHODDEF_PRIVATE:
			access = "private "; break;
		case IL_META_METHODDEF_FAM_AND_ASSEM:
			access = "fam_and_assem "; break;
		case IL_META_METHODDEF_ASSEM:
			access = "internal "; break;
		case IL_META_METHODDEF_FAMILY:
			access = "protected "; break;
		case IL_META_METHODDEF_FAM_OR_ASSEM:
			access = "protected internal "; break;
		case IL_META_METHODDEF_PUBLIC:
			access = "public "; break;
		default:
			access = "unknown_access "; break;
	}
	ILDbSourcePrint(file, access);
}

/*
 * Print the attributes of a method.
 */
static void PrintMethodAttrs(ILDbSourceFile *file, ILUInt32 attrs)
{
	PrintMemberAttrs(file, attrs);
	if((attrs & IL_META_METHODDEF_FINAL) != 0)
	{
		ILDbSourcePrint(file, "final ");
	}
	if((attrs & IL_META_METHODDEF_STATIC) != 0)
	{
		ILDbSourcePrint(file, "static ");
	}
	else if((attrs & IL_META_METHODDEF_VIRTUAL) != 0)
	{
		if((attrs & IL_META_METHODDEF_VTABLE_LAYOUT_MASK)
					== IL_META_METHODDEF_NEW_SLOT)
		{
			if((attrs & IL_META_METHODDEF_ABSTRACT) != 0)
			{
				ILDbSourcePrint(file, "abstract ");
			}
			else
			{
				ILDbSourcePrint(file, "virtual ");
			}
		}
		else
		{
			ILDbSourcePrint(file, "override ");
		}
	}
}

/*
 * Forward declaration.
 */
static void PrintType(ILDbSourceFile *file, ILType *type);

/*
 * Print the name of a method type in C# form.
 */
static void PrintMethodType(ILDbSourceFile *file, ILType *signature,
							ILMethod *method, int wantAttrs)
{
	ILType *type;
	int needComma;
	unsigned long numParams;
	unsigned long param;
	ILParameter *paramInfo;
	const char *name;

	/* Print the method return type and name information */
	if(method && ILMethod_IsConstructor(method))
	{
		/* This is a class instance constructor */
		if(wantAttrs)
		{
			PrintMethodAttrs(file, ILMethod_Attrs(method));
		}
		ILDbSourcePrint(file, ILClass_Name(ILMethod_Owner(method)));
	}
	else if(method && ILMethod_IsStaticConstructor(method))
	{
		/* This is a class static constructor */
		if(wantAttrs)
		{
			ILDbSourcePrint(file, "static ");
		}
		ILDbSourcePrint(file, ILClass_Name(ILMethod_Owner(method)));
	}
	else if(method && !strcmp(ILMethod_Name(method), "Finalize") &&
	        ILTypeGetReturn(signature) == ILType_Void &&
			ILTypeNumParams(signature) == 0 &&
			ILMethod_IsFamily(method))
	{
		/* This is a class destructor */
		ILDbSourcePrint(file, "~");
		ILDbSourcePrint(file, ILClass_Name(ILMethod_Owner(method)));
	}
	else
	{
		/* Print the method attributes if requested */
		if(wantAttrs && method)
		{
			PrintMethodAttrs(file, ILMethod_Attrs(method));
		}

		/* Print the method's return type */
		PrintType(file, ILTypeGetReturn(signature));

		/* Print the method name */
		if(method)
		{
			ILDbSourcePrint(file, " ");
			ILDbSourcePrint(file, ILMethod_Name(method));
		}
		else
		{
			ILDbSourcePrint(file, " (*)");
		}
	}

	/* Print the method parameters */
	needComma = 0;
	ILDbSourcePrint(file, "(");
	numParams = ILTypeNumParams(signature);
	for(param = 1; param <= numParams; ++param)
	{
		if(needComma)
		{
			ILDbSourcePrint(file, ", ");
		}
		else
		{
			needComma = 1;
		}
		paramInfo = 0;
		if(method)
		{
			while((paramInfo = ILMethodNextParam(method, paramInfo)) != 0)
			{
				if(ILParameter_Num(paramInfo) == param)
				{
					break;
				}
			}
		}
		type = ILTypeGetParam(signature, param);
		if(type != 0 && ILType_IsComplex(type) &&
		   ILType_Kind(type) == IL_TYPE_COMPLEX_BYREF)
		{
			/* This is an "out" or "ref" parameter */
			if(paramInfo && ILParameter_IsOut(paramInfo))
			{
				ILDbSourcePrint(file, "out ");
			}
			else
			{
				ILDbSourcePrint(file, "ref ");
			}
			PrintType(file, ILType_Ref(type));
		}
		else
		{
			/* This is a regular parameter */
			PrintType(file, type);
		}
		if(paramInfo)
		{
			name = ILParameter_Name(paramInfo);
			if(name)
			{
				ILDbSourcePrint(file, " ");
				ILDbSourcePrint(file, name);
			}
		}
	}
	if((ILType_CallConv(signature) & IL_META_CALLCONV_MASK) ==
			IL_META_CALLCONV_VARARG)
	{
		/* This is a vararg method, which has an extra hidden parameter */
		if(needComma)
		{
			ILDbSourcePrint(file, ", __arglist");
		}
		else
		{
			ILDbSourcePrint(file, "__arglist");
		}
	}
	ILDbSourcePrint(file, ")");
}

/*
 * Print an array type in its C# form.
 */
static void PrintArrayType(ILDbSourceFile *file, ILType *type)
{
	ILType *elemType;
	int rank;

	/* Find and print the innermost element type */
	elemType = type;
	while(elemType != 0 && ILType_IsComplex(elemType))
	{
		if(ILType_Kind(elemType) != IL_TYPE_COMPLEX_ARRAY &&
		   ILType_Kind(elemType) != IL_TYPE_COMPLEX_ARRAY_CONTINUE)
		{
			break;
		}
		elemType = ILType_ElemType(elemType);
	}
	PrintType(file, elemType);

	/* Print the dimension specifiers from outermost to innermost */
	while(type != 0 && ILType_IsComplex(type))
	{
		if(ILType_Kind(type) != IL_TYPE_COMPLEX_ARRAY &&
		   ILType_Kind(type) != IL_TYPE_COMPLEX_ARRAY_CONTINUE)
		{
			break;
		}
		rank = ILTypeGetRank(type);
		ILDbSourcePrint(file, "[");
		while(rank > 1)
		{
			ILDbSourcePrint(file, ",");
			--rank;
		}
		ILDbSourcePrint(file, "]");
		type = ILTypeGetElemType(type);
	}
}

/*
 * Print the name of a type in C# form.
 */
static void PrintType(ILDbSourceFile *file, ILType *type)
{
	type = ILTypeStripPrefixes(type);
	if(ILType_IsPrimitive(type))
	{
		const char *name = 0;
		switch(ILType_ToElement(type))
		{
			case IL_META_ELEMTYPE_VOID:			name = "void"; break;
			case IL_META_ELEMTYPE_BOOLEAN:		name = "bool"; break;
			case IL_META_ELEMTYPE_I1:			name = "sbyte"; break;
			case IL_META_ELEMTYPE_U1:			name = "byte"; break;
			case IL_META_ELEMTYPE_I2:			name = "short"; break;
			case IL_META_ELEMTYPE_U2:			name = "ushort"; break;
			case IL_META_ELEMTYPE_CHAR:			name = "char"; break;
			case IL_META_ELEMTYPE_I4:			name = "int"; break;
			case IL_META_ELEMTYPE_U4:			name = "uint"; break;
			case IL_META_ELEMTYPE_I8:			name = "long"; break;
			case IL_META_ELEMTYPE_U8:			name = "ulong"; break;
			case IL_META_ELEMTYPE_I:			name = "IntPtr"; break;
			case IL_META_ELEMTYPE_U:			name = "UIntPtr"; break;
			case IL_META_ELEMTYPE_R4:			name = "float"; break;
			case IL_META_ELEMTYPE_R8:			name = "double"; break;
			case IL_META_ELEMTYPE_R:			name = "long double"; break;
			case IL_META_ELEMTYPE_TYPEDBYREF:	name = "TypedReference"; break;
		}
		if(name)
		{
			ILDbSourcePrint(file, name);
		}
		else
		{
			ILDbSourcePrint(file, "Unknown_Type");
		}
	}
	else if(ILType_IsValueType(type) || ILType_IsClass(type))
	{
		PrintClassName(file, ILType_ToClass(type));
	}
	else if(type != 0 && ILType_IsComplex(type))
	{
		switch(ILType_Kind(type))
		{
			case IL_TYPE_COMPLEX_BYREF:
			{
				PrintType(file, ILType_Ref(type));
				ILDbSourcePrint(file, " &");
			}
			break;

			case IL_TYPE_COMPLEX_PTR:
			{
				PrintType(file, ILType_Ref(type));
				ILDbSourcePrint(file, " *");
			}
			break;

			case IL_TYPE_COMPLEX_ARRAY:
			case IL_TYPE_COMPLEX_ARRAY_CONTINUE:
			{
				PrintArrayType(file, type);
			}
			break;

			case IL_TYPE_COMPLEX_METHOD:
			{
				PrintMethodType(file, type, 0, 0);
			}
			break;

			default:
			{
				ILDbSourcePrint(file, "Unknown_Type");
			}
			break;
		}
	}
	else
	{
		ILDbSourcePrint(file, "Unknown_Type");
	}
}

/*
 * Print information about a field in C# form.
 */
static void PrintField(ILDbSourceFile *file, int indent, ILField *field)
{
	ILDbSourceRegisterItem(file, ILToProgramItem(field), -1);
	ILDbSourcePrintIndent(file, indent, "");
	PrintMemberAttrs(file, ILField_Attrs(field));
	if(ILField_IsInitOnly(field))
	{
		if(ILField_IsStatic(field))
		{
			ILDbSourcePrint(file, "static readonly ");
		}
		else
		{
			ILDbSourcePrint(file, "readonly ");
		}
	}
	else if(ILField_IsLiteral(field))
	{
		ILDbSourcePrint(file, "const ");
	}
	else if(ILField_IsStatic(field))
	{
		ILDbSourcePrint(file, "static ");
	}
	PrintType(file, ILField_Type(field));
	ILDbSourcePrint(file, " ");
	ILDbSourcePrint(file, ILField_Name(field));
	ILDbSourcePrint(file, ";\n");
}

/*
 * Print information about a method in C# form.
 */
static void PrintMethod(ILDbSourceFile *file, int indent, ILMethod *method)
{
	ILDbSourceRegisterItem(file, ILToProgramItem(method), -1);
	ILDbSourcePrintIndent(file, indent, "");
	if(!ILMethod_RVA(method))
	{
		ILDbSourcePrint(file, "extern ");
	}
	PrintMethodType(file, ILMethod_Signature(method), method, 1);
	if(!ILMethod_RVA(method) || ILMethod_IsAbstract(method))
	{
		ILDbSourceRegisterItem(file, ILToProgramItem(method), 0);
		ILDbSourcePrint(file, ";\n");
	}
	else
	{
		ILDbSourcePrint(file, "\n");
		ILDbSourcePrintIndent(file, indent, "{\n");
		ILDbSourceRegisterItem(file, ILToProgramItem(method), 0);
		ILDbSourcePrintIndent(file, indent + 1, "...\n");
		ILDbSourceRegisterItem(file, ILToProgramItem(method), 0x7FFFFFFF);
		ILDbSourcePrintIndent(file, indent, "}\n");
	}
}

/*
 * Create a pseudo source file for a class that does not
 * have debug information associated with it.
 */
static void CreatePseudoSource(ILDbSourceFile *file,
							   ILClass *classInfo, int indent)
{
	const char *name;
	ILClass *parent;
	ILImplements *impl;
	ILNestedInfo *nested;
	int haveColon;
	int skipParent;
	int isGlobal;
	ILField *field;
	ILMethod *method;
	int needGap;

	/* Register the location of the class definition */
	ILDbSourceRegisterItem(file, ILToProgramItem(classInfo), -1);

	/* Print the class attribute and name information */
	skipParent = 1;
	isGlobal = 0;
	name = ILClass_Name(classInfo);
	if(strcmp(name, "<Module>") != 0)
	{
		if(ILClassIsValueType(classInfo))
		{
			if(ILTypeIsEnum(ILType_FromValueType(classInfo)))
			{
				ILDbSourcePrint(file, "enum ");
				ILDbSourcePrint(file, name);
			}
			else
			{
				ILDbSourcePrint(file, "struct ");
				ILDbSourcePrint(file, name);
			}
		}
		else if(ILClass_IsInterface(classInfo))
		{
			ILDbSourcePrint(file, "interface ");
			ILDbSourcePrint(file, name);
		}
		else
		{
			ILDbSourcePrint(file, "class ");
			ILDbSourcePrint(file, name);
			skipParent = 0;
		}
	}
	else
	{
		/* The "<Module>" class indicates global fields and methods,
		   which we display without a class header or footer */
		--indent;
		isGlobal = 1;
	}

	/* Print the name of the parent and interface classes */
	if(!isGlobal)
	{
		haveColon = 0;
		if(!skipParent)
		{
			parent = ILClass_Parent(classInfo);
			if(parent && !ILTypeIsObjectClass(ILType_FromClass(parent)))
			{
				ILDbSourcePrint(file, " : ");
				haveColon = 1;
				PrintClassName(file, parent);
			}
		}
		impl = 0;
		while((impl = ILClassNextImplements(classInfo, impl)) != 0)
		{
			if(haveColon)
			{
				ILDbSourcePrint(file, ", ");
			}
			else
			{
				ILDbSourcePrint(file, " : ");
				haveColon = 1;
			}
			PrintClassName(file, ILImplementsGetInterface(impl));
		}
		ILDbSourcePrint(file, "\n");
		ILDbSourcePrintIndent(file, indent, "{\n");
	}

	/* Print the nested classes */
	needGap = 0;
	nested = 0;
	while((nested = ILClassNextNested(classInfo, nested)) != 0)
	{
		CreatePseudoSource
			(file, ILClassResolve(ILNestedInfoGetChild(nested)), indent + 1);
		ILDbSourcePrint(file, "\n");
	}

	/* Print the fields */
	field = 0;
	while((field = (ILField *)ILClassNextMemberByKind
				(classInfo, (ILMember *)field,
				 IL_META_MEMBERKIND_FIELD)) != 0)
	{
		PrintField(file, indent + 1, field);
		needGap = 1;
	}

	/* Print the methods */
	method = 0;
	while((method = (ILMethod *)ILClassNextMemberByKind
				(classInfo, (ILMember *)method,
				 IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if(needGap)
		{
			ILDbSourcePrint(file, "\n");
		}
		PrintMethod(file, indent + 1, method);
		needGap = 1;
	}

	/* Print the class footer */
	if(!isGlobal)
	{
		ILDbSourcePrintIndent(file, indent, "}\n");
	}
}

/*
 * Get the name of a class as an ILMalloc'ed string.
 */
static int GetClassNameInner(ILDb *db, ILClass *classInfo, char *buf)
{
	ILClass *nestedParent;
	const char *temp;
	int len;

	classInfo = ILClassResolve(classInfo);
	nestedParent = ILClass_NestedParent(classInfo);
	if(nestedParent)
	{
		len = GetClassNameInner(db, nestedParent, buf);
		temp = ILClass_Name(classInfo);
		if(buf)
		{
			buf[len] = '.';
			strcpy(buf + len + 1, temp);
		}
		len += strlen(temp) + 1;
	}
	else
	{
		len = 0;
		temp = ILClass_Namespace(classInfo);
		if(temp)
		{
			if(buf)
			{
				strcpy(buf + len, temp);
			}
			len += strlen(temp);
			if(buf)
			{
				buf[len] = '.';
			}
			++len;
		}
		temp = ILClass_Name(classInfo);
		if(buf)
		{
			strcpy(buf + len, temp);
		}
		len += strlen(temp);
	}
	return len;
}
static char *GetClassName(ILDb *db, ILClass *classInfo)
{
	int len = GetClassNameInner(db, classInfo, 0);
	char *buf = (char *)ILMalloc(len + 1);
	if(!buf)
	{
		ILDbOutOfMemory(db);
	}
	GetClassNameInner(db, classInfo, buf);
	buf[len] = '\0';
	return buf;
}

/*
 * Create a new empty source file, or retrieve an existing one.
 * Returns non-zero if an existing file has been retrieved.
 */
static int SourceCreate(ILDb *db, const char *filename,
						ILClass *classInfo, ILDbSourceFile **fileReturn)
{
	ILDbSourceFile *file;

	/* Look for an existing source file */
	file = db->sourceFiles;
	while(file != 0)
	{
		if(filename)
		{
			if(file->filename && !strcmp(file->filename, filename))
			{
				*fileReturn = file;
				return 1;
			}
		}
		else if(classInfo)
		{
			if(file->classInfo == classInfo)
			{
				*fileReturn = file;
				return 1;
			}
		}
		file = file->next;
	}

	/* Create a new source file record */
	if((file = (ILDbSourceFile *)ILMalloc(sizeof(ILDbSourceFile))) == 0)
	{
		ILDbOutOfMemory(db);
	}
	file->owner = db;
	file->next = db->sourceFiles;
	db->sourceFiles = file;
	if(filename)
	{
		file->filename = ILDupString(filename);
		if(!(file->filename))
		{
			ILDbOutOfMemory(db);
		}
	}
	else
	{
		file->filename = 0;
	}
	file->className = (classInfo ? GetClassName(db, classInfo) : 0);
	file->classInfo = classInfo;
	file->text = 0;
	file->textLen = 0;
	file->textMax = 0;
	file->lines = 0;
	file->numLines = 0;
	file->maxLines = 0;
	file->prevIsEOL = 1;
	file->fileIsTemp = 0;
	*fileReturn = file;
	return 0;
}

ILDbSourceFile *ILDbSourceCreatePseudo(ILDb *db, ILClass *classInfo)
{
	ILDbSourceFile *file;
	ILClass *nestedParent;
	const char *namespace;

	classInfo = ILClassResolve(classInfo);
	nestedParent = ILClass_NestedParent(classInfo);
	if(nestedParent)
	{
		/* Start from the outer-most class if this one is nested */
		return ILDbSourceCreatePseudo(db, nestedParent);
	}
	else
	{
		/* Create the pseudo source file in memory */
		if(SourceCreate(db, 0, classInfo, &file))
		{
			return file;
		}

		/* Print the namespace header information */
		namespace = ILClass_Namespace(classInfo);
		if(namespace)
		{
			ILDbSourcePrint(file, "namespace ");
			ILDbSourcePrint(file, namespace);
			ILDbSourcePrint(file, "\n{\n\n");
		}

		/* Dump this class and all of its nested children */
		CreatePseudoSource(file, classInfo, 0);

		/* Print the namespace footer */
		if(namespace)
		{
			ILDbSourcePrint(file, "\n}\n");
		}

		/* Return the pseudo source file to the caller */
		return file;
	}
}

/*
 * Add line information from a buffer that was just added to a file.
 */
static void AddLines(ILDbSourceFile *file, const char *buf, unsigned long len)
{
	unsigned long offset = file->textLen - len;
	const char *temp;
	unsigned long *newLines;
	while(len > 0)
	{
		if(file->prevIsEOL)
		{
			/* Expand the line buffer and add the new line */
			if(file->numLines >= file->maxLines)
			{
				newLines = (unsigned long *)
					(ILRealloc(file->lines, sizeof(unsigned long) *
											(file->maxLines + 256)));
				if(!newLines)
				{
					ILDbOutOfMemory(file->owner);
				}
				file->lines = newLines;
				file->maxLines += 256;
			}
			file->lines[(file->numLines)++] = offset;
		}
		temp = (const char *)(ILMemChr(buf, '\n', len));
		if(!temp)
		{
			/* Check for '\r' just in case we have a Mac-format text file */
			temp = (const char *)(ILMemChr(buf, '\r', len));
		}
		if(temp)
		{
			/* Advance to the next line */
			++temp;
			offset += (unsigned long)(temp - buf);
			len -= (unsigned long)(temp - buf);
			file->prevIsEOL = 1;
		}
		else
		{
			/* Last line in the buffer does not end in LF or CR */
			len = 0;
			file->prevIsEOL = 0;
		}
	}
}

ILDbSourceFile *ILDbSourceCreate(ILDb *db, const char *filename)
{
	/* TODO */
	return 0;
}

const char *ILDbSourceDiskFile(ILDbSourceFile *file)
{
	char *name;
	FILE *stream;

	/* If we already have a filename, then return it */
	if(file->filename)
	{
		return file->filename;
	}

	/* Create a temporary file to hold the pseudo class details */
	name = (char *)ILMalloc(strlen(file->className) + 7);
	if(!name)
	{
		ILDbOutOfMemory(file->owner);
	}
	strcpy(name, ".ildb-");
	strcat(name, file->className);
	file->filename = ILExpandFilename(name, (char *)0);
	if(!(file->filename))
	{
		ILDbOutOfMemory(file->owner);
	}
	ILFree(name);

	/* Copy the pseudo class into the temporary file */
	if((stream = fopen(file->filename, "w")) == NULL)
	{
		ILFree(file->filename);
		file->filename = 0;
		return 0;
	}
	file->fileIsTemp = 1;
	if(file->textLen > 0)
	{
		fwrite(file->text, 1, file->textLen, stream);
	}
	fclose(stream);

	/* Return the filename to the caller */
	return file->filename;
}

const char *ILDbSourceDisplayName(ILDbSourceFile *file)
{
	if(file->filename)
	{
		/* Use the base part of the filename as the display name */
		const char *filename = file->filename;
		int len = strlen(filename);
		while(len > 0 && filename[len - 1] != '/' && filename[len - 1] != '\\')
		{
			--len;
		}
		return filename + len;
	}
	else
	{
		/* Use the name of the class as the display name */
		return file->className;
	}
}

void ILDbSourcePrint(ILDbSourceFile *file, const char *str)
{
	/* Bail out if the string is empty */
	unsigned long len = (unsigned long)(strlen(str));
	if(!len)
	{
		return;
	}

	/* Expand the text buffer and add the new string */
	if((file->textLen + len) > file->textMax)
	{
		char *newText;
		unsigned long newLen = file->textLen + len;
		newLen = (newLen + 1023) & ~1023;
		newText = (char *)ILRealloc(file->text, newLen);
		if(!newText)
		{
			ILDbOutOfMemory(file->owner);
		}
		file->text = newText;
		file->textMax = newLen;
	}
	ILMemCpy(file->text + file->textLen, str, len);
	file->textLen += len;

	/* Add the lines within the string to the lines array */
	AddLines(file, str, len);
}

void ILDbSourcePrintIndent(ILDbSourceFile *file, int indent, const char *str)
{
	while(indent > 0)
	{
		ILDbSourcePrint(file, "    ");
		--indent;
	}
	ILDbSourcePrint(file, str);
}

void ILDbSourceRegisterItem(ILDbSourceFile *file, ILProgramItem *item,
							ILInt32 offset)
{
	/* TODO */
}

/*
 * Get a source file from the line number debug information
 * that is associated with an image.
 */
static ILDbSourceFile *GetSourceFromDebug(ILDb *db, ILDebugContext *dbg,
										  ILToken token, ILClass *classInfo)
{
	const char *filename;
	ILUInt32 line, column;
	char *fullName;
	ILDbSourceFile *file;

	filename = ILDebugGetLineInfo(dbg, token, IL_MAX_UINT32, &line, &column);
	if(filename)
	{
		/* Search for the source file */
		fullName = ILDbSearchFind(db, filename);
		if(!fullName)
		{
			ILDbError(db, "No source file named %s.\n", filename);
			return 0;
		}

		/* Load the contents of the source file into memory */
		file = ILDbSourceCreate(db, fullName);
		ILFree(fullName);
		return file;
	}
	else
	{
		return 0;
	}
}

ILDbSourceFile *ILDbSourceGet(ILDb *db, ILClass *classInfo, ILMember *member)
{
	ILDebugContext *dbg;
	ILDebugIter iter;

	/* Resolve the member or class */
	if(member)
	{
		member = ILMemberResolve(member);
		classInfo = ILMember_Owner(member);
	}
	if(classInfo)
	{
		classInfo = ILClassResolve(classInfo);
	}

	/* Try to find a debug context for the member or class */
	if(member)
	{
		dbg = ILDbGetDebugContext(db, ILProgramItem_Image(member));
		if(!dbg)
		{
			dbg = ILDbGetDebugContext(db, ILProgramItem_Image(classInfo));
		}
	}
	else if(classInfo)
	{
		dbg = ILDbGetDebugContext(db, ILProgramItem_Image(classInfo));
	}
	else
	{
		dbg = 0;
	}

	/* Look for debug blocks on the member or class */
	if(dbg)
	{
		if(member)
		{
			ILDebugIterInit(&iter, dbg, ILMember_Token(member));
			if(ILDebugIterNext(&iter) &&
			   iter.type >= IL_DEBUGTYPE_LINE_COL &&
			   iter.type <= IL_DEBUGTYPE_LINE_COL_OFFSETS)
			{
				return GetSourceFromDebug
					(db, dbg, ILMember_Token(member), classInfo);
			}
		}
		if(classInfo)
		{
			ILDebugIterInit(&iter, dbg, ILClass_Token(classInfo));
			if(ILDebugIterNext(&iter) &&
			   iter.type >= IL_DEBUGTYPE_LINE_COL &&
			   iter.type <= IL_DEBUGTYPE_LINE_COL_OFFSETS)
			{
				return GetSourceFromDebug
					(db, dbg, ILClass_Token(classInfo), classInfo);
			}
		}
	}

	/* Create a pseudo source file for the class */
	if(classInfo)
	{
		return ILDbSourceCreatePseudo(db, classInfo);
	}
	else
	{
		return 0;
	}
}

void ILDbSourceDestroy(ILDbSourceFile *list)
{
	ILDbSourceFile *next;
	while(list != 0)
	{
		next = list->next;
		if(list->fileIsTemp)
		{
			ILDeleteFile(list->filename);
		}
		if(list->filename)
		{
			ILFree(list->filename);
		}
		if(list->className)
		{
			ILFree(list->className);
		}
		if(list->text)
		{
			ILFree(list->text);
		}
		list = next;
	}
}

long ILDbSourceGetLine(ILDbSourceFile *file, long num, char **line)
{
	long len;
	--num;
	if(num < 0 || num >= (long)(file->numLines))
	{
		return -1;
	}
	if(num == (long)(file->numLines - 1))
	{
		len = (long)(file->textLen - file->lines[num]);
	}
	else
	{
		len = (long)(file->lines[num + 1] - file->lines[num]);
	}
	*line = file->text + file->lines[num];
	while(len > 0 && ((*line)[len - 1] == '\n' || (*line)[len - 1] == '\r'))
	{
		--len;
	}
	return len;
}

#ifdef	__cplusplus
};
#endif
