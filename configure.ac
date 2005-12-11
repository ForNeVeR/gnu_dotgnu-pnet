## Process this file with autoconf to produce a configure script.

# Initialize autoconf.
AC_PREREQ(2.59)
AC_INIT(Crayons, 0.0.1)
#AC_CONFIG_HEADERS([include/CrayonsConfig.h:include/CrayonsConfig.h.in])
AC_CONFIG_SRCDIR(src/CGraphics.c)

# Initialize automake.
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(include/CrayonsConfig.h)

# Check for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Check for libraries.
AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB([pixman], [pixman_image_create])
AC_CHECK_LIB([pthread], [pthread_mutex_init])
AC_CHECK_LIB([X11], [XOpenDisplay])

# Check for headers.
AC_HEADER_STDC
AC_PATH_XTRA
AC_CHECK_HEADERS([assert.h math.h memory.h stdlib.h string.h strings.h])
AC_CHECK_HEADERS([pixman.h pthread.h X11/Xlib.h])

# Check for compiler characteristics.
AC_C_CONST

# Check for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_CHECK_FUNCS(bcmp bcopy memcmp memcpy memmove memset)
AC_CHECK_FUNCS(cos sin sqrt erf)

# Determine the size of the int and long types on the target machine.
AC_COMPILE_CHECK_SIZEOF(int)
AC_COMPILE_CHECK_SIZEOF(long)
AC_COMPILE_CHECK_SIZEOF(long long)

# Make things easier to debug.
#CFLAGS="-g -W -Wall -O2"

# Set output files.
AC_CONFIG_FILES([Makefile
                 include/Makefile
                 src/Makefile
		 samples/Makefile])
AC_OUTPUT
