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
#if TIME_WITH_SYS_TIME
	#include <sys/time.h>
    #include <time.h>
#else
    #if HAVE_SYS_TIME_H
		#include <sys/time.h>
    #else
        #include <time.h>
    #endif
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

#ifdef	__cplusplus
extern	"C" {
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
				 sizeof(struct sockaddr_in)) == 0);
}

int ILSysIOSocketConnect(ILSysIOHandle sockfd, ILSysIOSockAddr *serv_addr)
{
	struct sockaddr_in addr;

	ILMemZero(&addr, sizeof(addr));
	addr.sin_port = htons(serv_addr->port);
	addr.sin_family = serv_addr->family;
	addr.sin_addr.s_addr = htonl(serv_addr->addr);

	return (connect((int)sockfd, (struct sockaddr *)&addr,
					sizeof(struct sockaddr_in)) == 0);
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
				  (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}

ILInt32 ILSysIOSocketRecvFrom(ILSysIOHandle sockfd, void *buf,
							  ILInt32 len, ILInt32 flags,
							  ILSysIOSockAddr *from)
{
	int fromlen, result;
	struct sockaddr_in addr;

	ILMemZero(&addr, sizeof(addr));
  	fromlen = sizeof(struct sockaddr_in);

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

ILInt32 ILSysIOSocketSelect(ILSysIOHandle **readfds, ILInt32 numRead,
						    ILSysIOHandle **writefds, ILInt32 numWrite,
						    ILSysIOHandle **exceptfds, ILInt32 numExcept,
						    ILInt64 timeout)
{
	fd_set readSet, writeSet, exceptSet;
	fd_set *readPtr, *writePtr, *exceptPtr;
	int highest = -1;
	int fd, result;
	struct timeval currtime;
	struct timeval endtime;
	struct timeval difftime;
	ILInt32 index;

	/* Convert the read array into an "fd_set" */
	FD_ZERO(&readSet);
	if(readfds)
	{
		readPtr = &readSet;
		for(index = 0; index < numRead; ++index)
		{
			fd = (int)(readfds[index]);
			if(fd != -1)
			{
				FD_SET(fd, &readSet);
				if(fd > highest)
				{
					highest = fd;
				}
			}
		}
	}
	else
	{
		readPtr = 0;
	}

	/* Convert the write array into an "fd_set" */
	FD_ZERO(&writeSet);
	if(writefds)
	{
		writePtr = &writeSet;
		for(index = 0; index < numWrite; ++index)
		{
			fd = (int)(writefds[index]);
			if(fd != -1)
			{
				FD_SET(fd, &writeSet);
				if(fd > highest)
				{
					highest = fd;
				}
			}
		}
	}
	else
	{
		writePtr = 0;
	}

	/* Convert the except array into an "fd_set" */
	FD_ZERO(&exceptSet);
	if(exceptfds)
	{
		exceptPtr = &exceptSet;
		for(index = 0; index < numExcept; ++index)
		{
			fd = (int)(exceptfds[index]);
			if(fd != -1)
			{
				FD_SET(fd, &exceptSet);
				if(fd > highest)
				{
					highest = fd;
				}
			}
		}
	}
	else
	{
		exceptPtr = 0;
	}

	/* Is this a timed select or an infinite select? */
	if(timeout >= 0)
	{
		/* Get the current time of day and determine the end time */
		gettimeofday(&currtime, 0);
		endtime.tv_sec = currtime.tv_sec + (long)(timeout / (ILInt64)1000000);
		endtime.tv_usec = currtime.tv_usec + (long)(timeout % (ILInt64)1000000);
		if(endtime.tv_usec >= 1000000L)
		{
			++(endtime.tv_sec);
			endtime.tv_usec -= 1000000L;
		}

		/* Loop while we are interrupted by signals */
		for(;;)
		{
			/* How long until the timeout? */
			difftime.tv_sec = endtime.tv_sec - currtime.tv_sec;
			if(endtime.tv_usec >= currtime.tv_usec)
			{
				difftime.tv_usec = endtime.tv_usec - currtime.tv_usec;
			}
			else
			{
				difftime.tv_usec = endtime.tv_usec + 1000000L -
								   currtime.tv_usec;
				difftime.tv_sec -= 1;
			}

			/* Perform a trial select, which may be interrupted */
			result = select(highest + 1, readPtr, writePtr,
							exceptPtr, &difftime);
			if(result >= 0 || errno != EINTR)
			{
				break;
			}

			/* We were interrupted, so get the current time again.
			   We have to this because many systems do not update
			   the 5th paramter of "select" to reflect how much time
			   is left to go */
			gettimeofday(&currtime, 0);

			/* Are we past the end time? */
			if(currtime.tv_sec > endtime.tv_sec ||
			   (currtime.tv_sec == endtime.tv_sec &&
			    currtime.tv_usec >= endtime.tv_usec))
			{
				/* We are, so simulate timeout */
				result = 0;
				break;
			}
		}
	}
	else
	{
		/* Infinite select */
		while((result = select(highest + 1, readPtr, writePtr, exceptPtr,
							   (struct timeval *)0)) < 0)
		{
			/* Keep looping while we are being interrupted by signals */
			if(result != EINTR)
			{
				break;
			}
		}
	}

	/* Update the descriptor sets if something fired */
	if(result > 0)
	{
		/* Update the read descriptors */
		if(readPtr)
		{
			for(index = 0; index < numRead; ++index)
			{
				fd = (int)(readfds[index]);
				if(fd != -1 && !FD_ISSET(fd, &readSet))
				{
					readfds[index] = ILSysIOHandle_Invalid;
				}
			}
		}

		/* Update the write descriptors */
		if(writePtr)
		{
			for(index = 0; index < numWrite; ++index)
			{
				fd = (int)(writefds[index]);
				if(fd != -1 && !FD_ISSET(fd, &writeSet))
				{
					writefds[index] = ILSysIOHandle_Invalid;
				}
			}
		}

		/* Update the except descriptors */
		if(exceptPtr)
		{
			for(index = 0; index < numExcept; ++index)
			{
				fd = (int)(exceptfds[index]);
				if(fd != -1 && !FD_ISSET(fd, &exceptSet))
				{
					exceptfds[index] = ILSysIOHandle_Invalid;
				}
			}
		}
	}

	/* Return the result to the caller */
	return (ILInt32)result;
}

#ifdef	__cplusplus
};
#endif
