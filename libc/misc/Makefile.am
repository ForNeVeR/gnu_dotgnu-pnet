AUTOMAKE_OPTIONS = no-dependencies

noinst_LIBRARIES = libCMisc.a

libCMisc_a_SOURCES = errno.c \
					 longjmp.c \
					 mmap.c \
					 mmap64.c \
					 munmap.c \
					 mprotect.c \
					 msync.c \
					 mlock.c \
					 mlockall.c \
					 munlock.c \
					 munlockall.c \
					 madvise.c \
					 mincore.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h
