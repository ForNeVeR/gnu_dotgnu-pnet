AUTOMAKE_OPTIONS = no-dependencies

noinst_LIBRARIES = libCStdio.a

libCStdio_a_SOURCES = printf.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h
