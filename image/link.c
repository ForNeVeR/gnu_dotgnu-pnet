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

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Test a specific pathname for an assembly.
 */
static char *TestAssemblyPath(const char *pathname, int pathlen,
							  const char *name, int namelen)
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
		strcpy(path + pathlen + 1 + namelen, ".dll");
		nameStart = pathlen + 1;
	}
	else
	{
		ILMemCpy(path, name, namelen);
		strcpy(path + namelen, ".dll");
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
								   ILUInt16 *version)
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
		path = TestAssemblyPath(list, len, name, namelen);
		if(path)
		{
			return path;
		}

		/* Advance to the next path */
		list += len;
	}
	return 0;
}

/*
 * Locate an assembly given its name and version.
 * Returns the malloc'ed path, or NULL if not found.
 */
static char *LocateAssembly(const char *name, ILUInt16 *version,
							const char *parentAssemblyPath)
{
	char *env;
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
	pathlen = strlen(parentAssemblyPath);
	while(pathlen > 0 && parentAssemblyPath[pathlen - 1] != '/' &&
	      parentAssemblyPath[pathlen - 1] != '\\')
	{
		--pathlen;
	}
	if(pathlen > 0)
	{
		path = TestAssemblyPath(parentAssemblyPath - 1, pathlen, name, namelen);
		if(path)
		{
			return path;
		}
	}
	else
	{
		/* The parent assembly is in the current directory */
		path = TestAssemblyPath(".", 0, name, namelen);
		if(path)
		{
			return path;
		}
	}

	/* Try looking in IL_LIBRARY_PATH for a version-specific assembly */
	env = getenv("IL_LIBRARY_PATH");
	if(env && *env != '\0')
	{
		path = SearchPathForAssembly(env, name, namelen, version);
		if(path)
		{
			return path;
		}
	}

	/* Try looking in CSCC_LIB_PATH for any assembly */
	env = getenv("CSCC_LIB_PATH");
	if(env && *env != '\0')
	{
		path = SearchPathForAssembly(env, name, namelen, 0);
		if(path)
		{
			return path;
		}
	}

	/* Could not locate the assembly */
	return 0;
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
		pathname = LocateAssembly(assem->name, assem->version, filename);
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

#ifdef	__cplusplus
};
#endif
