
noinst_LIBRARIES = libCSocket.a

SUFFIXES = .cs

libCSocket_a_SOURCES = \
		ntoh.c \
		socket.c \
		socket-glue.cs

AM_CFLAGS = -funsafe -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h \
			-lSystem -lDotGNU.SSL -lOpenSystem.C

.cs.o:
	$(COMPILE) -x cs -o $@ -c $<
