
.PHONY: System.Net.IrDA.resources.dll

all-local: System.Net.IrDA.resources.dll

System.Net.IrDA.resources.dll:
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build all

install-data-local:
	@$(NORMAL_INSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -i all

uninstall-local:
	@$(NORMALL_UNINSTALL)
	"$(CSANT)" $(CSANT_FLAGS) -f resources.build \
			-a $(DESTDIR)$(libdir)/cscc/lib -u all

CLEANFILES = System.Net.IrDA.de.resources \
			 System.Net.IrDA.resources.dll
