/*
 * link_create.c - Create and destroy linker contexts.
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

#include "linker.h"
#include "../image/image.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILLinker *ILLinkerCreate(FILE *stream, int seekable, int type, int flags)
{
	ILLinker *linker;

	/* Create the linker context */
	if((linker = (ILLinker *)ILMalloc(sizeof(ILLinker))) == 0)
	{
		return 0;
	}

	/* Create the image building context and image */
	linker->context = ILContextCreate();
	if(!(linker->context))
	{
		ILFree(linker);
		return 0;
	}
	linker->image = ILImageCreate(linker->context);
	if(!(linker->context))
	{
		ILContextDestroy(linker->context);
		ILFree(linker);
		return 0;
	}

	/* Create the image writer */
	linker->writer = ILWriterCreate(stream, seekable, type, flags);
	if(!(linker->writer))
	{
		ILContextDestroy(linker->context);
		ILFree(linker);
		return 0;
	}

	/* Initialize the other linker fields */
	linker->libraries = 0;
	linker->lastLibrary = 0;
	linker->libraryDirs = 0;
	linker->outOfMemory = 0;
	linker->error = 0;
	linker->resourceRVA = 0;
	linker->entryPoint = 0;

	/* Ready to go */
	return linker;
}

/*
 * Determine if a class reference is "System.Array".
 */
static int IsSystemArray(ILClass *classInfo)
{
	const char *temp;
	temp = ILClass_Namespace(classInfo);
	if(!temp || strcmp(temp, "System") != 0)
	{
		return 0;
	}
	if(strcmp(ILClass_Name(classInfo), "Array") != 0)
	{
		return 0;
	}
	return (ILClass_NestedParent(classInfo) == 0);
}

/*
 * Report that a particular class is unresolved.
 */
static void ReportUnresolvedClass(ILLinker *linker, ILClass *classInfo)
{
	ILDumpClassName(stderr, ILClassToImage(classInfo), classInfo, 0);
	fputs(" : unresolved type reference\n", stderr);
	linker->error = 1;
}

/*
 * Report unresolved type and member references.
 */
static void ReportUnresolved(ILLinker *linker)
{
	ILClass *classInfo;
	ILClass *parent;
	ILMember *member;
	int reported;

	/* Scan the TypeRef table for unresolved types */
	classInfo = 0;
	while((classInfo = (ILClass *)ILImageNextToken
				(linker->image, IL_META_TOKEN_TYPE_REF, classInfo)) != 0)
	{
		/* Skip the reference if it has since been defined */
		if(!ILClassIsRef(classInfo))
		{
			continue;
		}

		/* Skip the reference if it is synthetic */
		if(ILClassGetSynType(classInfo) != 0)
		{
			continue;
		}

		/* Make sure that all nested parents are references */
		parent = classInfo;
		reported = 0;
		while((parent = ILClassGetNestedParent(parent)) != 0)
		{
			if(!ILClassIsRef(parent))
			{
				ReportUnresolvedClass(linker, classInfo);
				reported = 1;
				break;
			}
		}
		if(reported)
		{
			continue;
		}

		/* If the reference is in a module scope, then it is dangling */
		if(ILProgramItemToModule(ILClassGetScope(classInfo)) != 0)
		{
			/* Special case: "System.Array" may get linked in due to the
			   use of synthetic array types, but we don't really need it */
			if(IsSystemArray(classInfo))
			{
				linker->image->tokenData[IL_META_TOKEN_TYPE_REF >> 24]
					[(ILClass_Token(classInfo) & 0x00FFFFFF) - 1] = 0;
				continue;
			}
			ReportUnresolvedClass(linker, classInfo);
		}
	}

	/* Scan the MemberRef table for unresolved members */
	member = 0;
	while((member = (ILMember *)ILImageNextToken
				(linker->image, IL_META_TOKEN_MEMBER_REF, member)) != 0)
	{
		classInfo = ILMember_Owner(member);
		if(!ILClassIsRef(classInfo) &&
		   (ILMember_Token(member) & IL_META_TOKEN_MASK) ==
		   		IL_META_TOKEN_MEMBER_REF)
		{
			/* The class has been defined, but not the member */
			if(ILMember_IsMethod(member))
			{
				ILDumpMethodType(stderr, ILProgramItem_Image(classInfo),
								 ILMethod_Signature((ILMethod *)member), 0,
								 classInfo, ILMember_Name(member),
								 (ILMethod *)member);
				fputs(" : unresolved method reference\n", stderr);
			}
			else if(ILMember_IsField(member))
			{
				ILDumpType(stderr, ILProgramItem_Image(classInfo),
						   ILField_Type((ILField *)member), 0);
				putc(' ', stderr);
				ILDumpClassName(stderr, ILProgramItem_Image(classInfo),
								classInfo, 0);
				fputs("::", stderr);
				fputs(ILMember_Name(member), stderr);
				fputs(" : unresolved field reference\n", stderr);
			}
			else
			{
				ILDumpClassName(stderr, ILProgramItem_Image(classInfo),
								classInfo, 0);
				fputs("::", stderr);
				fputs(ILMember_Name(member), stderr);
				fputs(" : unresolved member reference\n", stderr);
			}
			linker->error = 1;
		}
	}
}

int ILLinkerDestroy(ILLinker *linker)
{
	int result;
	ILLibraryDir *libraryDir;
	ILLibraryDir *nextLibraryDir;

	/* Report any remaining unresolved references */
	ReportUnresolved(linker);

	/* Flush the metadata to the image writer */
	if(!(linker->outOfMemory) && !(linker->error))
	{
		ILWriterOutputMetadata(linker->writer, linker->image);
	}

	/* Destroy the libraries */
	_ILLinkerDestroyLibraries(linker);

	/* Destroy the library directory list */
	libraryDir = linker->libraryDirs;
	while(libraryDir != 0)
	{
		nextLibraryDir = libraryDir->next;
		ILFree(libraryDir);
		libraryDir = nextLibraryDir;
	}

	/* Destroy the image writer and determine the result value */
	if(linker->outOfMemory)
	{
		result = -1;
	}
	else if(linker->error)
	{
		result = 0;
	}
	else
	{
		result = 1;
	}
	if(result > 0)
	{
		result = ILWriterDestroy(linker->writer);
	}
	else
	{
		ILWriterDestroy(linker->writer);
	}

	/* Destroy the image and context that we were building */
	ILContextDestroy(linker->context);

	/* Free the linker context */
	ILFree(linker);

	/* Done */
	return result;
}

void _ILLinkerOutOfMemory(ILLinker *linker)
{
	linker->outOfMemory = 1;
}

int ILLinkerCreateModuleAndAssembly(ILLinker *linker,
									const char *moduleName,
									const char *assemblyName,
									ILUInt16 *assemblyVersion,
									int hashAlgorithm)
{
	ILModule *module;
	ILAssembly *assembly;

	/* Create the module */
	module = ILModuleCreate(linker->image, 0, moduleName, 0);
	if(!module)
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}

	/* Create the assembly */
	assembly = ILAssemblyCreate(linker->image, 0, assemblyName, 0);
	if(!assembly)
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}
	ILAssemblySetVersion(assembly, assemblyVersion);
	ILAssemblySetHashAlgorithm(assembly, hashAlgorithm);

	/* Create the "<Module>" type, which holds global functions and variables */
	if(!ILClassCreate((ILProgramItem *)module, 0, "<Module>", 0, 0))
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}

	/* Ready to go */
	return 1;
}

#ifdef	__cplusplus
};
#endif
