AUTOMAKE_OPTIONS = no-dependencies

noinst_LIBRARIES = libCPthread.a

libCPthread_a_SOURCES = mutex.c \
						self.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h
