
noinst_LIBRARIES = libCSocket.a

SUFFIXES = .cs

libCSocket_a_SOURCES = \
		inet_addr.c \
		inet_lnaof.c \
		inet_mkadr.c \
		inet_net.c \
		inet_netof.c \
		inet_neta.c \
		inet_net_ntop.c \
		inet_net_pton.c \
		inet_ntoa.c \
		inet_ntop.c \
		inet_pton.c \
		nsap_addr.c \
		ntoh.c \
		socket.c \
		socket-glue.cs

AM_CFLAGS = -funsafe -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h \
			-lSystem -lDotGNU.SSL -lOpenSystem.C

.cs.o:
	$(COMPILE) -x cs -o $@ -c $<
