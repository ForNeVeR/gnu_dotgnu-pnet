/*
 * doc_texi.c - Convert csdoc into texinfo.
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
#include "doc_tree.h"
#include "doc_backend.h"

#ifdef	__cplusplus
extern	"C" {
#endif

char const ILDocProgramHeader[] =
	"CSDOC2TEXI " VERSION " - Convert C# documentation into texinfo";

char const ILDocProgramName[] = "CSDOC2TEXI";

ILCmdLineOption const ILDocProgramOptions[] = {
	{"-fstandalone", 'f', 1,
		"-fstandalone",
		"Make a stand-alone texinfo document (default)"},
	{"-fembedded", 'f', 1,
		"-fembedded",
		"Make an embedded texinfo document"},
	{"-fparent", 'f', 1,
		"-fparent=NAME",
		"Specify the name of the parent node (default is `Top')"},
	{"-ffilename", 'f', 1,
		"-ffilename=NAME",
		"Specify the value for @setfilename"},
	{"-ftitle", 'f', 1,
		"-ftitle=TITLE",
		"Specify the title to use for the document"},
	{"-fdescription", 'f', 1,
		"-fdescription=DESC",
		"Specify the description to use for the document"},
	{"-fchapternewpage", 'f', 1,
		"-fchapternewpage=VALUE",
		"Specify the value for @setchapternewpage (default is `on')"},
	{"-fcopyright", 'f', 1,
		"-fcopyright=MSG",
		"Specify the Copyright message to use for the document"},
	{"-findex", 'f', 1,
		"-findex",
		"Include the index in stand-alone documents (default)"},
	{"-fno-index", 'f', 1,
		"-fno-index",
		"Do not include the index in stand-alone documents"},
	{"-fnewpage", 'f', 1,
		"-fnewpage",
		"Use a new page for each type member description (default)"},
	{"-fno-newpage", 'f', 1,
		"-fno-newpage",
		"Do not use a new page for each type member description"},
	{0, 0, 0, 0, 0}
};

char *ILDocDefaultOutput(int numInputs, char **inputs, const char *progname)
{
	if(!strcmp(inputs[0], "-"))
	{
		/* The first input is stdin, so the output is too */
		return "-";
	}
	else
	{
		/* Adjust the first input filename to end in ".texi" */
		int len = strlen(inputs[0]);
		char *result;
		while(len > 0 && inputs[0][len - 1] != '/' &&
			  inputs[0][len - 1] != '\\' &&
			  inputs[0][len - 1] != '.')
		{
			--len;
		}
		if(len > 0 && inputs[0][len - 1] == '.')
		{
			result = (char *)ILMalloc(len + 5);
			if(!result)
			{
				ILDocOutOfMemory(progname);
			}
			ILMemCpy(result, inputs[0], len);
			strcpy(result + len, "texi");
		}
		else
		{
			result = (char *)ILMalloc(strlen(inputs[0]) + 6);
			if(!result)
			{
				ILDocOutOfMemory(progname);
			}
			strcpy(result, inputs[0]);
			strcat(result, ".texi");
		}
		return result;
	}
}

int ILDocValidateOutput(char *outputPath, const char *progname)
{
	/* Nothing to do here: any pathname is considered valid */
	return 1;
}

/*
 * Print a literal string to a texinfo stream.
 */
static void PrintString(FILE *stream, const char *str)
{
	if(str)
	{
		while(*str != '\0')
		{
			if(*str == '@' || *str == '{' || *str == '}')
			{
				putc('@', stream);
			}
			putc(*str, stream);
			++str;
		}
	}
}

/*
 * Print a literal string to a texinfo stream within
 * a menu or node name.
 */
static void PrintMenuString(FILE *stream, const char *str)
{
	if(str)
	{
		while(*str != '\0')
		{
			if(*str == '@' || *str == '{' || *str == '}')
			{
				putc('@', stream);
			}
			else if(*str == ',')
			{
				/* Note: texinfo has problems with commas in node
				   names, so we replace them with semi-colon until
				   we can find a better solution */
				putc(';', stream);
			}
			putc(*str, stream);
			++str;
		}
	}
}

/*
 * Print the full name of a type, including its kind.
 */
