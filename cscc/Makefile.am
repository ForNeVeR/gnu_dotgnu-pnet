
SUBDIRS = common csharp c vb

noinst_PROGRAMS = cscc-cs cscc-c-s cscc-vb cscc csdoc
man_MANS        = cscc.1 csdoc.1
EXTRA_DIST      = $(man_MANS)

cscc_SOURCES = cscc.c
cscc_LDADD   = common/libILCCommon.a ../ilasm/libILAsm.a \
			   ../ilalink/libILLink.a \
			   ../dumpasm/libILDumpAsm.a ../image/libILImage.a \
			   ../support/libILSupport.a

cscc_cs_SOURCES = cs_main.c
cscc_cs_LDADD   = csharp/libILCSharp.a common/libILCCommon.a \
				  ../codegen/libILCodeGen.a ../dumpasm/libILDumpAsm.a \
				  ../image/libILImage.a ../support/libILSupport.a

cscc_c_s_SOURCES = c_main.c
cscc_c_s_LDADD   = c/libILCLang.a common/libILCCommon.a \
				   ../codegen/libILCodeGen.a ../dumpasm/libILDumpAsm.a \
				   ../image/libILImage.a ../support/libILSupport.a

cscc_vb_SOURCES = vb_main.c
cscc_vb_LDADD   = vb/libILVB.a csharp/libILCSharp.a common/libILCCommon.a \
				  ../codegen/libILCodeGen.a ../dumpasm/libILDumpAsm.a \
				  ../image/libILImage.a ../support/libILSupport.a

csdoc_SOURCES = csdoc.c
csdoc_LDADD   = csharp/libILCSharp.a common/libILCCommon.a \
				../codegen/libILCodeGen.a ../dumpasm/libILDumpAsm.a \
				../image/libILImage.a ../support/libILSupport.a

AM_CFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/include

install-exec-local:
	@$(NORMALL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/plugins
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) cscc-cs $(DESTDIR)$(libdir)/cscc/plugins/cscc-cs
	$(INSTALL_PROGRAM) cscc-c-s $(DESTDIR)$(libdir)/cscc/plugins/cscc-c-s
	$(INSTALL_PROGRAM) cscc-vb $(DESTDIR)$(libdir)/cscc/plugins/cscc-vb
	$(INSTALL_PROGRAM) cscc $(DESTDIR)$(bindir)/cscc
	$(INSTALL_PROGRAM) csdoc $(DESTDIR)$(bindir)/csdoc
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-csharp
	$(LN_S) cscc-cs $(DESTDIR)$(libdir)/cscc/plugins/cscc-csharp

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-csharp
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-cs
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-c-s
	rm -f $(DESTDIR)$(libdir)/cscc/plugins/cscc-vb
	rm -f $(DESTDIR)$(bindir)/cscc
	rm -f $(DESTDIR)$(bindir)/csdoc
