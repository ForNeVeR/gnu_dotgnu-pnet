/*
 * link_library.c - Process libraries within a linker context.
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
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Size of the name hash table associated with a library.
 * Must be a power of 2.
 */
#define	IL_LINK_HASH_SIZE		512

/*
 * Determine if a directory exists.
 */
static int DirExists(const char *pathname)
{
#ifdef HAVE_STAT
	struct stat st;
	return (stat(pathname, &st) >= 0);
#else
#ifdef HAVE_ACCESS
	return (access(pathname, 0) >= 0);
#else
	return 1;
#endif
#endif
}

int ILLinkerAddLibraryDir(ILLinker *linker, const char *pathname)
{
	int index;
	char *dupStr;
	char **newDirs;

	/* Bail out if the directory doesn't exist, because there is
	   no point adding it to the list if it won't contain anything */
	if(!DirExists(pathname))
	{
		return 1;
	}

	/* Determine if the directory is already present.  There is
	   no point searching the same directory twice */
	for(index = 0; index < linker->numLibraryDirs; ++index)
	{
		if(!strcmp(linker->libraryDirs[index], pathname))
		{
			return 1;
		}
	}

	/* Add the directory to the list */
	dupStr = ILDupString(pathname);
	if(!dupStr)
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}
	newDirs = (char **)ILRealloc(linker->libraryDirs,
								 sizeof(char *) * (linker->numLibraryDirs + 1));
	if(!newDirs)
	{
		ILFree(dupStr);
		_ILLinkerOutOfMemory(linker);
		return 0;
	}
	linker->libraryDirs = newDirs;
	newDirs[(linker->numLibraryDirs)++] = dupStr;
	return 1;
}

void ILLinkerAddSystemDirs(ILLinker *linker)
{
	linker->useStdDirs = 1;
}

char *ILLinkerResolveLibrary(ILLinker *linker, const char *name)
{
	int len;
	char *newName;

	/* Does the filename contain a path specification? */
	len = strlen(name);
	while(len > 0 && name[len - 1] != '/' && name[len - 1] != '\\')
	{
		--len;
	}
	if(len > 0)
	{
		/* Yes it does, so check for direct file existence */
		if(!ILFileExists(name, (char **)0))
		{
			return 0;
		}
		newName = ILDupString(name);
		if(!newName)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
		return newName;
	}

	/* Search the library directories for the name */
	return ILImageSearchPath
		(name, 0, 0,
		 (const char **)(linker->libraryDirs), linker->numLibraryDirs,
		 0, 0, !(linker->useStdDirs), 0);
}

/*
 * Destroy a library context.
 */
static void LibraryDestroy(ILLibrary *library)
{
	ILLibrary *next;
	while(library != 0)
	{
		next = library->altNames;
		ILFree(library->name);
		ILHashDestroy(library->hashTable);
		ILMemPoolDestroy(&(library->classPool));
		ILFree(library);
		library = next;
	}
}

/*
 * Key that is used to index the library hash table.
 */
typedef struct
{
	const char *name;
	const char *namespace;
	ILLibraryClass *parent;

} LibHashKey;

/*
 * Compute the hash value for an element.
 */
static unsigned long LibHash_Compute(const ILLibraryClass *libClass)
{
	return ILHashString(0, libClass->name, strlen(libClass->name));
}

/*
 * Compute the hash value for a key.
 */
static unsigned long LibHash_KeyCompute(const LibHashKey *key)
{
	return ILHashString(0, key->name, strlen(key->name));
}

/*
 * Match a key against an element.
 */
