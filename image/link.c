/*
 * link.c - Dynamic linking support for IL images.
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

#include "program.h"
#include "il_serialize.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * The default system assembly link path.
 */
#ifdef CSCC_LIB_DIR
#define	ASSEMBLY_LINK_PATH	\
			CSCC_LIB_DIR "/cscc/lib:" \
			"/usr/local/lib/cscc/lib:" \
			"/usr/lib/cscc/lib"
#else
#define	ASSEMBLY_LINK_PATH	\
			"/usr/local/lib/cscc/lib:" \
			"/usr/lib/cscc/lib"
#endif

/*
 * The default system module link path for PInvoke modules.
 */
#ifdef CSCC_LIB_DIR
#define	MODULE_LINK_PATH	\
			CSCC_LIB_DIR ":" \
			"/usr/local/lib:" \
			"/usr/lib"
#else
#define	MODULE_LINK_PATH	\
			"/usr/local/lib:" \
			"/usr/lib"
#endif

/*
 * Test a specific pathname for an assembly.
 */
static char *TestAssemblyPath(const char *pathname, int pathlen,
							  const char *name, int namelen, int needSuffix)
{
	int nameStart;
#ifndef _WIN32
	int sawUpper;
#endif

	/* Build the full pathname */
	char *path = (char *)ILMalloc(pathlen + namelen + 6);
	if(!path)
	{
		return 0;
	}
	if(pathlen)
	{
		ILMemCpy(path, pathname, pathlen);
		path[pathlen] = '/';
		ILMemCpy(path + pathlen + 1, name, namelen);
		if(needSuffix)
		{
			strcpy(path + pathlen + 1 + namelen, ".dll");
		}
		else
		{
			path[pathlen + 1 + namelen] = '\0';
		}
		nameStart = pathlen + 1;
	}
	else
	{
		ILMemCpy(path, name, namelen);
		if(needSuffix)
		{
			strcpy(path + namelen, ".dll");
		}
		else
		{
			path[namelen] = '\0';
		}
		nameStart = 0;
	}

	/* Test for the file's presence */
	if(ILFileExists(path, 0))
	{
		return path;
	}

#ifndef _WIN32
	/* Try converting the name to lower case and retrying.
	   This is designed to get around Windows case issues on Unix */
	sawUpper = 0;
	while(path[nameStart] != '\0')
	{
		if(path[nameStart] >= 'A' && path[nameStart] <= 'Z')
		{
			sawUpper = 1;
			path[nameStart] = path[nameStart] - 'A' + 'a';
		}
		++nameStart;
	}
	if(sawUpper)
	{
		if(ILFileExists(path, 0))
		{
			return path;
		}
	}
#endif

	/* Not found */
	ILFree(path);
	return 0;
}

/*
 * Determine the pathname separator to use on this platform.
 */
#if (defined(_WIN32) || defined(WIN32)) && !defined(__CYGWIN__)
	#define	PATH_SEP	';'
#else
	#define	PATH_SEP	':'
#endif

/*
 * Search a pathname list for a specific assembly.
 */
static char *SearchPathForAssembly(char *list, const char *name, int namelen,
								   const ILUInt16 *version)
{
	int len;
	char *path;

	while(*list != '\0')
	{
		/* Skip separators between directory pathnames */
		if(*list == PATH_SEP || *list == ' ' || *list == '\t' ||
		   *list == '\r' || *list == '\n')
		{
			++list;
			continue;
		}

		/* Get the next directory pathname */
		len = 1;
		while(list[len] != '\0' && list[len] != PATH_SEP)
		{
			++len;
		}
		while(list[len - 1] == ' ' || list[len - 1] == '\t' ||
		      list[len - 1] == '\r' || list[len - 1] == '\n')
		{
			--len;
		}

		/* Test for the assembly within this directory */
		path = TestAssemblyPath(list, len, name, namelen, 1);
		if(path)
		{
			return path;
		}

		/* Advance to the next path */
		list += len;
	}
	return 0;
}

