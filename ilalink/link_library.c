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
 * The default system library link path.
 */
#ifdef CSCC_LIB_PREFIX
#define	LIB_PATH	\
			CSCC_LIB_PREFIX "/lib:" \
			"/usr/local/lib/cscc/lib:" \
			"/usr/lib/cscc/lib"
#else
#define	LIB_PATH	\
			"/usr/local/lib/cscc/lib:" \
			"/usr/lib/cscc/lib"
#endif

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

/*
 * Add a library directory to a linker context.
 */
static void AddLibraryDir(ILLinker *linker, ILLibraryDir *libraryDir)
{
	ILLibraryDir *temp, *last;

	/* Bail out if the directory doesn't exist, because there is
	   no point adding it to the list if it won't contain anything */
	if(!DirExists(libraryDir->name))
	{
		ILFree(libraryDir);
		return;
	}

	/* Determine if the directory is already present.  There is
	   no point searching the same directory twice */
	last = 0;
	temp = linker->libraryDirs;
	while(temp != 0)
	{
		if(!strcmp(libraryDir->name, temp->name))
		{
			ILFree(libraryDir);
			return;
		}
		last = temp;
		temp = temp->next;
	}

	/* Add the directory to the list */
	libraryDir->next = 0;
	if(last)
	{
		last->next = libraryDir;
	}
	else
	{
		linker->libraryDirs = libraryDir;
	}
}

int ILLinkerAddLibraryDir(ILLinker *linker, const char *pathname)
{
	ILLibraryDir *libraryDir;
	libraryDir = (ILLibraryDir *)ILMalloc(sizeof(ILLibraryDir) +
										  strlen(pathname));
	if(!libraryDir)
	{
		_ILLinkerOutOfMemory(linker);
		return 0;
	}
	libraryDir->len = strlen(pathname);
	strcpy(libraryDir->name, pathname);
	AddLibraryDir(linker, libraryDir);
	return 1;
}

int ILLinkerAddSystemDirs(ILLinker *linker)
{
	char *path;
	int len;
	ILLibraryDir *libraryDir;

	/* Find the environment path to use for the system library directories */
	path = getenv("CSCC_LIB_PATH");
	if(!path || *path == '\0')
	{
		path = LIB_PATH;
	}

	/* Add all path elements to the list */
	while(*path != '\0')
	{
		if(*path == ' ' || *path == ':')
		{
			++path;
			continue;
		}
		len = 1;
		while(path[len] != '\0' && path[len] != ':')
		{
			++len;
		}
		while(len > 0 && path[len - 1] == ' ')
		{
			--len;
		}
		libraryDir = (ILLibraryDir *)ILMalloc(sizeof(ILLibraryDir) + len);
		if(!libraryDir)
		{
			_ILLinkerOutOfMemory(linker);
			return 0;
		}
		ILMemCpy(libraryDir->name, path, len);
		libraryDir->len = len;
		libraryDir->name[len] = '\0';
		AddLibraryDir(linker, libraryDir);
		path += len;
	}

	/* Done */
	return 1;
}