static int LibHash_Match(const ILLibraryClass *libClass, const LibHashKey *key)
{
	if(strcmp(libClass->name, key->name) != 0)
	{
		return 0;
	}
	if(libClass->namespace)
	{
		if(key->namespace)
		{
			return !strcmp(libClass->namespace, key->namespace);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return (key->namespace == 0);
	}
	return (key->parent == libClass->parent);
}

/*
 * Create ILLibrary records for all assembly definitions
 * within a supplied library image.  Returns the list,
 * or NULL if an error occurred.
 */
static ILLibrary *ScanAssemblies(ILLinker *linker, ILImage *image,
								 const char *filename)
{
	ILLibrary *library;
	ILLibrary *lastLibrary;
	ILLibrary *nextLibrary;
	ILAssembly *assem;

	/* Scan the assembly definitions */
	library = 0;
	lastLibrary = 0;
	assem = 0;
	while((assem = (ILAssembly *)ILImageNextToken
				(image, IL_META_TOKEN_ASSEMBLY, (void *)assem)) != 0)
	{
		nextLibrary = (ILLibrary *)ILMalloc(sizeof(ILLibrary));
		if(!nextLibrary)
		{
			_ILLinkerOutOfMemory(linker);
			LibraryDestroy(library);
			return 0;
		}
		nextLibrary->name = ILDupString(ILAssembly_Name(assem));
		if(!(nextLibrary->name))
		{
			ILFree(nextLibrary);
			_ILLinkerOutOfMemory(linker);
			LibraryDestroy(library);
			return 0;
		}
		ILMemCpy(nextLibrary->version, ILAssemblyGetVersion(assem),
				 4 * sizeof(ILUInt16));
		nextLibrary->altNames = 0;
		if(library)
		{
			nextLibrary->hashTable = 0;
		}
		else
		{
			nextLibrary->hashTable =
				ILHashCreate(0, (ILHashComputeFunc)LibHash_Compute,
								(ILHashKeyComputeFunc)LibHash_KeyCompute,
								(ILHashMatchFunc)LibHash_Match,
								(ILHashFreeFunc)0);
			if(!(nextLibrary->hashTable))
			{
				ILFree(nextLibrary->name);
				ILFree(nextLibrary);
				LibraryDestroy(library);
				return 0;
			}
		}
		ILMemPoolInitType(&(nextLibrary->classPool), ILLibraryClass, 0);
		nextLibrary->next = 0;
		if(lastLibrary)
		{
			lastLibrary->altNames = nextLibrary;
		}
		else
		{
			library = nextLibrary;
		}
		lastLibrary = nextLibrary;
	}

	/* Bail out if there are no assembly definitions */
	if(!library)
	{
		fprintf(stderr, "%s: missing assembly definition tokens", filename);
		linker->error = 1;
		return 0;
	}

	/* Return the library list to the caller */
	return library;
}

/*
 * Walk a public type and all of its visible nested types.
 */
static int WalkTypeAndNested(ILLinker *linker, ILImage *image,
							 ILLibrary *library, ILClass *classInfo,
							 ILLibraryClass *parent)
{
	ILNestedInfo *nestedInfo;
	ILClass *child;
	ILLibraryClass *libClass;
	char *name;
	char *namespace;

	/* Add the name of this type to the library's hash table */
	name = (ILInternString((char *)(ILClass_Name(classInfo)), -1)).string;
	namespace = (char *)(ILClass_Namespace(classInfo));
	if(namespace)
	{
		namespace = (ILInternString(namespace, -1)).string;
	}
	if((libClass = ILMemPoolAlloc(&(library->classPool), ILLibraryClass)) == 0)
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}
	libClass->name = name;
	libClass->namespace = namespace;
	libClass->parent = parent;
	if(!ILHashAdd(library->hashTable, libClass))
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}

	/* Walk the visible nested types */
	nestedInfo = 0;
	while((nestedInfo = ILClassNextNested(classInfo, nestedInfo)) != 0)
	{
		child = ILNestedInfoGetChild(nestedInfo);
		switch(ILClass_Attrs(child) & IL_META_TYPEDEF_VISIBILITY_MASK)
		{
			case IL_META_TYPEDEF_PUBLIC:
			case IL_META_TYPEDEF_NESTED_PUBLIC:
			case IL_META_TYPEDEF_NESTED_FAMILY:
			case IL_META_TYPEDEF_NESTED_FAM_OR_ASSEM:
			{
				if(!WalkTypeAndNested(linker, image, library, child, libClass))
				{
					return 0;
				}
			}
			break;
		}
	}

	/* Done */
	return 1;
}

/*
 * Walk the type definitions within an image to build
 * a list of names that can be linked against.
 */
static int WalkTypeDefs(ILLinker *linker, ILImage *image, ILLibrary *library)
{
	ILClass *classInfo = 0;
	while((classInfo = (ILClass *)ILImageNextToken
				(image, IL_META_TOKEN_TYPE_DEF, (void *)classInfo)) != 0)
	{
		/* Nested classes are processed when their parent is scanned */
		if(!ILClass_NestedParent(classInfo))
		{
			/* Only "public" classes are visible outside the assembly */
			if(ILClass_IsPublic(classInfo))
			{
				if(!WalkTypeAndNested(linker, image, library, classInfo, 0))
				{
					return 0;
				}
			}
		}
	}
	return 1;
}

int ILLinkerAddLibrary(ILLinker *linker, ILImage *image, const char *filename)
{
	ILLibrary *library;

	/* Create ILLibrary records for the assembly definitions */
	library = ScanAssemblies(linker, image, filename);
	if(!library)
	{
		return 0;
	}

	/* Walk all of the TypeDef's in the library and build
	   a list that can be used for later linking */
	if(!WalkTypeDefs(linker, image, library))
	{
		LibraryDestroy(library);
		return 0;
	}

	/* Add the library to the linker's list */
	if(linker->lastLibrary)
	{
		linker->lastLibrary->next = library;
	}
	else
	{
		linker->libraries = library;
	}
	linker->lastLibrary = library;
	return 1;
}

int ILLinkerHasLibrary(ILLinker *linker, const char *name)
{
	return (_ILLinkerFindLibrary(linker, name) != 0);
}

void _ILLinkerDestroyLibraries(ILLinker *linker)
{
	ILLibrary *library;
	ILLibrary *next;
	library = linker->libraries;
	while(library != 0)
	{
		next = library->next;
		LibraryDestroy(library);
		library = next;
	}
}

