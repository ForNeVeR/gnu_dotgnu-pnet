AUTOMAKE_OPTIONS = no-dependencies

all-local: libm$(LIBRARY_SUFFIX).dll

noinst_LIBRARIES = libMath.a

libMath_a_SOURCES = fmax.c \
					fmin.c \
					fmod.c \
					redirect.c \
					test.c

AM_CFLAGS = -I$(top_srcdir)/include \
			-imacros $(top_srcdir)/include/libc-symbols.h

libm$(LIBRARY_SUFFIX).dll: libMath.a
	$(LINK) libMath.a -L../libc -llibc$(LIBRARY_SUFFIX)

CLEANFILES = libm$(LIBRARY_SUFFIX).dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(CYG_CACHE)
	$(ILGAC) -c "$(DESTDIR)$(CYG_CACHE)" -f -d \
			-i libm$(LIBRARY_SUFFIX).dll

uninstall-local:
	@$(NORMAL_UNINSTALL)
	$(ILGAC) -c "$(DESTDIR)$(CYG_CACHE)" -f -d \
			-u libm$(LIBRARY_SUFFIX) $(MSCORLIB_VERSION)
