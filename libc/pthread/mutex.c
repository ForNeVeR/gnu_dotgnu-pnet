/*
 * mutex.c - Mutex handling for the pthread emulation.
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

#include <pthread.h>

int
pthread_mutex_init (pthread_mutex_t *__restrict __mutex,
		    __const pthread_mutexattr_t *__restrict
		    __mutex_attr) __THROW
{
  __mutex->__m_handle = 0;
  __mutex->__m_count = 0;
  __mutex->__m_owner = 0;
  __mutex->__m_kind = PTHREAD_MUTEX_TIMED_NP;
  __mutex->__m_lock.__spinlock = 0;
  return 0;
}

int
pthread_mutex_destroy (pthread_mutex_t *__mutex) __THROW
{
  /* TODO */
  return 0;
}

int
pthread_mutex_trylock (pthread_mutex_t *__mutex) __THROW
{
  /* TODO */
  return 0;
}

int
pthread_mutex_lock (pthread_mutex_t *__mutex) __THROW
{
  /* TODO */
  return 0;
}

int
pthread_mutex_timedlock (pthread_mutex_t *__restrict __mutex,
			 __const struct timespec *__restrict
			 __abstime) __THROW
{
  /* TODO */
  return 0;
}

int
pthread_mutex_unlock (pthread_mutex_t *__mutex) __THROW
{
  /* TODO */
  return 0;
}


int
pthread_mutexattr_init (pthread_mutexattr_t *__attr) __THROW
{
  __attr->__mutexkind = PTHREAD_MUTEX_TIMED_NP;
  return 0;
}

int
pthread_mutexattr_destroy (pthread_mutexattr_t *__attr) __THROW
{
  return 0;
}

int
pthread_mutexattr_getpshared (__const pthread_mutexattr_t *
			      __restrict __attr,
			      int *__restrict __pshared) __THROW
{
  /* TODO */
  return 0;
}

int
pthread_mutexattr_setpshared (pthread_mutexattr_t *__attr,
			      int __pshared) __THROW
{
  /* TODO */
  return 0;
}

int
pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind) __THROW
{
  /* TODO */
  return 0;
}

int
pthread_mutexattr_gettype (__const pthread_mutexattr_t *__restrict
			   __attr, int *__restrict __kind) __THROW
{
  /* TODO */
  return 0;
}
