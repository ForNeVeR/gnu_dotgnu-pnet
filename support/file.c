/*
 * file.c - File-related functions.
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

#include <stdio.h>
#include "il_system.h"
#ifdef HAVE_SYS_STAT_H
	#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#ifdef _WIN32
	#include <windows.h>
	#include <io.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

void ILDeleteFile(const char *filename)
{
#ifdef HAVE_UNLINK
	unlink(filename);
#else
#ifdef HAVE_REMOVE
	remove(filename);
#endif
#endif
}

int ILFileExists(const char *filename, char **newExePath)
{
	if(newExePath)
	{
		*newExePath = 0;
	}

#ifdef __CYGWIN__
	/* If we are on Windows, then check for ".exe" first */
	if(newExePath)
	{
		char *newPath = (char *)ILMalloc(strlen(filename) + 5);
		if(newPath)
		{
			strcpy(newPath, filename);
			strcat(newPath, ".exe");
			if(access(newPath, 0) >= 0)
			{
				*newExePath = newPath;
				return 1;
			}
			ILFree(newPath);
		}
	}
#endif

#ifdef HAVE_ACCESS
	if(access(filename, 0) >= 0)
	{
		return 1;
	}
#else
#ifdef HAVE_STAT
	{
		struct stat st;
		if(stat(filename, &st) >= 0)
		{
			return 1;
		}
	}
#else
	{
		FILE *file = fopen(filename, "r");
		if(file)
		{
			fclose(file);
			return 1;
		}
	}
#endif
#endif

	return 0;
}
/*
 * Gopal's hacks
 * 
 * need to put in the Automake checks for fopen,fclose,fread etc..
 * and take care of portability issues if any. Also inline it if 
 * GCC supports it in all platforms
 */
ILFileHandle ILFileOpen(const char * path, const char * mode)
{		
	ILFileHandle retval;
	if(!path || !mode)return NULL;
	retval=(ILFileHandle)fopen(path,mode);
	return retval;
}
ILBool ILFileClose(ILFileHandle handle)
{
	if(!handle)return 0;
	return fclose((FILE*)handle);
}
ILBool ILFileCreate(const char * path)
{
	FILE *fp=NULL;
	if(!path)return 0;
	fp=fopen(path,"w");
	if(fp)
	{
		fclose(fp);
		return 1;
	}
	else return 0;	
}
int ILFileRead(void *buf,int size,int num,ILFileHandle handle)
{
	FILE *fp=(FILE*)handle;	
	return(fread(buf,size,num,fp));
}
int ILFileWrite(void *buf,int size,int num,ILFileHandle handle)
{
	FILE *fp=(FILE*)handle;
	return(fread(buf,size,num,fp));
}
int ILFileSeek(ILFileHandle handle, long offset,int from)
{
	FILE *fp=(FILE*)handle;
	return(fseek(fp,offset,from));
}
long ILFileTell (ILFileHandle handle)
{
	FILE *fp=(FILE*)handle;
	return (ftell(fp));
}
int ILFileFlush(ILFileHandle handle)
{
	FILE *fp=(FILE*)handle;
	return(fflush(fp));
}
int ILFileTruncate(ILFileHandle handle,long value)
{
	FILE *fp=(FILE*)handle;
	return(ftruncate(fileno(fp),value));//this will need porting 
}
int ILFileHasAsync(ILFileHandle handle)
{
	/* TODO */
#ifdef _POSIX_ASYNC_IO
	return 1;
#else
	return 0;
#endif
}

#ifdef	__cplusplus
};
#endif
