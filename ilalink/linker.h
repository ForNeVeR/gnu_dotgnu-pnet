/*
 * linker.h - Internal definitions for image linking.
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

#ifndef	_ILALINK_LINKER_H
#define	_ILALINK_LINKER_H

#include "il_linker.h"
#include "il_program.h"
#include "il_writer.h"
#include "il_program.h"
#include "il_system.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Information about a class in a library assembly.
 */
typedef struct _tagILLibraryClass ILLibraryClass;
struct _tagILLibraryClass
{
	char		   *name;			/* Intern'ed name of the class */
	char		   *namespace;		/* Intern'ed namespace of the class */
	ILLibraryClass *parent;			/* Parent for nesting purposes */
	ILLibraryClass *next;			/* Next in the hash table */

};

/*
 * Information that is stored for a library assembly.
 */
typedef struct _tagILLibrary ILLibrary;
struct _tagILLibrary
{
	char		   *name;			/* Name of the library's assembly */
	ILUInt16		version[4];		/* Version of the library's assembly */
	ILLibrary	   *altNames;		/* Alternative names for the library */
	ILLibraryClass **hashTable;		/* Hash table for class name lookup */
	ILMemPool		classPool;		/* Memory pool for class allocation */
	ILLibrary	   *next;			/* Next library used by the linker */

};

/*
 * Internal structure of the linker context.
 */
struct _tagILLinker
{
	ILContext	   *context;		/* Main context for the final image */
	ILImage        *image;			/* Final image that is being built */
	ILWriter	   *writer;			/* Writer being used by the linker */
	ILLibrary	   *libraries;		/* Libraries being used by the linker */
	ILLibrary      *lastLibrary;	/* Last library being used by the linker */
	int				outOfMemory;	/* Set to non-zero when out of memory */
	int				error;			/* Some other error occurred */

};

/*
 * Context that is used to find a class within the libraries.
 */
typedef struct
{
	ILLinker	   *linker;			/* Linker to use */
	ILLibrary	   *library;		/* Library to look within */
	ILLibraryClass *libClass;		/* Data pertaining to the class */
	ILLibraryClass *prevClass;		/* Previous class in find context */

} ILLibraryFind;

/*
 * Report that the linker is out of memory.
 */
void _ILLinkerOutOfMemory(ILLinker *linker);

/*
 * Destroy the libraries associated with a linker.
 */
void _ILLinkerDestroyLibraries(ILLinker *linker);

/*
 * Find a library given its assembly name.
 */
ILLibrary *_ILLinkerFindLibrary(ILLinker *linker, const char *name);

/*
 * Initialize a library find context.  If "library" is
 * NULL, then any library with the class will be used.
 */
void _ILLinkerFindInit(ILLibraryFind *find, ILLinker *linker,
					   ILLibrary *library);

/*
 * Locate a class within a library find context.
 */
int _ILLinkerFindClass(ILLibraryFind *find, const char *name,
					   const char *namespace);

/*
 * Print the name of a class that could not be found.
 */
void _ILLinkerPrintClass(ILLibraryFind *find, const char *name,
						 const char *namespace);

/*
 * Make a TypeRef for a class that was found within "image".
 */
ILClass *_ILLinkerMakeTypeRef(ILLibraryFind *find, ILImage *image);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILALINK_LINKER_H */