char *ILImageSearchPath(const char *name, const ILUInt16 *version,
						const char **beforePaths, unsigned long numBeforePaths,
						const char **afterPaths, unsigned long numAfterPaths,
						int suppressStandardPaths)
{
	static ILUInt16 const zeroVersion[4] = {0, 0, 0, 0};
	int namelen;
	unsigned long posn;
	char *path;
	char *env;

	/* Bail out immediately if the name is invalid */
	if(!name)
	{
		return 0;
	}
	namelen = strlen(name);
	if(namelen >= 4 && !ILStrICmp(name + namelen - 4, ".dll"))
	{
		/* Strip ".dll" from the end of the name */
		namelen -= 4;
	}
	if(namelen == 0)
	{
		return 0;
	}

	/* Make sure that "version" is non-NULL */
	if(!version)
	{
		version = zeroVersion;
	}

	/* Search the before path list */
	for(posn = 0; posn < numBeforePaths; ++posn)
	{
		path = TestAssemblyPath(beforePaths[posn], strlen(beforePaths[posn]),
								name, namelen, 1);
		if(path)
		{
			return path;
		}
	}

	/* Search the standard paths */
	if(!suppressStandardPaths)
	{
		/* Try looking in CSCC_LIB_PATH for the assembly */
		env = getenv("CSCC_LIB_PATH");
		if(env && *env != '\0')
		{
			path = SearchPathForAssembly(env, name, namelen, version);
			if(path)
			{
				return path;
			}
		}
		else
		{
			path = SearchPathForAssembly(ASSEMBLY_LINK_PATH,
										 name, namelen, version);
			if(path)
			{
				return path;
			}
		}

		/* Try looking in LD_LIBRARY_PATH or PATH for the assembly */
	#ifndef _WIN32
		env = getenv("LD_LIBRARY_PATH");
	#else
		env = getenv("PATH");
	#endif
		if(env && *env != '\0')
		{
			path = SearchPathForAssembly(env, name, namelen, version);
			if(path)
			{
				return path;
			}
		}
	}

	/* Search the after path list */
	for(posn = 0; posn < numAfterPaths; ++posn)
	{
		path = TestAssemblyPath(afterPaths[posn], strlen(afterPaths[posn]),
								name, namelen, 1);
		if(path)
		{
			return path;
		}
	}

	/* Could not find the assembly */
	return 0;
}

/*
 * Locate an assembly given its name and version.
 * Returns the malloc'ed path, or NULL if not found.
 */
static char *LocateAssembly(ILContext *context, const char *name,
							ILUInt16 *version, const char *parentAssemblyPath)
{
	int namelen;
	int pathlen;
	char *path;

	/* Strip ".dll" from the name, if present */
	namelen = strlen(name);
	if(namelen > 4 && name[namelen - 4] == '.' &&
	   (name[namelen - 3] == 'd' || name[namelen - 3] == 'D') &&
	   (name[namelen - 2] == 'l' || name[namelen - 2] == 'L') &&
	   (name[namelen - 1] == 'l' || name[namelen - 1] == 'L'))
	{
		namelen -= 4;
	}

	/* Look in the same directory as the parent assembly */
	if(parentAssemblyPath)
	{
		pathlen = strlen(parentAssemblyPath);
		while(pathlen > 0 && parentAssemblyPath[pathlen - 1] != '/' &&
		      parentAssemblyPath[pathlen - 1] != '\\')
		{
			--pathlen;
		}
		if(pathlen > 0)
		{
			path = TestAssemblyPath(parentAssemblyPath,
									pathlen - 1, name, namelen, 1);
			if(path)
			{
				return path;
			}
		}
		else
		{
			/* The parent assembly is in the current directory */
			path = TestAssemblyPath(".", 0, name, namelen, 1);
			if(path)
			{
				return path;
			}
		}
	}

	/* Use the standard search order */
	return ILImageSearchPath(name, version,
							 (const char **)(context->libraryDirs),
							 context->numLibraryDirs, 0, 0, 0);
}

int _ILImageDynamicLink(ILImage *image, const char *filename, int flags)
{
	ILAssembly *assem;
	char *pathname;
	int error;
	FILE *file;
	ILImage *newImage;

	/* Scan the AssemblyRef table for the assemblies that we require */
	assem = 0;
	while((assem = (ILAssembly *)ILImageNextToken
				(image, IL_META_TOKEN_ASSEMBLY_REF, assem)) != 0)
	{
		/* Ignore this assembly reference if we already have it */
		if(ILContextGetAssembly(image->context, assem->name) != 0)
		{
			continue;
		}

		/* Locate the assembly */
		pathname = LocateAssembly(image->context, assem->name,
								  assem->version, filename);
		if(!pathname)
		{
		#if IL_DEBUG_META
			fprintf(stderr, "could not locate the assembly %s/%u.%u.%u.%u\n",
					assem->name, (unsigned)(assem->version[0]),
					(unsigned)(assem->version[1]),
					(unsigned)(assem->version[2]),
					(unsigned)(assem->version[3]));
		#endif
			return IL_LOADERR_UNRESOLVED;
		}

		/* Load the image */
		file = fopen(pathname, "rb");
		if(!file)
		{
			/* Retry with "r" in case this libc does not support "rb" */
			file = fopen(pathname, "r");
		}
		if(!file)
		{
			ILFree(pathname);
			return IL_LOADERR_UNRESOLVED;
		}
		error = ILImageLoad(file, pathname, image->context, &newImage, flags);
		fclose(file);
		if(error != 0)
		{
			ILFree(pathname);
			return error;
		}

		/* Free the pathname that we no longer require */
		ILFree(pathname);
	}

	/* Done */
	return 0;
}

