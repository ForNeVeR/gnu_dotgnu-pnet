/*
 * writer.c - Program writer for IL executable images.
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
#include <time.h>

#ifdef	__cplusplus
extern	"C" {
#endif

void _ILWBufferListInit(ILWBufferList *list)
{
	list->firstBuffer = 0;
	list->lastBuffer = 0;
	list->bytesUsed = 0;
	list->offset = 0;
}

void _ILWBufferListDestroy(ILWBufferList *list)
{
	ILWBuffer *buffer = list->firstBuffer;
	ILWBuffer *nextBuffer;
	while(buffer != 0)
	{
		nextBuffer = buffer->next;
		ILFree(buffer);
		buffer = nextBuffer;
	}
}

int _ILWBufferListAdd(ILWBufferList *list, const void *buffer, unsigned size)
{
	ILWBuffer *wbuffer;
	unsigned temp;

	while(size > 0)
	{
		if(!(list->lastBuffer) || list->bytesUsed >= IL_WRITE_BUFFER_SIZE)
		{
			/* We need another buffer */
			wbuffer = (ILWBuffer *)ILMalloc(sizeof(ILWBuffer));
			if(!wbuffer)
			{
				return 0;
			}
			wbuffer->next = 0;
			if(list->lastBuffer)
			{
				list->lastBuffer->next = wbuffer;
			}
			else
			{
				list->firstBuffer = wbuffer;
			}
			list->lastBuffer = wbuffer;
			list->bytesUsed = 0;
		}
		temp = size;
		if(temp > (IL_WRITE_BUFFER_SIZE - list->bytesUsed))
		{
			temp = IL_WRITE_BUFFER_SIZE - list->bytesUsed;
		}
		ILMemCpy(list->lastBuffer->data + list->bytesUsed, buffer, temp);
		buffer = (void *)(((unsigned char *)buffer) + temp);
		size -= temp;
		list->bytesUsed += temp;
		list->offset += temp;
	}

	return 1;
}

ILWriter *ILWriterCreate(FILE *stream, int seekable, int type, int flags)
{
	ILWriter *writer;

	/* Create the writer control structure */
	writer = (ILWriter *)ILMalloc(sizeof(ILWriter));
	if(!writer)
	{
		return writer;
	}

	/* Initialize the writer fields */
	writer->type = type;
	writer->flags = flags;
	writer->offset = 0;
	writer->currSeek = 0;
	writer->peOffset = 0;
	writer->optOffset = 0;
	writer->firstSection = 0;
	writer->runtimeOffset = 0;
	writer->entryPoint = 0;
	writer->sections = 0;
	writer->stream = stream;
	writer->seekable = seekable;
	writer->outOfMemory = 0;
	writer->writeFailed = 0;
	writer->debugTokens = 0;
	writer->numDebugTokens = 0;
	writer->maxDebugTokens = 0;

	/* Initialize buffer lists */
	_ILWBufferListInit(&(writer->streamBuffer));
	_ILWBufferListInit(&(writer->indexBlob));
	_ILWBufferListInit(&(writer->guidBlob));
	_ILWBufferListInit(&(writer->debugData));
	_ILWBufferListInit(&(writer->debugStrings));

	/* Initialize the fixup list */
	ILMemPoolInitType(&(writer->fixups), ILFixup, 0);
	writer->firstFixup = 0;
	writer->lastFixup = 0;

	/* Write the headers to the output stream */
	_ILWriteHeaders(writer);

	/* Ready to go now */
	return writer;
}

/*
 * Write the contents of a buffer list to the ".text" section.
 */
static void WriteWBufferList(ILWriter *writer, ILWBufferList *list)
{
	ILWBuffer *buffer = list->firstBuffer;
	while(buffer != 0)
	{
		if(buffer->next)
		{
			ILWriterTextWrite(writer, buffer->data, IL_WRITE_BUFFER_SIZE);
		}
		else
		{
			ILWriterTextWrite(writer, buffer->data, list->bytesUsed);
		}
		buffer = buffer->next;
	}
}

/*
 * Write a byte pool from an ILImage structure to a writer.
 */
static void WriteImagePool(ILWriter *writer, char *pool,
						   unsigned long poolSize,
						   ILStringBlock *poolBlocks)
{
	if(pool)
	{
		ILWriterTextWrite(writer, pool, poolSize);
	}
	else
	{
		while(poolBlocks != 0)
		{
			ILWriterTextWrite(writer, (const void *)(poolBlocks + 1),
							  poolBlocks->used);
			poolBlocks = poolBlocks->next;
		}
	}
}

