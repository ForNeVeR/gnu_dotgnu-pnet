/*
 * lib_file.c - Internalcall methods for "System.IO" and subclasses
 *
 * Copyright (C) 2002  FSF INDIA and Southern Storm Software, Pty Ltd.
 *
 * Authors: Gopal.V
 *          Rhys Weatherley
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
#include "il_sysio.h"
#include "il_errno.h"

/*
 * public static IntPtr GetInvalidHandle();
 */
static ILNativeInt Platform_FileMethods_GetInvalidHandle(ILExecThread *thread)
{
	return (ILNativeInt)ILSysIOHandle_Invalid;
}

/*
 * public static bool ValidatePathname(String path);
 */
static ILBool Platform_FileMethods_ValidatePathname
							(ILExecThread *thread, ILString *path)
{
	char *cpath = ILStringToAnsi(thread, path);
	if(cpath)
	{
		return (ILBool)(ILSysIOValidatePathname(cpath));
	}
	else
	{
		return 0;
	}
}

/*
 * public static bool Open(String path, FileMode mode,
 *						   FileAccess access, FileShare share,
 *						   out IntPtr handle);
 */
static ILBool Platform_FileMethods_Open(ILExecThread *thread, 
										ILString *path, 
										ILUInt32 mode,
										ILUInt32 access, ILUInt32 share,
										ILNativeInt *handle)
{
	char *cpath = ILStringToAnsi(thread, path);
	if(!cpath)
	{
		ILSysIOSetErrno(IL_ERRNO_ENOMEM);
		return 0;
	}
	*handle = (ILNativeInt)ILSysIOOpenFile(cpath, mode, access, share);
	return (*handle != (ILNativeInt)ILSysIOHandle_Invalid);
}

/*
 * public static bool Write(IntPtr handle, byte[] buffer,
 *                          int offset, int count);
 */
static ILBool Platform_FileMethods_Write(ILExecThread *thread,
										 ILNativeInt handle,
										 System_Array *array,
										 ILInt32 offset,
										 ILInt32 count)
{
	ILUInt8 *buf = (ILUInt8 *)(ArrayToBuffer(array));
	return (ILSysIOWrite((ILSysIOHandle)handle, buf + offset, count) == count);
}

/*
 * public static int Read(IntPtr handle, byte[] buffer,
 *                        int offset, int count);
 */
static ILInt32 Platform_FileMethods_Read(ILExecThread *thread,
										 ILNativeInt handle,
										 System_Array *array,
										 ILInt32 offset,
										 ILInt32 count)
{
	ILUInt8 *buf = (ILUInt8 *)(ArrayToBuffer(array));
	return ILSysIORead((ILSysIOHandle)handle, buf + offset, count);
}

/*
 * public static bool HasAsync();
 */
static ILBool Platform_FileMethods_HasAsync(ILExecThread *thread)
{
	return ILSysIOHasAsync();
}

/*
 * public static bool CanSeek(IntPtr handle);
 */
static ILBool Platform_FileMethods_CanSeek(ILExecThread *thread,
										   ILNativeInt handle)
{
	/* Try seeking to the current position, which will fail
	   on non-seekable streams like pipes and sockets */
	return (ILSysIOSeek((ILSysIOHandle)handle, (ILInt64)0, 1) != (ILInt64)(-1));
}

/*
 * public static bool CheckHandleAccess(IntPtr handle, FileAccess access);
 */
static ILBool Platform_FileMethods_CheckHandleAccess(ILExecThread *thread,
													 ILNativeInt handle,
													 ILUInt32 access)
{
	return (ILBool)(ILSysIOCheckHandleAccess((ILSysIOHandle)handle, access));
}

/*
 * public static long Seek(IntPtr handle, long offset, SeekOrigin origin);
 */
static ILInt64 Platform_FileMethods_Seek(ILExecThread *thread, 
										 ILNativeInt handle,
										 ILInt64 offset,
										 ILUInt32 origin)
{
	return ILSysIOSeek((ILSysIOHandle)handle, offset, origin);
}

/*
 * public static bool Close(IntPtr handle);
 */
static ILBool Platform_FileMethods_Close(ILExecThread *thread, 
										 ILNativeInt handle)
{
	return (ILBool)(ILSysIOClose((ILSysIOHandle)handle));
}

/*
 * public static bool FlushWrite(IntPtr handle);
 */
static ILBool Platform_FileMethods_FlushWrite(ILExecThread *thread,
											  ILNativeInt handle)
{
	return (ILBool)(ILSysIOFlushWrite((ILSysIOHandle)handle));
}

/*
 * public static bool SetLength(IntPtr handle, long value);
 */
static ILBool Platform_FileMethods_SetLength(ILExecThread *thread,
											 ILNativeInt handle,
											 ILInt64 value)
{
	return (ILBool)(ILSysIOTruncate((ILSysIOHandle)handle, value));
}

/*
 * public static Errno GetErrno();
 */
static ILInt32 Platform_FileMethods_GetErrno(ILExecThread *thread)
{
	return ILSysIOGetErrno();
}

/*
 * public static String GetErrnoMessage(Errno error);
 */
static ILString *Platform_FileMethods_GetErrnoMessage(ILExecThread *thread,
													  ILInt32 error)
{
	const char *msg = ILSysIOGetErrnoMessage(error);
	if(msg)
	{
		return ILStringCreate(thread, msg);
	}
	else
	{
		return 0;
	}
}

/*
 * Method table for the "Platform.FileMethods" class.
 */
IL_METHOD_BEGIN(_ILPlatformFileMethods)
     IL_METHOD("GetInvalidHandle", "()j",
					 Platform_FileMethods_GetInvalidHandle)
     IL_METHOD("ValidatePathname", "(oSystem.String;)Z",
					 Platform_FileMethods_ValidatePathname)
	 IL_METHOD("Open", "(oSystem_String;vSystem.IO.FileMode;"
	 				   "vSystem.IO.FileAccess;vSystem.IO.FileShare;&j)Z",
					 Platform_FileMethods_Open)
     IL_METHOD("HasAsync", "()Z",
					 Platform_FileMethods_HasAsync)
	 IL_METHOD("CanSeek", "(j)Z",
					 Platform_FileMethods_CanSeek)
     IL_METHOD("CheckHandleAccess", "(jvSystem.IO.FileAccess;)Z",
	 				 Platform_FileMethods_CheckHandleAccess)
	 IL_METHOD("Seek", "(jlvSystem.IO.SeekOrigin;)l",
					 Platform_FileMethods_Seek)
	 IL_METHOD("Write", "(j[Bii)Z",
					 Platform_FileMethods_Write)
	 IL_METHOD("Read", "(j[Bii)Z",
					 Platform_FileMethods_Read)
     IL_METHOD("Close", "(j)Z",
               		 Platform_FileMethods_Close)
	 IL_METHOD("FlushWrite", "(j)Z",
					 Platform_FileMethods_FlushWrite)
	 IL_METHOD("SetLength", "(jl)Z",
					 Platform_FileMethods_SetLength)
	 IL_METHOD("GetErrno", "()vPlatform.Errno;",
					 Platform_FileMethods_GetErrno)
	 IL_METHOD("GetErrnoMessage","(vPlatform.Errno;)oSystem.String;",
					 Platform_FileMethods_GetErrnoMessage)
IL_METHOD_END
