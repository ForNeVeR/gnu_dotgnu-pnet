/*
 * lib_socket.c - Internalcall methods for "Platform.SocketMethods".
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
#include "il_sysio.h"
#include "il_errno.h"

/*
 * public static bool Create(int af, int st, int pt, out IntPtr handle);
 */
ILBool _IL_SocketMethods_Create(ILExecThread *_thread, ILInt32 af,
							    ILInt32 st, ILInt32 pt,
								ILNativeInt *handle)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return 0;
}

/*
 * public static bool Bind(IntPtr handle, int af, long address, int port);
 */
ILBool _IL_SocketMethods_Bind(ILExecThread *_thread, ILNativeInt handle,
							  ILInt32 af, ILInt64 address, ILInt32 port)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return 0;
}

/*
 * public static bool Shutdown(IntPtr handle, int how);
 */
ILBool _IL_SocketMethods_Shutdown(ILExecThread * _thread,
								  ILNativeInt handle, ILInt32 how)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return 0;
}

/*
 * public static bool Listen(IntPtr handle, int backlog);
 */
ILBool _IL_SocketMethods_Listen(ILExecThread *_thread,
								ILNativeInt handle, ILInt32 backlog)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return 0;
}

/*
 * public static bool Accept(IntPtr handle, out long address, out int port);
 */
ILInt32 _IL_SocketMethods_Accept(ILExecThread *_thread, ILNativeInt handle,
								 ILInt64 *address, ILInt32 *port)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return 0;
}

/*
 * public static bool Connect(IntPtr handle, int af, long address, int port);
 */
ILBool _IL_SocketMethods_Connect(ILExecThread *_thread, ILNativeInt handle,
								 ILInt32 af, ILInt64 address, ILInt32 port)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return 0;
}

/*
 * public static int Receive(IntPtr handle, byte[] buffer, int offset,
 *							 int size, int flags);
 */
ILInt32 _IL_SocketMethods_Receive(ILExecThread *_thread, ILNativeInt handle,
								  System_Array *buffer, ILInt32 offset,
								  ILInt32 size, ILInt32 flags)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return -1;
}

/*
 * public static int ReceiveFrom(IntPtr handle, byte[] buffer,
 *								 int offset, int size, int flags,
 *								 ref long address, ref int port);
 */
ILInt32 _IL_SocketMethods_ReceiveFrom(ILExecThread *_thread,
									  ILNativeInt handle,
									  System_Array *buffer,
									  ILInt32 offset, ILInt32 size,
									  ILInt32 flags, ILInt64 *address,
									  ILInt32 *port)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return -1;
}

/*
 * public static int Send(IntPtr handle, byte[] buffer, int offset,
 *						  int size, int flags);
 */
ILInt32 _IL_SocketMethods_Send(ILExecThread *_thread, ILNativeInt handle,
							   System_Array *buffer, ILInt32 offset,
							   ILInt32 size, ILInt32 flags)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return -1;
}

/*
 * public static int SendTo(IntPtr handle, byte[] buffer,
 *							int offset, int size, int flags,
 *							ref long address, ref int port);
 */
ILInt32 _IL_SocketMethods_SendTo(ILExecThread *_thread, ILNativeInt handle,
								 System_Array *buffer, ILInt32 offset,
								 ILInt32 size, ILInt32 flags,
								 ILInt64 *address, ILInt32 *port)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return -1;
}

ILBool _IL_SocketMethods_Close(ILExecThread *_thread, ILNativeInt handle)
{
	return (ILBool)(ILSysIOClose((ILSysIOHandle)handle));
}

/*
 * public static int Select(IntPtr[] readarray, IntPtr[] writearray,
 *                          IntPtr[] errorarray, long timeout);
 */
ILInt32 _IL_SocketMethods_Select(ILExecThread *_thread,
								 System_Array *readarray,
								 System_Array *writearray,
								 System_Array *errorarray,
								 ILInt64 timeout)
{
	/* TODO */
	ILSysIOSetErrno(IL_ERRNO_EPERM);
	return 0;
}

/*
 * public static Errno GetErrno();
 */
ILInt32 _IL_SocketMethods_GetErrno(ILExecThread *thread)
{
	return ILSysIOGetErrno();
}

/*
 * public static String GetErrnoMessage(Errno error);
 */
ILString *_IL_SocketMethods_GetErrnoMessage(ILExecThread *thread, ILInt32 error)
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