char *ILLinkerResolveLibrary(ILLinker *linker, const char *name)
{
	int len;
	int hasExt;
	char *newName;
	ILLibraryDir *libraryDir;

	/* Does the filename contain a path specification? */
	len = strlen(name);
	hasExt = 0;
	while(len > 0 && name[len - 1] != '/' && name[len - 1] != '\\')
	{
		--len;
		if(name[len] == '.')
		{
			hasExt = 1;
		}
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
	libraryDir = linker->libraryDirs;
	len = strlen(name);
	while(libraryDir != 0)
	{
		if(hasExt)
		{
			/* The name probably already has the ".dll" extension */
			newName = (char *)ILMalloc(libraryDir->len + len + 2);
			if(!newName)
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
			strcpy(newName, libraryDir->name);
			newName[libraryDir->len] = '/';
			strcpy(newName + libraryDir->len + 1, name);
		}
		else
		{
			/* Add the ".dll" extension when searching for the name */
			newName = (char *)ILMalloc(libraryDir->len + len + 6);
			if(!newName)
			{
				_ILLinkerOutOfMemory(linker);
				return 0;
			}
			strcpy(newName, libraryDir->name);
			newName[libraryDir->len] = '/';
			strcpy(newName + libraryDir->len + 1, name);
			strcpy(newName + libraryDir->len + 1 + len, ".dll");
		}
		if(ILFileExists(newName, (char **)0))
		{
			return newName;
		}
		ILFree(newName);
		libraryDir = libraryDir->next;
	}

	/* We could not resolve the name */
	return 0;
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
		if(library->hashTable)
		{
			ILFree(library->hashTable);
		}
		ILMemPoolDestroy(&(library->classPool));
		ILFree(library);
		library = next;
	}
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
				(ILLibraryClass **)ILCalloc(IL_LINK_HASH_SIZE,
											sizeof(ILLibraryClass *));
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
	unsigned long hash;

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
	hash = ILHashString(0, name, strlen(name));
	hash &= ~(IL_LINK_HASH_SIZE - 1);
	libClass->name = name;
	libClass->namespace = namespace;
	libClass->parent = parent;
	libClass->next = library->hashTable[hash];
	library->hashTable[hash] = libClass;

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
 * Find the next class with a specific name.
 */
static ILLibraryClass *FindNextNamed(ILLibraryClass *libClass,
									 char *name, char *namespace)
{
	while(libClass != 0)
	{
		if(libClass->name == name &&
		   libClass->namespace == namespace)
		{
			return libClass;
		}
		libClass = libClass->next;
	}
	return 0;
}

/*
 * Find a class by name within a library.
 */
static ILLibraryClass *FindClass(ILLibrary *library,
								 char *name, char *namespace,
								 int noParent)
{
	unsigned long hash;
	ILLibraryClass *libClass;

	/* Hash the class name */
	hash = ILHashString(0, name, strlen(name));
	hash &= ~(IL_LINK_HASH_SIZE - 1);

	/* Search the hash for a matching name */
	libClass = library->hashTable[hash];
	while(libClass != 0)
	{
		if(libClass->name == name &&
		   libClass->namespace == namespace)
		{
			if(!noParent || !(libClass->parent))
			{
				return libClass;
			}
		}
		libClass = libClass->next;
	}
	return 0;
}

int _ILLinkerFindClass(ILLibraryFind *find, const char *name,
					   const char *namespace)
{
	ILLibraryClass *parent;
	char *nameIntern = (ILInternString((char *)name, -1)).string;
	char *namespaceIntern =
		(namespace ? (ILInternString((char *)namespace, -1)).string : 0);
	find->prevClass = find->libClass;
	if(!(find->libClass))
	{
		/* Look for a top-level class within the library */
		if(find->library)
		{
			/* We already know which library to look in */
			find->libClass = FindClass(find->library,
									   nameIntern, namespaceIntern, 1);
		}
		else
		{
			/* Look for the class in any library */
			find->library = find->linker->libraries;
			while(find->library != 0)
			{
				find->libClass = FindClass(find->library,
										   nameIntern, namespaceIntern, 1);
				if(find->library)
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
								   nameIntern, namespaceIntern, 0);
		while(find->libClass != 0 && find->libClass->parent != parent)
		{
			find->libClass = FindNextNamed(find->libClass->next,
										   nameIntern, namespaceIntern);
		}
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
	ILClass *classInfo;
	ILAssembly *assem;

	/* TODO: Look for an existing type reference */

	/* Make a new type reference */
	if(libClass->parent)
	{
		/* Make a reference to a nested class */
		classInfo = MakeTypeRef(find, libClass->parent, image);
		if(!classInfo)
		{
			return 0;
		}
		classInfo = ILClassCreateRef(ILToProgramItem(classInfo), 0,
								     libClass->name,
									 libClass->namespace);
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
