
AUTOMAKE_OPTIONS = no-dependencies

SUBDIRS = malloc \
		  misc \
		  pthread \
		  pwd \
		  stdio \
		  string \
		  unistd

libcdir = $(libdir)/cscc/lib
libc_DATA = libc$(LIBRARY_SUFFIX).dll

LIBC_COMPONENTS = malloc/libCMalloc.a \
				  misc/libCMisc.a \
				  pthread/libCPthread.a \
				  pwd/libCPwd.a \
				  stdio/libCStdio.a \
				  string/libCString.a \
				  unistd/libCUnistd.a

libc$(LIBRARY_SUFFIX).dll: $(LIBC_COMPONENTS)
	$(LINK) $(LIBC_COMPONENTS)

CLEANFILES = libc$(LIBRARY_SUFFIX).dll

## Build a dummy library, which causes automake to output the
## correct definition for the "LINK" variable into "Makefile".
noinst_LIBRARIES = libCDummy.a
libCDummy_a_SOURCES = dummy.c
