
AUTOMAKE_OPTIONS = no-dependencies

noinst_LIBRARIES = libCPwd.a

libCPwd_a_SOURCES = endpwent.c \
					getpwent.c \
					getpwnam.c \
					getpwnam_r.c \
					getpwuid.c \
					getpwuid_r.c \
					pwent.c \
					pwent.h \
					pwpersist.c \
					setpwent.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-I$(top_srcdir)/libc/unistd \
			-imacros $(top_srcdir)/include/libc-symbols.h
