
.PHONY: compat

all-local: compat

compat:
	"$(CSANT)" $(CSANT_FLAGS) -f compat.build all

CLEANFILES = Microsoft.VisualC.dll Microsoft.Vsa.dll cscompmgd.dll \
			 Accessibility.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f compat.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f compat.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
