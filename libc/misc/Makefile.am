AUTOMAKE_OPTIONS = no-dependencies

noinst_LIBRARIES = libCMisc.a

libCMisc_a_SOURCES = errno.c \
					 longjmp.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h
