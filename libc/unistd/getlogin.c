/*
 * getlogin.c - Get the user's login name.
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

#include <unistd.h>
#include <string.h>
#include <pthread.h>

typedef __csharp__(System.String) String;
typedef __csharp__(System.Runtime.InteropServices.Marshal) Marshal;
typedef __csharp__(System.Environment) Environment;

static pthread_mutex_t loginMutex = PTHREAD_MUTEX_INITIALIZER;
static char *loginName;

char *
getlogin (void)
{
  char *login;
  pthread_mutex_lock(&loginMutex);
  if (!loginName)
    {
       String str = __invoke__ Environment.get_UserName ();
       loginName = (char *)__invoke__ Marshal.StringToHGlobalAnsi (str);
       if (!loginName)
         {
	   loginName = strdup("nobody");
	 }
    }
  login = loginName;
  pthread_mutex_lock(&loginMutex);
  return login;
}

int
getlogin_r (char *name, size_t name_len)
{
  char *login = getlogin();
  strncpy (name, login, name_len);
  return strlen (login);
}