/*
 * Version data to embed in the metadata header.
 * Must be padded to a multiple of 4 in size.
 */
#define	VERSION_STRING		"v1.0.2914\0\0\0"
#define	VERSION_STRING_LEN	12

void ILWriterOutputMetadata(ILWriter *writer, ILImage *image)
{
	unsigned long start;
	unsigned long current;
	int numSections;
	unsigned char header[256];
	unsigned size;
	unsigned indexOffset;
	unsigned stringsOffset;
	unsigned usOffset;
	unsigned blobOffset;
	unsigned guidOffset;

	/* Compact the TypeRef and MethodRef tables */
	if(!(writer->outOfMemory) && !(writer->writeFailed))
	{
		_ILCompactReferences(image);
	}

	/* Apply token fixups to the code section */
	_ILWriteTokenFixups(writer, image);

	/* Write all metadata structures to their respective buffers */
	_ILWriteMetadataIndex(writer, image);

	/* Align the ".text" section on a 4-byte boundary */
	ILWriterTextAlign(writer);

	/* Count the number of metadata sections that we need */
	numSections = 0;
	if(writer->indexBlob.firstBuffer)
	{
		++numSections;
	}
	if(image->stringPoolSize)
	{
		++numSections;
	}
	if(image->blobPoolSize)
	{
		++numSections;
	}
	if(image->userStringPoolSize)
	{
		++numSections;
	}
	if(writer->guidBlob.firstBuffer)
	{
		++numSections;
	}

	/* Build the header for the metadata section */
	if((writer->flags & IL_WRITEFLAG_OLD_META) != 0)
	{
		/* Use the Beta1 and earlier format */
		header[0]  = (unsigned char)'C';			/* Magic number */
		header[1]  = (unsigned char)'O';
		header[2]  = (unsigned char)'M';
		header[3]  = (unsigned char)'+';
		IL_WRITE_UINT16(header + 4, 1);				/* Major version */
		IL_WRITE_UINT16(header + 6, 0);				/* Minor version */
		IL_WRITE_UINT16(header + 8, 0);				/* Reserved */
		IL_WRITE_UINT16(header + 10, numSections);
		size = 12;
	}
	else
	{
		/* Use the ECMA/Beta2 and later format */
		header[0]  = (unsigned char)'B';			/* Magic number */
		header[1]  = (unsigned char)'S';
		header[2]  = (unsigned char)'J';
		header[3]  = (unsigned char)'B';
		IL_WRITE_UINT16(header + 4, 1);				/* Major version */
		IL_WRITE_UINT16(header + 6, 1);				/* Minor version */
		IL_WRITE_UINT32(header + 8, 0);				/* Reserved */
		IL_WRITE_UINT32(header + 12, VERSION_STRING_LEN);
		ILMemCpy(header + 16, VERSION_STRING, VERSION_STRING_LEN);
		size = 16 + VERSION_STRING_LEN;
		IL_WRITE_UINT16(header + size, 0);			/* Flags */
		IL_WRITE_UINT16(header + size + 2, numSections);
		size += 4;
	}

	/* Output the metadata directory entries */
	if(writer->indexBlob.firstBuffer)
	{
		indexOffset = size;
		size += 4;
		IL_WRITE_UINT32(header + size, writer->indexBlob.offset);
		size += 4;
		header[size++] = (unsigned char)'#';
		header[size++] = (unsigned char)'~';
		header[size++] = 0;
		header[size++] = 0;
	}
	else
	{
		indexOffset = 0;
	}
	if(image->stringPoolSize)
	{
		stringsOffset = size;
		size += 4;
		IL_WRITE_UINT32(header + size, image->stringPoolSize);
		size += 4;
		header[size++] = (unsigned char)'#';
		header[size++] = (unsigned char)'S';
		header[size++] = (unsigned char)'t';
		header[size++] = (unsigned char)'r';
		header[size++] = (unsigned char)'i';
		header[size++] = (unsigned char)'n';
		header[size++] = (unsigned char)'g';
		header[size++] = (unsigned char)'s';
		header[size++] = 0;
		header[size++] = 0;
		header[size++] = 0;
		header[size++] = 0;
	}
	else
	{
		stringsOffset = 0;
	}
	if(image->blobPoolSize)
	{
		blobOffset = size;
		size += 4;
		IL_WRITE_UINT32(header + size, image->blobPoolSize);
		size += 4;
		header[size++] = (unsigned char)'#';
		header[size++] = (unsigned char)'B';
		header[size++] = (unsigned char)'l';
		header[size++] = (unsigned char)'o';
		header[size++] = (unsigned char)'b';
		header[size++] = 0;
		header[size++] = 0;
		header[size++] = 0;
	}
	else
	{
		blobOffset = 0;
	}
	if(image->userStringPoolSize)
	{
		usOffset = size;
		size += 4;
		IL_WRITE_UINT32(header + size, image->userStringPoolSize);
		size += 4;
		header[size++] = (unsigned char)'#';
		header[size++] = (unsigned char)'U';
		header[size++] = (unsigned char)'S';
		header[size++] = 0;
	}
	else
	{
		usOffset = 0;
	}
	if(writer->guidBlob.offset)
	{
		guidOffset = size;
		size += 4;
		IL_WRITE_UINT32(header + size, writer->guidBlob.offset);
		size += 4;
		header[size++] = (unsigned char)'#';
		header[size++] = (unsigned char)'G';
		header[size++] = (unsigned char)'U';
		header[size++] = (unsigned char)'I';
		header[size++] = (unsigned char)'D';
		header[size++] = 0;
		header[size++] = 0;
		header[size++] = 0;
	}
	else
	{
		guidOffset = 0;
	}

	/* Back-patch the offsets into the directory entries */
	current = (unsigned long)size;
	if(indexOffset)
	{
		IL_WRITE_UINT32(header + indexOffset, current);
		current += ((writer->indexBlob.offset + 3) & ~3);
	}
	if(stringsOffset)
	{
		IL_WRITE_UINT32(header + stringsOffset, current);
		current += ((image->stringPoolSize + 3) & ~3);
	}
	if(blobOffset)
	{
		IL_WRITE_UINT32(header + blobOffset, current);
		current += ((image->blobPoolSize + 3) & ~3);
	}
	if(usOffset)
	{
		IL_WRITE_UINT32(header + usOffset, current);
		current += ((image->userStringPoolSize + 3) & ~3);
	}
	if(guidOffset)
	{
		IL_WRITE_UINT32(header + guidOffset, current);
	}

	/* Write the metadata section header */
	start = ILWriterGetTextRVA(writer);
	ILWriterTextWrite(writer, header, size);

	/* Write the contents of the blobs */
	WriteWBufferList(writer, &(writer->indexBlob));
	ILWriterTextAlign(writer);
	WriteImagePool(writer, image->stringPool,
				   image->stringPoolSize, image->stringBlocks);
	ILWriterTextAlign(writer);
	WriteImagePool(writer, image->blobPool,
				   image->blobPoolSize, image->blobBlocks);
	ILWriterTextAlign(writer);
	WriteImagePool(writer, image->userStringPool,
				   image->userStringPoolSize, image->userStringBlocks);
	ILWriterTextAlign(writer);
	WriteWBufferList(writer, &(writer->guidBlob));
	ILWriterTextAlign(writer);

	/* Update the IL runtime header with the metadata section's position */
	ILWriterUpdateHeader(writer, IL_IMAGEENTRY_METADATA,
						 start, ILWriterGetTextRVA(writer) - start);
}

