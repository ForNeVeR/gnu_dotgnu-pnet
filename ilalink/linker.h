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
#include "il_dumpasm.h"

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
	ILHashTable    *hashTable;		/* Hash table for class name lookup */
	ILMemPool		classPool;		/* Memory pool for class allocation */
	ILLibrary	   *next;			/* Next library used by the linker */

};

/*
 * Information about a directory in the linker's search order.
 */
typedef struct _tagILLibraryDir ILLibraryDir;
struct _tagILLibraryDir
{
	ILLibraryDir   *next;			/* Next in the path */
	int				len;			/* Length of the name */
	char			name[1];		/* Start of the name */

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
	ILLibraryDir   *libraryDirs;	/* List of library directories */
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

/*
 * Convert a class reference in a foreign image into a
 * reference in the output image.
 */
ILClass *_ILLinkerConvertClassRef(ILLinker *linker, ILClass *classInfo);

/*
 * Convert a member reference in a foreign image into a
 * reference in the output image.
 */
ILMember *_ILLinkerConvertMemberRef(ILLinker *linker, ILMember *member);

/*
 * Convert a type in a foreign image into a type in
 * the output image.
 */
ILType *_ILLinkerConvertType(ILLinker *linker, ILType *type);

/*
 * Convert a synthetic type reference in a foreign image into
 * a type specification in the output image.
 */
ILTypeSpec *_ILLinkerConvertTypeSpec(ILLinker *linker, ILType *type);

/*
 * Convert custom attributes from an old item in a link
 * image, and attach them to a new item in the final image.
 */
int _ILLinkerConvertAttrs(ILLinker *linker, ILProgramItem *oldItem,
						  ILProgramItem *newItem);

/*
 * Convert security declarations from an old item in a link
 * image, and attach them to a new item in the final image.
 */
int _ILLinkerConvertSecurity(ILLinker *linker, ILProgramItem *oldItem,
						     ILProgramItem *newItem);

/*
 * Convert all classes from a link image.
 */
int _ILLinkerConvertClasses(ILLinker *linker, ILImage *image);

/*
 * Convert a method from a link image into a method underneath
 * a specified new class in the final image.
 */
int _ILLinkerConvertMethod(ILLinker *linker, ILMethod *method,
						   ILClass *newClass);

/*
 * Convert a field from a link image into a field underneath
 * a specified new class in the final image.
 */
int _ILLinkerConvertField(ILLinker *linker, ILField *field,
						  ILClass *newClass);

/*
 * Convert field marshal and constant data from an old item in a link
 * image, and attach them to a new item in the final image.
 */
int _ILLinkerConvertMarshal(ILLinker *linker, ILProgramItem *oldItem,
						    ILProgramItem *newItem, int isParam);

/*
 * Convert a property from a link image into a property underneath
 * a specified new class in the final image.
 */
int _ILLinkerConvertProperty(ILLinker *linker, ILProperty *property,
						     ILClass *newClass);

/*
 * Convert an event from a link image into an event underneath
 * a specified new class in the final image.
 */
int _ILLinkerConvertEvent(ILLinker *linker, ILEvent *event,
						  ILClass *newClass);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILALINK_LINKER_H */
