
.PHONY: compat

all-local: compat

compat:
	"$(CSANT)" $(CSANT_FLAGS) -f compat.build all

CLEANFILES = Microsoft.VisualC.dll Microsoft.Vsa.dll cscompmgd.dll \
			 Accessibility.dll

install-exec-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	$(INSTALL_DATA) Microsoft.VisualC.dll $(DESTDIR)$(libdir)/cscc/lib/Microsoft.VisualC.dll
	$(INSTALL_DATA) Microsoft.Vsa.dll $(DESTDIR)$(libdir)/cscc/lib/Microsoft.Vsa.dll
	$(INSTALL_DATA) cscompmgd.dll $(DESTDIR)$(libdir)/cscc/lib/cscompmgd.dll
	$(INSTALL_DATA) Accessibility.dll $(DESTDIR)$(libdir)/cscc/lib/Accessibility.dll

uninstall-local:
	@$(NORMALL_UNINSTALL)
	rm -f $(DESTDIR)$(libdir)/cscc/lib/Microsoft.VisualC.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/Microsoft.Vsa.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/cscompmgd.dll
	rm -f $(DESTDIR)$(libdir)/cscc/lib/Accessibility.dll
