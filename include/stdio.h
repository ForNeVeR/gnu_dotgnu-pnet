#ifndef	_STDIO_H
#define	_STDIO_H

/* This is a temporary hack until we can get glibc's stdio working */

#include <stddef.h>
#include <stdarg.h>

extern int printf(const char *format, ...);

#endif /* _STDIO_H */
