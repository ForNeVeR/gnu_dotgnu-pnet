
noinst_LIBRARIES = libILCLang.a

TREECC_INPUTS = c_defs.tc \
				c_const.tc \
				c_invoke.tc \
				c_lvalue.tc \
				c_oper.tc \
				c_stmt.tc \
				c_stubs.tc

TREECC_SRCOUT = c_nodes.c c_semantics.c

TREECC_OUTPUT = $(TREECC_SRCOUT) c_defs.h

libILCLang_a_SOURCES = $(TREECC_SRCOUT) \
					   c_crt0.c \
					   c_declspec.c \
					   c_grammar.y \
					   c_internal.c \
					   c_lexutils.c \
					   c_scanner.l \
					   c_scope.c \
					   c_typeout.c \
					   c_types.c

YFLAGS = -d

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/include

STAMP = stamp-treecc

BUILT_SOURCES = $(STAMP)

libILCLang_a_DEPENDENCIES = $(STAMP)

$(STAMP): $(TREECC_INPUTS) ../../codegen/$(STAMP)
	$(TREECC) $(srcdir)/c_defs.tc && touch $@

CLEANFILES = c_grammar.c c_scanner.c c_grammar.h $(TREECC_OUTPUT) $(STAMP)
