/*
 * pecoff_loader.c - Deal with the ugly parts of loading PE/COFF images.
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

#include "image.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Seek to a particular offset within a stream by reading
 * and discarding data.  This is designed to work on streams
 * that may not necessarily be file-based.  Returns zero if
 * the seek failed.
 */
static int SeekWithinStream(FILE *file, char *buffer,
							unsigned long currentOffset,
							unsigned long destOffset)
{
	unsigned size;
	while(currentOffset < destOffset)
	{
		if((destOffset - currentOffset) > 1024)
		{
			size = 1024;
		}
		else
		{
			size = (unsigned)(destOffset - currentOffset);
		}
		if(fread(buffer, 1, size, file) != size)
		{
			return 0;
		}
		currentOffset += size;
	}
	return (currentOffset == destOffset);
}

/*
 * Convert a virtual address into a real address using a section map.
 * Returns IL_BAD_ADDRESS if the virtual address is outside all sections.
 */
static unsigned long ConvertVirtAddrToReal(ILSectionMap *map,
									   	   unsigned long addr)
{
	while(map != 0)
	{
		if(addr >= map->virtAddr &&
		   addr < (map->virtAddr + map->virtSize))
		{
			return (addr - map->virtAddr + map->realAddr);
		}
		map = map->next;
	}
	return IL_BAD_ADDRESS;
}

/*
 * Free a section map.
 */
void _ILFreeSectionMap(ILSectionMap *map)
{
	ILSectionMap *next;
	while(map != 0)
	{
		next = map->next;
		ILFree(map);
		map = next;
	}
}

/*
 * Find the top of a virtual address range that contains
 * a particular address.  Returns zero if none of the
 * ranges contain the address.
 */
static unsigned long TopOfVirtualRange(ILSectionMap *map, unsigned long addr)
{
	while(map != 0)
	{
		if(addr >= map->virtAddr &&
		   addr < (map->virtAddr + map->virtSize))
		{
			return map->virtAddr + map->virtSize;
		}
		map = map->next;
	}
	return 0;
}

void _ILImageFreeMemory(ILImage *image)
{
	if(image->mapped)
	{
		ILUnmapFileFromMemory(image->mapAddress, image->mapLength);
	}
	else if(image->data)
	{
		ILFree(image->data);
	}
}

