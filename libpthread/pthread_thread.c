/*
 * pthread_thread.c - Thread management routines for Pthreads.
 *
 * This file is part of the Portable.NET C library.
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
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
#include <setjmp.h>
#include <errno.h>
#include <string.h>
#include <pthread-support.h>

/*
 * Thread-specific information that is used to exit from a thread.
 */
static __thread_specific__ jmp_buf exit_buf;
static __thread_specific__ void *exit_retval;

/*
 * Imports from "pthread_glue.cs".
 */
extern pthread_t __pt_thread_create (void *start, void *arg);
extern int __pt_thread_is_detached (long long thread);

void *
__pt_thread_run (long long id, void *start_routine, void *arg)
{
  typedef void * (*startroutine_t) (void *);
  if (setjmp (exit_buf) == 0)
    exit_retval = (*((startroutine_t)start_routine)) (arg);
  return exit_retval;
}

int
pthread_create (pthread_t *__restrict thread,
                const pthread_attr_t *__restrict attr,
		void *(*start_routine) (void *),
		void *__restrict arg)
{
  *thread = __pt_thread_create ((void *)start_routine, arg);
  if (attr && attr->__detachstate != 0)
    {
      pthread_detach (*thread);
    }
  return 0;
}

pthread_t
pthread_self (void)
{
  return __pthread_self ();
}

int
pthread_equal (pthread_t thread1, pthread_t thread2)
{
  return (thread1 == thread2);
}

void
pthread_exit (void *retval)
{
  exit_retval = retval;
  longjmp (exit_buf, 1);
}

int
pthread_yield (void)
{
  /* Nothing to do here as C# does not support yielding */
  return 0;
}

void
__pt_thread_cleanup (void)
{
  /* TODO: Run the cleanup routines for the current thread */
}

int
pthread_setschedparam (pthread_t target_thread, int policy,
		       const struct sched_param *param)
{
  /* Only the superuser can set scheduling policies */
  errno = EPERM;
  return -1;
}

int
pthread_getschedparam (pthread_t target_thread,
                       int *__restrict policy,
                       struct sched_param *__restrict param)
{
  if (!policy || !param)
    {
      errno = EFAULT;
      return -1;
    }
  else if (__pt_thread_is_detached (target_thread) < 0)
    {
      errno = ESRCH;
      return -1;
    }
  *policy = SCHED_OTHER;
  param->__sched_priority = 0;
  return 0;
}

int
pthread_attr_init (pthread_attr_t *attr)
{
  memset (attr, 0, sizeof(pthread_attr_t));
  return 0;
}

int
pthread_attr_destroy (pthread_attr_t *attr)
{
  /* Nothing to do here */
  return 0;
}

int
pthread_attr_setdetachstate (pthread_attr_t *attr, int detachstate)
{
  attr->__detachstate = detachstate;
  return 0;
}

int
pthread_attr_getdetachstate (const pthread_attr_t *attr, int *detachstate)
{
  *detachstate = attr->__detachstate;
  return 0;
}

int
pthread_attr_setschedparam (pthread_attr_t *__restrict attr,
                            const struct sched_param *__restrict param)
{
  attr->__schedparam = *param;
  return 0;
}

int
pthread_attr_getschedparam (const pthread_attr_t *__restrict attr,
                            struct sched_param *__restrict param)
{
  *param = attr->__schedparam;
  return 0;
}

int
pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy)
{
  attr->__schedpolicy = policy;
  return 0;
}

int
pthread_attr_getschedpolicy (const pthread_attr_t *__restrict attr,
                             int *__restrict policy)
{
  *policy = attr->__schedpolicy;
  return 0;
}

int
pthread_attr_setinheritsched (pthread_attr_t *attr, int inherit)
{
  attr->__inheritsched = inherit;
  return 0;
}

int
pthread_attr_getinheritsched (const pthread_attr_t *__restrict attr,
                              int *__restrict inherit)
{
  *inherit = attr->__inheritsched;
  return 0;
}

int
pthread_attr_setscope (pthread_attr_t *attr, int scope)
{
  attr->__scope = scope;
  return 0;
}

int pthread_attr_getscope (const pthread_attr_t *__restrict attr,
                           int *__restrict scope)
{
  *scope = attr->__scope;
  return 0;
}

int
pthread_attr_setguardsize (pthread_attr_t *attr, size_t guardsize)
{
  attr->__guardsize = guardsize;
  return 0;
}

int
pthread_attr_getguardsize (const pthread_attr_t *__restrict attr,
                           size_t *__restrict guardsize)
{
  *guardsize = attr->__guardsize;
  return 0;
}

int
pthread_attr_setstackaddr (pthread_attr_t *attr, void *stackaddr)
{
  attr->__stackaddr = stackaddr;
  attr->__stackaddr_set = 1;
  return 0;
}

int
pthread_attr_getstackaddr (const pthread_attr_t *__restrict attr,
                           void **__restrict stackaddr)
{
  *stackaddr = attr->__stackaddr;
  return 0;
}

int
pthread_attr_setstack (pthread_attr_t *attr, void *stackaddr, size_t stacksize)
{
  attr->__stackaddr = stackaddr;
  attr->__stacksize = stacksize;
  attr->__stackaddr_set = 1;
  return 0;
}

int
pthread_attr_getstack (const pthread_attr_t *__restrict attr,
		       void **__restrict stackaddr,
		       size_t *__restrict stacksize)
{
  *stackaddr = attr->__stackaddr;
  *stacksize = attr->__stacksize;
  return 0;
}

int
pthread_attr_setstacksize (pthread_attr_t *attr, size_t stacksize)
{
  attr->__stacksize = stacksize;
  return 0;
}

int
pthread_attr_getstacksize (const pthread_attr_t *__restrict attr,
                           size_t *__restrict stacksize)
{
  *stacksize = attr->__stacksize;
  return 0;
}

int
pthread_getattr_np (pthread_t th, pthread_attr_t *attr)
{
  int result = __pt_thread_is_detached (th);
  if (result < 0)
    {
      /* Invalid thread identifier */
      errno = ESRCH;
      return -1;
    }
  pthread_attr_init (attr);
  attr->__detachstate = (result ? PTHREAD_CREATE_DETACHED
                                : PTHREAD_CREATE_JOINABLE);
  return 0;
}