static void PrintTypeName(FILE *stream, ILDocType *type)
{
	PrintString(stream, type->fullName);
	switch(type->kind)
	{
		case ILDocTypeKind_Class:		fputs(" Class", stream); break;
		case ILDocTypeKind_Interface:	fputs(" Interface", stream); break;
		case ILDocTypeKind_Struct:		fputs(" Structure", stream); break;
		case ILDocTypeKind_Enum:		fputs(" Enum", stream); break;
		case ILDocTypeKind_Delegate:	fputs(" Delegate", stream); break;
	}
}

/*
 * Print the short name of a type, including its kind.
 */
static void PrintShortTypeName(FILE *stream, ILDocType *type)
{
	if(type->fullyQualify)
	{
		/* The short name is ambiguous, so use the full name */
		PrintString(stream, type->fullName);
	}
	else
	{
		PrintString(stream, type->name);
	}
	switch(type->kind)
	{
		case ILDocTypeKind_Class:		fputs(" Class", stream); break;
		case ILDocTypeKind_Interface:	fputs(" Interface", stream); break;
		case ILDocTypeKind_Struct:		fputs(" Structure", stream); break;
		case ILDocTypeKind_Enum:		fputs(" Enum", stream); break;
		case ILDocTypeKind_Delegate:	fputs(" Delegate", stream); break;
	}
}

/*
 * Table of builtin types that look better in their
 * short form than in their long form.
 */
typedef struct
{
	const char *longName;
	int         longNameLen;
	const char *shortName;

} ILDocBuiltinType;
static ILDocBuiltinType const builtinTypes[] = {
	{"System.Boolean",		14,		"bool"},
	{"System.Char",			11,		"char"},
	{"System.SByte",		12,		"sbyte"},
	{"System.Byte",			11,		"byte"},
	{"System.Int16",		12,		"short"},
	{"System.UInt16",		13,		"ushort"},
	{"System.Int32",		12,		"int"},
	{"System.UInt32",		13,		"uint"},
	{"System.Int64",		12,		"long"},
	{"System.UInt64",		13,		"ulong"},
	{"System.Single",		13,		"float"},
	{"System.Double",		13,		"double"},
	{"System.Void",			11,		"void"},
};

/*
 * Print a parameter type name.
 */
static void PrintParamTypeName(FILE *stream, char *type)
{
	int posn, len;
	for(posn = 0; posn < (sizeof(builtinTypes) / sizeof(ILDocBuiltinType));
		++posn)
	{
		len = builtinTypes[posn].longNameLen;
		if(!strncmp(type, builtinTypes[posn].longName, len) &&
		   (type[len] == '\0' || type[len] == ' ' ||
		    type[len] == '*' || type[len] == '['))
		{
			fputs(builtinTypes[posn].shortName, stream);
			PrintMenuString(stream, type + len);
			return;
		}
	}
	PrintMenuString(stream, type);
}

/*
 * Print the full name of a member, including its kind.
 */
static void PrintMemberName(FILE *stream, ILDocMember *member)
{
	ILDocParameter *param;
	if(member->fullyQualify && member->name && member->returnType &&
	   (!strcmp(member->name, "op_Explicit") ||
	    !strcmp(member->name, "op_Implicit")))
	{
		/* Conversions need to include the return type in the signature */
		PrintParamTypeName(stream, member->returnType);
		putc(' ', stream);
	}
	if(member->type->fullyQualify)
	{
		PrintMenuString(stream, member->type->fullName);
	}
	else
	{
		PrintMenuString(stream, member->type->name);
	}
	if(member->memberType != ILDocMemberType_Constructor)
	{
		putc('.', stream);
		PrintString(stream, member->name);
	}
	if(member->fullyQualify)
	{
		/* We need to qualify the name using the parameter types */
		putc('(', stream);
		param = member->parameters;
		while(param != 0)
		{
			if(param != member->parameters)
			{
				/* Note: this should really be a comma, but texinfo
				   has problems using commas in node names, and so
				   this is the best we can do until we can find a
				   way around the problem */
				fputs("; ", stream);
			}
			PrintParamTypeName(stream, param->type);
			param = param->next;
		}
		putc(')', stream);
	}
	switch(member->memberType)
	{
		case ILDocMemberType_Field:		  fputs(" Field", stream); break;
		case ILDocMemberType_Method:	  fputs(" Method", stream); break;
		case ILDocMemberType_Constructor: fputs(" Constructor", stream); break;
		case ILDocMemberType_Property:	  fputs(" Property", stream); break;
		case ILDocMemberType_Event:       fputs(" Event", stream); break;
		case ILDocMemberType_Unknown:	  break;
	}
}

