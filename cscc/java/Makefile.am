
noinst_LIBRARIES = libILJava.a

TREECC_INPUTS = java_defs.tc \
				java_semantics.tc \
				java_lvalue.tc \
				java_types.tc \
				java_stmt.tc \
				java_decls.tc \
				java_invoke.tc \
				java_const.tc \
				java_oper.tc \
				java_misc.tc \
				java_cast.tc 

TREECC_SRCOUT = java_nodes.c java_semantics.c

TREECC_OUTPUT = $(TREECC_SRCOUT) java_defs.h

EXTRA_DIST = $(TREECC_INPUTS)

libILJava_a_SOURCES = java_internal.h \
					  java_grammar.y \
					  java_scanner.l \
					  java_gather.c \
					  java_modifiers.c \
					  java_lookup.c \
					  java_rename.h \
					  java_semvalue.c \
					  java_semvalue.h \
					  $(TREECC_SRCOUT)
							

AM_YFLAGS = -d

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/include

STAMP = stamp-treecc

BUILT_SOURCES = $(STAMP)

libILJava_a_DEPENDENCIES = $(STAMP)

$(STAMP): $(TREECC_INPUTS) ../../codegen/$(STAMP)
	$(TREECC) $(srcdir)/java_defs.tc && touch $@

java_scanner.l: java_grammar.c

CLEANFILES = java_grammar.c java_scanner.c java_grammar.h $(TREECC_OUTPUT) $(STAMP)
