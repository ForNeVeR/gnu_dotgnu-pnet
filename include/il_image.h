/*
 * il_image.h - Routines for manipulating IL executable images.
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

#ifndef	_IL_IMAGE_H
#define	_IL_IMAGE_H

#include <stdio.h>
#include "il_meta.h"
#include "il_values.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Load errors that may be returned by "ILImageLoad".
 */
#define	IL_LOADERR_TRUNCATED	1	/* File is truncated */
#define	IL_LOADERR_NOT_PE		2	/* Not a valid PE/COFF executable */
#define	IL_LOADERR_NOT_IL		3	/* Not a valid IL binary */
#define	IL_LOADERR_VERSION		4	/* Wrong IL version */
#define	IL_LOADERR_32BIT_ONLY	5	/* Program requires a 32-bit runtime */
#define	IL_LOADERR_BACKWARDS	6	/* Required sections in reverse order */
#define	IL_LOADERR_MEMORY		7	/* Not enough memory to load the image */
#define	IL_LOADERR_BAD_ADDR		8	/* Bad addresses in the image */
#define	IL_LOADERR_BAD_META		9	/* Something wrong with the metadata */
#define	IL_LOADERR_UNDOC_META	10	/* Uses undocumented metadata feature */
#define	IL_LOADERR_UNRESOLVED	11	/* Unresolved items in metadata */
#define	IL_LOADERR_MAX			11	/* Maximum error value */

/*
 * Flags that may be supplied to "ILImageLoad".
 */
#define	IL_LOADFLAG_FORCE_32BIT		1	/* Force 32-bit only images to load */
#define	IL_LOADFLAG_NO_METADATA		2	/* Don't parse the metadata */
#define	IL_LOADFLAG_PRE_VALIDATE	4	/* Pre-validate the token metadata */
#define	IL_LOADFLAG_NO_RESOLVE		8	/* Don't resolve external references */

/*
 * Image types.
 */
#define	IL_IMAGETYPE_DLL		0	/* PE/COFF dynamic link library */
#define	IL_IMAGETYPE_EXE		1	/* PE/COFF executable */
#define	IL_IMAGETYPE_OBJ		2	/* PE/COFF object file */
#define	IL_IMAGETYPE_BUILDING	3	/* Building an in-memory image */
#define	IL_IMAGETYPE_JAVA		4	/* Java .class or .jar file */

/*
 * Section identifiers for "ILImageGetSection".
 */
#define	IL_SECTION_HEADER		0	/* Raw IL runtime header */
#define	IL_SECTION_CODE			1	/* Code section */
#define	IL_SECTION_METADATA		2	/* Metadata section */
#define	IL_SECTION_RESOURCES	3	/* Resources section */
#define	IL_SECTION_STRONG_NAMES	4	/* StrongNameSignature section */
#define	IL_SECTION_CODE_MANAGER	5	/* Code Manager Table section */
#define	IL_SECTION_DEBUG		6	/* Debug section */

/*
 * Opaque data structure for a program context, that contains
 * all of the loaded images.
 */
typedef struct _tagILContext ILContext;

/*
 * Opaque data structure for a loaded executable image.
 */
typedef struct _tagILImage ILImage;

/*
 * Token identifier.
 */
typedef ILUInt32	ILToken;

/*
 * Create an IL context into which multiple images can be loaded.
 * Returns NULL if out of memory.
 */
ILContext *ILContextCreate(void);

/*
 * Destroy an IL context, and any remaining images associated with it.
 */
void ILContextDestroy(ILContext *context);

/*
 * Get an image from a context that has a particular module name.
 * Returns NULL if no such image.
 */
ILImage *ILContextGetModule(ILContext *context, const char *name);

/*
 * Get an image from a context that has a particular assembly name.
 * Returns NULL if no such image.
 */
ILImage *ILContextGetAssembly(ILContext *context, const char *name);

/*
 * Create an IL image.  This is typically used by compilers
 * when building an image in-memory in preparation for writing
 * it to an object file or executable.  Loaders should use
 * "ILImageLoad" instead.  Returns NULL if out of memory.
 */
ILImage *ILImageCreate(ILContext *context);

/*
 * Load an IL image into memory.  The specified "file" can
 * be a non-seekable stream.  The file is assumed to be positioned
 * at the beginning of the stream.  Returns 0 if OK (with an
 * image descriptor in "*image"), or an error code otherwise.
 * If "filename" is non-NULL, it is used to locate the directory
 * from which the image is loaded for linking purposes.
 */
int ILImageLoad(FILE *file, const char *filename, ILContext *context,
				ILImage **image, int flags);

/*
 * Destroy an IL image and all memory associated with it.
 */
void ILImageDestroy(ILImage *image);

/*
 * Get the context associated with an IL image.
 */
ILContext *ILImageToContext(ILImage *image);

/*
 * Get a loaded image's type.
 */
int ILImageType(ILImage *image);

/*
 * Determine if the loaded image had non-stub native code
 * associated with it, which was stripped during loading.
 */
int ILImageHadNative(ILImage *image);

/*
 * Determine if the loaded image was 32-bit only, but the image
 * was forced to load anyway.
 */
int ILImageIs32Bit(ILImage *image);

/*
 * Get the length of IL image.
 */
