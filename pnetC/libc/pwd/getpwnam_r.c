/*
 * getpwnam_r.c - Get a password file entry by name, using re-entrant scanning.
 *
 * This file is part of the Portable.NET C library.
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#include <pwd.h>
#include <string.h>
#include <errno.h>
#include "pwent.h"

int
getpwnam_r (const char *name,
            struct passwd * __restrict resultbuf,
            char * __restrict buffer, size_t buflen,
            struct passwd ** __restrict result)
{
  int posn;

  /* Validate the parameters */
  if (!resultbuf || !buffer || !result)
    {
      return ERANGE;
    }
  if (!name)
    {
      *result = 0;
      return 0;
    }

  /* Scan the password file until we find a match */
  posn = 0;
  while (__nextpwent (posn, resultbuf))
    {
      if (!strcmp (resultbuf->pw_name, name))
        {
          if (__persistpw (resultbuf, (char *)buffer, buflen))
            {
              *result = resultbuf;
              return 0;
            }
          else
            {
              return ERANGE;
            }
        }
      ++posn;
    }

  /* We were unable to find a match */
  *result = 0;
  return 0;
}
