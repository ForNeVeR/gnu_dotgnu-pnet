/*
 * dir.c - Directory Related Functions
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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

#include <stdio.h>
#include "il_system.h"
#include "il_sysio.h"
#include "il_errno.h"

#ifdef HAVE_SYS_STAT_H
	#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
	#include <fcntl.h>
#endif
#ifdef HAVE_DIRENT_H
	#include <dirent.h>
#endif
#ifdef _WIN32
	#include <windows.h>
	#include <io.h>
#endif
#include <errno.h>

#ifdef	__cplusplus
extern	"C" {
#endif
    
ILInt32 ILDeleteDir(const char *path)
{
	if (path == NULL)
	  {
	    return IL_ERRNO_ENOENT;
	  }

#ifdef HAVE_REMOVE
	remove(path);
	return ILSysIOConvertErrno(errno);
#else
    return IL_ERRNO_ENOSYS;
#endif

}

/*
 * Implementing this way because opendir seems to be somewhat non-standardised.
 * so basically I think this way will be a lot more portable.
 */
ILDir *ILOpenDir(char *path)
{
#ifdef HAVE_OPENDIR
	return (ILDir *)opendir(path);
#else
	return NULL;
#endif
}


/*  This function will return NULL on error  */
ILDirEnt *ILReadDir(ILDir *directory)
{
	ILDirEnt *result = NULL;

#ifndef HAVE_READDIR_R
	ILDirEnt *allocatedResult = NULL;
#endif
    
	/* Threadsafe version of readdir() */
#ifdef HAVE_READDIR_R
	/*  Fetch a directory entry  */
	if((result = (ILDirEnt *)ILMalloc(sizeof(ILDirEnt))) == NULL)
    {
        return NULL;
    }
    
	if(readdir_r(directory, result, &result) != 0)
	{
		ILFree(result);
		return NULL;
	}

	return result;
#else
#ifdef HAVE_READDIR
	/*  Not Threadsafe, so maby if systems need it, we should rewrite it.  */
	
	if((result = readdir(directory)) == NULL)
	{
		return NULL;
	}

	/*  After we know we HAVE a result, we copy it's contents into our 
	 * 	own struct  */
	allocatedResult = (ILDirEnt *)ILMalloc(sizeof(ILDirEnt));
	ILMemCpy(alocatedResult, result, sizeof(ILDirEnt));
	return allocatedResult;
#else
	return NULL; // fallback mode
#endif
#endif
}

int ILCloseDir(ILDir *directory)
{
#ifdef HAVE_CLOSEDIR
	return (closedir(directory)!=EBADF);
#else
	return 0;
#endif
}

#ifdef	__cplusplus
};
#endif

