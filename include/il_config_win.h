/*
 * il_config_win.h - Configuration values when compiling with Visual Studio.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

#ifndef	_IL_CONFIG_WIN_H
#define	_IL_CONFIG_WIN_H

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#undef HAVE_ALLOCA_H

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#undef HAVE_SYS_WAIT_H

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */

/* Define if you have the ANSI C header files.  */
#undef STDC_HEADERS

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#undef TIME_WITH_SYS_TIME

/* Define if lex declares yytext as a char * by default, not a char[].  */
#undef YYTEXT_POINTER

/* The number of bytes in a double.  */
#define SIZEOF_DOUBLE 8

/* The number of bytes in a float.  */
#define SIZEOF_FLOAT 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a long double.  */
/* Note: Visual Studio does have "long double", but it
   is the same as "double" */
#define SIZEOF_LONG_DOUBLE 0

/* The number of bytes in a long long.  */
#define SIZEOF_LONG_LONG 0

/* Define if you have the bcmp function.  */
#undef HAVE_BCMP

/* Define if you have the bcopy function.  */
#undef HAVE_BCOPY

/* Define if you have the bzero function.  */
#undef HAVE_BZERO

/* Define if you have the execv function.  */
#undef HAVE_EXECV

/* Define if you have the finite function.  */
#undef HAVE_FINITE

/* Define if you have the fmod function.  */
#define HAVE_FMOD 1

/* Define if you have the fork function.  */
#undef HAVE_FORK

/* Define if you have the getpagesize function.  */
#undef HAVE_GETPAGESIZE

/* Define if you have the getpid function.  */
#undef HAVE_GETPID

/* Define if you have the gettimeofday function.  */
#undef HAVE_GETTIMEOFDAY

/* Define if you have the isinf function.  */
#undef HAVE_ISINF

/* Define if you have the isnan function.  */
#undef HAVE_ISNAN

/* Define if you have the memchr function.  */
#define HAVE_MEMCHR 1

/* Define if you have the memcmp function.  */
#define HAVE_MEMCMP 1

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the memset function.  */
#define HAVE_MEMSET 1

/* Define if you have the mmap function.  */
#undef HAVE_MMAP

/* Define if you have the munmap function.  */
#undef HAVE_MUNMAP

/* Define if you have the open function.  */
#undef HAVE_OPEN

/* Define if you have the qsort function.  */
#define HAVE_QSORT 1

/* Define if you have the stat function.  */
#undef HAVE_STAT

/* Define if you have the strtod function.  */
#define HAVE_STRTOD 1

/* Define if you have the vfprintf function.  */
#define HAVE_VFPRINTF 1

/* Define if you have the wait function.  */
#undef HAVE_WAIT

/* Define if you have the waitpid function.  */
#undef HAVE_WAITPID

/* Define if you have the <fcntl.h> header file.  */
#undef HAVE_FCNTL_H

/* Define if you have the <math.h> header file.  */
#define HAVE_MATH_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#undef HAVE_STRINGS_H

/* Define if you have the <sys/mman.h> header file.  */
#undef HAVE_SYS_MMAN_H

/* Define if you have the <sys/stat.h> header file.  */
#undef HAVE_SYS_STAT_H

/* Define if you have the <sys/time.h> header file.  */
#undef HAVE_SYS_TIME_H

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

/* Define if you have the <varargs.h> header file.  */
#undef HAVE_VARARGS_H

/* Define if you have the m library (-lm).  */
#define HAVE_LIBM 1

/* Name of package */
#define PACKAGE "pnet"

/* Version number of package */
#define VERSION "0.0.5"

#endif	/* _IL_CONFIG_WIN_H */
