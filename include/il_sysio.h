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

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_SYSIO_H */
