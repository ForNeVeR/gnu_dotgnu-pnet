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
#ifdef IL_WIN32_NATIVE
	#include <windows.h>
	#include <io.h>
#endif
#include <errno.h>

#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef IL_WIN32_NATIVE
	#define	USE_WIN32_FIND 1
#endif

ILInt32 ILDeleteDir(const char *path)
{
	int retval;
	if (path == NULL)
	{
		return IL_ERRNO_ENOENT;
	}

#ifdef HAVE_REMOVE
	retval=remove(path);
	if(retval)
	{
		return ILSysIOConvertErrno(retval);
	}
	else
	{
		return IL_ERRNO_Success;
	}
#else
    return IL_ERRNO_ENOSYS;
#endif

}

ILInt32 ILRenameDir(const char *old_name, const char *new_name)
{
	int retVal;

	if (old_name == NULL || new_name == NULL)
	{
	    return IL_ERRNO_ENOENT;
	}

#ifdef HAVE_RENAME
	retVal = rename(old_name, new_name);
	if(retVal == 0)
		return IL_ERRNO_Success;
	else
        return ILSysIOConvertErrno(errno);
#else
	return IL_ERRNO_ENOSYS;
#endif	

}

ILInt32 ILChangeDir(const char *path)
{
	if(path==NULL)
	{
		return IL_ERRNO_ENOENT;
	}
#ifdef HAVE_CHDIR
	return ILSysIOConvertErrno(chdir(path));	
#else
	return IL_ERRNO_ENOSYS;
#endif

}

#ifndef USE_WIN32_FIND

#ifdef HAVE_DIRENT_H

/*
 * Define the ILDir type.
 */
struct _tagILDir
{
	char *pathname;
	DIR *dir;
};

/*
 * Define the ILDirEnt type.
 */
struct _tagILDirEnt
{
	int type;
	struct dirent *dptr;
	struct dirent de;
};

/*
 * Get the type of a directory entry.
 */
static void GetDirEntryType(ILDir *dir, ILDirEnt *entry)
{
	char *fullName;
	struct stat st;
	entry->type = ILFileType_Unknown;
	fullName = (char *)ILMalloc(strlen(dir->pathname) +
								strlen(entry->dptr->d_name) + 2);
	if(fullName)
	{
		strcpy(fullName, dir->pathname);
		strcat(fullName, "/");
		strcat(fullName, entry->dptr->d_name);
	#ifdef HAVE_LSTAT
		if(lstat(fullName, &st) >= 0)
	#else
		if(stat(fullName, &st) >= 0)
	#endif
		{
		#ifdef S_ISFIFO
			if(S_ISFIFO(st.st_mode))
			{
				entry->type = ILFileType_FIFO;
			}
		#endif
		#ifdef S_ISCHR
			if(S_ISCHR(st.st_mode))
			{
				entry->type = ILFileType_CHR;
			}
		#endif
		#ifdef S_ISDIR
			if(S_ISDIR(st.st_mode))
			{
				entry->type = ILFileType_DIR;
			}
		#endif
		#ifdef S_ISBLK
			if(S_ISBLK(st.st_mode))
			{
				entry->type = ILFileType_BLK;
			}
		#endif
		#ifdef S_ISREG
			if(S_ISREG(st.st_mode))
			{
				entry->type = ILFileType_REG;
			}
		#endif
		#ifdef S_ISLNK
			if(S_ISLNK(st.st_mode))
			{
				entry->type = ILFileType_LNK;
			}
		#endif
		#ifdef S_ISSOCK
			if(S_ISSOCK(st.st_mode))
			{
				entry->type = ILFileType_SOCK;
			}
		#endif
		}
		ILFree(fullName);
	}
}

#endif /* HAVE_DIRENT_H */

/*
 * Implementing this way because opendir seems to be somewhat non-standardised.
 * so basically I think this way will be a lot more portable.
 */
ILDir *ILOpenDir(char *path)
{
#ifdef HAVE_DIRENT_H
	ILDir *dir = (ILDir *)ILMalloc(sizeof(ILDir));
	if(dir)
	{
		dir->pathname = ILDupString(path);
		if(!(dir->pathname))
		{
			ILFree(dir);
			return (ILDir *)0;
		}
		dir->dir = opendir(path);
		if(!(dir->dir))
		{
			ILFree(dir->pathname);
			ILFree(dir);
			return (ILDir *)0;
		}
	}
	return dir;
#else
	errno = ENOENT;
	return (ILDir *)0;
#endif
}


