/*
 * lib_marshal.c - Internalcall methods for the Marshal class.
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

#include "engine_private.h"
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef IL_CONFIG_PINVOKE

/*
 * Determine if the caller is authorised to perform unmanaged operations,
 * and throw an exception if it isn't.
 */
static int UnmanagedOK(ILExecThread *thread)
{
	/* Check that the caller is secure */
	if(ILImageIsSecure(_ILClrCallerImage(thread)))
	{
		return 1;
	}

	/* Throw a SecurityException within the current thread */
	ILExecThreadThrowSystem(thread, "System.Security.SecurityException", 0);
	return 0;
}

/*
 * public static IntPtr AllocHGlobal(IntPtr cb);
 */
ILNativeInt _IL_Marshal_AllocHGlobal(ILExecThread *_thread, ILNativeInt cb)
{
	if(UnmanagedOK(_thread))
	{
		/* Use the underlying system "malloc", because "ILMalloc"
		   may have been redirected elsewhere */
		void *ptr = (void *)malloc((unsigned)cb);
		if(ptr)
		{
			return (ILNativeInt)ptr;
		}
		ILExecThreadThrowOutOfMemory(_thread);
	}
	return 0;
}

/*
 * private static void CopyMU(Array source, int startOffset,
 *							  IntPtr destination, int numBytes);
 */
void _IL_Marshal_CopyMU(ILExecThread *_thread, ILObject *source,
						ILInt32 startOffset, ILNativeInt destination,
						ILInt32 numBytes)
{
	if(UnmanagedOK(_thread) && source && destination && numBytes > 0)
	{
		ILMemMove((void *)destination,
				  ((unsigned char *)(ArrayToBuffer(source))) + startOffset,
				  (unsigned)numBytes);
	}
}

/*
 * private static void CopyUM(IntPtr source, Array destination,
 *							  int startOffset, int numBytes);
 */
void _IL_Marshal_CopyUM(ILExecThread *_thread, ILNativeInt source,
					    ILObject *destination, ILInt32 startOffset,
						ILInt32 numBytes)
{
	if(UnmanagedOK(_thread) && source && destination && numBytes > 0)
	{
		ILMemMove(((unsigned char *)(ArrayToBuffer(destination))) +
					startOffset, (void *)source, (unsigned)numBytes);
	}
}

/*
 * public static void FreeHGlobal(IntPtr hglobal);
 */
void _IL_Marshal_FreeHGlobal(ILExecThread *_thread, ILNativeInt hglobal)
{
	if(UnmanagedOK(_thread) && hglobal)
	{
		/* Use the underlying system "free", because "ILFree"
		   may have been redirected elsewhere */
		free((void *)hglobal);
	}
}

/*
 * public static IntPtr OffsetOf(Type t, String fieldName);
 */
ILNativeInt _IL_Marshal_OffsetOf(ILExecThread *_thread, ILObject *t,
								 ILString *fieldName)
{
	char *name;
	ILClass *classInfo;
	ILField *field;
	ILNativeInt offset;

	if(UnmanagedOK(_thread) && t && fieldName)
	{
		/* Convert the "Type" into an "ILClass *" structure */
		classInfo = _ILGetClrClass(_thread, t);
		if(!classInfo)
		{
			return 0;
		}

		/* Get the field name in UTF-8 */
		name = ILStringToUTF8(_thread, fieldName);
		if(!name)
		{
			return 0;
		}

		/* Make sure that the class has been laid out */
		IL_METADATA_WRLOCK(_thread);
		if(!_ILLayoutClass(classInfo))
		{
			IL_METADATA_UNLOCK(_thread);
			return 0;
		}

		/* Look for the field within the class */
		while(classInfo != 0)
		{
			field = 0;
			while((field = (ILField *)ILClassNextMemberByKind
						(classInfo, (ILMember *)field,
						 IL_META_MEMBERKIND_FIELD)) != 0)
			{
				if(!ILField_IsStatic(field) &&
				   !strcmp(ILField_Name(field), name))
				{
					offset = (ILNativeInt)(ILUInt32)(field->offset);
					IL_METADATA_UNLOCK(_thread);
					return offset;
				}
			}
			classInfo = ILClass_Parent(classInfo);
		}
		IL_METADATA_UNLOCK(_thread);
	}
	return 0;
}

/*
 * public static String PtrToStringAnsi(IntPtr ptr);
 */
