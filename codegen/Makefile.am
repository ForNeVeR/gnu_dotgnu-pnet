lib_LIBRARIES = libILCodeGen.a

TREECC_INPUTS = cg_all.tc \
				cg_arith.tc \
				cg_assign.tc \
				cg_cast.tc \
				cg_const.tc \
				cg_decls.tc \
				cg_logical.tc \
				cg_lvalue.tc \
				cg_misc.tc \
				cg_nodes.tc \
				cg_relop.tc \
				cg_stmt.tc \
				jv_arith.tc \
				jv_assign.tc \
				jv_cast.tc \
				jv_const.tc \
				jv_decls.tc \
				jv_logical.tc \
				jv_lvalue.tc \
				jv_misc.tc \
				jv_nodes.tc \
				jv_relop.tc \
				jv_stmt.tc

TREECC_SRCOUT = cg_arith.c \
				cg_assign.c \
				cg_cast.c \
				cg_const.c \
				cg_decls.c \
				cg_logical.c \
				cg_lvalue.c \
				cg_misc.c \
				cg_nodes.c \
				cg_relop.c \
				cg_stmt.c \
				jv_arith.c \
				jv_assign.c \
				jv_cast.c \
				jv_nodes.c

TREECC_OUTPUT = $(TREECC_SRCOUT) cg_nodes.h

libILCodeGen_a_SOURCES = $(TREECC_SRCOUT) \
						 cg_coerce.c \
						 cg_gen.c \
						 cg_library.c \
						 cg_optable.c \
						 cg_output.c \
						 cg_rbtree.c \
						 cg_resolve.c \
						 cg_scope.c \
						 cg_utils.c \
						 jv_output.c

if USINGGCC
INCLUDES = -Wall
endif

STAMP = stamp-treecc

BUILT_SOURCES = $(STAMP)

libILCodeGen_a_DEPENDENCIES = $(STAMP)

$(STAMP): $(TREECC_INPUTS)
	$(TREECC) cg_all.tc && touch $@

CLEANFILES = $(TREECC_OUTPUT) $(STAMP)