int ILWriterDestroy(ILWriter *writer)
{
	int retval;
	ILWSection *section, *nextSection;

	/* Write the debug information */
	if(writer->debugTokens)
	{
		_ILWriteDebug(writer);
	}

	/* Finalize the PE/COFF output */
	_ILWriteFinal(writer);

	/* Free the section table */
	section = writer->sections;
	while(section != 0)
	{
		nextSection = section->next;
		if(section->buffer)
		{
			ILFree(section->buffer);
		}
		ILFree(section);
		section = nextSection;
	}

	/* Free the buffer lists */
	_ILWBufferListDestroy(&(writer->streamBuffer));
	_ILWBufferListDestroy(&(writer->indexBlob));
	_ILWBufferListDestroy(&(writer->guidBlob));
	_ILWBufferListDestroy(&(writer->debugData));
	_ILWBufferListDestroy(&(writer->debugStrings));

	/* Free the fixup pool */
	ILMemPoolDestroy(&(writer->fixups));

	/* Free the debug token table */
	if(writer->debugTokens)
	{
		ILFree(writer->debugTokens);
	}

	/* Determine the return value */
	if(writer->outOfMemory)
	{
		retval = -1;
	}
	else if(writer->writeFailed)
	{
		retval = 0;
	}
	else
	{
		retval = 1;
	}

	/* Free the writer itself and then exit */
	ILFree(writer);
	return retval;
}

#ifdef	__cplusplus
};
#endif
