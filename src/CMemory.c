/*
 * SDMemory.c - Memory management and manipulation implementation.
 *
 * Copyright (C) 2005  Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "CrayonsInternal.h"

#ifdef HAVE_STRING_H
	#include <string.h>
#else
	#ifdef HAVE_STRINGS_H
		#include <strings.h>
	#endif
#endif
#ifdef HAVE_MEMORY_H
	#include <memory.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL void *
SDMalloc(SDUInt32 size)
{
	return malloc(size);
}

SDINTERNAL void *
SDRealloc(void     *ptr,
          SDUInt32  size)
{
	return realloc(ptr, size);
}

SDINTERNAL void *
SDCalloc(SDUInt32 count,
         SDUInt32 size)
{
	return calloc(count, size);
}

SDINTERNAL void
SDFree(void *ptr)
{
	free(ptr);
}

SDINTERNAL void *
SDMemSet(void     *_dst,
         SDByte    value,
         SDUInt32  length)
{
#ifdef HAVE_MEMSET
	return memset(_dst, value, length);
#else
	/* get the data pointer */
	SDByte *dst = (SDByte *)_dst;

	/* set the memory */
	while(length > 0)
	{
		/* set the current data */
		*dst++ = value;

		/* move to the next position */
		--length;
	}

	/* return the destination */
	return _dst;
#endif
}

SDINTERNAL void *
SDMemCopy(void       *_dst,
          const void *_src,
          SDUInt32    length)
{
#ifdef HAVE_MEMCPY
	return memcpy(_dst, _src, length);
#elif defined(HAVE_BCOPY)
	return bcopy((char *)_src, (char *)_dst, length);
#else
	/* get the source data pointer */
	const SDByte *src = (const SDByte *)_src;

	/* get the destination data pointer */
	SDByte *dst = (SDByte *)_dst;

	/* copy the memory */
	while(length > 0)
	{
		/* set the current data */
		*dst++ = *src++;

		/* move to the next position */
		--length;
	}

	/* return the destination */
	return _dst;
#endif
}

SDINTERNAL void *
SDMemMove(void       *_dst,
          const void *_src,
          SDUInt32    length)
{
#ifdef HAVE_MEMMOVE
	return memmove(_dst, _src, length);
#else
	/* get the source data pointer */
	const SDByte *src = (const SDByte *)_src;

	/* get the destination data pointer */
	SDByte *dst = (SDByte *)_dst;

	/* move the memory, based on the direction */
	if(dst < src)
	{
		/* move the memory */
		while(length > 0)
		{
			/* set the current data */
			*dst++ = *src++;

			/* move to the next position */
			--length;
		}
	}
	else
	{
		/* set the data pointers to the end */
		src += length;
		dst += length;

		/* move the memory */
		while(length > 0)
		{
			/* set the current data */
			*(--dst) = *(--src);

			/* move to the next position */
			--length;
		}
	}

	/* return the destination */
	return _dst;
#endif
}

SDINTERNAL int
SDMemCmp(const void *_a,
         const void *_b,
         SDUInt32    length)
{
#ifdef HAVE_MEMCMP
	return memcmp(_a, _b, length);
#elif defined(HAVE_BCMP)
	return bcmp((char *)_a, (char *)_b, length);
#else
	/* get the first data pointer */
	const SDByte *a = (SDByte *)_a;

	/* get the second data pointer */
	const SDByte *b = (SDByte *)_b;

	/* compare the memory */
	while(length > 0)
	{
		/* bail out now if we've found a difference */
		if(*a > *b) { return  1; }
		if(*a < *b) { return -1; }

		/* move to the next position */
		++a; ++b; --length;
	}

	/* return equality flag */
	return 0;
#endif
}


#ifdef __cplusplus
};
#endif