int ILImageLoad(FILE *file, const char *filename,
				ILContext *context, ILImage **image, int flags)
{
	char buffer[1024];
	int isDLL;
	int isOBJ = 0;
	int hadNative;
	int only32Bit = 0;
	unsigned headerSize;
	unsigned numSections;
	unsigned long numDirectories;
	unsigned long virtualBase;
	unsigned long base;
	unsigned long size;
	unsigned long runtimeHdrSize;
	unsigned long offset;
	unsigned long minAddress;
	unsigned long maxAddress;
	unsigned long debugRVA;
	unsigned long debugSize;
	ILSectionMap *map;
	ILSectionMap *newMap;
	char *data;
	unsigned char *runtimeHdr;
	int isMapped;
	void *mapAddress;
	unsigned long mapLength;
	int error;

	/* Read the first 2 bytes and look for either an MS-DOS
	   stub (executables and DLL's), or the beginning of a
	   PE/COFF header (object files) */
	if(fread(buffer, 1, 2, file) != 2)
	{
		return IL_LOADERR_TRUNCATED;
	}
	if(buffer[0] == 'M' && buffer[1] == 'Z')
	{
		/* Read the MS-DOS stub and find the start of the PE header */
		if(fread(buffer + 2, 1, 62, file) != 62)
		{
			return IL_LOADERR_TRUNCATED;
		}
		offset = 64;
		base = IL_READ_UINT32(buffer + 60);
		if(base < offset)
		{
			return IL_LOADERR_BACKWARDS;
		}
		if(!SeekWithinStream(file, buffer, offset, base))
		{
			return IL_LOADERR_TRUNCATED;
		}
		offset = base;
		if(fread(buffer, 1, 4, file) != 4)
		{
			return IL_LOADERR_TRUNCATED;
		}
		offset += 4;
		if(buffer[0] != 'P' || buffer[1] != 'E' ||
		   buffer[2] != '\0' || buffer[3] != '\0')
		{
			return IL_LOADERR_NOT_PE;
		}
		if(fread(buffer, 1, 20, file) != 20)
		{
			return IL_LOADERR_TRUNCATED;
		}
		offset += 20;
	}
	else if(buffer[0] == (char)0x4C && buffer[1] == (char)0x01)
	{
		/* This is an i386 PE/COFF object file: read the rest of the header */
		if(fread(buffer + 2, 1, 18, file) != 18)
		{
			return IL_LOADERR_TRUNCATED;
		}
		offset = 20;
		isOBJ = 1;
	}
#ifndef IL_NO_JAVA
	else if((buffer[0] == (char)0xCA && buffer[1] == (char)0xFE) ||
	        (buffer[0] == 'P' && buffer[1] == 'K'))
	{
		/* This looks like a Java ".class" or ".jar" file, which
		   we need to pass off to "_ILImageJavaLoad" to handle */
		return _ILImageJavaLoad(file, filename, context, image, flags, buffer);
	}
#endif
	else
	{
		/* Unknown file format */
		return IL_LOADERR_NOT_PE;
	}

	/* Extract interesting information from the PE/COFF header */
	isDLL = ((IL_READ_UINT16(buffer + 18) & 0x2000) != 0);
	headerSize = (unsigned)(IL_READ_UINT16(buffer + 16));
	numSections = (unsigned)(IL_READ_UINT16(buffer + 2));
	if(headerSize != 0 && (headerSize < 216 || headerSize > 1024))
	{
		return IL_LOADERR_NOT_IL;
	}
	if(numSections == 0)
	{
		return IL_LOADERR_NOT_IL;
	}

	/* Read the optional header into memory.  This should contain
	   the data directory information for the IL runtime header */
	if(headerSize != 0)
	{
		if(fread(buffer, 1, headerSize, file) != headerSize)
		{
			return IL_LOADERR_TRUNCATED;
		}
		offset += headerSize;
		if(buffer[0] != 0x0B || buffer[1] != 0x01)
		{
			return IL_LOADERR_NOT_PE;
		}
		numDirectories = IL_READ_UINT32(buffer + 92);
		if(numDirectories < 15)
		{
			return IL_LOADERR_NOT_IL;
		}
		base = IL_READ_UINT32(buffer + 208);
		runtimeHdrSize = IL_READ_UINT32(buffer + 212);
		if(runtimeHdrSize < 48 || runtimeHdrSize > 1024)
		{
			return IL_LOADERR_NOT_IL;
		}
	}
	else
	{
		/* We don't have an optional header, so we will need to
		   extract the runtime header from the ".text$il" section */
		base = 0;
		runtimeHdrSize = 0;
	}

	/* Read the COFF section table.  We need this to be able
	   to convert virtual addresses into file seek offsets */
	map = 0;
	minAddress = (unsigned long)IL_MAX_UINT32;
	maxAddress = 0;
	debugRVA = 0;
	debugSize = 0;
	while(numSections > 0)
	{
		if(fread(buffer, 1, 40, file) != 40)
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_TRUNCATED;
		}
		offset += 40;
		if((newMap = (ILSectionMap *)ILMalloc(sizeof(ILSectionMap))) == 0)
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_MEMORY;
		}
		newMap->virtAddr = IL_READ_UINT32(buffer + 12);
		newMap->virtSize = IL_READ_UINT32(buffer + 8);
		newMap->realAddr = IL_READ_UINT32(buffer + 20);
		newMap->realSize = IL_READ_UINT32(buffer + 16);
		if(newMap->virtSize > newMap->realSize)
		{
			/* Paranoia check - usually won't happen */
			newMap->virtSize = newMap->realSize;
		}
		else if(!(newMap->virtSize))
		{
			/* Object files may set this field to zero */
			newMap->virtSize = newMap->realSize;
		}
		if(((newMap->virtAddr + newMap->virtSize) &
					(unsigned long)IL_MAX_UINT32) < newMap->virtAddr)
		{
			/* The virtual size is too big */
			_ILFreeSectionMap(map);
			return IL_LOADERR_NOT_PE;
		}
		if(((newMap->realAddr + newMap->realSize) &
					(unsigned long)IL_MAX_UINT32) < newMap->realAddr)
		{
			/* The real size is too big */
			_ILFreeSectionMap(map);
			return IL_LOADERR_NOT_PE;
		}
		newMap->next = map;
		map = newMap;
		if(!runtimeHdrSize && !ILMemCmp(buffer, ".text$il", 8))
		{
			/* We are processing an object file that has the
			   IL data embedded within the ".text$il" section */
			base = newMap->virtAddr;
		}
		else if(!ILMemCmp(buffer, ".ildebug", 8))
		{
			debugRVA = newMap->virtAddr;
			debugSize = newMap->virtSize;
		}
		if(newMap->realAddr < minAddress)
		{
			minAddress = newMap->realAddr;
		}
		if((newMap->realAddr + newMap->realSize) > maxAddress)
		{
			maxAddress = newMap->realAddr + newMap->realSize;
		}
		--numSections;
	}

	/* If the maximum address is less than the minimum, then there
	   are no sections in the file, and it cannot possibly be IL */
	if(maxAddress <= minAddress)
	{
		_ILFreeSectionMap(map);
		return IL_LOADERR_NOT_IL;
	}

	/* If we don't have a runtime header yet, then bail out.
	   This can happen if we are processing an object file that
	   does not have a ".text$il" section within it */
	if(!runtimeHdrSize && !base)
	{
		_ILFreeSectionMap(map);
		return IL_LOADERR_NOT_IL;
	}

	/* Seek to the beginning of the first section */
	if(!SeekWithinStream(file, buffer, offset, minAddress))
	{
		_ILFreeSectionMap(map);
		return IL_LOADERR_TRUNCATED;
	}

	/* Map the contents of every section into memory.  We would like
	   to only map those parts of the file that are relevant to IL, but
	   there is too much variation in how compilers lay out binaries.
	   In particular, the IL bytecode can be pretty much anywhere */
	if(ILMapFileToMemory(fileno(file), minAddress, maxAddress,
					     &mapAddress, &mapLength, &data))
	{
		isMapped = 1;
	}
	else
	{
		/* Read the IL program data into memory */
		if((data = (char *)ILMalloc(maxAddress - minAddress)) == 0)
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_MEMORY;
		}
		if(fread(data, 1, maxAddress - minAddress, file) !=
					(maxAddress - minAddress))
		{
			ILFree(data);
			_ILFreeSectionMap(map);
			return IL_LOADERR_TRUNCATED;
		}
		isMapped = 0;
		mapAddress = 0;
		mapLength = 0;
	}

	/* Adjust the section map to account for the new location of the program */
	newMap = map;
	while(newMap != 0)
	{
		newMap->realAddr -= minAddress;
		newMap = newMap->next;
	}

	/* Convert the virtual address of the runtime header into a real address */
	virtualBase = base;
	base = ConvertVirtAddrToReal(map, base);
	if(base == IL_BAD_ADDRESS)
	{
		_ILFreeSectionMap(map);
		return IL_LOADERR_BAD_ADDR;
	}

	/* Find the IL runtime header, read it, and validate it */
	runtimeHdr = data + base;
	size = (maxAddress - minAddress) - base;
	if(runtimeHdrSize)
	{
		/* We already know how big the runtime header is from
		   the contents of the optional header */
		if(size < runtimeHdrSize)
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_TRUNCATED;
		}
		if(IL_READ_UINT32(runtimeHdr) != runtimeHdrSize)
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_NOT_IL;
		}
	}
	else
	{
		/* We are processing an object file that stores the runtime
		   header at the start of the ".text$il" section */
		if(size < 4)
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_TRUNCATED;
		}
		runtimeHdrSize = IL_READ_UINT32(runtimeHdr);
		if(runtimeHdrSize < 48 || runtimeHdrSize > 1024)
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_NOT_IL;
		}
		if(size < runtimeHdrSize)
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_TRUNCATED;
		}
	}
	if(IL_READ_UINT16(runtimeHdr + 4) != 2 ||
	   IL_READ_UINT16(runtimeHdr + 6) != 0)
	{
		_ILFreeSectionMap(map);
		return IL_LOADERR_VERSION;
	}
	hadNative = ((IL_READ_UINT32(runtimeHdr + 16) & 0x00000001) == 0);
