/*
 * lib_file.c - Internalcall methods for "System.IO" and subclasses
 *
 * Copyright (C) 2002  FSF INDIA
 *
 * Author : Gopal.V
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

#define ILFileMode_CreateNew	1
#define ILFileMode_Create		2
#define	ILFileMode_Open			3
#define ILFileMode_OpenOrCreate	4
#define ILFileMode_Truncate		5
#define ILFileMode_Append		6

#define ILFileAccess_Read 0x01
#define ILFileAccess_Write 0x02
#define ILFileAccess_ReadWrite (ILFileAccess_Read | ILFileAccess_Write)

static ILNativeInt Platform_FileMethods_GetInvalidHandle(ILExecThread *thread)
{
	return (ILNativeInt)NULL;//that's it ?
}
static ILBool Platform_FileMethods_ValidatePathname(ILExecThread *thread
				,ILString *path)
{
	/* TODO */
	return 1;
}
static ILBool Platform_FileMethods_Open (ILExecThread *thread, 
				ILString * path, 
				ILUInt32 mode,
				ILUInt32 access, ILUInt32 share,
				ILNativeInt * handle)
{
	char * cpath = ILStringToAnsi(thread,path);
	if(cpath==NULL)return 0;
	switch (mode)
	{
		case ILFileMode_CreateNew:
			if(ILFileExists(cpath,NULL))
			{
				return 0;//exists -- exception
			}
			else if(ILFileCreate(cpath))
			{
				if(access==ILFileAccess_Read)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"r");
					return (*handle & 1);
				}
				else if(access==ILFileAccess_Write)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"w");
					return (*handle & 1);
				}
				else if(access==ILFileAccess_ReadWrite)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"w+");//create
					return (*handle & 1);
				}
				else return 0;
			}
			else return 0;
			break;
		case ILFileMode_Create:
			if(ILFileCreate(cpath))
			{
				if(access==ILFileAccess_Read)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"r");
					return (*handle & 1);
				}
				else if(access==ILFileAccess_Write)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"w");
					return (*handle & 1);
				}
				else if(access==ILFileAccess_ReadWrite)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"w+");//create
					return (*handle & 1);
				}
				else return 0;
			}
			else return 0;
			
	  		break;
		case ILFileMode_Open:
			if(!ILFileExists(cpath,NULL))
			{
				return 0;
			}
			if(access==ILFileAccess_Read)
			{
				*handle=(ILNativeInt)ILFileOpen(cpath,"r");
				return (*handle & 1);
			}
			else if(access==ILFileAccess_Write)
			{
				*handle=(ILNativeInt)ILFileOpen(cpath,"w");
				return (*handle & 1);
			}
			else if(access==ILFileAccess_ReadWrite)
			{
				*handle=(ILNativeInt)ILFileOpen(cpath,"r+");//no create 
				return (*handle & 1);
			}
			else return 0;
	  		break;
		case ILFileMode_OpenOrCreate:
			if(!ILFileExists(cpath,NULL))
			{
				ILFileCreate(cpath);//create if not exist
			}
			if(ILFileExists(cpath,NULL))
			{
				if(access==ILFileAccess_Read)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"r");
					return (*handle & 1);
				}
				else if(access==ILFileAccess_Write)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"w");
					return (*handle & 1);
				}
				else if(access==ILFileAccess_ReadWrite)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"w+");//create
					return (*handle & 1);
				}
				else return 0;
			}
			else return 0;
	  		break;
		case ILFileMode_Truncate:
			if(ILFileExists(cpath,NULL))
			{
				if(access==ILFileAccess_Write)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"w");
					return (*handle & 1);
				}
				else return 0;//illegal mode
			}
			else return 0;
			break;
		case ILFileMode_Append:
			if(ILFileExists(cpath,NULL))
			{
				if(access==ILFileAccess_Write)
				{
					*handle=(ILNativeInt)ILFileOpen(cpath,"a");
					return (*handle & 1);
				}
				else return 0;//illegal mode
			}
			else return 0;
			break;
		default:
			return 0;
	}
	return 0;
}

