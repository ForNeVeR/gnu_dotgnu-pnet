AUTOMAKE_OPTIONS = no-dependencies

noinst_LIBRARIES = libCTime.a

SUFFIXES = .cs

libCTime_a_SOURCES = clock.c \
                     gmtime.c \
                     time.c \
                     time-defs.h \
					 time-glue.cs \
					 time-vars.c \
					 tzset.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h \
			-lOpenSystem.C

.cs.o:
	$(COMPILE) -x cs -o $@ -c $<
