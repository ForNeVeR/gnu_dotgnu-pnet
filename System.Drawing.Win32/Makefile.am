
.PHONY: System.Drawing.Win32.dll

all-local: System.Drawing.Win32.dll

System.Drawing.Win32.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Win32.build all

CLEANFILES = System.Drawing.Win32.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Win32.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f System.Drawing.Win32.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
