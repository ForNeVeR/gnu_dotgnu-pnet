
noinst_LIBRARIES = libILCCommon.a

libILCCommon_a_SOURCES = cc_compat.c \
						 cc_cppdefs.c \
						 cc_errors.c \
						 cc_main.c \
						 cc_options.c \
						 cc_preproc.c

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/include

$(srcdir)/cc_cppdefs.c: $(srcdir)/cpp_defs.sh
	$(SHELL) $(srcdir)/cpp_defs.sh >$(srcdir)/cc_cppdefs.c

CLEANFILES = $(srcdir)/cc_cppdefs.c