ILString *_IL_Marshal_PtrToStringAnsi_j(ILExecThread *_thread, ILNativeInt ptr)
{
	if(UnmanagedOK(_thread))
	{
		if(ptr)
		{
			return ILStringCreate(_thread, (const char *)ptr);
		}
		ILExecThreadThrowArgNull(_thread, "ptr");
	}
	return 0;
}

/*
 * public static String PtrToStringAnsi(IntPtr ptr, int len);
 */
ILString *_IL_Marshal_PtrToStringAnsi_ji(ILExecThread *_thread,
										 ILNativeInt ptr, ILInt32 len)
{
	if(UnmanagedOK(_thread))
	{
		if(ptr)
		{
			if(len >= 0)
			{
				return ILStringCreateLen(_thread, (const char *)ptr, len);
			}
			else
			{
				ILExecThreadThrowSystem
					(_thread, "System.ArgumentException", 0);
			}
		}
		else
		{
			ILExecThreadThrowArgNull(_thread, "ptr");
		}
	}
	return 0;
}

/*
 * private static IntPtr ObjectToPtr(Object obj);
 */
ILNativeInt _IL_Marshal_ObjectToPtr(ILExecThread *_thread, ILObject *obj)
{
	/* In this implementation, the object handle points at the object data */
	return (ILNativeInt)obj;
}

/*
 * public static byte ReadByte(IntPtr ptr, int ofs);
 */
ILUInt8 _IL_Marshal_ReadByte(ILExecThread *_thread, ILNativeInt ptr,
							 ILInt32 ofs)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		return ((ILUInt8 *)ptr)[ofs];
	}
	return 0;
}

/*
 * public static short ReadInt16(IntPtr ptr, int ofs);
 */
ILInt16 _IL_Marshal_ReadInt16(ILExecThread *_thread, ILNativeInt ptr,
							  ILInt32 ofs)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		/* Handle the possibility of misaligned accesses carefully */
		ILInt16 temp;
		ILMemCpy(&temp, ((unsigned char *)ptr) + ofs, sizeof(ILInt16));
		return temp;
	}
	return 0;
}

/*
 * public static int ReadInt32(IntPtr ptr, int ofs);
 */
ILInt32 _IL_Marshal_ReadInt32(ILExecThread *_thread, ILNativeInt ptr,
							  ILInt32 ofs)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		/* Handle the possibility of misaligned accesses carefully */
		ILInt32 temp;
		ILMemCpy(&temp, ((unsigned char *)ptr) + ofs, sizeof(ILInt32));
		return temp;
	}
	return 0;
}

/*
 * public static long ReadInt64(IntPtr ptr, int ofs);
 */
ILInt64 _IL_Marshal_ReadInt64(ILExecThread *_thread, ILNativeInt ptr,
							  ILInt32 ofs)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		/* Handle the possibility of misaligned accesses carefully */
		ILInt64 temp;
		ILMemCpy(&temp, ((unsigned char *)ptr) + ofs, sizeof(ILInt64));
		return temp;
	}
	return 0;
}

/*
 * public static IntPtr ReadIntPtr(IntPtr ptr, int ofs);
 */
ILNativeInt _IL_Marshal_ReadIntPtr(ILExecThread *_thread, ILNativeInt ptr,
								   ILInt32 ofs)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		/* Handle the possibility of misaligned accesses carefully */
		ILNativeInt temp;
		ILMemCpy(&temp, ((unsigned char *)ptr) + ofs, sizeof(ILNativeInt));
		return temp;
	}
	return 0;
}

/*
 * public static IntPtr ReAllocHGlobal(IntPtr pv, IntPtr cb);
 */
ILNativeInt _IL_Marshal_ReAllocHGlobal(ILExecThread *_thread,
									   ILNativeInt pv, ILNativeInt cb)
{
	if(UnmanagedOK(_thread))
	{
		/* Use the underlying system "realloc", because "ILRealloc"
		   may have been redirected elsewhere */
		void *ptr = (void *)realloc((void *)pv, (unsigned)cb);
		if(ptr)
		{
			return (ILNativeInt)ptr;
		}
		ILExecThreadThrowOutOfMemory(_thread);
	}
	return 0;
}

/*
 * public static int SizeOf(Type t);
 */
