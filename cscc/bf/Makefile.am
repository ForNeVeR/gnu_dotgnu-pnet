
noinst_LIBRARIES = libILBf.a

TREECC_INPUTS = bf_defs.tc \
				bf_codegen.tc

TREECC_SRCOUT = bf_nodes.c bf_codegen.c

TREECC_OUTPUT = $(TREECC_SRCOUT) bf_defs.h

EXTRA_DIST = $(TREECC_INPUTS)

libILBf_a_SOURCES  = bf_grammar.y \
					 bf_scanner.l \
					 bf_optimize.c \
					 bf_internal.h \
					 bf_rename.h \
					 $(TREECC_SRCOUT)

AM_YFLAGS = -d

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/include

STAMP = stamp-treecc

BUILT_SOURCES = $(STAMP)

libILBf_a_DEPENDENCIES = $(STAMP)

$(STAMP): $(TREECC_INPUTS) ../../codegen/$(STAMP)
	$(TREECC) $(srcdir)/bf_defs.tc && touch $@

bf_scanner.l: bf_grammar.c

CLEANFILES = bf_grammar.c bf_scanner.c bf_grammar.h $(TREECC_OUTPUT) $(STAMP)
