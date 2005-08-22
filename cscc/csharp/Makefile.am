
noinst_LIBRARIES = libILCSharp.a

TREECC_INPUTS = cs_cast.tc \
				cs_const.tc \
				cs_decls.tc \
			    cs_defs.tc \
				cs_lvalue.tc \
				cs_misc.tc \
				cs_invoke.tc \
				cs_oper.tc \
				cs_semantics.tc \
				cs_stmt.tc \
				cs_types.tc

TREECC_SRCOUT = cs_nodes.c cs_semantics.c

TREECC_OUTPUT = $(TREECC_SRCOUT) cs_defs.h

EXTRA_DIST = $(TREECC_INPUTS)

libILCSharp_a_SOURCES  = cs_attrs.c \
						 cs_gather.c \
						 cs_grammar.y \
						 cs_internal.h \
						 cs_lookup.c \
						 cs_modifiers.c \
						 cs_rename.h \
						 cs_scanner.l \
						 cs_semvalue.c \
						 cs_semvalue.h \
						 cs_valdoc.c \
						 $(TREECC_SRCOUT)

AM_YFLAGS = -d

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/include

STAMP = stamp-treecc

BUILT_SOURCES = $(STAMP)

libILCSharp_a_DEPENDENCIES = $(STAMP)

$(STAMP): $(TREECC_INPUTS) ../../codegen/$(STAMP)
	$(TREECC) $(srcdir)/cs_defs.tc && touch $@

cs_scanner.l: cs_grammar.c

CLEANFILES = cs_grammar.c cs_scanner.c cs_grammar.h $(TREECC_OUTPUT) $(STAMP)