#ifdef IL_NATIVE_INT64
	if((IL_READ_UINT32(runtimeHdr + 16) & 0x00000002) != 0)
	{
		if((flags & IL_LOADFLAG_FORCE_32BIT) != 0)
		{
			only32Bit = 1;
		}
		else
		{
			_ILFreeSectionMap(map);
			return IL_LOADERR_32BIT_ONLY;
		}
	}
#endif

	/* Create and populate the ILImage structure */
	if((*image = _ILImageCreate(context, sizeof(ILImage))) == 0)
	{
		if(isMapped)
		{
			ILUnmapFileFromMemory(mapAddress, mapLength);
		}
		else
		{
			ILFree(data);
		}
		_ILFreeSectionMap(map);
		return IL_LOADERR_MEMORY;
	}
	if(filename)
	{
		/* Save the filename for later use in dynamic linking */
		(*image)->filename = ILDupString(filename);
		if(!((*image)->filename))
		{
			if(isMapped)
			{
				ILUnmapFileFromMemory(mapAddress, mapLength);
			}
			else
			{
				ILFree(data);
			}
			_ILFreeSectionMap(map);
			return IL_LOADERR_MEMORY;
		}
	}
	(*image)->type = (isOBJ ? IL_IMAGETYPE_OBJ :
						(isDLL ? IL_IMAGETYPE_DLL : IL_IMAGETYPE_EXE));
	(*image)->hadNative = hadNative;
	(*image)->only32Bit = only32Bit;
	(*image)->mapped = isMapped;
	(*image)->map = map;
	(*image)->data = data;
	(*image)->len = (maxAddress - minAddress);
	(*image)->mapAddress = mapAddress;
	(*image)->mapLength = mapLength;
	(*image)->headerAddr = virtualBase;
	(*image)->headerSize = runtimeHdrSize;
	(*image)->realStart = minAddress;
	(*image)->debugRVA = debugRVA;
	(*image)->debugSize = debugSize;

	/* Load the meta information from the image */
	if((flags & IL_LOADFLAG_NO_METADATA) == 0)
	{
		if((error = _ILImageParseMeta(*image, filename, flags)) != 0)
		{
			ILImageDestroy(*image);
			return error;
		}
	}

	/* The image is loaded and ready to go */
	return 0;
}

