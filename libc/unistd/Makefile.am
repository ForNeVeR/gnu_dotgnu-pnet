AUTOMAKE_OPTIONS = no-dependencies

noinst_LIBRARIES = libCUnistd.a

libCUnistd_a_SOURCES = chown-stub.c \
					   guid-stub.c \
					   isatty.c \
					   pid-stub.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h
