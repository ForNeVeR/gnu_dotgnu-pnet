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

	/* Ready to go */
	return linker;
}

int ILLinkerDestroy(ILLinker *linker)
{
	int result;
	ILLibraryDir *libraryDir;
	ILLibraryDir *nextLibraryDir;

	/* Finalize the link by reporting any remaining unresolved references */

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

#ifdef	__cplusplus
};
#endif
