AUTOMAKE_OPTIONS = no-dependencies

libmdir = $(libdir)/cscc/lib
libm_DATA = libm$(LIBRARY_SUFFIX).dll

noinst_LIBRARIES = libMath.a

libMath_a_SOURCES = fmax.c \
					fmin.c \
					fmod.c \
					redirect.c \
					test.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h

libm$(LIBRARY_SUFFIX).dll: libMath.a
	$(LINK) libMath.a -lOpenSystem.C -L../libc -llibc$(LIBRARY_SUFFIX)

CLEANFILES = libm$(LIBRARY_SUFFIX).dll