#if defined(CSCC_HOST_TRIPLET) || defined(CSCC_HOST_ALIAS)

/*
 * Determine if a platform name matches a host name that
 * was discovered during the autoconfiguration process.
 */
static int MatchHostName(const char *host, const char *platform,
						 int platformLen)
{
	while(platformLen > 0 && *host != '\0')
	{
		if(*platform == '*')
		{
			/* If this is the last charcter, then we have a match */
			++platform;
			--platformLen;
			if(platformLen == 0)
			{
				return 1;
			}

			/* Search for matches on the next character */
			while(*host != '\0')
			{
				if(*host == *platform)
				{
					if(MatchHostName(host + 1, platform + 1, platformLen - 1))
					{
						return 1;
					}
				}
				++host;
			}
			return 0;
		}
		else
		{
			/* Perform a normal character match */
			if(*platform != *host)
			{
				return 0;
			}
			++platform;
			--platformLen;
			++host;
		}
	}
	return (platformLen == 0 && *host == '\0');
}

#endif	/* CSCC_HOST_TRIPLET || CSCC_HOST_ALIAS */

/*
 * Determine if we have a match against a "DllImportMap" attribute.
 */
static int DllMapMatch(const char *name,
					   const char *platform, int platformLen,
					   const char *oldName, int oldNameLen)
{
	/* Check for a name match first */
	if(strlen(name) != oldNameLen ||
	   ILMemCmp(name, oldName, oldNameLen) != 0)
	{
		return 0;
	}

	/* Match against either the host triplet or alias that
	   was discovered during the autoconfiguration process */
#ifdef CSCC_HOST_TRIPLET
	if(MatchHostName(CSCC_HOST_TRIPLET, platform, platformLen))
	{
		return 1;
	}
#endif
#ifdef CSCC_HOST_ALIAS
	if(MatchHostName(CSCC_HOST_ALIAS, platform, platformLen))
	{
		return 1;
	}
#endif

	/* Is the platform code a standard fallback? */
#ifndef _WIN32
	if(platformLen == 17 &&
	   !ILMemCmp(platform, "std-shared-object", 17))
	{
		return 1;
	}
#else
	if(platformLen == 13 &&
	   !ILMemCmp(platform, "std-win32-dll", 13))
	{
		return 1;
	}
#endif

	/* The platform did not match */
	return 0;
}

/*
 * Search for a "DllImportMap" attribute on a program item.
 * Returns the length of the new name, or -1 if not found.
 */
static int SearchForDllMap(ILProgramItem *item, const char *name,
						   const char **remapName)
{
	ILAttribute *attr;
	ILMethod *method;
	const void *blob;
	unsigned long blobLen;
	ILSerializeReader *reader;
	const char *platform;
	int platformLen;
	const char *oldName;
	int oldNameLen;
	const char *newName;
	int newNameLen;

	attr = 0;
	while((attr = ILProgramItemNextAttribute(item, attr)) != 0)
	{
		method = ILProgramItemToMethod(ILAttributeTypeAsItem(attr));
		if(method != 0 && !strcmp(method->member.owner->name,
								  "DllImportMapAttribute"))
		{
			blob = ILAttributeGetValue(attr, &blobLen);
			if(blob &&
			   (reader = ILSerializeReaderInit(method, blob, blobLen)) != 0)
			{
				/* Get the parameter values from the attribute */
				if(ILSerializeReaderGetParamType(reader) ==
						IL_META_SERIALTYPE_STRING)
				{
					platformLen = ILSerializeReaderGetString
							(reader, &platform);
					if(platformLen != -1 &&
					   ILSerializeReaderGetParamType(reader) ==
							IL_META_SERIALTYPE_STRING)
					{
						oldNameLen = ILSerializeReaderGetString
								(reader, &oldName);
						if(oldNameLen != -1 &&
						   ILSerializeReaderGetParamType(reader) ==
								IL_META_SERIALTYPE_STRING)
						{
							newNameLen = ILSerializeReaderGetString
									(reader, &newName);
							if(newNameLen != -1 &&
							   DllMapMatch(name, platform, platformLen,
							   			   oldName, oldNameLen))
							{
								/* We have found a match */
								*remapName = newName;
								return newNameLen;
							}
						}
					}
				}
				ILSerializeReaderDestroy(reader);
			}
		}
	}

	/* The attribute is not present */
	return -1;
}