/*  This function will return NULL on error  */
ILDirEnt *ILReadDir(ILDir *directory)
{
#ifdef HAVE_DIRENT_H
    
#ifdef HAVE_READDIR_R
	ILDirEnt *result = NULL;

	/* Threadsafe version of readdir() */
	/*  Fetch a directory entry  */
	if((result = (ILDirEnt *)ILMalloc(sizeof(ILDirEnt))) == NULL)
    {
        return NULL;
    }
    
	if(readdir_r(directory->dir, &(result->de), &(result->dptr)) != 0)
	{
		ILFree(result);
		return NULL;
	}
	if(!(result->dptr)) /* yet another terminating condition */
	{
		ILFree(result);
		return NULL;
	}

	GetDirEntryType(directory, result);
	return result;
#else
#ifdef HAVE_READDIR
	/*  Not Threadsafe, so maybe if systems need it, we should rewrite it.  */
	struct dirent *result;
	ILDirEnt *allocatedResult = NULL;
	if((result = readdir(directory->dir)) == NULL)
	{
		return NULL;
	}

	/*  After we know we HAVE a result, we copy it's contents into our 
	 * 	own struct  */
	allocatedResult = (ILDirEnt *)ILMalloc(sizeof(ILDirEnt));
	if(allocatedResult != NULL)
	{
		allocatedResult->dptr = &(allocatedResult->de);
		ILMemCpy(&(allocatedResult->de), result, sizeof(struct dirent));
		GetDirEntryType(directory, allocatedResult);
	}
	return allocatedResult;
#else
	return NULL;
#endif
#endif
#else
	return NULL;
#endif
}

int ILCloseDir(ILDir *directory)
{
#ifdef HAVE_DIRENT_H
	int result = (closedir(directory->dir) == 0);
	ILFree(directory->pathname);
	ILFree(directory);
	return result;
#else
	return 0;
#endif
}

const char *ILDirEntName(ILDirEnt *entry)
{
#ifdef HAVE_DIRENT_H
	return entry->dptr->d_name;
#else
	return (const char *)0;
#endif
}

int ILDirEntType(ILDirEnt *entry)
{
#ifdef HAVE_DIRENT_H
	return entry->type;
#else
	return ILFileType_Unknown;
#endif
}

#else /* USE_WIN32_FIND */

/*
 * Define the ILDir type.
 */
struct _tagILDir
{
	long handle;
	struct _finddata_t fileinfo;
	int havefirst;
};

/*
 * Define the ILDirEnt type.
 */
struct _tagILDirEnt
{
	unsigned attrib;
	char name[1];
};

ILDir *ILOpenDir(char *path)
{
	char *spec;
	int len;
	ILDir *dir = (ILDir *)ILMalloc(sizeof(ILDir));
	if(!dir)
	{
		return 0;
	}
	spec = (char *)ILMalloc(strlen(path) + 5);
	if(!spec)
	{
		ILFree(dir);
		return 0;
	}
	strcpy(spec, path);
	len = strlen(spec);
	if(len > 0 && spec[len - 1] != '/' && spec[len - 1] != '\\')
	{
		spec[len++] = '\\';
	}
	strcpy(spec + len, "*.*");
	dir->handle = _findfirst(spec, &(dir->fileinfo));
	dir->havefirst = 1;
	if(dir->handle < 0)
	{
		int error = errno;
		ILFree(dir);
		ILFree(spec);
		errno = error;
		return 0;
	}
	ILFree(spec);
	return dir;
}

ILDirEnt *ILReadDir(ILDir *dir)
{
	ILDirEnt *entry;
	if(!(dir->havefirst))
	{
		if(_findnext(dir->handle, &(dir->fileinfo)) < 0)
		{
			return 0;
		}
	}
	else
	{
		dir->havefirst = 0;
	}
	entry = (ILDirEnt *)ILMalloc(sizeof(ILDirEnt) + strlen(dir->fileinfo.name));
	if(!entry)
	{
		return 0;
	}
	entry->attrib = dir->fileinfo.attrib;
	strcpy(entry->name, dir->fileinfo.name);
	return entry;
}

int ILCloseDir(ILDir *dir)
{
	_findclose(dir->handle);
	ILFree(dir);
	return 0;
}

const char *ILDirEntName(ILDirEnt *entry)
{
	return entry->name;
}


int ILDirEntType(ILDirEnt *entry)
{
	if((entry->attrib & _A_SUBDIR) != 0)
	{
		return ILFileType_DIR;
	}
	else
	{
		return ILFileType_REG;
	}
}

#endif /* USE_WIN32_FIND */

void ILGetPathInfo(ILPathInfo *info)
{
#if defined(IL_WIN32_NATIVE)
	info->dirSep = '\\';
	info->altDirSep = '/';
	info->volumeSep = ':';
	info->pathSep = ';';
	info->invalidPathChars = "\"<>|\r\n";
#elif defined(IL_WIN32_CYGWIN)
	info->dirSep = '/';
	info->altDirSep = '\\';
	info->volumeSep = 0;
	info->pathSep = ':';
	info->invalidPathChars = "\"<>|\r\n";
#else
	info->dirSep = '/';
	info->altDirSep = 0;
	info->volumeSep = 0;
	info->pathSep = ':';
	info->invalidPathChars = "\r\n";
#endif
}

#ifdef	__cplusplus
};
#endif
