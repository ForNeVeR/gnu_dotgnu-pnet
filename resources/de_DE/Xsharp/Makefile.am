
.PHONY: Xsharp.resources.dll

all-local: Xsharp.resources.dll

Xsharp.resources.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build all

install-data-local:
	@$(NORMAL_INSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all

CLEANFILES = Xsharp.de.resources \
			 Xsharp.resources.dll