int ILImageLoadFromFile(const char *filename, ILContext *context,
						ILImage **image, int flags, int printErrors)
{
	FILE *file;
	int closeStream;
	int loadError;

	/* Open the specified file */
	if(!strcmp(filename, "-"))
	{
		file = stdin;
		closeStream = 0;
	}
	else if((file = fopen(filename, "rb")) == NULL)
	{
		/* Try again, in case libc does not understand "rb" */
		if((file = fopen(filename, "r")) == NULL)
		{
			if(printErrors)
			{
				perror(filename);
			}
			return -1;
		}
		closeStream = 1;
	}
	else
	{
		closeStream = 1;
	}

	/* Load the file as an image */
	loadError = ILImageLoad(file, filename, context, image, flags);
	if(closeStream)
	{
		fclose(file);
	}

	/* Report errors to stderr, if necessary */
	if(loadError != 0 && printErrors)
	{
		fprintf(stderr, "%s: %s\n", (closeStream ? filename : "stdin"),
				ILImageLoadError(loadError));
	}

	/* Done */
	return loadError;
}

void *ILImageMapAddress(ILImage *image, unsigned long address)
{
	unsigned long realAddr = ConvertVirtAddrToReal(image->map, address);
	if(realAddr != IL_BAD_ADDRESS)
	{
		return (void *)(image->data + realAddr);
	}
	else
	{
		return (void *)0;
	}
}

void *ILImageMapRVA(ILImage *image, unsigned long rva, unsigned long *len)
{
	ILSectionMap *map = image->map;
	while(map != 0)
	{
		if(rva >= map->virtAddr &&
		   rva < (map->virtAddr + map->virtSize))
		{
			/* The RVA is within this PE/COFF section */
			rva = rva - map->virtAddr + map->realAddr;
			*len = map->realSize - (rva - map->realAddr);
			return (void *)(image->data + rva);
		}
		map = map->next;
	}
	return 0;
}

unsigned long ILImageRealOffset(ILImage *image, unsigned long address)
{
	unsigned long realAddr = ConvertVirtAddrToReal(image->map, address);
	if(realAddr != IL_BAD_ADDRESS)
	{
		return image->realStart + realAddr;
	}
	else
	{
		return 0;
	}
}

