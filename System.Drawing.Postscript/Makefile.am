
.PHONY: System.Drawing.Postscript.dll

all-local: System.Drawing.Postscript.dll

System.Drawing.Postscript.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Postscript.build all

CLEANFILES = System.Drawing.Postscript.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Postscript.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Postscript.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