/*
 * Print a C# signature.
 */
static void PrintSignature(FILE *stream, char *signature)
{
	if(signature)
	{
		fputs("@example\n@cartouche\n", stream);
		PrintString(stream, signature);
		fputs("\n@end cartouche\n@end example\n\n", stream);
	}
}

/*
 * Print a specific child of a documentation node.
 */
static void PrintDocChild(FILE *stream, ILDocText *doc, const char *name)
{
}

/*
 * Convert the contents of a type into texinfo.
 */
static void ConvertType(FILE *stream, ILDocType *type,
						const char *namespaceName,
						int newPage)
{
	ILDocMember *member;
	ILDocMemberType lastMemberType;
	unsigned long index;
	const char *heading;

	/* Validate that the type node is more or less meaningful */
	if(!(type->name) || !(type->fullName))
	{
		return;
	}

	/* Output the node heading */
	if(newPage)
	{
		fputs("@page\n\n", stream);
	}
	fputs("@node ", stream);
	PrintTypeName(stream, type);
	fputs(",,, ", stream);
	PrintString(stream, namespaceName);
	fputs(" Namespace\n@section ", stream);
	PrintTypeName(stream, type);
	fputs("\n@tindex ", stream);
	PrintTypeName(stream, type);
	fputs("\n", stream);
	if(strcmp(type->name, type->fullName) != 0 && !(type->fullyQualify))
	{
		fputs("@tindex ", stream);
		PrintShortTypeName(stream, type);
		fputs("\n", stream);
	}
	fputs("\n", stream);

	/* Print the node summary */
	PrintDocChild(stream, type->doc, "summary");

	/* Print the C# definition for the type */
	PrintSignature(stream, type->csSignature);

	/* Print the node remarks */
	PrintDocChild(stream, type->doc, "remarks");

	/* Output a menu for all of the type members */
	member = type->members;
	if(member != 0)
	{
		fputs("@menu", stream);
		lastMemberType = ILDocMemberType_Unknown;
		index = 0;
		while(member != 0)
		{
			heading = 0;
			switch(member->memberType)
			{
				case ILDocMemberType_Field:
				{
					if(lastMemberType != ILDocMemberType_Field)
					{
						heading = "Fields";
					}
				}
				break;

				case ILDocMemberType_Method:
				{
					/* Print a new section header if necessary */
					if(lastMemberType != ILDocMemberType_Method)
					{
						heading = "Methods";
					}
				}
				break;

				case ILDocMemberType_Constructor:
				{
					/* Print a new section header if necessary */
					if(lastMemberType != ILDocMemberType_Constructor)
					{
						heading = "Constructors";
					}
				}
				break;

				case ILDocMemberType_Property:
				{
					/* Print a new section header if necessary */
					if(lastMemberType != ILDocMemberType_Property)
					{
						heading = "Properties";
					}
				}
				break;

				case ILDocMemberType_Event:
				{
					/* Print a new section header if necessary */
					if(lastMemberType != ILDocMemberType_Event)
					{
						heading = "Events";
					}
				}
				break;

				case ILDocMemberType_Unknown: break;
			}
			if(heading)
			{
				putc('\n', stream);
				if(type->fullyQualify)
				{
					PrintString(stream, type->fullName);
				}
				else
				{
					PrintString(stream, type->name);
				}
				putc(' ', stream);
				fputs(heading, stream);
				fputs("\n\n", stream);
			}
			fputs("* ", stream);
			PrintMemberName(stream, member);
			fputs("::\n", stream);
			lastMemberType = member->memberType;
			member = member->next;
		}
		fputs("@end menu\n\n", stream);
	}

	/* Print information about each of the members */
	index = 0;
	member = type->members;
	while(member != 0)
	{
		/* Print the node header */
		if(newPage)
		{
			fputs("@page\n\n", stream);
		}
		fputs("@node ", stream);
		PrintMemberName(stream, member);
		fputs(",,, ", stream);
		PrintTypeName(stream, type);
		fputs("\n", stream);
		fputs("@subsection ", stream);
		PrintMemberName(stream, member);
		fputs("\n", stream);
		if(member->memberType == ILDocMemberType_Field ||
		   member->memberType == ILDocMemberType_Property)
		{
			fputs("@vindex ", stream);
		}
		else
		{
			fputs("@findex ", stream);
		}
		PrintMemberName(stream, member);
		fputs("\n\n", stream);

		/* Print the signature for the member */
		PrintSignature(stream, member->csSignature);

		switch(member->memberType)
		{
			case ILDocMemberType_Field:
			{
			}
			break;

			case ILDocMemberType_Method:
			{
			}
			break;

			case ILDocMemberType_Constructor:
			{
			}
			break;

			case ILDocMemberType_Property:
			{
			}
			break;

			case ILDocMemberType_Event:
			{
			}
			break;

			case ILDocMemberType_Unknown: break;
		}
		member = member->next;
	}
}

