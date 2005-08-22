
noinst_LIBRARIES = libILCLang.a

TREECC_INPUTS = c_ainit.tc \
				c_builtin.tc \
				c_defs.tc \
				c_const.tc \
				c_invoke.tc \
				c_lvalue.tc \
				c_name.tc \
				c_oper.tc \
				c_stmt.tc \
				c_stubs.tc

TREECC_SRCOUT = c_nodes.c c_semantics.c c_name.c

TREECC_OUTPUT = $(TREECC_SRCOUT) c_defs.h

EXTRA_DIST = $(TREECC_INPUTS)

libILCLang_a_SOURCES = c_coerce.c \
					   c_crt0.c \
					   c_declspec.c \
					   c_declspec.h \
					   c_function.c \
					   c_function.h \
					   c_grammar.y \
					   c_internal.c \
					   c_internal.h \
					   c_lexutils.c \
					   c_lexutils.h \
					   c_library.c \
					   c_rename.h \
					   c_scanner.l \
					   c_scope.c \
					   c_scope.h \
					   c_typeout.c \
					   c_types.c \
					   c_types.h \
					   $(TREECC_SRCOUT)

AM_YFLAGS = -d

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/include

STAMP = stamp-treecc

BUILT_SOURCES = $(STAMP)

libILCLang_a_DEPENDENCIES = $(STAMP)

$(STAMP): $(TREECC_INPUTS) ../../codegen/$(STAMP)
	$(TREECC) $(srcdir)/c_defs.tc && touch $@

c_scanner.l: c_grammar.c

CLEANFILES = c_grammar.c c_scanner.c c_grammar.h $(TREECC_OUTPUT) $(STAMP)
