AUTOMAKE_OPTIONS = no-dependencies

all-local: libpthread$(LIBRARY_SUFFIX).dll

SUFFIXES = .cs

noinst_LIBRARIES = libPthread.a

libPthread_a_SOURCES = \
			pthread_cond.c \
			pthread_glue.cs \
			pthread_kill.c \
			pthread_mutex.c \
			pthread_once.c \
			pthread_thread.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h \
			-lOpenSystem.C

libpthread$(LIBRARY_SUFFIX).dll: libPthread.a
	$(LINK) libPthread.a -L../libc -llibc$(LIBRARY_SUFFIX)

CLEANFILES = libpthread$(LIBRARY_SUFFIX).dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(CYG_CACHE)
	$(ILGAC) -c "$(DESTDIR)$(CYG_CACHE)" -f -d \
			-i libpthread$(LIBRARY_SUFFIX).dll

uninstall-local:
	@$(NORMAL_UNINSTALL)
	$(ILGAC) -c "$(DESTDIR)$(CYG_CACHE)" -f -d \
			-u libpthread$(LIBRARY_SUFFIX) $(MSCORLIB_VERSION)

.cs.o:
	$(COMPILE) -funsafe -x cs -o $@ -c $<
