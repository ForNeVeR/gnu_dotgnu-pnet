/*
 * lib_dir.c - Internalcall methods for the "Platform.DirMethods" class.
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

#include "engine.h"
#include "lib_defs.h"
#include "il_utils.h"
#include "il_errno.h"

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
	/* TODO */
	return IL_ERRNO_EPERM;
}

/*
 * public static Errno GetLastModification(String path, out long last_mod);
 */
ILInt32 _IL_DirMethods_GetLastModification(ILExecThread *_thread,
										   ILString *path, ILInt64 *last_mod)
{
	/* TODO */
	return IL_ERRNO_EPERM;
}

/*
 * public static Errno GetCreationTime(String path, out long create_time);
 */
ILInt32 _IL_DirMethods_GetCreationTime(ILExecThread *_thread,
									   ILString *path, ILInt64 *create_time)
{
	/* TODO */
	return IL_ERRNO_EPERM;
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
	/* TODO */
	return IL_ERRNO_EPERM;
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
 * public static Errno GetFilesInDirectory(String path, out String files);
 */
ILInt32 _IL_DirMethods_GetFilesInDirectory(ILExecThread *_thread,
										   ILString *path,
										   ILString **files)
{
	/* TODO */
	return IL_ERRNO_EPERM;
}

#ifdef	__cplusplus
};
#endif