ILInt32 _IL_Marshal_SizeOf(ILExecThread * _thread, ILObject *t)
{
	ILClass *classInfo;

	if(UnmanagedOK(_thread) && t)
	{
		/* Convert the "Type" into an "ILClass *" structure */
		classInfo = _ILGetClrClass(_thread, t);
		if(!classInfo)
		{
			return 0;
		}

		/* Get the size of the type and return it */
		return (ILInt32)(ILSizeOfType
					(_thread, ILType_FromValueType(classInfo)));
	}
	else
	{
		return 0;
	}
}

/*
 * public static IntPtr StringToHGlobalAnsi(String s);
 */
ILNativeInt _IL_Marshal_StringToHGlobalAnsi(ILExecThread *_thread, ILString *s)
{
	if(UnmanagedOK(_thread) && s)
	{
		ILUInt16 *buf = StringToBuffer(s);
		ILInt32 len = ((System_String *)s)->length;
		unsigned long size = ILAnsiGetByteCount(buf, (unsigned long)len);
		char *newStr = (char *)malloc(size + 1);
		if(!newStr)
		{
			ILExecThreadThrowOutOfMemory(_thread);
			return 0;
		}
		ILAnsiGetBytes(buf, (unsigned long)len,
					   (unsigned char *)newStr, size);
		newStr[size] = '\0';
		return (ILNativeInt)newStr;
	}
	else
	{
		return 0;
	}
}

/*
 * public static IntPtr UnsafeAddrOfPinnedArrayElement(Array arr, int index);
 */
ILNativeInt _IL_Marshal_UnsafeAddrOfPinnedArrayElement(ILExecThread *_thread,
													   ILObject *arr,
													   ILInt32 index)
{
	ILType *type;
	ILInt32 elemSize;

	if(UnmanagedOK(_thread) && arr && index >= 0 &&
	   _ILIsSArray((System_Array *)arr) &&
	   index < ((System_Array *)arr)->length)
	{
		type = ILClassGetSynType(GetObjectClass(arr));
		type = ILTypeGetEnumType(ILType_ElemType(type));
		elemSize = (ILInt32)(ILSizeOfType(_thread, type));
		return (ILNativeInt)(((unsigned char *)(ArrayToBuffer(arr))) +
								elemSize * index);
	}
	else
	{
		return 0;
	}
}

/*
 * public static void WriteByte(IntPtr ptr, int ofs, byte val);
 */
void _IL_Marshal_WriteByte(ILExecThread *_thread, ILNativeInt ptr,
						   ILInt32 ofs, ILUInt8 val)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		((ILUInt8 *)ptr)[ofs] = val;
	}
}

/*
 * public static void WriteInt16(IntPtr ptr, int ofs, short val);
 */
void _IL_Marshal_WriteInt16(ILExecThread *_thread, ILNativeInt ptr,
							ILInt32 ofs, ILInt16 val)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		/* Handle the possibility of misaligned accesses carefully */
		ILMemCpy(((unsigned char *)ptr) + ofs, &val, sizeof(val));
	}
}

/*
 * public static void WriteInt32(IntPtr ptr, int ofs, int val);
 */
void _IL_Marshal_WriteInt32(ILExecThread *_thread, ILNativeInt ptr,
						    ILInt32 ofs, ILInt32 val)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		/* Handle the possibility of misaligned accesses carefully */
		ILMemCpy(((unsigned char *)ptr) + ofs, &val, sizeof(val));
	}
}

/*
 * public static void WriteInt64(IntPtr ptr, int ofs, long val);
 */
void _IL_Marshal_WriteInt64(ILExecThread *_thread, ILNativeInt ptr,
							ILInt32 ofs, ILInt64 val)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		/* Handle the possibility of misaligned accesses carefully */
		ILMemCpy(((unsigned char *)ptr) + ofs, &val, sizeof(val));
	}
}

/*
 * public static void WriteIntPtr(IntPtr ptr, int ofs, IntPtr val);
 */
void _IL_Marshal_WriteIntPtr(ILExecThread *_thread, ILNativeInt ptr,
							 ILInt32 ofs, ILNativeInt val)
{
	if(UnmanagedOK(_thread) && ptr)
	{
		/* Handle the possibility of misaligned accesses carefully */
		ILMemCpy(((unsigned char *)ptr) + ofs, &val, sizeof(val));
	}
}

#endif /* IL_CONFIG_PINVOKE */

#ifdef	__cplusplus
};
#endif
