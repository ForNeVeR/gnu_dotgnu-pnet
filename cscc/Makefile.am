
SUBDIRS = common csharp

noinst_PROGRAMS = cscc-cs cscc csdoc
man_MANS        = cscc.1 csdoc.1
EXTRA_DIST      = $(man_MANS)

cscc_SOURCES = cscc.c
cscc_LDADD   = common/libILCCommon.a ../image/libILImage.a \
			   ../support/libILSupport.a

cscc_cs_SOURCES = cs_main.c
cscc_cs_LDADD   = csharp/libILCSharp.a common/libILCCommon.a \
				  ../codegen/libILCodeGen.a ../dumpasm/libILDumpAsm.a \
				  ../image/libILImage.a ../support/libILSupport.a

csdoc_SOURCES = csdoc.c
csdoc_LDADD   = csharp/libILCSharp.a common/libILCCommon.a \
				../codegen/libILCodeGen.a ../dumpasm/libILDumpAsm.a \
				../image/libILImage.a ../support/libILSupport.a

if USINGGCC
INCLUDES = -Wall -DCSCC_LIB_PREFIX=\"$(libdir)/cscc\" \
				 -DCSCC_BIN_PREFIX=\"$(bindir)\"
else
INCLUDES = -DCSCC_LIB_PREFIX=\"$(libdir)/cscc\" \
		   -DCSCC_BIN_PREFIX=\"$(bindir)\"
endif

install-exec-local:
	@$(NORMALL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/plugins
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) cscc-cs $(DESTDIR)$(libdir)/cscc/plugins/cscc-cs
	$(INSTALL_PROGRAM) cscc $(DESTDIR)$(bindir)/cscc
	$(INSTALL_PROGRAM) csdoc $(DESTDIR)$(bindir)/csdoc
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-csharp
	$(LN_S) cscc-cs $(DESTDIR)$(libdir)/cscc/plugins/cscc-csharp

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-cs
	rm -f $(DESTDIR)$(bindir)/cscc
	rm -f $(DESTDIR)$(bindir)/csdoc