unsigned long ILImageLength(ILImage *image);

/*
 * Map a virtual address to a real address within a loaded image.
 * Returns NULL if the virtual address is invalid for the image.
 */
void *ILImageMapAddress(ILImage *image, unsigned long address);

/*
 * Map a method or data RVA to an address and a maximum
 * length.  The method or data begins at the return value
 * and can occupy up to "*len" bytes within the image.
 * Returns NULL if the RVA is invalid.  This function works
 * even if "ILImageGetSection" cannot find the code section.
 */
void *ILImageMapRVA(ILImage *image, unsigned long rva, unsigned long *len);

/*
 * Get the real file seek offset that corresponds to a virtual
 * address within a loaded image.  Returns 0 if the virtual
 * address is invalid for the image.
 */
unsigned long ILImageRealOffset(ILImage *image, unsigned long address);

/*
 * Get the virtual address of a particular section.
 */
unsigned long ILImageGetSectionAddr(ILImage *image, int section);

/*
 * Get the address and size of a particular image section.
 * Returns zero if the section was not found.
 */
int ILImageGetSection(ILImage *image, int section,
					  void **address, unsigned long *size);

/*
 * Get the entry point token for the image.  Returns zero if
 * there is no entry point token.
 */
ILToken ILImageGetEntryPoint(ILImage *image);

/*
 * Get a specific entry from the directory that is stored
 * in the metadata section of an IL image.  Returns the address
 * of the entry, or NULL if the entry is not present.  The size
 * of the entry is returned in "*size" if the entry is found.
 */
void *ILImageGetMetaEntry(ILImage *image, const char *name,
						  unsigned long *size);

/*
 * Get the number of entries in the directory that is stored
 * in the metadata section of an IL image.
 */
unsigned long ILImageNumMetaEntries(ILImage *image);

/*
 * Information about a numbered entry in the directory that is
 * stored in the metadata section of an IL image.  The address of
 * the data is returned, or NULL if the entry number is invalid.
 */
void *ILImageMetaEntryInfo(ILImage *image, unsigned long entry,
						   char **name, unsigned long *virtAddr,
						   unsigned long *size);

/*
 * Get the size in bytes of the directory header that is stored in
 * the metadata section of an IL image.
 */
unsigned long ILImageMetaHeaderSize(ILImage *image);

/*
 * Get a string from the string pool.  Returns NULL if "offset" is invalid.
 * The return pointer is guaranteed to be fixed for the lifetime of the image.
 */
const char *ILImageGetString(ILImage *image, unsigned long offset);

/*
 * Add a string to the string pool.  This can only be used if
 * the image is being built.  Returns the offset, or zero if
 * the string is empty or we are out of memory.
 */
unsigned long ILImageAddString(ILImage *image, const char *str);

/*
 * Get a blob from the blob pool.  Returns NULL if "offset" is invalid.
 * The return pointer is guaranteed to be fixed for the lifetime of the image.
 * The length of the blob is returned in "*len".
 */
const void *ILImageGetBlob(ILImage *image, unsigned long offset,
						   unsigned long *len);

/*
 * Add a blob to the blob pool.  This can only be used if
 * the image is being built.  Returns the offset, or zero if
 * we are out of memory.
 */
unsigned long ILImageAddBlob(ILImage *image, const void *blob,
							 unsigned long len);

/*
 * Get a Unicode string from the user string pool.  Returns NULL if
 * "offset" is invalid.  The return pointer is guaranteed to be
 * fixed for the lifetime of the image.  The number of characters in
 * the string is returned in "*len".  The characters themselves are
 * stored in little-endian order beginning at the return pointer.
 */
const char *ILImageGetUserString(ILImage *image, unsigned long offset,
								 unsigned long *len);

/*
 * Add a UTF-8 string to the user string pool.  This can only be
 * used if the image is being built.  Returns the offset, or zero
 * if we are out of memory.
 */
unsigned long ILImageAddUserString(ILImage *image, const char *str, int len);

/*
 * Get the module name from an image.  Returns NULL if no such name.
 * If there are multiple module names, this returns the first.
 */
const char *ILImageGetModuleName(ILImage *image);

/*
 * Get the assembly name from an image.  Returns NULL if no such name.
 * If there are multiple assembly names, this returns the first.
 */
const char *ILImageGetAssemblyName(ILImage *image);

/*
 * Get the number of tokens of a particular type.
 */
unsigned long ILImageNumTokens(ILImage *image, ILToken tokenType);

/*
 * Get the information block associated with a token.
 * Returns the block, or NULL if the token is invalid.
 */
void *ILImageTokenInfo(ILImage *image, ILToken token);

/*
 * Iterate through the tokens in a specific token table.
 */
void *ILImageNextToken(ILImage *image, ILToken tokenType, void *prev);

/*
 * Search through the tokens in a specific token table for a match.
 * Returns the information block, or NULL if no match.
 */
typedef int (*ILImageCompareFunc)(void *item, void *userData);
void *ILImageSearchForToken(ILImage *image, ILToken tokenType,
							ILImageCompareFunc compareFunc,
							void *userData);

/*
 * Get an error message string for an "IL_LOADERR_*" value.
 * Returns a default message if the value is invalid.
 */
const char *ILImageLoadError(int error);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_IMAGE_H */
