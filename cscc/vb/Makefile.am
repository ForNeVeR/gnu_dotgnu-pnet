
noinst_LIBRARIES = libILVB.a

libILVB_a_SOURCES  = vb_date.c \
					 vb_grammar.y \
					 vb_modifiers.c \
					 vb_options.c \
					 vb_scanner.l

AM_YFLAGS = -d

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/include

CLEANFILES = vb_grammar.c vb_scanner.c vb_grammar.h
