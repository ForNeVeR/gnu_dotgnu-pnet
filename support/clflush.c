/*
 * clflush.c - Flush CPU cache lines.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Note: if you port this function to another platform, you must
 * also update the #ifdef logic at the bottom of "il_system.h"
 * that detects if cache flushing is available.
 */

void ILCacheFlush(void *buf, long length)
{
	/* There's no code here for i386, because it isn't needed */
#if IL_HAVE_CACHE_FLUSH
#if defined(PPC)

	/* Flush the CPU cache on PPC platforms */
	register unsigned char *p;

	/* Flush the data out of the data cache */
	p = (unsigned char *)buf;
	while(length > 0)
	{
		__asm__ __volatile__ ("dcbst 0,%0" :: "r"(p));
		p += 4;
		length -= 4;
	}
	__asm__ __volatile__ ("sync");

	/* Invalidate the cache lines in the instruction cache */
	p = (unsigned char *)buf;
	while(length > 0)
	{
		__asm__ __volatile__ ("icbi 0,%0; isync" :: "r"(p));
		p += 4;
		length -= 4;
	}
	__asm__ __volatile__ ("isync");

#elif defined(__sparc)

	/* Flush the CPU cache on sparc platforms */
	register unsigned char *p = (unsigned char *)buf;
	__asm__ __volatile__ ("stbar");
	while(length > 0)
	{
		__asm__ __volatile__ ("flush %0" :: "r"(p));
		p += 4;
		length -= 4;
	}
	__asm__ __volatile__ ("nop; nop; nop; nop; nop");

#endif
#endif /* IL_HAVE_CACHE_FLUSH */
}

#ifdef	__cplusplus
};
#endif