ILLibrary *_ILLinkerFindLibrary(ILLinker *linker, const char *name)
{
	ILLibrary *library;
	ILLibrary *altName;
	library = linker->libraries;
	while(library != 0)
	{
		altName = library;
		do
		{
			if(!ILStrICmp(altName->name, name))
			{
				return altName;
			}
			altName = altName->altNames;
		}
		while(altName != 0);
		library = library->next;
	}
	return 0;
}

void _ILLinkerFindInit(ILLibraryFind *find, ILLinker *linker,
					   ILLibrary *library)
{
	find->linker = linker;
	find->library = library;
	find->libClass = 0;
	find->prevClass = 0;
}

/*
 * Find a class by name within a library.
 */
static ILLibraryClass *FindClass(ILLibrary *library,
								 const char *name, const char *namespace,
								 ILLibraryClass *parent)
{
	LibHashKey key;
	key.name = name;
	key.namespace = namespace;
	key.parent = parent;
	return ILHashFindType(library->hashTable, &key, ILLibraryClass);
}

int _ILLinkerFindClass(ILLibraryFind *find, const char *name,
					   const char *namespace)
{
	ILLibraryClass *parent;
	find->prevClass = find->libClass;
	if(!(find->libClass))
	{
		/* Look for a top-level class within the library */
		if(find->library)
		{
			/* We already know which library to look in */
			find->libClass = FindClass(find->library,
									   name, namespace, 0);
		}
		else
		{
			/* Look for the class in any library */
			find->library = find->linker->libraries;
			while(find->library != 0)
			{
				find->libClass = FindClass(find->library,
										   name, namespace, 0);
				if(find->libClass)
				{
					break;
				}
				find->library = find->library->next;
			}
		}
	}
	else
	{
		/* Look for a nested class within the last class found */
		parent = find->libClass;
		find->libClass = FindClass(find->library,
								   name, namespace, parent);
	}
	return (find->libClass != 0);
}

static void PrintParentClass(ILLibraryClass *classInfo)
{
	if(classInfo)
	{
		PrintParentClass(classInfo->parent);
		if(classInfo->namespace)
		{
			fputs(classInfo->namespace, stderr);
			putc('.', stderr);
		}
		fputs(classInfo->name, stderr);
		putc('/', stderr);
	}
}

void _ILLinkerPrintClass(ILLibraryFind *find, const char *name,
						 const char *namespace)
{
	if(find->library)
	{
		putc('[', stderr);
		fputs(find->library->name, stderr);
		putc(']', stderr);
	}
	PrintParentClass(find->prevClass);
	if(namespace)
	{
		fputs(namespace, stderr);
		putc('.', stderr);
	}
	fputs(name, stderr);
}

/*
 * Make a type reference within an image.
 */
static ILClass *MakeTypeRef(ILLibraryFind *find, ILLibraryClass *libClass,
							ILImage *image)
{
	ILClass *parent;
	ILClass *classInfo;
	ILAssembly *assem;

	/* Make a new type reference */
	if(libClass->parent)
	{
		/* Make a reference to a nested class */
		parent = MakeTypeRef(find, libClass->parent, image);
		if(!parent)
		{
			return 0;
		}
		classInfo = ILClassLookup(ILToProgramItem(parent),
								  libClass->name, libClass->namespace);
		if(classInfo)
		{
			return classInfo;
		}
		classInfo = ILClassCreateRef(ILToProgramItem(parent), 0,
								     libClass->name, libClass->namespace);
		if(!classInfo)
		{
			_ILLinkerOutOfMemory(find->linker);
		}
		return classInfo;
	}
	else
	{
		/* Look for an AssemblyRef to use to import the class */
		assem = 0;
		while((assem = (ILAssembly *)ILImageNextToken
					(image, IL_META_TOKEN_ASSEMBLY_REF, (void *)assem)) != 0)
		{
			if(!ILStrICmp(find->library->name, ILAssembly_Name(assem)) &&
			   !ILMemCmp(find->library->version,
			   			 ILAssemblyGetVersion(assem), sizeof(ILUInt16) * 4))
			{
				break;
			}
		}
		if(!assem)
		{
			/* Create an AssemblyRef to use to import the class */
			assem = ILAssemblyCreate(image, 0, find->library->name, 1);
			if(!assem)
			{
				_ILLinkerOutOfMemory(find->linker);
				return 0;
			}
			ILAssemblySetVersion(assem, find->library->version);
		}

		/* See if we already have a reference */
		classInfo = ILClassLookup(ILToProgramItem(assem),
								  libClass->name, libClass->namespace);
		if(classInfo)
		{
			return classInfo;
		}

		/* Import the class using the AssemblyRef */
		classInfo = ILClassCreateRef(ILToProgramItem(assem), 0,
									 libClass->name,
									 libClass->namespace);
		if(!classInfo)
		{
			_ILLinkerOutOfMemory(find->linker);
		}
		return classInfo;
	}
}

ILClass *_ILLinkerMakeTypeRef(ILLibraryFind *find, ILImage *image)
{
	if(find->libClass)
	{
		return MakeTypeRef(find, find->libClass, image);
	}
	else
	{
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif
