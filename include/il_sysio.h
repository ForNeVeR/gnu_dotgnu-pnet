/*
 * il_sysio.h - Wrapper around system I/O support services that we need.
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

#ifndef	_IL_SYSIO_H
#define	_IL_SYSIO_H

#include "il_system.h"
#include "il_config.h"
#include <errno.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Opaque type for a system I/O handle.
 */
typedef void *ILSysIOHandle;

/*
 * Invalid I/O handle for error returns.
 */
#define	ILSysIOHandle_Invalid	((ILSysIOHandle)(-1))

/*
 * File open modes.
 */
#define ILFileMode_CreateNew	1
#define ILFileMode_Create		2
#define	ILFileMode_Open			3
#define ILFileMode_OpenOrCreate	4
#define ILFileMode_Truncate		5
#define ILFileMode_Append		6

/*
 * File access modes.
 */
#define ILFileAccess_Read		0x01
#define ILFileAccess_Write		0x02
#define ILFileAccess_ReadWrite	(ILFileAccess_Read | ILFileAccess_Write)

/*
 * File share modes.
 */
#define	ILFileShare_None		0x00
#define	ILFileShare_Read		0x01
#define	ILFileShare_Write		0x02
#define	ILFileShare_ReadWrite	(ILFileShare_Read | ILFileAccess_Write)
#define	ILFileShare_Inheritable	0x10

/*
 * Get the platform-independent error number for the current thread.
 */
int ILSysIOGetErrno(void);

/*
 * Set the platform-indepedent error number for the current thread.
 */
void ILSysIOSetErrno(int code);

/*
 * Map an underlying system error code (e.g. errno) to
 * a platform-independent error number.
 */
int ILSysIOConvertErrno(int code);

/*
 * Map a platform-independent error number to a system
 * error code.  Returns -1 if no appropriate mapping.
 */
int ILSysIOConvertFromErrno(int code);

/*
 * Get the system error message corresponding to a platform-independent
 * error number.  Returns NULL if no message available.
 */
const char *ILSysIOGetErrnoMessage(int code);

/*
 * Validate a pathname to check for invalid operating system characters.
 */
int ILSysIOValidatePathname(const char *path);

/*
 * Open a file and return a system I/O handle.  Returns an
 * invalid handle on error.
 */
ILSysIOHandle ILSysIOOpenFile(const char *path, ILUInt32 mode,
						      ILUInt32 access, ILUInt32 share);

/*
 * Determine if an I/O handle was opened with a particular
 * file access mode.
 */
int ILSysIOCheckHandleAccess(ILSysIOHandle handle, ILUInt32 access);

/*
 * Close a specific I/O handle.
 */
int ILSysIOClose(ILSysIOHandle handle);

/*
 * Read from an I/O handle.
 */
ILInt32 ILSysIORead(ILSysIOHandle handle, void *buf, ILInt32 size);

/*
 * Write to an I/O handle.
 */
ILInt32 ILSysIOWrite(ILSysIOHandle handle, const void *buf, ILInt32 size);

/*
 * Seek to a new position on an I/O handle.
 */
ILInt64 ILSysIOSeek(ILSysIOHandle handle, ILInt64 offset, int whence);

/*
 * Flush the read data from an I/O handle.
 */
int ILSysIOFlushRead(ILSysIOHandle handle);

/*
 * Flush the write data to an I/O handle.
 */
int ILSysIOFlushWrite(ILSysIOHandle handle);

/*
 * Truncate a file at a particular position.
 */
int ILSysIOTruncate(ILSysIOHandle handle, ILInt64 posn);

/*
 * Determine if it is possible to perform asynchronous I/O operations.
 */
int ILSysIOHasAsync(void);

/*
 * Sets the modification time of a specified file.
 */
int ILSysIOSetModificationTime(const char *path, ILInt64 time);

/*
 * Sets the access time of a specified file.
 */
int ILSysIOSetAccessTime(const char *path, ILInt64 time);

/*
 * Sets the creation time of a specified file.
 */
int ILSysIOSetCreationTime(const char *path, ILInt64 time);

/*
 * Copies a file from src to dest
 */
ILInt32 ILCopyFile(const char * src, const char * dest);

/*
 * IPv4 Address placeholder for socket related functions.
 */
typedef struct _ILSysIOSockAddr
{
  int family;
  unsigned short port;
  unsigned long addr;

} ILSysIOSockAddr;

/*
 * Create a new socket.  Returns ILSysIOHandle_Invalid on error.
 */
ILSysIOHandle ILSysIOSocket(ILInt32 domain, ILInt32 type, ILInt32 protocol);

/*
 * Bind a socket to an address.  Returns zero on error.
 */
int ILSysIOSocketBind(ILSysIOHandle sockfd, ILSysIOSockAddr *local_addr);

/*
 * Connect to a remote socket address.  Returns zero on error.
 */
int ILSysIOSocketConnect(ILSysIOHandle sockfd, ILSysIOSockAddr *serv_addr);

/*
 * Set a socket to listen mode.  Returns zero on error.
 */
int ILSysIOSocketListen(ILSysIOHandle sockfd, ILInt32 backlog);

/*
 * Accept a connection from a remote address on a socket.
 */
ILSysIOHandle ILSysIOSocketAccept(ILSysIOHandle sockfd, ILSysIOSockAddr *out);

/*
 * Receive data on a socket.
 */
ILInt32 ILSysIOSocketReceive(ILSysIOHandle sockfd, void *buff,
							 ILInt32 len, ILInt32 flags);

/*
 * Send data on a socket.
 */
ILInt32 ILSysIOSocketSend(ILSysIOHandle sockfd, const void *msg,
					      ILInt32 len, ILInt32 flags);

/*
 * Send data on a socket to a specific address.
 */
ILInt32 ILSysIOSocketSendTo(ILSysIOHandle sockfd, const void *msg, ILInt32 len,
					        ILInt32 flags, const ILSysIOSockAddr *to);

/*
 * Receive data on a socket from a specific address.
 */
ILInt32 ILSysIOSocketRecvFrom(ILSysIOHandle sockfd, void *buf, ILInt32 len,
							  ILInt32 flags, ILSysIOSockAddr *from);

/*
 * Close a socket.  Returns zero on error.
 */
int ILSysIOSocketClose(ILSysIOHandle sockfd);

/*
 * Perform a shutdown operation on one or more socket directions.
 * Returns zero on error.
 *
 * Values for "how":
 *		0 - Further receives are disallowed
 *		1 - Further sends are disallowed
 *		2 - Further sends and receives are disallowed
 */
int ILSysIOSocketShutdown(ILSysIOHandle sockfd, ILInt32 how);

/*
 * Perform a select operation on a collection of sockets.
 * Returns -1 on error, 0 on timeout, or the number of
 * file descriptors that fired.  The input arrays will be
 * modified so that any descriptors that did not fire are
 * replaced with ILSysIOHandle_Invalid.
 */
ILInt32 ILSysIOSocketSelect(ILSysIOHandle **readfds, ILInt32 numRead,
						    ILSysIOHandle **writefds, ILInt32 numWrite,
						    ILSysIOHandle **exceptfds, ILInt32 numExcept,
						    ILInt64 timeout);

/* dns.c */
struct hostent* ILGetHostByName(const char *name);
struct hostent* ILGetHostByAddr(const void *addr, unsigned int len, int type);
	
/*
 * Obtains the last access time of 'path' and stores that information in 'time'.
 * Returns 0 on success, errno otherwise.
 */
int ILSysIOPathGetLastAccess(const char *path, ILInt64 *time);

/*
 * Obtains the last modification time of 'path' and stores that information in 'time'.
 * Returns 0 on success, errno otherwise.
 */
int ILSysIOPathGetLastModification(const char *path, ILInt64 *time);

/*
 * Obtains the creation time of 'path' and stores that information in 'time'.
 * Returns 0 on success, errno otherwise.
 */
int ILSysIOPathGetCreation(const char *path, ILInt64 *time);


/*
 * Opaque type that is used to represent an open directory and an entry.
 */
typedef struct _tagILDir    ILDir;
typedef struct _tagILDirEnt ILDirEnt;

/*
 * Values returned from "ILDirEntType".
 */
#define ILFileType_Unknown			0
#define	ILFileType_FIFO				1
#define	ILFileType_CHR				2
#define	ILFileType_DIR				4
#define	ILFileType_BLK				6
#define	ILFileType_REG				8
#define	ILFileType_LNK				10
#define	ILFileType_SOCK				12
#define	ILFileType_WHT				14

/*
 * Directory access functions.
 */
ILInt32 ILDeleteDir(const char *path);
ILInt32 ILRenameDir(const char *old_name, const char *new_name);
ILDir *ILOpenDir(char *path);
ILDirEnt *ILReadDir(ILDir *directory);
int ILCloseDir(ILDir *directory);
const char *ILDirEntName(ILDirEnt *entry);
int ILDirEntType(ILDirEnt *entry);

/*
 * Change Directory
 */
ILInt32 ILChangeDir(const char *path);

#ifdef	__cplusplus 
};
#endif

#endif	/* _IL_SYSIO_H */
