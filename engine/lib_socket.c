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
#ifdef HAVE_NETDB_H
	#include <netdb.h>
#endif

/*
 * public static bool Create(int af, int st, int pt, out IntPtr handle);
 */
ILBool _IL_SocketMethods_Create(ILExecThread *_thread, ILInt32 af,
							    ILInt32 st, ILInt32 pt,
								ILNativeInt *handle)
{
	*handle = (ILNativeInt)(ILSysIOSocket(af, st, pt));
	return (*handle != (ILNativeInt)ILSysIOHandle_Invalid);
}

/*
 * public static bool Bind(IntPtr handle, int af, long address, int port);
 */
ILBool _IL_SocketMethods_Bind(ILExecThread *_thread, ILNativeInt handle,
							  ILInt32 af, ILInt64 address, ILInt32 port)
{
	ILSysIOSockAddr addr;

	addr.family = (int)af;
	addr.port = (unsigned short)port;
	addr.addr = (unsigned long)address;

	return (ILBool)(ILSysIOSocketBind((ILSysIOHandle)handle, &addr));
}

/*
 * public static bool Shutdown(IntPtr handle, int how);
 */
ILBool _IL_SocketMethods_Shutdown(ILExecThread * _thread,
								  ILNativeInt handle, ILInt32 how)
{
	return (ILBool)(ILSysIOSocketShutdown((ILSysIOHandle)handle, how));
}

/*
 * public static bool Listen(IntPtr handle, int backlog);
 */
ILBool _IL_SocketMethods_Listen(ILExecThread *_thread,
								ILNativeInt handle, ILInt32 backlog)
{
	return (ILBool)(ILSysIOSocketListen((ILSysIOHandle)handle, backlog));
}

/*
 * public static bool Accept(IntPtr handle, out long address, out int port,
 *                           out IntPtr newHandle);
 */
ILBool _IL_SocketMethods_Accept(ILExecThread *_thread, ILNativeInt handle,
								ILInt64 *address, ILInt32 *port,
								ILNativeInt *newHandle)
{
	ILSysIOSockAddr addr;

	*newHandle = (ILNativeInt)(ILSysIOSocketAccept
					((ILSysIOHandle)handle, &addr));

	*address = (ILInt64)(addr.addr);
	*port = (ILInt32)(addr.port);

	return (*newHandle != (ILNativeInt)ILSysIOHandle_Invalid);
}

/*
 * public static bool Connect(IntPtr handle, int af, long address, int port);
 */
ILBool _IL_SocketMethods_Connect(ILExecThread *_thread, ILNativeInt handle,
								 ILInt32 af, ILInt64 address, ILInt32 port)
{
	ILSysIOSockAddr addr;

	addr.family = (int)af;
	addr.addr = (unsigned long)address;
	addr.port = (unsigned short)port;

	return (ILBool)(ILSysIOSocketConnect((ILSysIOHandle)handle, &addr));
}

/*
 * public static int Receive(IntPtr handle, byte[] buffer, int offset,
 *							 int size, int flags);
 */
ILInt32 _IL_SocketMethods_Receive(ILExecThread *_thread, ILNativeInt handle,
								  System_Array *buffer, ILInt32 offset,
								  ILInt32 size, ILInt32 flags)
{
	return ILSysIOSocketReceive((ILSysIOHandle)handle,
								((ILUInt8 *)(ArrayToBuffer(buffer))) + offset,
								size, flags);
}

/*
 * public static int ReceiveFrom(IntPtr handle, byte[] buffer,
 *								 int offset, int size, int flags,
 *								 out long address, out int port);
 */
ILInt32 _IL_SocketMethods_ReceiveFrom(ILExecThread *_thread,
									  ILNativeInt handle,
									  System_Array *buffer,
									  ILInt32 offset, ILInt32 size,
									  ILInt32 flags, ILInt64 *address,
									  ILInt32 *port)
{
	ILSysIOSockAddr addr;
	ILInt32 result;

	result = ILSysIOSocketRecvFrom
				((ILSysIOHandle)handle,
			     ((ILUInt8 *)(ArrayToBuffer(buffer))) + offset,
				 size, flags, &addr);

	*address = (ILInt64)(addr.addr);
	*port = (ILInt32)(addr.port);

	return result;
}

