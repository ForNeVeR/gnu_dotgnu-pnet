
.PHONY: DotGNU.Images.dll

all-local: DotGNU.Images.dll

DotGNU.Images.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f DotGNU.Images.build all

CLEANFILES = DotGNU.Images.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f DotGNU.Images.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f DotGNU.Images.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
