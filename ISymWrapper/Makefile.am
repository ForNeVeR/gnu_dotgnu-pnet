
.PHONY: ISymWrapper.dll

all-local: ISymWrapper.dll

ISymWrapper.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f ISymWrapper.build all

CLEANFILES = ISymWrapper.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f ISymWrapper.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f ISymWrapper.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