/*
 * Range data structure.
 */
#define	IL_MAX_RANGES		16
typedef struct
{
	unsigned long	start;
	unsigned long	end;

} ILRange;
typedef struct
{
	ILRange	range[IL_MAX_RANGES];
	int		numRanges;

} ILRanges;

/*
 * Subtract a consequetive sequence of bytes from a range list.
 */
static void SubtractFromRange(ILRanges *ranges,
							  unsigned long start,
							  unsigned long end)
{
	int posn = 0;
	while(posn < ranges->numRanges)
	{
		if(start <= ranges->range[posn].start &&
		   end > ranges->range[posn].start)
		{
			/* Overlap with the start of the range, so shorten at the start */
			if(end >= ranges->range[posn].end)
			{
				ranges->range[posn].start = ranges->range[posn].end;
			}
			else
			{
				ranges->range[posn].start = end;
			}
		}
		else if(start < ranges->range[posn].end &&
		        end >= ranges->range[posn].end)
		{
			/* Overlap with the end of the range, so shorten at the end */
			if(start <= ranges->range[posn].start)
			{
				ranges->range[posn].end = ranges->range[posn].start;
			}
			else
			{
				ranges->range[posn].end = start;
			}
		}
		else if(start >= ranges->range[posn].start &&
				end <= ranges->range[posn].end)
		{
			/* Overlap with the middle of the range, so split into two.
			   If we have run out of space, then leave the list as-is */
			if(ranges->numRanges < IL_MAX_RANGES)
			{
				ranges->range[ranges->numRanges].start = end;
				ranges->range[ranges->numRanges].end = ranges->range[posn].end;
				ranges->range[posn].end = start;
				++(ranges->numRanges);
			}
		}
		++posn;
	}
}

/*
 * Trim any range that ends at a particular high address.
 */
static void TrimHighRange(ILRanges *ranges, unsigned long address)
{
	int posn;
	for(posn = 0; posn < ranges->numRanges; ++posn)
	{
		if(ranges->range[posn].end == address)
		{
			ranges->range[posn].start = ranges->range[posn].end;
		}
	}
}

/*
 * Find the largest range that still exists.  This is probably the code.
 */
static void FindLargestRange(ILRanges *ranges, unsigned long *start,
							 unsigned long *size)
{
	unsigned long end;
	unsigned long newSize;
	int posn;
	*start = ranges->range[0].start;
	end = ranges->range[0].end;
	*size = (end - *start);
	for(posn = 1; posn < ranges->numRanges; ++posn)
	{
		newSize = ranges->range[posn].end - ranges->range[posn].start;
		if(newSize > *size)
		{
			*start = ranges->range[posn].start;
			end = ranges->range[posn].end;
		}
	}
}

