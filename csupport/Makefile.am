
.PHONY: OpenSystem.C.dll

all-local: OpenSystem.C.dll

OpenSystem.C.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f csupport.build all

CLEANFILES = OpenSystem.C.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f csupport.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f csupport.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
