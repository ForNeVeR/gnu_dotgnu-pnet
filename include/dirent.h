/*
 * dirent.h - Format of directory entries.
 *
 * This file is part of the Portable.NET C library.
 * Copyright (C) 2003  Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	_DIRENT_H
#define	_DIRENT_H

#include <features.h>
#include <bits/types.h>

__BEGIN_DECLS

struct dirent
  {
    ino_t d_ino; /* TODO: file serial number */
    char d_name[256]; /* name of entry */
  };

#define d_ino d_fileno /* backward compatibility */

typedef struct
  {
    void *gc_handle; /* handle for managed directory information */
    struct dirent current; /* storage for readdir() calls */
  } DIR;

extern int closedir (DIR *__dirp);
extern DIR *opendir (const char *__name);
extern struct dirent *readdir (DIR *__dirp);
extern int readdir_r (DIR * __restrict __dirp,
                      struct dirent * __restrict __entry,
                      struct dirent ** __restrict __result);
extern void rewinddir (DIR *__dirp);
extern void seekdir (DIR *__dirp, long __pos);
extern long telldir (DIR *__dirp);

__END_DECLS

#endif /* dirent.h  */