char *ILPInvokeResolveModule(ILPInvoke *pinvoke)
{
	const char *name;
	const char *remapName;
	int namelen;
	int posn;
	char *baseName;
	char *fullName;
	ILContext *context;

	/* Validate the module name that was provided */
	if(!pinvoke || !(pinvoke->module) || !(pinvoke->module->name) ||
	   pinvoke->module->name[0] == '\0')
	{
		return 0;
	}
	name = pinvoke->module->name;

	/* Does the name need to be remapped for this platform? */
	namelen = SearchForDllMap(&(pinvoke->method->member.programItem),
							  name, &remapName);
	if(namelen == -1)
	{
		namelen = SearchForDllMap(&(pinvoke->method->member.owner->programItem),
							  	  name, &remapName);
		if(namelen == -1)
		{
			namelen = strlen(name);
		}
		else
		{
			name = remapName;
		}
	}
	else
	{
		name = remapName;
	}

	/* Determine the platform-specific suffix to add, and then
	   allocate a string that contains the base search name */
#ifndef _WIN32
	{
		int posn = 0;
		int needSuffix = 1;
		while(posn <= (namelen - 3))
		{
			if(name[posn] == '.' && name[posn + 1] == 's' &&
			   name[posn + 2] == 'o' &&
			   ((posn + 3) == namelen || name[posn + 3] == '.'))
			{
				/* The name already includes ".so" somewhere */
				needSuffix = 0;
				break;
			}
			++posn;
		}
		if(needSuffix)
		{
			baseName = (char *)ILMalloc(namelen + 4);
			if(!baseName)
			{
				return 0;
			}
			ILMemCpy(baseName, name, namelen);
			baseName[namelen++] = '.';
			baseName[namelen++] = 's';
			baseName[namelen++] = 'o';
			baseName[namelen] = '\0';
		}
		else
		{
			baseName = (char *)ILMalloc(namelen + 1);
			if(!baseName)
			{
				return 0;
			}
			ILMemCpy(baseName, name, namelen);
			baseName[namelen] = '\0';
		}
	}
#else
	if(namelen < 4 || name[namelen - 4] != '.' ||
	   (name[namelen - 3] != 'd' && name[namelen - 3] != 'D') ||
	   (name[namelen - 2] != 'l' && name[namelen - 2] != 'L') ||
	   (name[namelen - 1] != 'l' && name[namelen - 1] != 'L'))
	{
		baseName = (char *)ILMalloc(namelen + 5);
		if(!baseName)
		{
			return 0;
		}
		ILMemCpy(baseName, name, namelen);
		strcpy(baseName + namelen, ".dll");
	}
	else
	{
		baseName = (char *)ILMalloc(namelen + 1);
		if(!baseName)
		{
			return 0;
		}
		ILMemCpy(baseName, name, namelen);
		baseName[namelen] = '\0';
	}
#endif

	/* If the name already includes a directory specification,
	   then assume that this is the path we are looking for */
	name = baseName;
	while(*name != '\0')
	{
		if(*name == '/' || *name == '\\')
		{
			return baseName;
		}
		++name;
	}

	/* Look in the same directory as the assembly first,
	   in case the programmer has shipped the native library
	   along-side the assembly that imports it */
	if((name = pinvoke->member.programItem.image->filename) != 0)
	{
		namelen = strlen(name);
		while(namelen > 0 && name[namelen - 1] != '/' &&
			  name[namelen - 1] != '\\')
		{
			--namelen;
		}
		if(namelen <= 0)
		{
			/* The assembly was loaded from the current directory */
			name = "./";
			namelen = 2;
		}
		fullName = (char *)ILMalloc(namelen + strlen(baseName) + 1);
		if(fullName)
		{
			ILMemCpy(fullName, name, namelen);
			fullName[namelen++] = '/';
			strcpy(fullName + namelen, baseName);
			if(ILFileExists(fullName, (char **)0))
			{
				ILFree(baseName);
				return fullName;
			}
			ILFree(fullName);
		}
	}

	/* Look in the user-specified library search directories */
	context = pinvoke->member.programItem.image->context;
	for(posn = 0; posn < context->numLibraryDirs; ++posn)
	{
		fullName = TestAssemblyPath(context->libraryDirs[posn],
									strlen(context->libraryDirs[posn]),
									baseName, strlen(baseName), 0);
		if(fullName)
		{
			ILFree(baseName);
			return fullName;
		}
	}

	/* Assume that the library is somewhere on the standard
	   search path and let "ILDynLibraryOpen" do the hard work
	   of finding it later */
	return baseName;
}

#ifdef	__cplusplus
};
#endif