int _ILImageGetSection(ILImage *image, int section,
				  	   unsigned long *address, unsigned long *size)
{
	unsigned char *runtimeHdr;
	unsigned char *probe;

	/* Find the runtime header */
	runtimeHdr = (unsigned char *)ILImageMapAddress(image, image->headerAddr);
	if(!runtimeHdr)
	{
		return 0;
	}

	/* Find the section from the index information in the runtime header */
	switch(section)
	{
		case IL_SECTION_HEADER:
		{
			/* Get the address and size of the IL runtime header */
			*address = image->headerAddr;
			*size = image->headerSize;
		}
		break;

		case IL_SECTION_CODE:
		{
			/* The code section is the odd one out in IL binaries,
			   because it doesn't actually have a header field
			   associated with it.  We start with the full range
			   and subtract sections we know about.  Whatever is
			   left must be the code section */
			ILRanges ranges;
			unsigned long addrLowest;
			unsigned long addrHighest;
			unsigned long addrTest;
			unsigned long sizeTest;
			unsigned long headerPosn;

			/* Find the initial lower and upper bounds on the entire image */
			addrLowest = image->headerAddr + image->headerSize;
			addrHighest = TopOfVirtualRange(image->map, addrLowest);
			ranges.range[0].start = addrLowest;
			ranges.range[0].end   = addrHighest;
			ranges.numRanges = 1;

			/* Subtract out the resources section, if present */
			addrTest = IL_READ_UINT32(runtimeHdr + 24);
			sizeTest = IL_READ_UINT32(runtimeHdr + 28);
			if(addrTest != 0 && sizeTest != 0 && addrTest < addrHighest)
			{
				if((addrTest + 136) <= addrHighest && sizeTest >= 136)
				{
					/* It looks like there were bugs in some very early
					   code generators that output the start of the
					   resource section as 128 bytes before it actually
					   starts.  This overlaps with the end of the code
					   section, making it appear as though the code section
					   is truncated.  We probe the resources to see if this
					   is the case.  If so, we add an extra 128 bytes to
					   the code section size.  Yuk!! */
					probe = ILImageMapAddress(image, addrTest);
					if(probe &&
					   probe[132] == (unsigned char)0xCE &&
					   probe[133] == (unsigned char)0xCA &&
					   probe[134] == (unsigned char)0xEF &&
					   probe[135] == (unsigned char)0xBE)
					{
						/* We found the magic number, so adjust the size */
						addrTest += 128;
						sizeTest -= 128;
					}
				}
			}
			SubtractFromRange(&ranges, addrTest, addrTest + sizeTest);

			/* Subtract out the metadata section, if present */
			addrTest = IL_READ_UINT32(runtimeHdr + 8);
			sizeTest = IL_READ_UINT32(runtimeHdr + 12);
			if(addrTest != 0 && sizeTest != 0)
			{
				SubtractFromRange(&ranges, addrTest, addrTest + sizeTest);
			}

			/* Subtract out other sections, starting at the
			   strong name signature section */
			headerPosn = 32;
			while((headerPosn + 8) <= image->headerSize)
			{
				addrTest = IL_READ_UINT32(runtimeHdr + headerPosn);
				sizeTest = IL_READ_UINT32(runtimeHdr + headerPosn + 4);
				if(addrTest != 0 && sizeTest != 0)
				{
					SubtractFromRange(&ranges, addrTest, addrTest + sizeTest);
				}
				headerPosn += 8;
			}

			/* Trim any range that ends at "addrHighest", because
			   that will contain left-over data of no interest */
			TrimHighRange(&ranges, addrHighest);

			/* Return whatever is left to the caller */
			FindLargestRange(&ranges, &addrTest, &sizeTest);
			if(!sizeTest)
			{
				/* Somehow we ended up with no data at all.  This
				   may happen if the code is in the range that we
				   trimmed above.  This is a very rare case, but if
				   it occurs, then return everything to the caller.
				   This is the safest fallback position */
				addrTest = addrLowest;
				sizeTest = addrHighest - addrLowest;
			}
			if(sizeTest)
			{
				*address = addrTest;
				*size = sizeTest;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;

		case IL_SECTION_METADATA:
		{
			/* Locate the metadata section */
			*address = IL_READ_UINT32(runtimeHdr + 8);
			*size = IL_READ_UINT32(runtimeHdr + 12);
		}
		break;

		case IL_SECTION_RESOURCES:
		{
			/* Locate the resources section */
			*address = IL_READ_UINT32(runtimeHdr + 24);
			*size = IL_READ_UINT32(runtimeHdr + 28);
			if(*size >= 136)
			{
				/* Do we need to correct for buggy code generators
				   that overlap the code and resource sections? */
				probe = ILImageMapAddress(image, *address);
				if(probe &&
				   probe[132] == (unsigned char)0xCE &&
				   probe[133] == (unsigned char)0xCA &&
				   probe[134] == (unsigned char)0xEF &&
				   probe[135] == (unsigned char)0xBE)
				{
					/* We found the magic number, so correct the size */
					*address += 128;
					*size -= 128;
				}
			}
		}
		break;

		case IL_SECTION_STRONG_NAMES:
		{
			/* Locate the strong name signature section */
			*address = IL_READ_UINT32(runtimeHdr + 32);
			*size = IL_READ_UINT32(runtimeHdr + 36);
		}
		break;

		case IL_SECTION_CODE_MANAGER:
		{
			/* Locate the code manager table section */
			*address = IL_READ_UINT32(runtimeHdr + 40);
			*size = IL_READ_UINT32(runtimeHdr + 44);
		}
		break;

		case IL_SECTION_DEBUG:
		{
			/* Debug information section */
			*address = image->debugRVA;
			*size = image->debugSize;
		}
		break;

		default: return 0;
	}
	return (*address != 0 && *size != 0);
}

#ifdef	__cplusplus
};
#endif