
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

libILCSharp_a_SOURCES  = $(TREECC_SRCOUT) \
				         cs_gather.c \
				         cs_grammar.y \
				         cs_lookup.c \
				         cs_modifiers.c \
				         cs_scanner.l \
				         cs_valdoc.c

YFLAGS = -d

if USINGGCC
INCLUDES = -Wall
endif

STAMP = stamp-treecc

BUILT_SOURCES = $(STAMP)

libILCSharp_a_DEPENDENCIES = $(STAMP)

$(STAMP): $(TREECC_INPUTS) ../../codegen/$(STAMP)
	$(TREECC) cs_defs.tc && touch $@

CLEANFILES = cs_grammar.c cs_scanner.c cs_grammar.h $(TREECC_OUTPUT) $(STAMP)
