/*
 * socket.c - Socket-related functions.
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

#include "il_sysio.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

ILSysIOHandle ILSysIOSocket(ILInt32 domain, ILInt32 type, ILInt32 protocol)
{
	return (ILSysIOHandle)(socket(domain, type, protocol));
}

int ILSysIOSocketBind(ILSysIOHandle sockfd, ILSysIOSockAddr *local_addr)
{
	struct sockaddr_in addr;

	ILMemZero(&addr, sizeof(addr));
	addr.sin_port = htons(local_addr->port);
	addr.sin_family = local_addr->family;
	addr.sin_addr.s_addr = htonl(local_addr->addr);

	return (bind((int)sockfd, (struct sockaddr *)&addr,
				 sizeof(struct sockaddr)) == 0);
}

int ILSysIOSocketConnect(ILSysIOHandle sockfd, ILSysIOSockAddr *serv_addr)
{
	struct sockaddr_in addr;

	ILMemZero(&addr, sizeof(addr));
	addr.sin_port = htons(serv_addr->port);
	addr.sin_family = serv_addr->family;
	addr.sin_addr.s_addr = htonl(serv_addr->addr);

	return (connect((int)sockfd, (struct sockaddr *)&addr,
					sizeof(struct sockaddr)) == 0);
}

int ILSysIOSocketListen(ILSysIOHandle sockfd, ILInt32 backlog)
{
	return (listen((int)sockfd, backlog) == 0);
}

ILSysIOHandle ILSysIOSocketAccept(ILSysIOHandle sockfd, ILSysIOSockAddr *out)
{
	int newfd, size;
	struct sockaddr_in addr;

	size = sizeof(struct sockaddr_in);
	ILMemZero(&addr, sizeof(addr));
	
	newfd = accept((int)sockfd, (struct sockaddr *)&addr, &size);
  
	out->port = ntohs(addr.sin_port);
	out->family = addr.sin_family;
	out->addr = ntohl(addr.sin_addr.s_addr);

	return (ILSysIOHandle)newfd;
}

ILInt32 ILSysIOSocketReceive(ILSysIOHandle sockfd, void *buff,
						     ILInt32 len, ILInt32 flags)
{
	return (ILInt32)(recv((int)sockfd, buff, len, flags));
}

ILInt32 ILSysIOSocketSend(ILSysIOHandle sockfd, const void *msg,
					      ILInt32 len, ILInt32 flags)
{
	return (ILInt32)(send((int)sockfd, msg, len, flags));
}

ILInt32 ILSysIOSocketSendTo(ILSysIOHandle sockfd, const void *msg,
					        ILInt32 len, ILInt32 flags,
					        const ILSysIOSockAddr *to)
{
	struct sockaddr_in addr;

	ILMemZero(&addr, sizeof(addr));
	addr.sin_port = htons(to->port);
	addr.sin_family = to->family;
	addr.sin_addr.s_addr = htonl(to->addr);

	return sendto((int)sockfd, msg, len, flags,
				  (struct sockaddr *)&addr, sizeof(struct sockaddr));
}

ILInt32 ILSysIOSocketRecvFrom(ILSysIOHandle sockfd, void *buf,
							  ILInt32 len, ILInt32 flags,
							  ILSysIOSockAddr *from)
{
	int fromlen, result;
	struct sockaddr_in addr;

	ILMemZero(&addr, sizeof(addr));
  	fromlen = sizeof(struct sockaddr);

	result = recvfrom((int)sockfd, buf, len, flags,
					  (struct sockaddr *)&addr, &fromlen);

	from->port = ntohs(addr.sin_port);
	from->family = addr.sin_family;
	from->addr = ntohl(addr.sin_addr.s_addr);

	return (ILInt32)result;
}

int ILSysIOSocketClose(ILSysIOHandle sockfd)
{
	return (close((int)sockfd) == 0);
}

int ILSysIOSocketShutdown(ILSysIOHandle sockfd, ILInt32 how)
{
	return (shutdown((int)sockfd, how) == 0);
}
