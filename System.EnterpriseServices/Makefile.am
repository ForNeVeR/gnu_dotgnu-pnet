
.PHONY: System.EnterpriseServices.dll

all-local: System.EnterpriseServices.dll

System.EnterpriseServices.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f System.EnterpriseServices.build all

CLEANFILES = System.EnterpriseServices.dll

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)/cscc/lib
	"$(CSANT)" $(CSANT_FLAGS) -f System.EnterpriseServices.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f System.EnterpriseServices.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all
