/*
 * lib_dir.c - Internalcall methods for the "Platform.DirMethods" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  Richard Baumann
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

#include "engine.h"
#include "lib_defs.h"
#include "il_utils.h"
#include "il_errno.h"
#include "il_sysio.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Structure of the "Platform.PathInfo" class.
 */
typedef struct
{
	ILUInt16	dirSeparator;
	ILUInt16	altDirSeparator;
	ILUInt16	volumeSeparator;
	ILUInt16	pathSeparator;
	ILObject   *invalidPathChars;

} Platform_PathInfo;

typedef struct
{
  ILString *fileName;
  ILInt32 fileType;
} Platform_FileInfo;

/*
 * public static PathInfo GetPathInfo();
 */
void _IL_DirMethods_GetPathInfo(ILExecThread *thread, void *result)
{
	Platform_PathInfo *info = (Platform_PathInfo *)result;
#if defined(_WIN32) || defined(WIN32)
	info->dirSeparator = '\\';
	info->altDirSeparator = '/';
	info->volumeSeparator = ':';
	info->pathSeparator = ';';
	info->invalidPathChars = 0;
#else
	info->dirSeparator = '/';
	info->altDirSeparator = 0;
	info->volumeSeparator = 0;
	info->pathSeparator = ':';
	info->invalidPathChars = 0;
#endif
}

/*
 * public static String GetSystemDirectory();
 */
ILString *_IL_DirMethods_GetSystemDirectory(ILExecThread *thread)
{
	/* We don't support system directories on this platform */
	return 0;
}

/*
 * public static Errno GetLastAccess(String path, out long lastac);
 */
ILInt32 _IL_DirMethods_GetLastAccess(ILExecThread *_thread,
									 ILString *path, ILInt64 *lastac)
{
	char *cpath;
	if (!(cpath = ILStringToAnsi(_thread, path)))
	{
		return IL_ERRNO_ENOMEM;
	}
	return ILSysIOPathGetLastAccess(cpath, lastac);
}

/*
 * public static Errno GetLastModification(String path, out long last_mod);
 */
ILInt32 _IL_DirMethods_GetLastModification(ILExecThread *_thread,
										   ILString *path, ILInt64 *last_mod)
{
	char *cpath;
	if (!(cpath = ILStringToAnsi(_thread, path)))
	{
		return IL_ERRNO_ENOMEM;
	}
	return ILSysIOPathGetLastModification(cpath, last_mod);
}

/*
 * public static Errno GetCreationTime(String path, out long create_time);
 */
ILInt32 _IL_DirMethods_GetCreationTime(ILExecThread *_thread,
									   ILString *path, ILInt64 *create_time)
{
	char *cpath;
	if (!(cpath = ILStringToAnsi(_thread, path)))
	{
		return IL_ERRNO_ENOMEM;
	}
	return ILSysIOPathGetCreation(cpath, create_time);
}

/*
 * public static Errno Copy(String src, String dest);
 */
ILInt32 _IL_DirMethods_Copy(ILExecThread *_thread,
							ILString *src, ILString *dest)
{
	/* TODO */
	return IL_ERRNO_EPERM;
}

/*
 * public static Errno Delete(String path);
 */
ILInt32 _IL_DirMethods_Delete(ILExecThread *_thread, ILString *path)
{
	return ILDeleteDir(ILStringToAnsi(_thread, path));
}

/*
 * public static Errno Rename(String old_name, String new_name);
 */
ILInt32 _IL_DirMethods_Rename(ILExecThread *_thread,
							  ILString *old_name,
							  ILString *new_name)
{
	/* TODO */
	return IL_ERRNO_EPERM;
}

/*
 * public static String GetCurrentDirectory();
 */
ILString *_IL_DirMethods_GetCurrentDirectory(ILExecThread *_thread)
{
	char *dir = ILGetCwd();
	ILString *str;
	if(dir)
	{
		str = ILStringCreate(_thread, dir);
		ILFree(dir);
		return str;
	}
	else
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return 0;
	}
}

/*
 * public static Errno GetFilesInDirectory(String path, out 
 * 											Platform.FileInfo[] files);
 */
extern ILInt32 _IL_DirMethods_GetFilesInDirectory(ILExecThread * _thread,ILString * path, System_Array * * files)
{
	ILDir *dirStream = NULL; 
	ILDirEnt *dirEntry;
	ILInt32 arraySize = 0;
	Platform_FileInfo *buffer=NULL; 
	ILInt32 i;  /*  How surprising, an index var named 'i' */
	ILQueueEntry *fileQueue;

	errno = 0;
	fileQueue = ILQueueCreate();

	/*  dirStream and dirEnt are typedeffed based on underlying platform 
	 *  This could cause a compile time error for people trying to port, 
	 * 	as dirEnt may not exist . If you are trying to port, I put the 
	 * 	typedefs in PNETROOT/il_sysio.h, you should edit that file, as 
	 * 	well as PNETROOT/support/dir.c, as the logic will need to be 
	 * 	re-vamped a bit*/

	dirStream = ILOpenDir(ILStringToAnsi(_thread, path));
	if(!dirStream)
	{
 		return ILSysIOConvertErrno(errno);
	}

	while((dirEntry = ILReadDir(dirStream)) != NULL)
	{
		ILQueueAdd(&fileQueue, dirEntry);
		arraySize++;
    }

	if(!ILCloseDir(dirStream))
	{
        return ILSysIOConvertErrno(errno);
	}
	
	
	*files = (System_Array*) ILExecThreadNew(_thread,
						"[vPlatform.FileInfo;","(Ti)V",(ILVaInt)(arraySize));
	if(!(*files))
	{
		ILExecThreadThrowOutOfMemory(_thread);
		return IL_ERRNO_ENOMEM;
	}

    buffer = (Platform_FileInfo *)(ArrayToBuffer(*files));

	for(i = 0; i < arraySize; i++)
	{
		dirEntry = ILQueueRemove(&fileQueue);

		if(!dirEntry)
		{
			break;
		}
        
		buffer->fileName = ILStringCreate(_thread, dirEntry->d_name);
		buffer->fileType = dirEntry->d_type;
		ILFree(dirEntry);
		buffer++;
	}

    ILQueueDestroy(&fileQueue);

 	return ILSysIOConvertErrno(errno);
}
#ifdef	__cplusplus
};
#endif