int ILDocConvert(ILDocTree *tree, int numInputs, char **inputs,
				 char *outputPath, const char *progname)
{
	FILE *stream;
	int closeStream;
	ILDocNamespace *namespace;
	char *namespaceName;
	ILDocType *type;
	int isStandAlone;
	int hasIndex;
	int newPage;
	const char *chapterNewPage;
	const char *topNode;
	const char *title;
	const char *description;
	const char *copyright;
	const char *filename;
	char *newPath;
	int len;

	/* Attempt to open the output stream */
	if(!strcmp(outputPath, "-"))
	{
		stream = stdout;
		closeStream = 0;
		outputPath = inputs[0];
		if(!strcmp(outputPath, "-"))
		{
			outputPath = "stdout.info";
		}
	}
	else if((stream = fopen(outputPath, "w")) == NULL)
	{
		perror(outputPath);
		return 0;
	}
	else
	{
		closeStream = 1;
	}

	/* Replace the extension with ".info" to get the filename */
	len = strlen(outputPath);
	while(len > 0 && outputPath[len - 1] != '/' &&
	      outputPath[len - 1] != '\\')
	{
		--len;
	}
	newPath = (char *)ILMalloc(strlen(outputPath + len) + 16);
	if(!newPath)
	{
		ILDocOutOfMemory(progname);
	}
	strcpy(newPath, outputPath + len);
	outputPath = newPath;
	len = strlen(outputPath);
	while(len > 0 && outputPath[len - 1] != '.')
	{
		--len;
	}
	if(len > 0)
	{
		strcpy(outputPath + len, "info");
	}
	else
	{
		strcat(outputPath, ".info");
	}

	/* Process texinfo-specific options */
	isStandAlone = !ILDocFlagSet("embedded");
	if(isStandAlone)
	{
		hasIndex = !ILDocFlagSet("no-index");
		topNode = "Top";
	}
	else
	{
		hasIndex = ILDocFlagSet("no-index");
		topNode = ILDocFlagValue("parent");
		if(!topNode)
		{
			topNode = "Top";
		}
	}
	chapterNewPage = ILDocFlagValue("chapternewpage");
	if(!chapterNewPage)
	{
		chapterNewPage = "on";
	}
	filename = ILDocFlagValue("filename");
	if(!filename)
	{
		filename = outputPath;
	}
	title = ILDocFlagValue("title");
	if(!title)
	{
		title = filename;
	}
	description = ILDocFlagValue("description");
	if(!description)
	{
		description = title;
	}
	copyright = ILDocFlagValue("copyright");
	newPage = !ILDocFlagSet("no-newpage");
	if(!strcmp(chapterNewPage, "off"))
	{
		newPage = 0;
	}

	/* Dump the texinfo header material */
	if(isStandAlone)
	{
		/* Main texinfo header */
		fputs("\\input texinfo	@c -*-texinfo-*-\n", stream);
		fputs("@c %** start of header\n", stream);
		fputs("@setfilename ", stream);
		PrintString(stream, filename);
		fputs("\n@settitle ", stream);
		PrintString(stream, title);
		fputs("\n@setchapternewpage ", stream);
		PrintString(stream, chapterNewPage);
		fputs("\n", stream);
		fputs("@syncodeindex tp cp\n", stream);
		fputs("@syncodeindex fn cp\n", stream);
		fputs("@syncodeindex vr cp\n", stream);
		fputs("@c %** end of header\n\n", stream);

		/* Output the description for the info file */
		fputs("@ifinfo\n", stream);
		PrintString(stream, description);
		fputs("\n", stream);
		if(copyright)
		{
			fputs("\n", stream);
			fputs(copyright, stream);
			fputs("\n", stream);
		}
		fputs("@end ifinfo\n\n", stream);

		/* Output the title page for the printed documentation */
		fputs("@titlepage\n@sp 10\n@center @titlefont{", stream);
		PrintString(stream, title);
		fputs("}\n", stream);
		if(copyright)
		{
			fputs("\n@vskip 0pt plus 1fill\n@center{\n", stream);
			fputs(copyright, stream);
			fputs("}\n", stream);
		}
		fputs("@end titlepage\n\n", stream);

		/* Top node for stand-alone documents */
		fputs("@node Top,,, (dir)\n\n", stream);
	}

	/* Dump a menu that refers to all namespaces in the documentation tree */
	fputs("@menu\n", stream);
	namespace = tree->namespaces;
	while(namespace != 0)
	{
		fputs("* ", stream);
		if(namespace->name[0] == '\0')
		{
			PrintString(stream, "Global");
		}
		else
		{
			PrintString(stream, namespace->name);
		}
		fputs(" Namespace::\n", stream);
		namespace = namespace->next;
	}
	if(hasIndex)
	{
		fputs("* Index::\n", stream);
	}
	fputs("@end menu\n\n", stream);

	/* Dump all types within the documentation tree, ordered by namespace */
	namespace = tree->namespaces;
	while(namespace != 0)
	{
		/* Print the namespace chapter header */
		if(namespace->name[0] == '\0')
		{
			namespaceName = "Global";
		}
		else
		{
			namespaceName = namespace->name;
		}
		fputs("@node ", stream);
		PrintString(stream, namespaceName);
		fputs(" Namespace,,, ", stream);
		PrintString(stream, topNode);
		fputs("\n@chapter ", stream);
		PrintString(stream, namespaceName);
		fputs(" Namespace\n@cindex ", stream);
		PrintString(stream, namespaceName);
		fputs(" Namespace\n\n", stream);

		/* Print a menu of all types in the namespace */
		fputs("@menu\n", stream);
		type = namespace->types;
		while(type != 0)
		{
			fputs("* ", stream);
			PrintTypeName(stream, type);
			fputs("::\n", stream);
			type = type->nextNamespace;
		}
		fputs("@end menu\n\n", stream);

		/* Output the chapter-specific index for the TeX version */
		fputs("@iftex\n", stream);
		type = namespace->types;
		while(type != 0)
		{
			fputs("@noindent\n@ref{", stream);
			PrintTypeName(stream, type);
			fputs("}.@*\n", stream);
			type = type->nextNamespace;
		}
		fputs("@end iftex\n\n", stream);

		/* Convert all types into texinfo */
		type = namespace->types;
		while(type != 0)
		{
			ConvertType(stream, type, namespaceName, newPage);
			type = type->nextNamespace;
		}

		/* Move on to the next namespace */
		namespace = namespace->next;
	}

	/* Print the end matter for the texinfo file */
	if(isStandAlone)
	{
		if(hasIndex)
		{
			if(!strcmp(chapterNewPage, "off"))
			{
				/* Flush the page prior to the index, to make sure that
				   print mode calculates the index page sizes correctly */
				fputs("@page\n\n", stream);
			}
			fputs("@node Index,,, ", stream);
			PrintString(stream, topNode);
			fputs("\n@unnumbered Index\n\n", stream);
			fputs("@printindex cp\n\n", stream);
		}
		fputs("@contents\n@bye\n", stream);
	}

	/* Clean up and exit */
	if(closeStream)
	{
		fclose(stream);
	}
	return 1;
}

#ifdef	__cplusplus
};
#endif
