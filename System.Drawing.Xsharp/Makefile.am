
.PHONY: System.Drawing.Xsharp.dll

all-local: System.Drawing.Xsharp.dll

System.Drawing.Xsharp.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Xsharp.build all

CLEANFILES = System.Drawing.Xsharp.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Xsharp.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Xsharp.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