/*
 * public static int Send(IntPtr handle, byte[] buffer, int offset,
 *						  int size, int flags);
 */
ILInt32 _IL_SocketMethods_Send(ILExecThread *_thread, ILNativeInt handle,
							   System_Array *buffer, ILInt32 offset,
							   ILInt32 size, ILInt32 flags)
{
	return ILSysIOSocketSend((ILSysIOHandle)handle,
							 ((ILUInt8 *)(ArrayToBuffer(buffer))) + offset,
							 size, flags);
}

/*
 * public static int SendTo(IntPtr handle, byte[] buffer,
 *							int offset, int size, int flags,
 *							long address, int port);
 */
ILInt32 _IL_SocketMethods_SendTo(ILExecThread *_thread, ILNativeInt handle,
								 System_Array *buffer, ILInt32 offset,
								 ILInt32 size, ILInt32 flags,
								 ILInt64 address, ILInt32 port)
{
	ILSysIOSockAddr addr;

	addr.addr = (unsigned long)address;
	addr.port = (unsigned short)port;

	return ILSysIOSocketSendTo
				((ILSysIOHandle)handle,
			     ((ILUInt8 *)(ArrayToBuffer(buffer))) + offset,
				 size, flags, &addr);
}

ILBool _IL_SocketMethods_Close(ILExecThread *_thread, ILNativeInt handle)
{
	return (ILBool)(ILSysIOSocketClose((ILSysIOHandle)handle));
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
	return ILSysIOSocketSelect
		((readarray ? (ILSysIOHandle **)(ArrayToBuffer(readarray)) : 0),
		 (readarray ? readarray->length : 0),
		 (writearray ? (ILSysIOHandle **)(ArrayToBuffer(writearray)) : 0),
		 (writearray ? writearray->length : 0),
		 (errorarray ? (ILSysIOHandle **)(ArrayToBuffer(errorarray)) : 0),
		 (errorarray ? errorarray->length : 0), timeout);
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

/*
 * public static long HostToNetworkOrder(long host);
 */
ILInt64 _IL_IPAddress_HostToNetworkOrder_l(ILExecThread *thread, ILInt64 host)
{
	unsigned char value[8];
	value[0] = (unsigned char)(host >> 56);
	value[1] = (unsigned char)(host >> 48);
	value[2] = (unsigned char)(host >> 40);
	value[3] = (unsigned char)(host >> 32);
	value[4] = (unsigned char)(host >> 24);
	value[5] = (unsigned char)(host >> 16);
	value[6] = (unsigned char)(host >> 8);
	value[7] = (unsigned char)host;
	return *((ILInt64 *)value);
}

/*
 * public static int HostToNetworkOrder(int host);
 */
ILInt32 _IL_IPAddress_HostToNetworkOrder_i(ILExecThread *thread, ILInt32 host)
{
	unsigned char value[4];
	value[0] = (unsigned char)(host >> 24);
	value[1] = (unsigned char)(host >> 16);
	value[2] = (unsigned char)(host >> 8);
	value[3] = (unsigned char)host;
	return *((ILInt32 *)value);
}

/*
 * public static short HostToNetworkOrder(short host);
 */
ILInt16 _IL_IPAddress_HostToNetworkOrder_s(ILExecThread *thread, ILInt16 host)
{
	unsigned char value[2];
	value[0] = (unsigned char)(host >> 8);
	value[1] = (unsigned char)host;
	return *((ILInt16 *)value);
}

#define	GETBYTE(type,value,offset,shift)	\
			(((type)(((unsigned char *)&(value))[(offset)])) << (shift))

/*
 * public static long NetworkToHostOrder(long host);
 */
ILInt64 _IL_IPAddress_NetworkToHostOrder_l(ILExecThread *thread,
										   ILInt64 network)
{
	return GETBYTE(ILInt64, network, 0, 56) |
		   GETBYTE(ILInt64, network, 1, 48) |
		   GETBYTE(ILInt64, network, 2, 40) |
		   GETBYTE(ILInt64, network, 3, 32) |
		   GETBYTE(ILInt64, network, 4, 24) |
		   GETBYTE(ILInt64, network, 5, 16) |
		   GETBYTE(ILInt64, network, 6,  8) |
		   GETBYTE(ILInt64, network, 7,  0);
}

/*
 * public static int NetworkToHostOrder(int host);
 */
ILInt32 _IL_IPAddress_NetworkToHostOrder_i(ILExecThread *thread,
										   ILInt32 network)
{
	return GETBYTE(ILInt32, network, 0, 24) |
		   GETBYTE(ILInt32, network, 1, 16) |
		   GETBYTE(ILInt32, network, 2,  8) |
		   GETBYTE(ILInt32, network, 3,  0);
}

/*
 * public static int NetworkToHostOrder(int host);
 */
ILInt16 _IL_IPAddress_NetworkToHostOrder_s(ILExecThread *thread,
										   ILInt16 network)
{
	return GETBYTE(ILInt16, network, 0, 8) |
		   GETBYTE(ILInt16, network, 1, 0);
}


ILBool ToIPHostEntry (ILExecThread *_thread,
				struct hostent *h_ent,
				ILString ** h_name,
				System_Array ** h_aliases,
				System_Array ** h_addr_list
				)
{
	int length=-1;
	ILObject *objs;
	ILInt64 *buffer;

	*h_name=ILStringCreate(_thread,h_ent->h_name);
	
	/* Count the aliases using the NULL sentinel */
	while(h_ent->h_aliases[++length]!=NULL);

	/* Construct an array */
	*h_aliases = (System_Array *)ILExecThreadNew(_thread, "[oSystem.String;", 
					"(Ti)V",(ILVaInt)length);
	
	if(!(*h_aliases)) 
	{
		return 0; /* Assert memory is allocated */
	}

	objs=(ILObject*)(*h_aliases);
	
	while(length--)
	{
		ILExecThreadSetElem(_thread, objs, (ILInt32)length,
			(ILObject*)ILStringCreate(_thread,h_ent->h_aliases[length]));
	}

	/* length=-1; */ /* I'm already sure it should be */
	
	/* Count the addresses using the NULL sentinel */
	while(h_ent->h_addr_list[++length]!=NULL);

	/* Construct an array */
	*h_addr_list = (System_Array *)ILExecThreadNew(_thread, "[l", 
					"(Ti)V",(ILVaInt)length);
	
	if(!(*h_addr_list)) return 0; /* Assert memory is allocated */

	buffer=ArrayToBuffer(*h_addr_list);
	while(length--)
	{
		/* crude hack to reuse GETBYTE , but gcc will optimise*/
		buffer[length]=
				GETBYTE(ILInt64,*(h_ent->h_addr_list[length]), 0, 0) |
				GETBYTE(ILInt64,*(h_ent->h_addr_list[length]), 1, 8) |
				GETBYTE(ILInt64,*(h_ent->h_addr_list[length]), 2, 16) |
				GETBYTE(ILInt64,*(h_ent->h_addr_list[length]), 3, 24);
	}
	return 1;
}

/*
 * public static bool InternalGetHostByName(String host, out String h_name ,
 * 							out String [] h_aliases, out long[] h_addr_list);
 */

ILBool _IL_Dns_InternalGetHostByName(ILExecThread * _thread, 
				ILString * host, ILString * * h_name, 
				System_Array * * h_aliases, System_Array * * h_addr_list)
{
	struct hostent* h_ent;
	
	h_ent=ILGetHostByName(ILStringToAnsi(_thread,host));
	
	if(!h_ent)
	{
		return 0; /* false on error */
	}

	return ToIPHostEntry(_thread,h_ent,h_name,h_aliases,h_addr_list);
}

/*
 * public static bool InternalGetHostByAddr(long address, out String h_name ,
 * 							out String [] h_aliases, out long[] h_addr_list);
 */

ILBool _IL_Dns_InternalGetHostByAddr(ILExecThread * _thread, ILInt64 address, 
				ILString * * h_name, System_Array * * h_aliases, 
				System_Array * * h_addr_list)
{
	struct hostent *h_ent;	
	ILInt32 ip=address;//attempt a conversion to 4 byte form...

	h_ent=ILGetHostByAddr(&ip,sizeof(ip),AF_INET);
	
	if(!h_ent)
	{
		return 0; /* false on error */
	}

	return ToIPHostEntry(_thread,h_ent,h_name,h_aliases,h_addr_list);
}
