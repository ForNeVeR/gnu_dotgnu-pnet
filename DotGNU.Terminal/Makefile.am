
.PHONY: DotGNU.Terminal.dll

all-local: DotGNU.Terminal.dll

DotGNU.Terminal.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f DotGNU.Terminal.build all

CLEANFILES = DotGNU.Terminal.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f DotGNU.Terminal.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f DotGNU.Terminal.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
