noinst_PROGRAMS = cscc-cs cscc
man_MANS = cscc.1
EXTRA_DIST = $(man_MANS)

cscc_SOURCES = cscc.c \
			   cs_options.c
cscc_LDADD   = ../image/libILImage.a ../support/libILSupport.a

TREECC_INPUTS = cs_cast.tc \
				cs_const.tc \
				cs_decls.tc \
			    cs_defs.tc \
				cs_lvalue.tc \
				cs_misc.tc \
				cs_oper.tc \
				cs_semantics.tc \
				cs_stmt.tc \
				cs_types.tc

TREECC_SRCOUT = cs_nodes.c cs_semantics.c

TREECC_OUTPUT = $(TREECC_SRCOUT) cs_defs.h

cscc_cs_SOURCES = $(TREECC_SRCOUT) \
				  cs_errors.c \
				  cs_gather.c \
				  cs_grammar.y \
				  cs_main.c \
				  cs_modifiers.c \
			      cs_options.c \
				  cs_preproc.c \
				  cs_scanner.l
cscc_cs_LDADD   = ../codegen/libILCodeGen.a ../dumpasm/libILDumpAsm.a \
				  ../image/libILImage.a ../support/libILSupport.a

YFLAGS = -d

if USINGGCC
INCLUDES = -Wall -DCSCC_LIB_PREFIX=\"$(libdir)/cscc\" \
				 -DCSCC_BIN_PREFIX=\"$(bindir)\"
else
INCLUDES = -DCSCC_LIB_PREFIX=\"$(libdir)/cscc\" \
		   -DCSCC_BIN_PREFIX=\"$(bindir)\"
endif

STAMP = stamp-treecc

BUILT_SOURCES = $(STAMP)

cscc_cs_DEPENDENCIES = $(STAMP)

$(STAMP): $(TREECC_INPUTS)
	$(TREECC) cs_defs.tc && touch $@

install-exec-local:
	@$(NORMALL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/plugins
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) cscc-cs $(DESTDIR)$(libdir)/cscc/plugins/cscc-cs
	$(INSTALL_PROGRAM) cscc $(DESTDIR)$(bindir)/cscc
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-csharp
	$(LN_S) cscc-cs $(DESTDIR)$(libdir)/cscc/plugins/cscc-csharp

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-cs
	rm -f $(DESTDIR)$(bindir)/cscc

CLEANFILES = cs_grammar.c cs_scanner.c cs_grammar.h $(TREECC_OUTPUT) $(STAMP)
