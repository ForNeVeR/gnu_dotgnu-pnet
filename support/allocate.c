/*
 * allocate.c - System memory allocation routines.
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

#include "il_system.h"
#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#ifdef HAVE_SYS_MMAN_H
	#include <sys/mman.h>
#endif
#ifdef _WIN32
	#include <windows.h>
	#include <io.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

void *ILMalloc(unsigned long size)
{
	return malloc(size);
}

void *ILRealloc(void *ptr, unsigned long size)
{
	return realloc(ptr, size);
}

void *ILCalloc(unsigned long nelems, unsigned long size)
{
	return calloc(nelems, size);
}

void ILFree(void *ptr)
{
	free(ptr);
}

char *ILDupString(const char *str)
{
	if(str)
	{
		char *newstr = (char *)ILMalloc(strlen(str) + 1);
		if(newstr)
		{
			strcpy(newstr, str);
		}
		return newstr;
	}
	else
	{
		return 0;
	}
}

unsigned long ILPageAllocSize(void)
{
#ifndef _WIN32
	/* Get the page size using a Unix-like sequence */
	#ifdef HAVE_GETPAGESIZE
		return (unsigned long)getpagesize();
	#else
		#ifdef NBPG
			return NBPG;
		#else
			#ifdef PAGE_SIZE
				return PAGE_SIZE;
			#else
				return 4096;
			#endif
		#endif
	#endif
#else
	/* Get the page size from a Windows-specific API */
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return (unsigned long)(sysInfo.dwPageSize);
#endif
}

unsigned long ILPageMapSize(void)
{
#ifndef _WIN32
	/* Get the page size using a Unix-like sequence */
	#ifdef HAVE_GETPAGESIZE
		return (unsigned long)getpagesize();
	#else
		#ifdef NBPG
			return NBPG;
		#else
			#ifdef PAGE_SIZE
				return PAGE_SIZE;
			#else
				return 4096;
			#endif
		#endif
	#endif
#else
	/* Get the page size from a Windows-specific API */
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return (unsigned long)(sysInfo.dwAllocationGranularity);
#endif
}

void *ILPageAlloc(unsigned long size)
{
	return ILCalloc(size, 1);
}

void  ILPageFree(void *ptr, unsigned long size)
{
	ILFree(ptr);
}

#ifdef	__cplusplus
};
#endif
