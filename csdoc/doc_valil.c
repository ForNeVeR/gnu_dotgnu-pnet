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
	{"-fextras-ok", 'f', 1,
		"-fextras-ok",
		"It is OK for extra types and methods to appear in the image."},
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
 * Validate a documentation member against an IL image.
 * Returns zero if a validation error occurred.
 */
static int ValidateMember(FILE *stream, ILImage *image, ILClass *classInfo,
						  ILDocType *type, ILDocMember *member)
{
	int valid = 1;

	/* Done */
	return valid;
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
	int extrasOK = ILDocFlagSet("extras-ok");
	int implemented;
	ILDocMember *member;

	/* See if the type exists within the image */
	classInfo = ILClassLookup(ILClassGlobalScope(image),
							  type->name, type->namespace->name);
	if(!classInfo)
	{
		fprintf(stream, "%s is missing\n", type->fullName);
		return 0;
	}

	/* Validate the type kind */
	switch(type->kind)
	{
		case ILDocTypeKind_Class:
		{
			if(ILClass_IsInterface(classInfo))
			{
				fprintf(stderr, "%s is an interface, but should be a class\n",
						type->fullName);
				valid = 0;
			}
			else if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) !=
						ILType_FromValueType(classInfo))
				{
					fprintf(stderr, "%s is an enum, but should be a class\n",
							type->fullName);
					valid = 0;
				}
				else
				{
					fprintf(stderr, "%s is a struct, but should be a class\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(IsDelegateType(classInfo))
			{
				fprintf(stderr, "%s is a delegate, but should be a class\n",
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
					fprintf(stderr, "%s is an enum, but should "
									"be an interface\n",
							type->fullName);
					valid = 0;
				}
				else
				{
					fprintf(stderr, "%s is a struct, but should "
									"be an interface\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(IsDelegateType(classInfo))
			{
				fprintf(stderr, "%s is a delegate, but should "
								"be an interface\n",
						type->fullName);
				valid = 0;
			}
			else
			{
				fprintf(stderr, "%s is a class, but should "
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
				fprintf(stderr, "%s is an interface, but should be a struct\n",
						type->fullName);
				valid = 0;
			}
			else if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) !=
						ILType_FromValueType(classInfo))
				{
					fprintf(stderr, "%s is an enum, but should be a struct\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(IsDelegateType(classInfo))
			{
				fprintf(stderr, "%s is a delegate, but should be a struct\n",
						type->fullName);
				valid = 0;
			}
			else
			{
				fprintf(stderr, "%s is a class, but should be a struct\n",
						type->fullName);
				valid = 0;
			}
		}
		break;

		case ILDocTypeKind_Enum:
		{
			if(ILClass_IsInterface(classInfo))
			{
				fprintf(stderr, "%s is an interface, but should be an enum\n",
						type->fullName);
				valid = 0;
			}
			else if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) ==
						ILType_FromValueType(classInfo))
				{
					fprintf(stderr, "%s is a struct, but should be an enum\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(IsDelegateType(classInfo))
			{
				fprintf(stderr, "%s is a delegate, but should be an enum\n",
						type->fullName);
				valid = 0;
			}
			else
			{
				fprintf(stderr, "%s is a class, but should be an enum\n",
						type->fullName);
				valid = 0;
			}
		}
		break;

		case ILDocTypeKind_Delegate:
		{
			if(ILClass_IsInterface(classInfo))
			{
				fprintf(stderr, "%s is an interface, but should "
								"be a delegate\n",
						type->fullName);
				valid = 0;
			}
			else if(ILClassIsValueType(classInfo))
			{
				if(ILTypeGetEnumType(ILType_FromValueType(classInfo)) !=
						ILType_FromValueType(classInfo))
				{
					fprintf(stderr, "%s is an enum, but should be a delegate\n",
							type->fullName);
					valid = 0;
				}
				else
				{
					fprintf(stderr, "%s is a struct, but should "
									"be a delegate\n",
							type->fullName);
					valid = 0;
				}
			}
			else if(!IsDelegateType(classInfo))
			{
				fprintf(stderr, "%s is a class, but should be a delegate\n",
						type->fullName);
				valid = 0;
			}
		}
		break;
	}

	/* Validate the type attributes */

	/* Validate the type flags */

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
				fprintf(stream, "%s should have base type %s, "
								"but has base %s instead\n",
						type->fullName, type->baseType, fullName);
				valid = 0;
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
	if(!ILDocFlagSet("extras-ok"))
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
