
.PHONY: System.Configuration.Install.resources.dll

all-local: System.Configuration.Install.resources.dll

System.Configuration.Install.resources.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build all

install-data-local:
	@$(NORMAL_INSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all

CLEANFILES = System.Configuration.Install.ca.resources \
			 System.Configuration.Install.resources.dll
