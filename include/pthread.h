/*
 * pthread.h - Threading routines.
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

#ifndef _PTHREAD_H
#define _PTHREAD_H	1

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

/*
 * Mutex types.
 */
enum
{
  PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP
#ifdef __USE_UNIX98
  ,
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
#endif
#ifdef __USE_GNU
  /* For compatibility.  */
  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_ADAPTIVE_NP
#endif
};

/*
 * Thread identifier.
 */
typedef long long pthread_t;

/*
 * Fast spin lock.
 */
struct _pthread_fastlock
{
  int __spinlock;
};
#define	__LOCK_INITIALIZER	{0}

/*
 * Structure of a mutex.
 */
typedef struct
{
  int __m_handle;                 /* Underlying mutex object handle */
  int __m_count;                  /* Depth of recursive locking */
  pthread_t __m_owner;            /* Owner thread (if recursive or errcheck) */
  int __m_kind;                   /* Mutex kind: fast, recursive or errcheck */
  struct _pthread_fastlock __m_lock; /* Underlying fast lock */

} pthread_mutex_t;
#define PTHREAD_MUTEX_INITIALIZER \
  {0, 0, 0, PTHREAD_MUTEX_TIMED_NP, __LOCK_INITIALIZER}
#ifdef __USE_GNU
# define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
  {0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, __LOCK_INITIALIZER}
# define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP \
  {0, 0, 0, PTHREAD_MUTEX_ERRORCHECK_NP, __LOCK_INITIALIZER}
# define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP \
  {0, 0, 0, PTHREAD_MUTEX_ADAPTIVE_NP, __LOCK_INITIALIZER}
#endif

/*
 * Mutex attributes.
 */
typedef struct
{
  int __mutexkind;

} pthread_mutexattr_t;

/* Functions for mutex handling.  */

/* Initialize MUTEX using attributes in *MUTEX_ATTR, or use the
   default values if later is NULL.  */
extern int pthread_mutex_init (pthread_mutex_t *__restrict __mutex,
			       __const pthread_mutexattr_t *__restrict
			       __mutex_attr) __THROW;

/* Destroy MUTEX.  */
extern int pthread_mutex_destroy (pthread_mutex_t *__mutex) __THROW;

/* Try to lock MUTEX.  */
extern int pthread_mutex_trylock (pthread_mutex_t *__mutex) __THROW;

/* Wait until lock for MUTEX becomes available and lock it.  */
extern int pthread_mutex_lock (pthread_mutex_t *__mutex) __THROW;

#ifdef __USE_XOPEN2K
/* Wait until lock becomes available, or specified time passes. */
extern int pthread_mutex_timedlock (pthread_mutex_t *__restrict __mutex,
				    __const struct timespec *__restrict
				    __abstime) __THROW;
#endif

/* Unlock MUTEX.  */
extern int pthread_mutex_unlock (pthread_mutex_t *__mutex) __THROW;


/* Functions for handling mutex attributes.  */

/* Initialize mutex attribute object ATTR with default attributes
   (kind is PTHREAD_MUTEX_TIMED_NP).  */
extern int pthread_mutexattr_init (pthread_mutexattr_t *__attr) __THROW;

/* Destroy mutex attribute object ATTR.  */
extern int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr) __THROW;

/* Get the process-shared flag of the mutex attribute ATTR.  */
extern int pthread_mutexattr_getpshared (__const pthread_mutexattr_t *
					 __restrict __attr,
					 int *__restrict __pshared) __THROW;

/* Set the process-shared flag of the mutex attribute ATTR.  */
extern int pthread_mutexattr_setpshared (pthread_mutexattr_t *__attr,
					 int __pshared) __THROW;

#ifdef __USE_UNIX98
/* Set the mutex kind attribute in *ATTR to KIND (either PTHREAD_MUTEX_NORMAL,
   PTHREAD_MUTEX_RECURSIVE, PTHREAD_MUTEX_ERRORCHECK, or
   PTHREAD_MUTEX_DEFAULT).  */
extern int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind)
     __THROW;

/* Return in *KIND the mutex kind attribute in *ATTR.  */
extern int pthread_mutexattr_gettype (__const pthread_mutexattr_t *__restrict
				      __attr, int *__restrict __kind) __THROW;
#endif


__END_DECLS

#endif  /* !_PTHREAD_H */
