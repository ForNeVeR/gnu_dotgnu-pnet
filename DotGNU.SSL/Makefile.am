
.PHONY: DotGNU.SSL.dll

all-local: DotGNU.SSL.dll

DotGNU.SSL.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f SSL.build all

CLEANFILES = DotGNU.SSL.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f SSL.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f SSL.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