static ILBool Platform_FileMethods_Write(ILExecThread *thread,
				ILNativeInt handle ,System_Array *array,ILInt32 offset,
				ILInt32 count)
{
	ILFileHandle fh=(ILFileHandle)handle;
	ILUInt8 *buf=(ILUInt8*)(ArrayToBuffer(array));
	if(fh==NULL)return 0;	
	if((offset+count) > (sizeof(buf)/sizeof(ILUInt8)))return 0;
	//offset out of bounds
	if(ILFileWrite(&(buf[offset]),sizeof(ILUInt8),count,fh))
	{
		return 1;
	}
	return 0;
}
static ILBool Platform_FileMethods_Read(ILExecThread *thread,
				ILNativeInt handle, System_Array *array, ILInt32 offset,
				ILInt32 count)
{
	ILFileHandle fh=(ILFileHandle)handle;
	ILUInt8 *buf=(ILUInt8*)(ArrayToBuffer(array));
	if((offset+count) > (sizeof(buf)/sizeof(ILUInt8)))return 0;
	//offset out of bounds
	if(ILFileRead(&(buf[offset]),sizeof(ILUInt8),count,fh))
	{
		return 1;
	}
	return 0;
}
static ILBool Platform_FileMethods_HasAsync(ILExecThread *thread)
{
	/* TODO */
	return 0;
}
static ILBool Platform_FileMethods_CanSeek(ILExecThread *thread,
				ILNativeInt handle)
{
	/* TODO */
	ILFileHandle fh=(ILFileHandle)handle;
	if(handle == 0)return 0;//NULL
//	stdin/out/err ?
	return (1+ILFileSeek(fh,0,SEEK_SET));
}
static ILBool Platform_FileMethods_CheckHandleAccess(ILExecThread *thread,
				ILNativeInt handle,ILUInt32 access)
{
  ILFileHandle fh = (ILFileHandle)handle;
  return ILFileCheckHandleAccess(fh, access);
}
static ILInt64 Platform_FileMethods_Seek(ILExecThread *thread, 
				ILNativeInt handle,ILInt64 offset,ILUInt32 origin)
{
	ILFileHandle fh=(ILFileHandle)handle;
	if(fh==NULL)return -1;
	if(origin>2)return -1;
	if(ILFileSeek(fh,offset,origin)>=0)return -1;
	return (ILInt64)(ILFileTell(fh));
}



static ILBool Platform_FileMethods_Close(ILExecThread *thread, 
				ILNativeInt handle)
{
	ILFileHandle fh=(ILFileHandle)handle;
	if(fh==NULL)return 0;//SEGFAULT_PROTECT 
    return ILFileClose(fh);
}
static ILBool Platform_FileMethods_FlushWrite(ILExecThread * thread,
				ILNativeInt handle)
{
	ILFileHandle fh=(ILFileHandle)handle;
	if(fh==NULL)return 0;//SEGFAULT_PROTECT 
	return (1+ILFileFlush(fh));
}
static ILBool Platform_FileMethods_SetLength(ILExecThread * thread,
				ILNativeInt handle, ILInt64 value)
{
	ILFileHandle fh=(ILFileHandle)handle;
	if(fh==NULL)return 0;//SEGFAULT_PROTECT 
	return (1+ILFileTruncate(fh,value));
}
/*
 * Export table
 */
IL_METHOD_BEGIN(_ILPlatformFileMethods)
     IL_METHOD("GetInvalidHandle","()j",
					 Platform_FileMethods_GetInvalidHandle)
	 IL_METHOD("Open","(oSystem_String;iii&j)Z",
					 Platform_FileMethods_Open)
	 IL_METHOD("CanSeek","(j)Z",
					 Platform_FileMethods_CanSeek)
     IL_METHOD("CheckHandleAccess", "(ji)Z", Platform_FileMethods_CheckHandleAccess)
	 IL_METHOD("Seek","(jli)l",
					 Platform_FileMethods_Seek)
	 IL_METHOD("Write","(j[Bii)Z",
					 Platform_FileMethods_Write)
	 IL_METHOD("Read","(j[Bii)Z",
					 Platform_FileMethods_Read)
     IL_METHOD("Close", "(j)Z",
               Platform_FileMethods_Close)
	 IL_METHOD("FlushWrite","(jl)Z",
					 Platform_FileMethods_FlushWrite)
	 IL_METHOD("SetLength","(jl)Z",
					 Platform_FileMethods_SetLength)
IL_METHOD_END
