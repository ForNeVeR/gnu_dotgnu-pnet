/*
 * readdir_r.c - Read a directory entry into the given entry storage.
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

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>

typedef __csharp__(System.Runtime.InteropServices.Marshal) Marshal;

extern void *__syscall_readdir(void *gc_handle, void *err, void *len);

int
__readdir_r (DIR *dirp, struct dirent *entry, struct dirent **result)
{
  char *str;
  int err;
  int len;

  if (dirp == NULL || dirp->gc_handle == NULL)
    {
      errno = EBADF;
      return EBADF;
    }

  err = 0;
  len = 0;
  str = (char *)__syscall_readdir (dirp->gc_handle, &err, &len);

  if (err)
    {
      errno = err;
    }
  if (str == NULL)
    {
      *result = NULL;
      return err;
    }
  if (len == 0)
    {
      (void)Marshal::FreeHGlobal((__native__ int)str);
      *result = NULL;
      return err;
    }

  strncpy (entry->d_name, str, len);
  entry->d_name[len] = '\0';
  (void)Marshal::FreeHGlobal((__native__ int)str);
  *result = entry;

  return err;
}
weak_alias (__readdir_r, readdir_r)
